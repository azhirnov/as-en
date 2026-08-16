// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef XR_HEADER_PATH
# include "OpenXRLoaderGen.h"

namespace AE::Parsers
{

/*
=================================================
	GenXrLoaders
=================================================
*/
	bool  OpenXRLoaderGen::GenXrLoaders (const Path &outputFolder, Version2 minVer) const
	{
		struct Group
		{
			FixedArray< String, 16 >	inline_funcs;
			FixedArray< String, 16 >	func_pointers;
			FixedArray< String, 16 >	func_address;
			FixedArray< String, 16 >	dummy_func;
			FixedArray< String, 16 >	loader;
			String						filename;
			StringView					prevExt;
		};

		StaticArray< Group, 2 >			groups;
		StaticArray< StringView, 8 >	prev_exts	= {};
		const auto						feats		= _GetFeatures( minVer );
		HashSet<StringView>				include_fn	= {  };
		HashSet<StringView>				exclude_fn	= { "xrGetInstanceProcAddr" };
		HashSet<StringView>				lib_fn		= { "xrGetInstanceProcAddr", "xrEnumerateApiLayerProperties", "xrEnumerateInstanceExtensionProperties", "xrCreateInstance" };

		for (auto& g : groups)
		{
			g.inline_funcs	.resize( _fileData.size() );
			g.func_pointers	.resize( _fileData.size() );
			g.func_address	.resize( _fileData.size() );
			g.dummy_func	.resize( _fileData.size() );
			g.loader		.resize( _fileData.size() );
		}

		groups[0].filename = "openxr_fn_lib.h";
		groups[1].filename = "openxr_fn_inst.h";

		for (auto& fn : _funcs)
		{
			if ( not EndsWith( fn.data.name, "KHR" ) and
				 not EndsWith( fn.data.name, "EXT" ))
			{
				if ( _funcs.contains( SearchableFunc{ String{fn.data.name} + "KHR" }))
					exclude_fn.insert( fn.data.name );

				if ( _funcs.contains( SearchableFunc{ String{fn.data.name} + "EXT" }))
					exclude_fn.insert( fn.data.name );
			}
		}

		std::map< StringView, Array<FunctionInfo const*> >	fn_per_ext;

		for (auto& fn : _funcs)
		{
			if ( exclude_fn.contains( fn.data.name ))
				continue;

			if ( not (fn.data.extension.empty() or feats.enabledExt.contains( fn.data.extension )) )
			{
				if ( not include_fn.contains( fn.data.name ))
					continue;
			}

			fn_per_ext[ fn.data.extension ].push_back( &fn.data );
		}

		for (auto& [ext, fns] : fn_per_ext)
		{
			for (auto& fn : fns)
			{
				// select group
				uint		gr_idx = 0;
				Group*		gr = null;

				if ( lib_fn.contains( fn->name ))
					gr_idx = 0;
				else
					gr_idx = 1;

				gr = &groups[gr_idx];

				StringView&	prev_ext = prev_exts[ gr_idx + fn->fileIndex*2 ];

				if ( prev_ext != ext )
				{
					String	ext_def;
					if ( not prev_ext.empty() )
						ext_def << "#  endif // " << prev_ext << '\n';

					if ( not ext.empty() )
						ext_def << "#  ifdef " << ext << '\n';

					prev_ext = ext;

					gr->func_pointers[ fn->fileIndex ]	<< ext_def;
					gr->func_address[ fn->fileIndex ]	<< ext_def;
					gr->loader[ fn->fileIndex ]			<< ext_def;
					gr->dummy_func[ fn->fileIndex ]		<< ext_def;
					gr->inline_funcs[ fn->fileIndex ]	<< ext_def;
				}

				// add function pointer decl
				{
					String&	src = gr->func_pointers[ fn->fileIndex ];

					src << "\tstatic PFN_" << fn->name << "  _var_" << fn->name << ";\n";
				}

				// add function pointer
				{
					String&	src = gr->func_address[ fn->fileIndex ];

					src << "\tPFN_" << fn->name << "  OpenXRInstanceFn::_var_" << fn->name << " = null;\n";
				}

				// add function loader
				{
					String&	src = gr->loader[ fn->fileIndex ];

					src << "\tLoad( OUT " << "OpenXRInstanceFn::_var_" << fn->name << ", \"" << fn->name << "\","
						<< " Dummy_" << fn->name << " );\n";
				}


				// add dummy functions
				{
					String&	src = gr->dummy_func[ fn->fileIndex ];

					src << "\tXRAPI_ATTR static ";

					// add result type
					for (auto& type : fn->result.type) {
						src << type << " ";
					}

					// add name
					src << "XRAPI_CALL Dummy_" << fn->name << " (";

					// add arguments to function declaration
					for (auto& arg : fn->args)
					{
						src << (&arg != fn->args.data() ? ", " : "");

						for (auto& type : arg.type) {
							src << type << " ";
						}
					}

					// add body
					src << ")\t\t\t{"
						<< "  XR_LOG( \"used dummy function '" << fn->name << "'\" );"
						<< "  return";

					if ( fn->result.type.size() == 1 and
						 fn->result.type.front() == "void" )
					{}
					else
					{
						bool								is_pointer		= false;
						StructMap_t::const_iterator			struct_info		= _structs.end();
						EnumMap_t::const_iterator			enum_info		= _enums.end();
						BitfieldMap_t::const_iterator		bitfield_info	= _bitfields.end();
						ResourceTypes_t::const_iterator		res_info		= _resourceTypes.end();
						BasicTypeMap_t::const_iterator		basic_info		= _basicTypes.end();

						for (auto& type : fn->result.type)
						{
							if ( not (struct_info	!= _structs.end()		or
									  enum_info		!= _enums.end()			or
									  bitfield_info	!= _bitfields.end()		or
									  res_info		!= _resourceTypes.end()	or
									  basic_info	!= _basicTypes.end()	))
							{
								struct_info		= _structs.find( SearchableStruct{type} );
								enum_info		= _enums.find( SearchableEnum{type} );
								bitfield_info	= _bitfields.find( SearchableBitfield{type} );
								res_info		= _resourceTypes.find( type );
								basic_info		= _basicTypes.find( type );
							}
							is_pointer |= (type == "*");
						}

						if ( is_pointer )
							src << " null";
						else
						if ( res_info != _resourceTypes.end() )
							src << " " << res_info->second.typeName << "(0)";
						else
						if ( enum_info != _enums.end() )
							src << " " << enum_info->data.fields.back().name;
						else
						if ( bitfield_info != _bitfields.end() )
							src << " " << bitfield_info->data.name << "(0)";
						else
						if ( basic_info != _basicTypes.end() )
							src << " " << basic_info->first << "(0)";
						else
						if ( fn->result.type.size() == 1 and
							 fn->result.type[0] == "PFN_xrVoidFunction" )
						{
							src << " null";
						}
						else
							RETURN_ERR( "unknown type" );
					}

					src	<< ";  }\n";
				}


				// add inline function decl
				{
					String&	src = gr->inline_funcs[ fn->fileIndex ];
					src << "\t";

					if ( fn->result.type.size() > 1		or
						 fn->result.type.front() != "void" )
					{
						src << "ND_ ";
					}
					else
						src << "\t";

					src << "XRAPI_ATTR forceinline static ";

					// add result type
					for (auto& type : fn->result.type) {
						src << type << " ";
					}

					// add name
					src << fn->name << " (";

					// add arguments to function declaration
					for (auto& arg : fn->args)
					{
						src << (&arg != fn->args.data() ? ", " : "");

						String	suffix;
						bool	has_suffix = false;

						for (auto& type : arg.type)
						{
							if ( type == "[" )
								has_suffix = true;

							if ( has_suffix )
								suffix << type;
							else
								src << type << " ";
						}
						src << arg.name << suffix;
					}

					src << ") __NE___"
						<< "\t\t\t\t\t\t\t\t{ ";

					if (false) {
						src << "XR_LOG( \"" << fn->name << "\" );  ";
					}

					src << "return " << "_var_" << fn->name << "( ";

					// add arguments to function call
					for (auto& arg : fn->args)
					{
						src << (&arg != fn->args.data() ? ", " : "") << arg.name;
					}
					src << " ); }\n";
				}
			}
		}

		for (uint gr_idx = 0; gr_idx < 2; ++gr_idx)
		{
			Group*	gr = &groups[gr_idx];

			for (usize file_idx = 0; file_idx < gr->func_pointers.size(); ++file_idx)
			{
				auto	prev_ext = prev_exts[ gr_idx + file_idx*2 ];
				if ( prev_ext.empty() )
					continue;

				String	ext_def = "#  endif // "s << prev_ext << '\n';

				gr->func_pointers[ file_idx ]	<< ext_def;
				gr->func_address[ file_idx ]	<< ext_def;
				gr->loader[ file_idx ]			<< ext_def;
				gr->dummy_func[ file_idx ]		<< ext_def;
				gr->inline_funcs[ file_idx ]	<< ext_def;
			}
		}

		// save to file
		for (auto& gr : groups)
		{
			String	src;

			src << "\n#ifdef XRLOADER_STAGE_DECLFNPOINTER\n";
			for (usize i = 0; i < _fileData.size(); ++i)
			{
				if ( gr.func_pointers[i].empty() ) continue;
				if ( i > 0 )				src << "\n# ifdef " << _fileData[i].macro << "\n";
				src << gr.func_pointers[i];
				if ( i > 0 )				src << "# endif // " << _fileData[i].macro << "\n";
			}
			src << "#endif // XRLOADER_STAGE_DECLFNPOINTER\n\n";


			src << "\n#ifdef XRLOADER_STAGE_FNPOINTER\n";
			for (usize i = 0; i < _fileData.size(); ++i)
			{
				if ( gr.func_address[i].empty() ) continue;
				if ( i > 0 )				src << "\n# ifdef " << _fileData[i].macro << "\n";
				src << gr.func_address[i];
				if ( i > 0 )				src << "# endif // " << _fileData[i].macro << "\n";
			}
			src << "#endif // XRLOADER_STAGE_FNPOINTER\n\n";


			src << "\n#ifdef XRLOADER_STAGE_INLINEFN\n";
			for (usize i = 0; i < _fileData.size(); ++i)
			{
				if ( gr.inline_funcs[i].empty() ) continue;
				if ( i > 0 )				src << "\n# ifdef " << _fileData[i].macro << "\n";
				src << gr.inline_funcs[i];
				if ( i > 0 )				src << "# endif // " << _fileData[i].macro << "\n";
			}
			src << "#endif // XRLOADER_STAGE_INLINEFN\n\n";


			src << "\n#ifdef XRLOADER_STAGE_DUMMYFN\n";
			for (usize i = 0; i < _fileData.size(); ++i)
			{
				if ( gr.dummy_func[i].empty() ) continue;
				if ( i > 0 )				src << "\n# ifdef " << _fileData[i].macro << "\n";
				src << gr.dummy_func[i];
				if ( i > 0 )				src << "# endif // " << _fileData[i].macro << "\n";
			}
			src << "#endif // XRLOADER_STAGE_DUMMYFN\n\n";


			src << "\n#ifdef XRLOADER_STAGE_GETADDRESS\n";
			for (usize i = 0; i < _fileData.size(); ++i)
			{
				if ( gr.loader[i].empty() ) continue;
				if ( i > 0 )				src << "\n# ifdef " << _fileData[i].macro << "\n";
				src << gr.loader[i];
				if ( i > 0 )				src << "# endif // " << _fileData[i].macro << "\n";
			}
			src << "#endif // XRLOADER_STAGE_GETADDRESS\n\n";

			FileSystem::CreateDirectory( outputFolder );
			CHECK_ERR( FileSystem::IsDirectory( outputFolder ));

			FileWStream	file{ Path{outputFolder}.append( gr.filename ) };
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Write( src ));
		}
		return true;
	}

