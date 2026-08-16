// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef VULKAN_HEADER_PATH
# include "VulkanParser.h"

namespace AE::Parsers
{
	Nd__In bool  operator == (HeaderParser::EResourceType lhs, VkObjectType rhs)
	{
		return uint(lhs) == uint(rhs);
	}

/*
=================================================
	constructor
=================================================
*/
	VulkanParser::VulkanParser ()
	{
		_macroPrefix	= "VK_";
		_structPrefix	= "Vk";
	}

/*
=================================================
	ParseVkHeaders
=================================================
*/
	bool  VulkanParser::ParseVkHeaders (const Path &folder)
	{
		CHECK_ERR( FileSystem::IsDirectory( folder ));

		struct VkHeaderFile
		{
			StringView			filename;
			Array<StringView>	enableIfdef;
			Array<StringView>	disableIfdef;
			bool				defaultSkip;
		};

		const VkHeaderFile  file_names[] = {
			{ "vulkan_core.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_CORE_H_",			"__cplusplus" },	false },
			{ "vulkan_beta.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_BETA_H_",			"__cplusplus" },	false },
			{ "vulkan_android.h",		{ "VK_NO_PROTOTYPES" },		{ "VULKAN_ANDROID_H_",		"__cplusplus" },	false },
			{ "vulkan_win32.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_WIN32_H_",		"__cplusplus" },	false },
			/*,
			{ "vulkan_fuchsia.h",		{ "VK_NO_PROTOTYPES" },		{ "VULKAN_FUCHSIA_H_",		"__cplusplus" },	false },
			{ "vulkan_ios.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_IOS_H_",			"__cplusplus" },	false },
			{ "vulkan_macos.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_MACOS_H_",		"__cplusplus" },	false },
			{ "vulkan_metal.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_METAL_H_",		"__cplusplus" },	false },
			{ "vulkan_vi.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_VI_H_",			"__cplusplus" },	false },
			{ "vulkan_wayland.h",		{ "VK_NO_PROTOTYPES" },		{ "VULKAN_WAYLAND_H_",		"__cplusplus" },	false },
			{ "vulkan_xcb.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_XCB_H_",			"__cplusplus" },	false },
			{ "vulkan_xlib.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_XLIB_H_",			"__cplusplus" },	false },
			{ "vulkan_directfb.h",		{ "VK_NO_PROTOTYPES" },		{ "VULKAN_DIRECTFB_H_",		"__cplusplus" },	false },
			{ "vulkan_xlib_xrandr.h",	{ "VK_NO_PROTOTYPES" },		{ "VULKAN_XLIB_XRANDR_H_",	"__cplusplus" },	false },
			{ "vulkan_ggp.h",			{ "VK_NO_PROTOTYPES" },		{ "VULKAN_GGP_H_",			"__cplusplus" },	false },
			{ "vulkan_screen.h",		{ "VK_NO_PROTOTYPES" },		{ "VULKAN_SCREEN_H_",		"__cplusplus" },	false } */
		};

		const StringView	skip_funcs [] = {
			// VkAllocationCallbacks
			"vkAllocationFunction",
			"vkReallocationFunction",
			"vkFreeFunction",
			"vkInternalAllocationNotification",
			"vkInternalFreeNotification",

			"vkVoidFunction",

			"VkDebugReportCallbackEXT",
		};

		_funcs.clear();
		_enums.clear();
		_bitfields.clear();
		_structs.clear();

		for (const auto [info, i] : WithIndex(file_names))
		{
			const Path		path = folder / info.filename;
			FileRStream		file{ path };

			if ( not file.IsOpen() )
				continue;

			_fileData.push_back({ "", info.disableIfdef.front() });

			String&		buf = _fileData.back().data;
			CHECK( file.Read( usize(file.Size()), OUT buf ));

			CHECK_ERR( _BuildExtensionList( buf, i ));
			CHECK_ERR( _ParseHeader( buf, uint(_fileData.size()-1), info.enableIfdef, info.disableIfdef, skip_funcs, info.defaultSkip ));
		}

		_BuildExtensionInfo();
		return true;
	}

