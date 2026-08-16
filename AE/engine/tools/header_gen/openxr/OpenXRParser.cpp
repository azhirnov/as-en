// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef XR_HEADER_PATH
# include "OpenXRParser.h"

namespace AE::Parsers
{

	Nd__In bool  operator == (HeaderParser::EResourceType lhs, XrObjectType rhs)
	{
		return uint(lhs) == uint(rhs);
	}

/*
=================================================
	constructor
=================================================
*/
	OpenXRParser::OpenXRParser ()
	{
		_macroPrefix	= "XR_";
		_structPrefix	= "Xr";
	}

/*
=================================================
	ParseXrHeaders
=================================================
*/
	bool  OpenXRParser::ParseXrHeaders (const Path &folder)
	{
		CHECK_ERR( FileSystem::IsDirectory( folder ));

		struct XrHeaderFile
		{
			StringView			filename;
			Array<StringView>	enableIfdef;
			Array<StringView>	disableIfdef;
			bool				defaultSkip;
		};

		const XrHeaderFile  file_names[] = {
			{ "openxr.h",			{ "XR_NO_PROTOTYPES" },		{ "OPENXR_H_",			"__cplusplus" },	false },
			{ "openxr_platform.h",	{ "XR_NO_PROTOTYPES" },		{ "OPENXR_PLATFORM_H_",	"__cplusplus" },	false },
		};

		const StringView	skip_funcs [] = {
			"xrVoidFunction"
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
	bool  OpenXRParser::SetFunctionsScope ()
	{
		CHECK_ERR( not _resourceTypes.empty() );

		// TODO
		return true;
	}

/*
=================================================
	BuildBasicTypeMap
=================================================
*/
	bool  OpenXRParser::BuildBasicTypeMap ()
	{
		_basicTypes.clear();
		_basicTypes.insert({ "void",			EBasicType::Void });
		_basicTypes.insert({ "XrBool32",		EBasicType::Bool });
		_basicTypes.insert({ "char",			EBasicType::Char });
		_basicTypes.insert({ "int",				EBasicType::Int });
		_basicTypes.insert({ "int32_t",			EBasicType::Int });
		_basicTypes.insert({ "uint32_t",		EBasicType::UInt });
		_basicTypes.insert({ "uint8_t",			EBasicType::UInt });
		_basicTypes.insert({ "uint16_t",		EBasicType::UInt });
		_basicTypes.insert({ "uint64_t",		EBasicType::ULong });
		_basicTypes.insert({ "int64_t",			EBasicType::Long });
		_basicTypes.insert({ "size_t",			EBasicType::USize });
		_basicTypes.insert({ "float",			EBasicType::Float });
		_basicTypes.insert({ "double",			EBasicType::Double });

		return true;
	}

/*
=================================================
	_AddResourceType
=================================================
*/
	inline void  OpenXRParser::_AddResourceType (const char* name, XrObjectType type, const char* typeName)
	{
		_resourceTypes.insert({ name, { EResourceType(type), typeName }});
	}

/*
=================================================
	BuildResourceTypeMap
=================================================
*/
	bool  OpenXRParser::BuildResourceTypeMap ()
	{
		_resourceTypes.clear();
		_AddResourceType( "XrInstance",							XR_OBJECT_TYPE_INSTANCE,								"XR_OBJECT_TYPE_INSTANCE"								);
		_AddResourceType( "XrSession",							XR_OBJECT_TYPE_SESSION,									"XR_OBJECT_TYPE_SESSION"								);
		_AddResourceType( "XrSwapchain",						XR_OBJECT_TYPE_SWAPCHAIN,								"XR_OBJECT_TYPE_SWAPCHAIN"								);
		_AddResourceType( "XrSpace",							XR_OBJECT_TYPE_SPACE,									"XR_OBJECT_TYPE_SPACE"									);
		_AddResourceType( "XrActionSet",						XR_OBJECT_TYPE_ACTION_SET,								"XR_OBJECT_TYPE_ACTION_SET"								);
		_AddResourceType( "XrAction",							XR_OBJECT_TYPE_ACTION,									"XR_OBJECT_TYPE_ACTION"									);
		_AddResourceType( "XrDebugUtilsMessengerEXT",			XR_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT,				"XR_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT"				);
		_AddResourceType( "XrSpatialAnchorMSFT",				XR_OBJECT_TYPE_SPATIAL_ANCHOR_MSFT,						"XR_OBJECT_TYPE_SPATIAL_ANCHOR_MSFT"					);
		_AddResourceType( "XrSpatialGraphNodeBindingMSFT",		XR_OBJECT_TYPE_SPATIAL_GRAPH_NODE_BINDING_MSFT,			"XR_OBJECT_TYPE_SPATIAL_GRAPH_NODE_BINDING_MSFT"		);
		_AddResourceType( "XrHandTrackerEXT",					XR_OBJECT_TYPE_HAND_TRACKER_EXT,						"XR_OBJECT_TYPE_HAND_TRACKER_EXT"						);
		_AddResourceType( "XrBodyTrackerFB",					XR_OBJECT_TYPE_BODY_TRACKER_FB,							"XR_OBJECT_TYPE_BODY_TRACKER_FB"						);
		_AddResourceType( "XrSceneObserverMSFT",				XR_OBJECT_TYPE_SCENE_OBSERVER_MSFT,						"XR_OBJECT_TYPE_SCENE_OBSERVER_MSFT"					);
		_AddResourceType( "XrSceneMSFT",						XR_OBJECT_TYPE_SCENE_MSFT,								"XR_OBJECT_TYPE_SCENE_MSFT"								);
		_AddResourceType( "XrFacialTrackerHTC",					XR_OBJECT_TYPE_FACIAL_TRACKER_HTC,						"XR_OBJECT_TYPE_FACIAL_TRACKER_HTC"						);
		_AddResourceType( "XrFoveationProfileFB",				XR_OBJECT_TYPE_FOVEATION_PROFILE_FB,					"XR_OBJECT_TYPE_FOVEATION_PROFILE_FB"					);
		_AddResourceType( "XrTriangleMeshFB",					XR_OBJECT_TYPE_TRIANGLE_MESH_FB,						"XR_OBJECT_TYPE_TRIANGLE_MESH_FB"						);
		_AddResourceType( "XrPassthroughFB",					XR_OBJECT_TYPE_PASSTHROUGH_FB,							"XR_OBJECT_TYPE_PASSTHROUGH_FB"							);
		_AddResourceType( "XrPassthroughLayerFB",				XR_OBJECT_TYPE_PASSTHROUGH_LAYER_FB,					"XR_OBJECT_TYPE_PASSTHROUGH_LAYER_FB"					);
		_AddResourceType( "XrGeometryInstanceFB",				XR_OBJECT_TYPE_GEOMETRY_INSTANCE_FB,					"XR_OBJECT_TYPE_GEOMETRY_INSTANCE_FB"					);
		_AddResourceType( "XrMarkerDetectorML",					XR_OBJECT_TYPE_MARKER_DETECTOR_ML,						"XR_OBJECT_TYPE_MARKER_DETECTOR_ML"						);
		_AddResourceType( "XrExportedLocalizationMapML",		XR_OBJECT_TYPE_EXPORTED_LOCALIZATION_MAP_ML,			"XR_OBJECT_TYPE_EXPORTED_LOCALIZATION_MAP_ML"			);
		_AddResourceType( "XrSpatialAnchorsStorageML",			XR_OBJECT_TYPE_SPATIAL_ANCHORS_STORAGE_ML,				"XR_OBJECT_TYPE_SPATIAL_ANCHORS_STORAGE_ML"				);
		_AddResourceType( "XrSpatialAnchorStoreConnectionMSFT",	XR_OBJECT_TYPE_SPATIAL_ANCHOR_STORE_CONNECTION_MSFT,	"XR_OBJECT_TYPE_SPATIAL_ANCHOR_STORE_CONNECTION_MSFT"	);
		_AddResourceType( "XrFaceTrackerFB",					XR_OBJECT_TYPE_FACE_TRACKER_FB,							"XR_OBJECT_TYPE_FACE_TRACKER_FB"						);
		_AddResourceType( "XrEyeTrackerFB",						XR_OBJECT_TYPE_EYE_TRACKER_FB,							"XR_OBJECT_TYPE_EYE_TRACKER_FB"							);
		_AddResourceType( "XrVirtualKeyboardMETA",				XR_OBJECT_TYPE_VIRTUAL_KEYBOARD_META,					"XR_OBJECT_TYPE_VIRTUAL_KEYBOARD_META"					);
		_AddResourceType( "XrSpaceUserFB",						XR_OBJECT_TYPE_SPACE_USER_FB,							"XR_OBJECT_TYPE_SPACE_USER_FB"							);
		_AddResourceType( "XrPassthroughColorLutMETA",			XR_OBJECT_TYPE_PASSTHROUGH_COLOR_LUT_META,				"XR_OBJECT_TYPE_PASSTHROUGH_COLOR_LUT_META"				);
		_AddResourceType( "XrFaceTracker2FB",					XR_OBJECT_TYPE_FACE_TRACKER2_FB,						"XR_OBJECT_TYPE_FACE_TRACKER2_FB"						);
		_AddResourceType( "XrEnvironmentDepthProviderMETA",		XR_OBJECT_TYPE_ENVIRONMENT_DEPTH_PROVIDER_META,			"XR_OBJECT_TYPE_ENVIRONMENT_DEPTH_PROVIDER_META"		);
		_AddResourceType( "XrEnvironmentDepthSwapchainMETA",	XR_OBJECT_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_META,		"XR_OBJECT_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_META"		);
		_AddResourceType( "XrRenderModelEXT",					XR_OBJECT_TYPE_RENDER_MODEL_EXT,						"XR_OBJECT_TYPE_RENDER_MODEL_EXT"						);
		_AddResourceType( "XrRenderModelAssetEXT",				XR_OBJECT_TYPE_RENDER_MODEL_ASSET_EXT,					"XR_OBJECT_TYPE_RENDER_MODEL_ASSET_EXT"					);
		_AddResourceType( "XrPassthroughHTC",					XR_OBJECT_TYPE_PASSTHROUGH_HTC,							"XR_OBJECT_TYPE_PASSTHROUGH_HTC"						);
		_AddResourceType( "XrBodyTrackerHTC",					XR_OBJECT_TYPE_BODY_TRACKER_HTC,						"XR_OBJECT_TYPE_BODY_TRACKER_HTC"						);
		_AddResourceType( "XrBodyTrackerBD",					XR_OBJECT_TYPE_BODY_TRACKER_BD,							"XR_OBJECT_TYPE_BODY_TRACKER_BD"						);
		_AddResourceType( "XrSenseDataProviderBD",				XR_OBJECT_TYPE_SENSE_DATA_PROVIDER_BD,					"XR_OBJECT_TYPE_SENSE_DATA_PROVIDER_BD"					);
		_AddResourceType( "XrSenseDataSnapshotBD",				XR_OBJECT_TYPE_SENSE_DATA_SNAPSHOT_BD,					"XR_OBJECT_TYPE_SENSE_DATA_SNAPSHOT_BD"					);
		_AddResourceType( "XrAnchorBD",							XR_OBJECT_TYPE_ANCHOR_BD,								"XR_OBJECT_TYPE_ANCHOR_BD"								);
		_AddResourceType( "XrPlaneDetectorEXT",					XR_OBJECT_TYPE_PLANE_DETECTOR_EXT,						"XR_OBJECT_TYPE_PLANE_DETECTOR_EXT"						);
		_AddResourceType( "XrTrackableTrackerANDROID",			XR_OBJECT_TYPE_TRACKABLE_TRACKER_ANDROID,				"XR_OBJECT_TYPE_TRACKABLE_TRACKER_ANDROID"				);
		_AddResourceType( "XrDeviceAnchorPersistenceANDROID",	XR_OBJECT_TYPE_DEVICE_ANCHOR_PERSISTENCE_ANDROID,		"XR_OBJECT_TYPE_DEVICE_ANCHOR_PERSISTENCE_ANDROID"		);
		_AddResourceType( "XrWorldMeshDetectorML",				XR_OBJECT_TYPE_WORLD_MESH_DETECTOR_ML,					"XR_OBJECT_TYPE_WORLD_MESH_DETECTOR_ML"					);
		_AddResourceType( "XrFacialExpressionClientML",			XR_OBJECT_TYPE_FACIAL_EXPRESSION_CLIENT_ML,				"XR_OBJECT_TYPE_FACIAL_EXPRESSION_CLIENT_ML"			);
		_AddResourceType( "XrSpatialEntityEXT",					XR_OBJECT_TYPE_SPATIAL_ENTITY_EXT,						"XR_OBJECT_TYPE_SPATIAL_ENTITY_EXT"						);
		_AddResourceType( "XrSpatialContextEXT",				XR_OBJECT_TYPE_SPATIAL_CONTEXT_EXT,						"XR_OBJECT_TYPE_SPATIAL_CONTEXT_EXT"					);
		_AddResourceType( "XrSpatialSnapshotEXT",				XR_OBJECT_TYPE_SPATIAL_SNAPSHOT_EXT,					"XR_OBJECT_TYPE_SPATIAL_SNAPSHOT_EXT"					);
		_AddResourceType( "XrSpatialPersistenceContextEXT",		XR_OBJECT_TYPE_SPATIAL_PERSISTENCE_CONTEXT_EXT,			"XR_OBJECT_TYPE_SPATIAL_PERSISTENCE_CONTEXT_EXT"		);

		// TODO: validation
		return true;
	}

/*
=================================================
	_BuildExtensionInfo
=================================================
*/
	void  OpenXRParser::_BuildExtensionInfo ()
	{
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
			CHECK( tokens[0] == "XR" );
			return tokens;
		}};


		Array< StructMap_t::const_iterator >		defer_structs;
		Array< StructTypedefs_t::const_iterator >	defer_typedefs;

		for (auto it = _structs.begin(); it != _structs.end(); ++it)
		{
			auto&	ext		= it->data.extension;
			auto&	st_name	= it->data.name;

			if ( ext.empty() )
				continue;

			//if ( skip_ext.contains( ext ))
			//	continue;

			Array<StringView>	tokens = ExtToTokens( ext );

			if ( not EndsWith( st_name, tokens[1] ))
			{
				defer_structs.push_back( it );
				continue;
			}

			auto&	info = _extInfo[ ext ];
			info.structs.push_back( it );
		}

		// find alias with feature and properties for each extension
		for (auto it = _typedefs.begin(); it != _typedefs.end(); ++it)
		{
			auto&	ext			= it->second.extension;
			auto&	st_name		= it->first;			// 'typedef <st_name>  <new_name>' or 'using <new_name> = <st_name>'
			auto&	new_name	= it->second.dstType;

			if ( ext.empty() )
				continue;

			//if ( skip_ext.contains( ext ))
			//	continue;

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

#endif // XR_HEADER_PATH