/*
=================================================
	_GetFeatures
=================================================
*/

	// defined in 'openxr_platform.h'
	#define XR_KHR_ANDROID_THREAD_SETTINGS_EXTENSION_NAME					"XR_KHR_android_thread_settings"
	#define XR_KHR_ANDROID_SURFACE_SWAPCHAIN_EXTENSION_NAME					"XR_KHR_android_surface_swapchain"
	#define XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME					"XR_KHR_android_create_instance"
	#define XR_KHR_VULKAN_SWAPCHAIN_FORMAT_LIST_EXTENSION_NAME				"XR_KHR_vulkan_swapchain_format_list"
	#define XR_KHR_OPENGL_ENABLE_EXTENSION_NAME								"XR_KHR_opengl_enable"
	#define XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME							"XR_KHR_opengl_es_enable"
	#define XR_KHR_VULKAN_ENABLE_EXTENSION_NAME								"XR_KHR_vulkan_enable"
	#define XR_KHR_D3D11_ENABLE_EXTENSION_NAME								"XR_KHR_D3D11_enable"
	#define XR_KHR_D3D12_ENABLE_EXTENSION_NAME								"XR_KHR_D3D12_enable"
	#define XR_KHR_METAL_ENABLE_EXTENSION_NAME								"XR_KHR_metal_enable"
	#define XR_KHR_WIN32_CONVERT_PERFORMANCE_COUNTER_TIME_EXTENSION_NAME	"XR_KHR_win32_convert_performance_counter_time"
	#define XR_KHR_CONVERT_TIMESPEC_TIME_EXTENSION_NAME						"XR_KHR_convert_timespec_time"
	#define XR_KHR_LOADER_INIT_ANDROID_EXTENSION_NAME						"XR_KHR_loader_init_android"
	#define XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME							"XR_KHR_vulkan_enable2"								// simple initialization for Vulkan
	#define XR_MNDX_EGL_ENABLE_EXTENSION_NAME								"XR_MNDX_egl_enable"
	#define XR_MSFT_PERCEPTION_ANCHOR_INTEROP_EXTENSION_NAME				"XR_MSFT_perception_anchor_interop"
	#define XR_MSFT_HOLOGRAPHIC_WINDOW_ATTACHMENT_EXTENSION_NAME			"XR_MSFT_holographic_window_attachment"
	#define XR_FB_ANDROID_SURFACE_SWAPCHAIN_CREATE_EXTENSION_NAME			"XR_FB_android_surface_swapchain_create"
	#define XR_ML_COMPAT_EXTENSION_NAME										"XR_ML_compat"
	#define XR_OCULUS_AUDIO_DEVICE_GUID_EXTENSION_NAME						"XR_OCULUS_audio_device_guid"
	#define XR_FB_FOVEATION_VULKAN_EXTENSION_NAME							"XR_FB_foveation_vulkan"
	#define XR_FB_SWAPCHAIN_UPDATE_STATE_ANDROID_SURFACE_EXTENSION_NAME		"XR_FB_swapchain_update_state_android_surface"
	#define XR_FB_SWAPCHAIN_UPDATE_STATE_OPENGL_ES_EXTENSION_NAME			"XR_FB_swapchain_update_state_opengl_es"
	#define XR_FB_SWAPCHAIN_UPDATE_STATE_VULKAN_EXTENSION_NAME				"XR_FB_swapchain_update_state_vulkan"
	#define XR_META_VULKAN_SWAPCHAIN_CREATE_INFO_EXTENSION_NAME				"XR_META_vulkan_swapchain_create_info"
	#define XR_ANDROID_ANCHOR_SHARING_EXPORT_EXTENSION_NAME					"XR_ANDROID_anchor_sharing_export"

	OpenXRLoaderGen::FeatureSet  OpenXRLoaderGen::_GetFeatures (Version2 minVer) const
	{
		constexpr Version2	NoVer	= Version2::Max();	// only as extension

		const FeatureInfo	instanceFeatures[] =
		{
		// debugging //
			{ "debugUtils",						XR_EXT_DEBUG_UTILS_EXTENSION_NAME,							NoVer,	{1,0},	{} },

		// vulkan //
			{ "vulkan",							XR_KHR_VULKAN_ENABLE_EXTENSION_NAME,						NoVer,	{1,0},	{} },
			{ "vulkanSwapchainFormatList",		XR_KHR_VULKAN_SWAPCHAIN_FORMAT_LIST_EXTENSION_NAME,			NoVer,	{1,0},	{} },

		// metal //
			{ "metal",							XR_KHR_METAL_ENABLE_EXTENSION_NAME,							NoVer,	{1,0},	{} },

		// khr //

		// ext //

		// android //
		//	XR_ANDROID_ANCHOR_SHARING_EXPORT_EXTENSION_NAME
		//	XR_ANDROID_TRACKABLES_MARKER_EXTENSION_NAME
		//	XR_ANDROID_TRACKABLES_EXTENSION_NAME
		//	XR_ANDROID_DEVICE_ANCHOR_PERSISTENCE_EXTENSION_NAME
		//	XR_ANDROID_PASSTHROUGH_CAMERA_STATE_EXTENSION_NAME
		//	XR_ANDROID_RAYCAST_EXTENSION_NAME
		//	XR_ANDROID_TRACKABLES_OBJECT_EXTENSION_NAME

		// ByteDance //

		// meta/fb/oculus //

		// varjo //
		};

		usize	max_name_len = 0;
		for (auto& feat : instanceFeatures) {
			max_name_len = Max( max_name_len, feat.shortName.size() );
		}
		max_name_len = (max_name_len & 1 ? max_name_len : max_name_len + 1);

		// check that all required extensions are exists and in the correct order
		HashSet<StringView>	existing_ext;

		for (auto& feat : instanceFeatures)
		{
			if ( not feat.extension.empty() )
			{
				existing_ext.insert( feat.extension );

				CHECK_MSG( _extensions.contains( feat.extension ),
					"Extension '"s << feat.extension << "' is not found in headers" );
			}
		}

		for (auto& feat : instanceFeatures) {
			for (auto& ext : feat.requireExts) {
				CHECK_MSG( existing_ext.find( ext ) != existing_ext.end(),
					"Instance extension '"s << feat.extension << "' requires extension '" << ext << "' which is not enabled/exists" );
			}
		}

		FeatureSet	set;
		set.instance.assign( std::begin(instanceFeatures), std::end(instanceFeatures) );
		set.enabledExt	= RVRef(existing_ext);
		set.minVer		= minVer;
		set.maxNameLen	= max_name_len;

		for (auto& feat : set.instance)
		{
			feat.propsType = EPropsType::Instance;

			if ( not feat.extension.empty() and not _extensions.contains( feat.extension ))
				continue; // feature is not exists in headers

			feat.enabled = true;
		}

		return set;
	}