/*
=================================================
	SetFunctionsScope
=================================================
*/
	bool  VulkanParser::SetFunctionsScope ()
	{
		CHECK_ERR( not _resourceTypes.empty() );

		const HashSet<StringView>	lib_fns = {
			"vkGetInstanceProcAddr"
		};
		const HashSet<StringView>	inst_fns = {
			"vkDeviceMemoryReportCallbackEXT",
			"vkDebugReportCallbackEXT",
			"vkDebugUtilsMessengerCallbackEXT",
			"vkGetDeviceProcAddr"
		};

		for (auto& fn : _funcs)
		{
			CHECK_ERR( not fn.data.args.empty() );

			ResourceTypes_t::const_iterator		res_info;

			for (auto& type : fn.data.args.front().type)
			{
				res_info = _resourceTypes.find( type );
				if ( res_info != _resourceTypes.end() )
					break;
			}

			bool	is_inst	= inst_fns.find( fn.data.name ) != inst_fns.end();
			bool	is_lib	= lib_fns.find( fn.data.name ) != lib_fns.end();

			if ( not is_inst and
				(res_info == _resourceTypes.end() or is_lib) )
			{
				fn.data.scope = EFuncScope::Library;
			}
			else
			if ( res_info != _resourceTypes.end() and
				 (res_info->second.type == VK_OBJECT_TYPE_INSTANCE or
				  res_info->second.type == VK_OBJECT_TYPE_PHYSICAL_DEVICE or
				  is_inst) )
			{
				fn.data.scope = EFuncScope::Instance;
			}
			else
			{
				fn.data.scope = EFuncScope::Device;
			}
		}
		return true;
	}

/*
=================================================
	BuildBasicTypeMap
=================================================
*/
	bool  VulkanParser::BuildBasicTypeMap ()
	{
		_basicTypes.clear();
		_basicTypes.insert({ "void",			EBasicType::Void });
		_basicTypes.insert({ "VkBool32",		EBasicType::Bool });
		_basicTypes.insert({ "char",			EBasicType::Char });
		_basicTypes.insert({ "int",				EBasicType::Int });
		_basicTypes.insert({ "int32_t",			EBasicType::Int });
		_basicTypes.insert({ "uint32_t",		EBasicType::UInt });
		_basicTypes.insert({ "uint8_t",			EBasicType::UInt });
		_basicTypes.insert({ "uint16_t",		EBasicType::UInt });
		_basicTypes.insert({ "uint64_t",		EBasicType::ULong });
		_basicTypes.insert({ "int64_t",			EBasicType::Long });
		_basicTypes.insert({ "size_t",			EBasicType::USize });
		_basicTypes.insert({ "VkDeviceSize",	EBasicType::ULong });
		//_basicTypes.insert({ "VkFlags",		EBasicType::UInt });
		_basicTypes.insert({ "VkSampleMask",	EBasicType::UInt });
		_basicTypes.insert({ "float",			EBasicType::Float });
		_basicTypes.insert({ "double",			EBasicType::Double });
		_basicTypes.insert({ "VkDeviceAddress",	EBasicType::Handle });
		//_basicTypes.insert({ "PFN_vkDebugUtilsMessengerCallbackEXT",	EBasicType::Handle });
		_basicTypes.insert({ "VkDebugReportCallbackEXT",				EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkDebugReportCallbackEXT",			EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkAllocationFunction",				EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkReallocationFunction",				EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkFreeFunction",						EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkInternalAllocationNotification",	EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkInternalFreeNotification",			EBasicType::Handle });
		_basicTypes.insert({ "PFN_vkDebugUtilsMessengerCallbackEXT",	EBasicType::Handle });

		// vulkan_android
		_basicTypes.insert({ "ANativeWindow",		EBasicType::Struct });
		_basicTypes.insert({ "AHardwareBuffer",		EBasicType::Struct });

		// vulkan_mir
		_basicTypes.insert({ "MirConnection",		EBasicType::Struct });
		_basicTypes.insert({ "MirSurface",			EBasicType::Struct });

		// vulkan_wayland
		_basicTypes.insert({ "wl_display",			EBasicType::Struct });
		_basicTypes.insert({ "wl_surface",			EBasicType::Struct });

		// vulkan_win32
		_basicTypes.insert({ "HINSTANCE",			EBasicType::Handle });
		_basicTypes.insert({ "HWND",				EBasicType::Handle });
		_basicTypes.insert({ "HANDLE",				EBasicType::Handle });
		_basicTypes.insert({ "LPCWSTR",				EBasicType::WCharString });
		_basicTypes.insert({ "SECURITY_ATTRIBUTES",	EBasicType::Struct });
		_basicTypes.insert({ "DWORD",				EBasicType::UInt });
		_basicTypes.insert({ "HMONITOR",			EBasicType::Handle });

		// vulkan_xcb
		_basicTypes.insert({ "xcb_connection_t",	EBasicType::Struct });
		_basicTypes.insert({ "xcb_window_t",		EBasicType::Handle });
		_basicTypes.insert({ "xcb_visualid_t",		EBasicType::Handle });	// TODO: check

		// vulkan_xlib
		_basicTypes.insert({ "Display",				EBasicType::Struct });
		_basicTypes.insert({ "Window",				EBasicType::Handle });
		_basicTypes.insert({ "VisualID",			EBasicType::Handle });	// TODO: check

		// vulkan_xlib_xrandr
		_basicTypes.insert({ "RROutput",			EBasicType::Handle });	// TODO: check

		return true;
	}