/*
=================================================
	_IsPlatformSpecificExt
=================================================
*/
	bool  OpenXRLoaderGen::_IsPlatformSpecificExt (StringView ext) const
	{
		auto	it = _extensions.find( ext );
		if ( it != _extensions.end() )
			return it->second != 0;

		return false;
	}

/*
=================================================
	_GetFeaturesBoolStruct
=================================================
*/
	String  OpenXRLoaderGen::_GetFeaturesBoolStruct (const FeatureSet &feats) const
	{
		String	str;
		str << "\tstruct XRExtensions\n\t{\n";

		for (auto& feat : feats.instance)
		{
			str << "\t\tbool  " << feat.shortName;
			AppendToString( INOUT str, feats.maxNameLen - feat.shortName.size() );
			str << " : 1;   // " << feat.extension << " \n";
		}

		str << "\n\t\tXRExtensions () { UnsafeZeroMem( *this ); }\n"
			<< "\t};\n";
		return str;
	}

/*
=================================================
	_GetLogFeaturesFunc
=================================================
*/
	String  OpenXRLoaderGen::_GetLogFeaturesFunc (const FeatureSet &feats) const
	{
		String	str;
		str << "\tString  OpenXRDeviceInitializer::_GetExtensionsString () const\n"
			<< "\t{\n"
			<< "\t\tString src;\n"
			<< "\t\tsrc";

		usize	j = 0;
		for (auto& feat : feats.instance)
		{
			++j;

			str << "\n\t\t\t<< \"\\n  " << feat.shortName << ':';
			AppendToString( INOUT str, feat.shortName.size(), feats.maxNameLen, j&1, '.', ' ' );
			str << " \" << ToString( _extensions." << feat.shortName << " )";
		}

		str << ";\n\t\treturn src;\n"
			<< "\t}\n";
		return str;
	}

/*
=================================================
	ExtToName
=================================================
*/
	ND_ static String  ExtToName (StringView name)
	{
		String	res;
		res.reserve( name.size() + 20 );

		for (usize i = 0; i < name.size(); ++i)
		{
			const char	c = name[i];
			if ( i+1 == name.size() )
			{
				const char	p = name[i-1];
				if ( not ((p >= '0' and p <= '9') or p == '_') and (c >= '0' and c < '8') )
					res << '_';
			}
			res << ToUpperCase( c );
		}
		res << "_EXTENSION_NAME";

		return res;
	}

/*
=================================================
	_GetCheckFeaturesFunc
=================================================
*/
	String  OpenXRLoaderGen::_GetCheckFeaturesFunc (const FeatureSet &feats) const
	{
		String	gstr;
		gstr << "\tvoid  OpenXRDeviceInitializer::_CheckExtensions ()\n\t{";

		for (auto& feat : feats.instance)
		{
			if ( not feat.enabled )
				continue;

			CHECK_ERR( not feat.extension.empty() );

			String	ext_name	= ExtToName( feat.extension );
			bool	check_macro	= _IsPlatformSpecificExt( feat.extension );

			String	str;

			if ( check_macro )
				str << "\n\t  #ifdef " << ext_name;

			str << "\n\t\t_extensions." << feat.shortName;
			AppendToString( INOUT str, feats.maxNameLen - feat.shortName.size() );

			str << " = ";
			str << "HasExtension( " << ext_name << " );";

			if ( check_macro )
				str << "\n\t  #endif";

			gstr << str;
		}

		gstr << "\n\t}\n";
		return gstr;
	}