/*
=================================================
	_AddResourceType
=================================================
*/
	inline void  VulkanParser::_AddResourceType (const char* name, VkObjectType type, const char* typeName)
	{
		_resourceTypes.insert({ name, { EResourceType(type), typeName }});
	}

/*
=================================================
	BuildResourceTypeMap
=================================================
*/
	bool  VulkanParser::BuildResourceTypeMap ()
	{
		_resourceTypes.clear();
		_AddResourceType( "VkInstance",					VK_OBJECT_TYPE_INSTANCE,					"VK_OBJECT_TYPE_INSTANCE"					);
		_AddResourceType( "VkPhysicalDevice",			VK_OBJECT_TYPE_PHYSICAL_DEVICE,				"VK_OBJECT_TYPE_PHYSICAL_DEVICE"			);
		_AddResourceType( "VkDevice",					VK_OBJECT_TYPE_DEVICE,						"VK_OBJECT_TYPE_DEVICE"						);
		_AddResourceType( "VkQueue",					VK_OBJECT_TYPE_QUEUE,						"VK_OBJECT_TYPE_QUEUE"						);
		_AddResourceType( "VkSemaphore",				VK_OBJECT_TYPE_SEMAPHORE,					"VK_OBJECT_TYPE_SEMAPHORE"					);
		_AddResourceType( "VkCommandBuffer",			VK_OBJECT_TYPE_COMMAND_BUFFER,				"VK_OBJECT_TYPE_COMMAND_BUFFER"				);
		_AddResourceType( "VkFence",					VK_OBJECT_TYPE_FENCE,						"VK_OBJECT_TYPE_FENCE"						);
		_AddResourceType( "VkDeviceMemory",				VK_OBJECT_TYPE_DEVICE_MEMORY,				"VK_OBJECT_TYPE_DEVICE_MEMORY"				);
		_AddResourceType( "VkBuffer",					VK_OBJECT_TYPE_BUFFER,						"VK_OBJECT_TYPE_BUFFER"						);
		_AddResourceType( "VkImage",					VK_OBJECT_TYPE_IMAGE,						"VK_OBJECT_TYPE_IMAGE"						);
		_AddResourceType( "VkEvent",					VK_OBJECT_TYPE_EVENT,						"VK_OBJECT_TYPE_EVENT"						);
		_AddResourceType( "VkQueryPool",				VK_OBJECT_TYPE_QUERY_POOL,					"VK_OBJECT_TYPE_QUERY_POOL"					);
		_AddResourceType( "VkBufferView",				VK_OBJECT_TYPE_BUFFER_VIEW,					"VK_OBJECT_TYPE_BUFFER_VIEW"				);
		_AddResourceType( "VkImageView",				VK_OBJECT_TYPE_IMAGE_VIEW,					"VK_OBJECT_TYPE_IMAGE_VIEW"					);
		_AddResourceType( "VkShaderModule",				VK_OBJECT_TYPE_SHADER_MODULE,				"VK_OBJECT_TYPE_SHADER_MODULE"				);
		_AddResourceType( "VkPipelineCache",			VK_OBJECT_TYPE_PIPELINE_CACHE,				"VK_OBJECT_TYPE_PIPELINE_CACHE"				);
		_AddResourceType( "VkPipelineLayout",			VK_OBJECT_TYPE_PIPELINE_LAYOUT,				"VK_OBJECT_TYPE_PIPELINE_LAYOUT"			);
		_AddResourceType( "VkRenderPass",				VK_OBJECT_TYPE_RENDER_PASS,					"VK_OBJECT_TYPE_RENDER_PASS"				);
		_AddResourceType( "VkPipeline",					VK_OBJECT_TYPE_PIPELINE,					"VK_OBJECT_TYPE_PIPELINE"					);
		_AddResourceType( "VkDescriptorSetLayout",		VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT,		"VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT"		);
		_AddResourceType( "VkSampler",					VK_OBJECT_TYPE_SAMPLER,						"VK_OBJECT_TYPE_SAMPLER"					);
		_AddResourceType( "VkDescriptorPool",			VK_OBJECT_TYPE_DESCRIPTOR_POOL,				"VK_OBJECT_TYPE_DESCRIPTOR_POOL"			);
		_AddResourceType( "VkDescriptorSet",			VK_OBJECT_TYPE_DESCRIPTOR_SET,				"VK_OBJECT_TYPE_DESCRIPTOR_SET"				);
		_AddResourceType( "VkFramebuffer",				VK_OBJECT_TYPE_FRAMEBUFFER,					"VK_OBJECT_TYPE_FRAMEBUFFER"				);
		_AddResourceType( "VkCommandPool",				VK_OBJECT_TYPE_COMMAND_POOL,				"VK_OBJECT_TYPE_COMMAND_POOL"				);
		_AddResourceType( "VkSurfaceKHR",				VK_OBJECT_TYPE_SURFACE_KHR,					"VK_OBJECT_TYPE_SURFACE_KHR"				);
		_AddResourceType( "VkSwapchainKHR",				VK_OBJECT_TYPE_SWAPCHAIN_KHR,				"VK_OBJECT_TYPE_SWAPCHAIN_KHR"				);
		_AddResourceType( "VkDisplayKHR",				VK_OBJECT_TYPE_DISPLAY_KHR,					"VK_OBJECT_TYPE_DISPLAY_KHR"				);
		_AddResourceType( "VkDisplayModeKHR",			VK_OBJECT_TYPE_DISPLAY_MODE_KHR,			"VK_OBJECT_TYPE_DISPLAY_MODE_KHR"			);
		_AddResourceType( "VkValidationCacheEXT",		VK_OBJECT_TYPE_VALIDATION_CACHE_EXT,		"VK_OBJECT_TYPE_VALIDATION_CACHE_EXT"		);
		_AddResourceType( "VkSamplerYcbcrConversion",	VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION,	"VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION"	);
		_AddResourceType( "VkDescriptorUpdateTemplate",	VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE,	"VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE"	);
		_AddResourceType( "VkAccelerationStructureNV",	VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV,	"VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV"	);


		// TODO: validation
		return true;
	}

/*
=================================================
	ExtToStruct
=================================================
*/
	Nd__In String  ExtToStruct (StringView extName, StringView stSuffix, StringView extSuffix)
	{
		String	res;
		usize	num_divs = 0;
		for (usize i = 1; i < extName.size(); ++i)
		{
			const char	c = extName[i];
			const char	p = extName[i-1];
			if ( c == '_' )
				++num_divs;
			else
			if ( num_divs >= 2 )
				res << (p == '_' ? ToUpperCase(c) : ToLowerCase(c));
		}
		return res << stSuffix << extSuffix;
	}

/*
=================================================
	_BuildExtensionInfo
=================================================
*/
	void  VulkanParser::_BuildExtensionInfo ()
	{
		const auto	FindSType = [&] (StringView structName) -> StringView
		{{
			auto	en_iter	= _enums.find( SearchableEnum{"VkStructureType"} );
			CHECK_ERR( en_iter != _enums.end() );

			const StringView	field_prefix	= "VK_STRUCTURE_TYPE_";
			const StringView	rhs				= structName.substr( "Vk"sv.size() );

			StringView	best_match;
			usize		num_chars	= 0;

			for (const auto& field : en_iter->data.fields)
			{
				const auto	lhs = field.name.substr( field_prefix.size() );

				for (usize lpos = 0, rpos = 0, match = 0;;)
				{
					if ( lpos >= lhs.size() or rpos >= rhs.size() )
					{
						if ( match > num_chars )
						{
							num_chars	= match;
							best_match	= field.name;
						}
						break;
					}

					if ( ToLowerCase(lhs[lpos]) == ToLowerCase(rhs[rpos]) )
					{
						++lpos;
						++rpos;
						++match;
						continue;
					}

					++lpos;
				}

				if ( num_chars == rhs.size() )
					break;
			}

			if ( num_chars + 2 < rhs.size() )
				RETURN_ERR( "Failed to find sType for "s << structName );

			return best_match;
		}};

		const auto	BuildNewExt = [] (ArrayView<StringView> tokens, StringView newExt)
		{{
			String	res;
			for (usize i = 0; i < tokens.size(); ++i)
			{
				if ( i > 0 ) res << '_';
				if ( i == 1 ) { res << newExt;  continue; }
				res << tokens[i];
			}
			return res;
		}};

		const auto	ExtractExt = [] (StringView structName) -> StringView
		{{
			for (usize i = structName.length()-1; i < structName.length(); --i)
			{
				if ( not IsUpperCase( structName[i] ))
					return structName.substr( i+1 );
			}
			return {};
		}};

		const auto	ExtToTokens = [] (StringView ext) -> Array<StringView>
		{{
			Array<StringView>	tokens;
			Parser::Tokenize( ext, '_', OUT tokens );

			CHECK( tokens.size() > 2 );
			CHECK( tokens[0] == "VK" );
			return tokens;
		}};


		const HashSet<StringView>	skip_ext = {
			VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
		};

		Array< StructMap_t::const_iterator >		defer_structs;
		Array< StructTypedefs_t::const_iterator >	defer_typedefs;

		// find struct with feature and properties for each extension
		for (auto it = _structs.begin(); it != _structs.end(); ++it)
		{
			auto&	ext		= it->data.extension;
			auto&	st_name	= it->data.name;

			if ( ext.empty() )
				continue;

			if ( skip_ext.contains( ext ))
				continue;

			if ( not StartsWith( st_name, "VkPhysicalDevice" ))
				continue;

			Array<StringView>	tokens = ExtToTokens( ext );

			if ( not EndsWith( st_name, tokens[1] ))
			{
				defer_structs.push_back( it );
				continue;
			}

			auto&	info = _extInfo[ ext ];
			info.structs.push_back( it );

			if ( EndsWithIC( st_name, ExtToStruct( ext, "Features", tokens[1] )))
			{
				CHECK( not info.feats.has_value() );
				info.feats		= it;
				info.featsSType	= FindSType( st_name );
			}
			else
			if ( EndsWithIC( st_name, ExtToStruct( ext, "Properties", tokens[1] )))
			{
				CHECK( not info.props.has_value() );
				info.props		= it;
				info.propsSType	= FindSType( st_name );
			}
			else
			// some extensions don't duplicate 'Properties' in properties struct,
			// example: VK_AMD_shader_core_properties and VkPhysicalDeviceShaderCorePropertiesAMD
			if ( EndsWithIC( ext, "Properties" ) and EndsWithIC( st_name, ExtToStruct( ext, "", tokens[1] )))
			{
				CHECK( not info.props.has_value() );
				info.props		= it;
				info.propsSType	= FindSType( st_name );
			}
		}

		// find alias with feature and properties for each extension
		for (auto it = _typedefs.begin(); it != _typedefs.end(); ++it)
		{
			auto&	ext			= it->second.extension;
			auto&	st_name		= it->first;			// 'typedef <st_name>  <new_name>' or 'using <new_name> = <st_name>'
			auto&	new_name	= it->second.dstType;

			if ( ext.empty() )
				continue;

			if ( skip_ext.contains( ext ))
				continue;

			if ( not StartsWith( new_name, "VkPhysicalDevice" ))
				continue;

			Array<StringView>	tokens = ExtToTokens( ext );

			if ( not EndsWith( st_name, tokens[1] ))
			{
				defer_typedefs.push_back( it );
				continue;
			}

			auto	st_it = _structs.find( SearchableStruct{new_name} );
			if ( st_it == _structs.end() )
				continue;

			auto&	info = _extInfo[ ext ];
			info.structs.push_back( st_it );

			if ( not info.feats and EndsWithIC( st_name, ExtToStruct( ext, "Features", tokens[1] )))
			{
				info.feats		= st_it;
				info.featsSType	= FindSType( st_it->data.name );
			}
			else
			if ( not info.props and EndsWithIC( st_name, ExtToStruct( ext, "Properties", tokens[1] )))
			{
				info.props		= st_it;
				info.propsSType	= FindSType( st_it->data.name );
			}
			else
			// some extensions don't duplicate 'Properties' in properties struct,
			// example: VK_AMD_shader_core_properties and VkPhysicalDeviceShaderCorePropertiesAMD
			if ( not info.props and EndsWithIC( ext, "Properties" ) and EndsWithIC( st_name, ExtToStruct( ext, "", tokens[1] )))
			{
				info.props		= st_it;
				info.propsSType	= FindSType( st_it->data.name );
			}
		}

		// another way to find alias
		// EXT to KHR
		// NV, AMD, ARM, etc to EXT, KHR
		for (auto it : defer_structs)
		{
			auto&		st_name		= it->data.name;
			StringView	short_ext	= ExtractExt( st_name );
			auto&		old_ext		= it->data.extension;
			auto		tokens		= ExtToTokens( old_ext );
			String		new_ext		= BuildNewExt( tokens, short_ext );
			auto		ext_it		= _extensions.find( StringView{new_ext} );

			if ( ext_it == _extensions.end() )
			{
				AE_LOGW( "unknown extension '"s << new_ext << "'" );
				continue;
			}

			auto&	info = _extInfo[ ext_it->first ];	// use 'StringView' instead of 'String'
			info.structs.push_back( it );

			if ( EndsWithIC( st_name, ExtToStruct( new_ext, "Features", short_ext )))
			{
				CHECK( not info.feats.has_value() );
				info.feats		= it;
				info.featsSType	= FindSType( st_name );
			}
			else
			if ( EndsWithIC( st_name, ExtToStruct( new_ext, "Properties", short_ext )))
			{
				CHECK( not info.props.has_value() );
				info.props		= it;
				info.propsSType	= FindSType( st_name );
			}
			else
			// some extensions don't duplicate 'Properties' in properties struct,
			// example: VK_AMD_shader_core_properties and VkPhysicalDeviceShaderCorePropertiesAMD
			if ( EndsWithIC( new_ext, "Properties" ) and EndsWithIC( st_name, ExtToStruct( new_ext, "", short_ext )))
			{
				CHECK( not info.props.has_value() );
				info.props		= it;
				info.propsSType	= FindSType( st_name );
			}
		}

		for (auto it : defer_typedefs)
		{
			auto&		old_ext		= it->second.extension;
			auto&		st_name		= it->first;			// 'typedef <st_name>  <new_name>' or 'using <new_name> = <st_name>'
			auto&		new_name	= it->second.dstType;
			StringView	short_ext	= ExtractExt( st_name );
			auto		tokens		= ExtToTokens( old_ext );
			String		new_ext		= BuildNewExt( tokens, short_ext );
			auto		ext_it		= _extensions.find( StringView{new_ext} );

			if ( ext_it == _extensions.end() )
			{
				AE_LOGW( "unknown extension '"s << new_ext << "'" );
				continue;
			}

			auto	st_it = _structs.find( SearchableStruct{new_name} );
			if ( st_it == _structs.end() )
				continue;

			auto&	info = _extInfo[ ext_it->first ];	// use 'StringView' instead of 'String'
			info.structs.push_back( st_it );

			if ( EndsWithIC( st_name, ExtToStruct( new_ext, "Features", short_ext )))
			{
				CHECK( not info.feats.has_value() );
				info.feats		= st_it;
				info.featsSType	= FindSType( st_name );
			}
			else
			if ( EndsWithIC( st_name, ExtToStruct( new_ext, "Properties", short_ext )))
			{
				CHECK( not info.props.has_value() );
				info.props		= st_it;
				info.propsSType	= FindSType( st_name );
			}
			else
			// some extensions don't duplicate 'Properties' in properties struct,
			// example: VK_AMD_shader_core_properties and VkPhysicalDeviceShaderCorePropertiesAMD
			if ( EndsWithIC( new_ext, "Properties" ) and EndsWithIC( st_name, ExtToStruct( new_ext, "", short_ext )))
			{
				CHECK( not info.props.has_value() );
				info.props		= st_it;
				info.propsSType	= FindSType( st_name );
			}
		}

		for (auto it = _enums.begin(); it != _enums.end(); ++it)
		{
			if ( not it->data.extension.empty() )
				_extInfo[ it->data.extension ].enums.push_back( it );
		}

		for (auto it = _bitfields.begin(); it != _bitfields.end(); ++it)
		{
			if ( not it->data.extension.empty() )
				_extInfo[ it->data.extension ].bitfields.push_back( it );
		}
	}

} // AE::Parsers
#endif // VULKAN_HEADER_PATH