/*
=================================================
	_GetExtensionsListFunc
=================================================
*/
	String  OpenXRLoaderGen::_GetExtensionsListFunc (const FeatureSet &feats) const
	{
		String	str;
		str << "\tArrayView<const char*>  OpenXRDeviceInitializer::_GetExtensions ()\n\t{\n"
			<< "\t\tstatic char const* const  extensions1[] = {";

		for (auto& feat : feats.instance)
		{
			String	ext_name	= ExtToName( feat.extension );
			bool	check_macro	= _IsPlatformSpecificExt( feat.extension );

			if ( check_macro )
				str << "\n\t\t  #ifdef " << ext_name;

			str << "\n\t\t\t" << ext_name << ",";

			if ( check_macro )
				str << "\n\t\t  #endif";
		}
		str << "\n\t\t};"
			<< "\n\t\treturn extensions1;"
			<< "\n\t}\n";

		return str;
	}

/*
=================================================
	GenXrFeatures
=================================================
*/
	bool  OpenXRLoaderGen::GenXrFeatures (const Path &outputFolder, Version2 minVer) const
	{
		const auto	feats = _GetFeatures( minVer );

		String	str;
		str << "#ifdef XRFEATS_STRUCT\n"
			<< _GetFeaturesBoolStruct( feats )
			<< "#endif // XRFEATS_STRUCT\n\n\n";

		str << "#ifdef XRFEATS_FN_DECL\n"
			<< "\tND_ static ArrayView<const char*>  _GetExtensions ();\n"
			<< "\tND_ String  _GetExtensionsString () const;\n"
			<< "\tvoid  _CheckExtensions ();\n"
			<< "#endif // XRFEATS_FN_DECL\n\n\n";

		str << "#ifdef XRFEATS_FN_IMPL\n"
			<< _GetExtensionsListFunc( feats ) << "\n"
			<< _GetCheckFeaturesFunc( feats ) << "\n"
			<< _GetLogFeaturesFunc( feats )
			<< "#endif // XRFEATS_FN_IMPL\n\n";

		const Path		file_name = outputFolder / "xr_features.h";
		FileWStream		file{ file_name };
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( file.Write( str ));
		return true;
	}

/*
=================================================
	GenEnumToString
=================================================
*/
	bool  OpenXRLoaderGen::GenEnumToString (const Path &outputFolder) const
	{
		// TODO
		return true;
	}

} // AE::Parsers

#endif // XR_HEADER_PATH
