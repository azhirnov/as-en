// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_OPENXR
# include "platform/OpenXR/OpenXRDevice.h"
# include "platform/OpenXR/NextChain.h"

// platform
# ifdef AE_PLATFORM_ANDROID
#	define XR_USE_PLATFORM_ANDROID
#	include "platform/Android/AndroidCommon.h"
# endif
# ifdef AE_PLATFORM_LINUX
#	define XR_USE_PLATFORM_XCB
#	define XR_USE_PLATFORM_WAYLAND
# endif
# ifdef AE_PLATFORM_WINDOWS
#	define XR_USE_PLATFORM_WIN32
#	include "base/Platforms/WindowsHeader.cpp.h"
# endif

// graphics api
# ifdef AE_ENABLE_VULKAN
#	define XR_USE_GRAPHICS_API_VULKAN
#	include "graphics_rhi/Vulkan/VCommon.h"
# endif
# ifdef AE_ENABLE_METAL
#	define XR_USE_GRAPHICS_API_METAL
#	include "graphics_rhi/Metal/MCommon.h"
# endif

# include "openxr/openxr_platform.h"

# include "graphics_rhi/GraphicsImpl.h"

# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Private/EnumToString.h"

namespace AE::App
{
namespace
{
/*
=================================================
	CopyString
=================================================
*/
	#ifdef AE_COMPILER_MSVC
	#	pragma warning (push, 1)
	#	pragma warning(disable: 4996)
	#endif

	template <usize C>
	static void  CopyString (OUT char (&dst)[C], U8StringView src) __NE___
	{
		ASSERT_Lt( src.length(), C );

		usize	len = Min( C, src.length() );
		strncpy( OUT dst, Cast<char>(src.data()), len );
	}

	#ifdef AE_COMPILER_MSVC
	#	pragma warning (pop)
	#endif

/*
=================================================
	XrObjectTypeToString
=================================================
*/
#  ifdef AE_ENABLE_LOGS
	ND_ static StringView  XrObjectTypeToString (XrObjectType type) __NE___
	{
		switch_enum( type )
		{
			case XR_OBJECT_TYPE_INSTANCE :								return "Instance";
			case XR_OBJECT_TYPE_SESSION :								return "Swapchain";
			case XR_OBJECT_TYPE_SWAPCHAIN :								return "Swapchain";
			case XR_OBJECT_TYPE_SPACE :									return "Space";
			case XR_OBJECT_TYPE_ACTION_SET :							return "ActionSet";
			case XR_OBJECT_TYPE_ACTION :								return "Action";
			case XR_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT :				return "DebugUtils";
			case XR_OBJECT_TYPE_SPATIAL_ANCHOR_MSFT :					return "SpatialAnchor_MSFT";
			case XR_OBJECT_TYPE_SPATIAL_GRAPH_NODE_BINDING_MSFT :		return "SpatialGraphNodeBinding_MSFT";
			case XR_OBJECT_TYPE_HAND_TRACKER_EXT :						return "HandTracker";
			case XR_OBJECT_TYPE_BODY_TRACKER_FB :						return "BodyTracker_FB";
			case XR_OBJECT_TYPE_SCENE_OBSERVER_MSFT :					return "SceneObserver_MSFT";
			case XR_OBJECT_TYPE_SCENE_MSFT :							return "Scene_MSFT";
			case XR_OBJECT_TYPE_FACIAL_TRACKER_HTC :					return "FacialTracker_HTC";
			case XR_OBJECT_TYPE_FOVEATION_PROFILE_FB :					return "FoveationProfile_FB";
			case XR_OBJECT_TYPE_TRIANGLE_MESH_FB :						return "TriangleMesh_FB";
			case XR_OBJECT_TYPE_PASSTHROUGH_FB :						return "Passthrough_FB";
			case XR_OBJECT_TYPE_PASSTHROUGH_LAYER_FB :					return "PassthroughLayer_FB";
			case XR_OBJECT_TYPE_GEOMETRY_INSTANCE_FB :					return "GeometryInstance_FB";
			case XR_OBJECT_TYPE_MARKER_DETECTOR_ML :					return "MarkerDetector_ML";
			case XR_OBJECT_TYPE_EXPORTED_LOCALIZATION_MAP_ML :			return "ExportedLocalizationMap_ML";
			case XR_OBJECT_TYPE_SPATIAL_ANCHORS_STORAGE_ML :			return "SpatialAnchorsStorage_ML";
			case XR_OBJECT_TYPE_SPATIAL_ANCHOR_STORE_CONNECTION_MSFT :	return "SpatialAnchorStoreConnection_MSFT";
			case XR_OBJECT_TYPE_FACE_TRACKER_FB :						return "FaceTracker_FB";
			case XR_OBJECT_TYPE_EYE_TRACKER_FB :						return "EyeTracker_FB";
			case XR_OBJECT_TYPE_VIRTUAL_KEYBOARD_META :					return "VirtualKeyboard_META";
			case XR_OBJECT_TYPE_SPACE_USER_FB :							return "SpaceUser_FB";
			case XR_OBJECT_TYPE_PASSTHROUGH_COLOR_LUT_META :			return "PassthroughColorLUT_META";
			case XR_OBJECT_TYPE_FACE_TRACKER2_FB :						return "FaceTracker2_FB";
			case XR_OBJECT_TYPE_ENVIRONMENT_DEPTH_PROVIDER_META :		return "EnvironmentDepthProvider_META";
			case XR_OBJECT_TYPE_ENVIRONMENT_DEPTH_SWAPCHAIN_META :		return "EnvironmentDepthSwapchain_META";
			case XR_OBJECT_TYPE_PASSTHROUGH_HTC :						return "Passthrough_HTC";
			case XR_OBJECT_TYPE_BODY_TRACKER_HTC :						return "BodyTracker_HTC";
			case XR_OBJECT_TYPE_BODY_TRACKER_BD :						return "BodyTracker_BD";
			case XR_OBJECT_TYPE_PLANE_DETECTOR_EXT :					return "PlaneDetector";
			case XR_OBJECT_TYPE_WORLD_MESH_DETECTOR_ML :				return "WorldMeshDetector_ML";
			case XR_OBJECT_TYPE_FACIAL_EXPRESSION_CLIENT_ML :			return "FacialExpressionClient_ML";

		#ifdef AE_ENABLE_PICO_OPENXR
			case XR_OBJECT_TYPE_EYE_TRACKER_PICO :                      return "EyeTracker_PICO";
			case XR_OBJECT_TYPE_SECURE_MR_FRAMEWORK_PICO :              return "SecureMRFramework_PICO";
			case XR_OBJECT_TYPE_SECURE_MR_PIPELINE_PICO :               return "SecureMRPipeline_PICO";
			case XR_OBJECT_TYPE_SECURE_MR_TENSOR_PICO :                 return "SecureMRTensor_PICO";
		#else
			case XR_OBJECT_TYPE_RENDER_MODEL_EXT :						return "RenderModel";
			case XR_OBJECT_TYPE_RENDER_MODEL_ASSET_EXT :				return "RenderModelAsset";
			case XR_OBJECT_TYPE_SENSE_DATA_PROVIDER_BD :				return "SenseDataProvider_BD";
			case XR_OBJECT_TYPE_SENSE_DATA_SNAPSHOT_BD :				return "SenseDataSnapshot_BD";
			case XR_OBJECT_TYPE_ANCHOR_BD :								return "Anchor_BD";
			case XR_OBJECT_TYPE_TRACKABLE_TRACKER_ANDROID :				return "TrackableTracker_Android";
			case XR_OBJECT_TYPE_DEVICE_ANCHOR_PERSISTENCE_ANDROID :		return "DeviceAnchorPersistence_Android";
			case XR_OBJECT_TYPE_SPATIAL_ENTITY_EXT :					return "SpatialEntity";
			case XR_OBJECT_TYPE_SPATIAL_CONTEXT_EXT :					return "SpatialContext";
			case XR_OBJECT_TYPE_SPATIAL_SNAPSHOT_EXT :					return "SpatialSnapshot";
			case XR_OBJECT_TYPE_SPATIAL_PERSISTENCE_CONTEXT_EXT :		return "SpatialPersistenceContext";
		#endif

			case XR_OBJECT_TYPE_UNKNOWN :
			case XR_OBJECT_TYPE_MAX_ENUM : break;
		}
		switch_end
		return Default;
	}
#	endif

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	OpenXRDevice::OpenXRDevice () __NE___
	{}

/*
=================================================
	destructor
=================================================
*/
	OpenXRDevice::~OpenXRDevice () __NE___
	{
		CHECK( _xrSession == Default );
		CHECK( _xrInstance == Default );
	}

/*
=================================================
	HasExtension
=================================================
*/
	bool  OpenXRDevice::HasExtension (StringView name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		return _extensionNames.contains( ExtensionName_t{name} );
	}

/*
=================================================
	BeginDebugGroup
=================================================
*/
	void  OpenXRDevice::BeginDebugGroup (NtStringView name) C_NE___
	{
	#ifndef AE_CFG_RELEASE
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERRV( _xrSession == Default );

		if ( _extensions.debugUtils )
		{
			XrDebugUtilsLabelEXT	info = {};
			info.type		= XR_TYPE_DEBUG_UTILS_LABEL_EXT;
			info.labelName	= name.c_str();

			XR_CHECK( xrSessionBeginDebugUtilsLabelRegionEXT( _xrSession, &info ));
		}
	#endif
	}

/*
=================================================
	EndDebugGroup
=================================================
*/
	void  OpenXRDevice::EndDebugGroup () C_NE___
	{
	#ifndef AE_CFG_RELEASE
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERRV( _xrSession == Default );

		if ( _extensions.debugUtils )
		{
			XR_CHECK( xrSessionEndDebugUtilsLabelRegionEXT( _xrSession ));
		}
	#endif
	}

/*
=================================================
	InsertLabel
=================================================
*/
	void  OpenXRDevice::InsertLabel (NtStringView label) C_NE___
	{
	#ifndef AE_CFG_RELEASE
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERRV( _xrSession == Default );

		if ( _extensions.debugUtils )
		{
			XrDebugUtilsLabelEXT	info = {};
			info.type		= XR_TYPE_DEBUG_UTILS_LABEL_EXT;
			info.labelName	= label.c_str();

			XR_CHECK( xrSessionInsertDebugUtilsLabelEXT( _xrSession, &info ));
		}
	#endif
	}

/*
=================================================
	SetObjectName
=================================================
*/
	void  OpenXRDevice::SetObjectName (ulong id, NtStringView name, XrObjectType type) C_NE___
	{
	#ifndef AE_CFG_RELEASE
		DRC_SHAREDLOCK( _drCheck );
		CHECK_ERRV( _xrInstance == Default );

		if ( _extensions.debugUtils )
		{
			XrDebugUtilsObjectNameInfoEXT	info = {};
			info.type			= XR_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			info.objectHandle	= id;
			info.objectName		= name.c_str();
			info.objectType		= type;

			XR_CHECK( xrSetDebugUtilsObjectNameEXT( _xrInstance, &info ));
		}
	#endif
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	OpenXRDeviceInitializer::OpenXRDeviceInitializer (Bool enableInfoLog) __NE___ :
		_enableInfoLog{ enableInfoLog }
	{}

/*
=================================================
	destructor
=================================================
*/
	OpenXRDeviceInitializer::~OpenXRDeviceInitializer () __NE___
	{}

/*
=================================================
	_PlatformSpecificSetup
=================================================
*/
	bool  OpenXRDeviceInitializer::_PlatformSpecificSetup (const InstanceCreateInfo &info) __NE___
	{
		#ifndef AE_CFG_RELEASE
		# if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_LINUX)
			// search layers near to executable
			if ( not PlatformUtils::HasEnvironmentVariable( "XR_API_LAYER_PATH" ))
			{
				Path path = PlatformUtils::GetExeLocation();
				CHECK( FileSystem::IsFile( path / "XrApiLayer_core_validation.json" ));

				Unused( PlatformUtils::SetEnvironmentVariable( "XR_API_LAYER_PATH", ToString(path) ));
			}
		# endif
		#endif

		CHECK_ERR( OpenXRLoader::Initialize( info.xrLib ));

		return true;
	}

/*
=================================================
	CreateInstance
=================================================
*/
	bool  OpenXRDeviceInitializer::CreateInstance (const InstanceCreateInfo &info) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _xrInstance == Default );

		CHECK_ERR( _PlatformSpecificSetup( info ));

		Array<const char*>	inst_layers;
		Array<const char*>	inst_extensions;

		TRY{
			inst_layers.assign( info.layers.begin(), info.layers.end() );  // throw
			inst_extensions.assign( info.extensions.begin(), info.extensions.end() );  // throw

			auto	default_ext = _GetExtensions();
			inst_extensions.insert( inst_extensions.end(), default_ext.begin(), default_ext.end() );

			#ifndef AE_CFG_RELEASE
				inst_layers.push_back( "XR_APILAYER_LUNARG_core_validation" );
			#endif
			#ifdef AE_PLATFORM_ANDROID
				inst_extensions.push_back( XR_KHR_ANDROID_CREATE_INSTANCE_EXTENSION_NAME );
			#endif

			_ValidateLayers( INOUT inst_layers, Bool{not _enableInfoLog} );  // throw
			_ValidateExtensions( inst_layers, INOUT inst_extensions, Bool{not _enableInfoLog} );  // throw

		}CATCH_ALL(
			RETURN_ERR( "unexpected failure" );
		)

		AE_LOG_DBG( "xrCreateInstance, version: "s << ToString( info.version.major ) << '.' << ToString( info.version.minor ));

		XrInstanceCreateInfo	inst_info	= {};
		XrApplicationInfo&		app_info	= inst_info.applicationInfo;
		XRNextChain				next		{inst_info};

		// When using the OpenXR API to implement an individual application without a shared engine,
		// the input engineName should be left empty and engineVersion should be set to 0.
		app_info.engineName[0]			= 0;
		app_info.engineVersion			= 0;

		CopyString( OUT app_info.applicationName, info.appName );

		app_info.applicationVersion		= info.appVer;
		app_info.apiVersion				= XR_MAKE_VERSION( info.version.major, info.version.minor, info.version.patch );

		inst_info.type					= XR_TYPE_INSTANCE_CREATE_INFO;
		inst_info.createFlags			= 0;
		inst_info.enabledApiLayerCount	= uint(inst_layers.size());
		inst_info.enabledApiLayerNames	= inst_layers.empty() ? null : inst_layers.data();
		inst_info.enabledExtensionCount	= uint(inst_extensions.size());
		inst_info.enabledExtensionNames	= inst_extensions.empty() ? null : inst_extensions.data();

		#if 1
		{
			String	str {"Requested extensions:\n"};
			for (auto* ext : inst_extensions) {
				str << "  " << ext << '\n';
			}
			AE_LOGI( str );
		}
		#endif

		#ifdef AE_PLATFORM_ANDROID
			XrInstanceCreateInfoAndroidKHR	android_ci = {};
			next.Add( android_ci );
			
			android_ci.type = XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR;
			CHECK_ERR( OpenXRLoader::GetAndroidVMAndActivity( OUT android_ci.applicationVM, OUT android_ci.applicationActivity ));
		#endif

		XR_CHECK_ERR( xrCreateInstance( &inst_info, OUT &_xrInstance ));

		CHECK_ERR( OpenXRLoader::LoadInstance( _xrInstance ));

		_OnCreateInstance( inst_layers, inst_extensions );
		return true;
	}

/*
=================================================
	DestroyInstance
=================================================
*/
	void  OpenXRDeviceInitializer::DestroyInstance () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _xrInstance == Default )
			return;

		CHECK_ERRV( _xrSession == Default );

		DestroyDebugCallback();

		XR_CHECK( xrDestroyInstance( _xrInstance ));

		OpenXRLoader::Unload();

		_extensionNames.clear();
		_xrInstance	= Default;

		if ( _enableInfoLog )
			AE_LOG_DBG( "Destroyed OpenXR instance" );
	}

/*
=================================================
	CreateDebugCallback
=================================================
*/
	bool  OpenXRDeviceInitializer::CreateDebugCallback (XrDebugUtilsMessageSeverityFlagsEXT severity,
														XrDebugUtilsMessageTypeFlagsEXT		types,
														DebugReport_t						callback) __NE___
	{
	#ifndef AE_CFG_RELEASE
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _xrInstance != Default );

		auto	dbg_report = _dbgReport.WriteLock();

		CHECK_ERR( dbg_report->debugUtilsMessenger == Default );

		// TODO: check extension

		XrDebugUtilsMessengerCreateInfoEXT	msg_ci = {};
		msg_ci.type					= XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		msg_ci.messageSeverities	= severity;
		msg_ci.messageTypes			= types;
		msg_ci.userCallback			= &_DebugMessageCallback;
		msg_ci.userData				= this;

		XR_CHECK_ERR( xrCreateDebugUtilsMessengerEXT( _xrInstance, &msg_ci, OUT &dbg_report->debugUtilsMessenger ));

		dbg_report->callback = RVRef(callback);
		return true;
	#else

		AE_LOGW( "OpenXR debug utils should not be used in release build" );
		Unused( severity, types, callback );

	#endif
		return false;
	}

/*
=================================================
	DestroyDebugCallback
=================================================
*/
	void  OpenXRDeviceInitializer::DestroyDebugCallback () __NE___
	{
	#ifndef AE_CFG_RELEASE
		DRC_EXLOCK( _drCheck );

		auto	dbg_report = _dbgReport.WriteLock();

		if ( _xrInstance != Default and dbg_report->debugUtilsMessenger != Default )
		{
			XR_CHECK( xrDestroyDebugUtilsMessengerEXT( dbg_report->debugUtilsMessenger ));
		}

		dbg_report->callback			= {};
		dbg_report->debugUtilsMessenger	= Default;
	#endif
	}

/*
=================================================
	_DebugMessageCallback
=================================================
*/
	XrBool32  OpenXRDeviceInitializer::_DebugMessageCallback (XrDebugUtilsMessageSeverityFlagsEXT			messageSeverity,
															  XrDebugUtilsMessageTypeFlagsEXT				/*messageTypes*/,
															  const XrDebugUtilsMessengerCallbackDataEXT	*callbackData,
															  void*											userData) __NE___
	{
		auto*	self		= static_cast<OpenXRDeviceInitializer *>(userData);
		auto	dbg_report	= self->_dbgReport.WriteLock();

		TRY{
			dbg_report->tempObjectDbgInfos.resize( callbackData->objectCount );	// throw

			for (usize i = 0; i < dbg_report->tempObjectDbgInfos.size(); ++i)
			{
				auto&	obj = callbackData->objects[i];

				dbg_report->tempObjectDbgInfos[i] = { XrObjectTypeToString( obj.objectType ),
													  obj.objectName != null ? StringView{obj.objectName} : StringView{},
													  obj.objectHandle };
			}

			const bool	is_error = dbg_report->breakOnValidationError and AllBits( messageSeverity, XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT );
			//if ( is_error )
			//	self->_hasError.store( true );

			if ( dbg_report->callback )
			{
				dbg_report->callback( DebugReport{ dbg_report->tempObjectDbgInfos, callbackData->message, is_error });
				return XR_FALSE;
			}

		  #ifdef AE_ENABLE_LOGS
			String&	str = dbg_report->tempString;
			str << callbackData->message << '\n';

			for (auto& obj : dbg_report->tempObjectDbgInfos)
			{
				str << "object{ " << obj.type << ", \"" << obj.name << "\", " << ToString(obj.handle) << " }\n";
			}
			str << "----------------------------\n";

			if ( is_error ){
				AE_LOGE( str );
			}else{
				AE_LOGW( str );
			}
		  #endif
		}CATCH_ALL();

		// Applications should always return XR_FALSE so that they see the same behavior with and without validation layers enabled.
		return XR_FALSE;
	}

/*
=================================================
	CreateSession
=================================================
*/
	bool  OpenXRDeviceInitializer::CreateSession () __NE___
	{
		DRC_EXLOCK( _drCheck );

		CHECK_ERR( _xrInstance != Default );
		CHECK_ERR( _xrSystemId == Default );
		CHECK_ERR( _xrSession == Default );

		XrSystemGetInfo			system_gi = {};
		system_gi.type			= XR_TYPE_SYSTEM_GET_INFO;
		system_gi.formFactor	= _formFactor;
		XR_CHECK_ERR( xrGetSystem( _xrInstance, &system_gi, OUT &_xrSystemId ));

		_systemProps.type = XR_TYPE_SYSTEM_PROPERTIES;

		XR_CHECK_ERR( xrGetSystemProperties( _xrInstance, _xrSystemId, OUT &_systemProps ));

	  #ifdef XR_USE_GRAPHICS_API_VULKAN
		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.GetVkDevice() != Default );

		XrGraphicsBindingVulkanKHR		gapi_bindings = {};
		gapi_bindings.type				= XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR;
		gapi_bindings.instance			= dev.GetVkInstance();
		gapi_bindings.physicalDevice	= dev.GetVkPhysicalDevice();
		gapi_bindings.device			= dev.GetVkDevice();
		gapi_bindings.queueFamilyIndex	= 0;	// TODO
		gapi_bindings.queueIndex		= 0;	// TODO
	  #endif

	  #ifdef XR_USE_GRAPHICS_API_METAL
		// TODO
	  #endif

		XrSessionCreateInfo		session_ci = {};
		session_ci.type			= XR_TYPE_SESSION_CREATE_INFO;
		session_ci.next			= &gapi_bindings;
		session_ci.createFlags	= 0;
		session_ci.systemId		= _xrSystemId;

		XR_CHECK_ERR( xrCreateSession( _xrInstance, &session_ci, OUT &_xrSession ));

		if ( _enableInfoLog )
		{
			_LogSession();

			_LogViewConfigurationViews();
			_LogSwapchainFormats();
		}
		return true;
	}

/*
=================================================
	DestroySession
=================================================
*/
	void  OpenXRDeviceInitializer::DestroySession () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _xrSession == Default )
			return;

		XR_CHECK( xrDestroySession( _xrSession ));

		_xrSession	= Default;
		_xrSystemId	= Default;

		if ( _enableInfoLog )
			AE_LOGI( "Destroyed OpenXR session" );
	}

/*
=================================================
	_ValidateLayers
=================================================
*/
	void  OpenXRDeviceInitializer::_ValidateLayers (INOUT Array<const char*> &layers, Bool silent) C_Th___
	{
		if ( layers.empty() )
			return;

		uint	count = 0;
		XR_CHECK_ERRV( xrEnumerateApiLayerProperties( 0, OUT &count, null ));

		Array<XrApiLayerProperties>	supported_layers;
		supported_layers.resize( count );  // throw

		XR_CHECK_ERRV( xrEnumerateApiLayerProperties( count, OUT &count, OUT supported_layers.data() ));
		supported_layers.resize( Min( supported_layers.size(), count ));

	  #if 1
		String	str {"All layers:"};
		for (auto& prop : supported_layers)
		{
			str << "\n  " << prop.layerName
				<< " v" << ToString( prop.layerVersion )
				<< ": " << prop.description;
		}
		AE_LOGI( str );
	  #endif

		// validate
		for (auto it = layers.begin(); it != layers.end();)
		{
			bool		found		= false;
			XrVersion	spec_ver	= 0;
			StringView	req_layer	{*it};

			for (auto& prop : supported_layers)
			{
				if_unlikely( req_layer == prop.layerName )
				{
					found		= true;
					spec_ver	= prop.specVersion;
					break;
				}
			}

			if ( not found )
			{
				if_unlikely( not silent )
					AE_LOG_DBG( "Removed layer '"s << req_layer << "'" );

				it = layers.erase( it );
			}
			else
			{
				// TODO: compare spec version

				++it;
			}
		}
	}

/*
=================================================
	_ValidateExtensions
=================================================
*/
	void  OpenXRDeviceInitializer::_ValidateExtensions (Array<const char*> layers, INOUT Array<const char*> &extensions, Bool silent) C_Th___
	{
		Unused( layers );

		uint	count = 0;
		XR_CHECK_ERRV( xrEnumerateInstanceExtensionProperties( null, 0, OUT &count, null ));

		Array<XrExtensionProperties>	supported_ext;
		supported_ext.resize( count );  // throw

		for (auto& prop : supported_ext) {
			prop.type = XR_TYPE_EXTENSION_PROPERTIES;
		}

		XR_CHECK_ERRV( xrEnumerateInstanceExtensionProperties( null, count, OUT &count, OUT supported_ext.data() ));
		supported_ext.resize( Min( supported_ext.size(), count ));

	  #if 1
		String	str {"All extensions:"};
		for (auto& prop : supported_ext)
		{
			str << "\n  " << prop.extensionName << " v" << ToString( prop.extensionVersion );
		}
		AE_LOGI( str );
	  #endif

		for (auto it = extensions.begin(); it != extensions.end();)
		{
			bool		found		= true;
			XrVersion	spec_ver	= 0;
			StringView	req_ext		{*it};

			for (auto& prop : supported_ext)
			{
				if_unlikely( req_ext == prop.extensionName )
				{
					found		= true;
					spec_ver	= prop.extensionVersion;
					break;
				}
			}

			if ( not found )
			{
				if_unlikely( not silent )
					AE_LOG_DBG( "Removed extension '"s << req_ext << "'" );

				it = extensions.erase( it );
			}
			else
			{
				// TODO: compare spec version

				++it;
			}
		}
	}

/*
=================================================
	_OnCreateInstance
=================================================
*/
	void  OpenXRDeviceInitializer::_OnCreateInstance (ArrayView<const char*> layers, ArrayView<const char*> extensions) __NE___
	{
		_extensionNames.clear();
		for (auto* ext : extensions) {
			_extensionNames.insert( ext );
		}

		_CheckExtensions();

	  #ifdef AE_CFG_RELEASE
		_extensions.debugUtils	= false;
	  #endif

		if ( _enableInfoLog )
		{
			_LogInstance( layers );
		}
	}

/*
=================================================
	_LogInstance
=================================================
*/
	void  OpenXRDeviceInitializer::_LogInstance (ArrayView<const char*> layers) C_NE___
	{
	#ifdef AE_ENABLE_LOGS
		TRY{
			XrInstanceProperties	inst_props = {};
			inst_props.type = XR_TYPE_INSTANCE_PROPERTIES;

			XR_CHECK( xrGetInstanceProperties( _xrInstance, OUT &inst_props ));

			String	str;
			str << "Created OpenXR instance: " << inst_props.runtimeName;
			str << "\n  version:  " << ToString(XR_VERSION_MAJOR( inst_props.runtimeVersion ))
				<< '.' << ToString(XR_VERSION_MINOR( inst_props.runtimeVersion ))
				<< '.' << ToString(XR_VERSION_PATCH( inst_props.runtimeVersion ));

			str << "\nLayers:";
			for (const char* layer : layers) {
				str << "\n  " << layer;
			}

			str << "\nExtensions:"
				<< _GetExtensionsString();

			AE_LOGI( str );
		}
		CATCH_ALL()
	#endif
	}

/*
=================================================
	_LogSession
=================================================
*/
	void  OpenXRDeviceInitializer::_LogSession () C_NE___
	{
	#ifdef AE_ENABLE_LOGS
		TRY{
			String	str;
			str << "Started OpenXR session: " << _systemProps.systemName
				<< "\n  vendor:                  " << ToString( _systemProps.vendorId )
				<< "\n  maxSwapchainImageWidth:  " << ToString( _systemProps.graphicsProperties.maxSwapchainImageWidth )
				<< "\n  maxSwapchainImageHeight: " << ToString( _systemProps.graphicsProperties.maxSwapchainImageHeight )
				<< "\n  maxLayerCount:           " << ToString( _systemProps.graphicsProperties.maxLayerCount )
				<< "\n  orientationTracking:     " << ToString( _systemProps.trackingProperties.orientationTracking )
				<< "\n  positionTracking:        " << ToString( _systemProps.trackingProperties.positionTracking );

			AE_LOGI( str );
		}
		CATCH_ALL()
	#endif
	}

/*
=================================================
	_LogViewConfigurationViews
=================================================
*/
	void  OpenXRDeviceInitializer::_LogViewConfigurationViews () C_NE___
	{
		uint	count = 0;
		XR_CHECK_ERRV( xrEnumerateViewConfigurations( _xrInstance, _xrSystemId, 0, OUT &count, null ));

		Array<XrViewConfigurationType>	types;
		types.resize( count );

		XR_CHECK_ERRV( xrEnumerateViewConfigurations( _xrInstance, _xrSystemId, count, OUT &count, OUT types.data() ));
		types.resize( count );

		Array<XrViewConfigurationView>	views;
		Array<XrEnvironmentBlendMode>	blend_modes;

		String	str {"ViewConfigurations:"};
		for (auto& type : types)
		{
			str << "\n  ";

			switch_enum( type )
			{
				case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_MONO :								str << "PRIMARY_MONO";  break;
				case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO :							str << "PRIMARY_STEREO";  break;
				case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO_WITH_FOVEATED_INSET :		str << "PRIMARY_STEREO_WITH_FOVEATED_INSET";  break;
				case XR_VIEW_CONFIGURATION_TYPE_SECONDARY_MONO_FIRST_PERSON_OBSERVER_MSFT :	str << "SECONDARY_MONO_FIRST_PERSON_OBSERVER_MSFT";  break;
				case XR_VIEW_CONFIGURATION_TYPE_MAX_ENUM :									break;
			  #ifdef AE_ENABLE_PICO_OPENXR
				case XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO_SYMMETRIC_FOV_REPROJECTION_PICO: str << "PRIMARY_STEREO_SYMMETRIC_FOV_REPROJECTION_PICO";  break;
			  #endif
			}
			switch_end

			XR_CHECK_ERRV( xrEnumerateViewConfigurationViews( _xrInstance, _xrSystemId, type, 0, OUT &count, null ));

			views.resize( count );
			for (auto& view : views) {
				view.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
			}

			XR_CHECK_ERRV( xrEnumerateViewConfigurationViews( _xrInstance, _xrSystemId, type, count, OUT &count, OUT views.data() ));

			str << "\n  Views:";
			for (auto& view : views)
			{
				str << "\n    recommendedImageRectWidth:       " << ToString( view.recommendedImageRectWidth )
					<< "\n    recommendedImageRectHeight:      " << ToString( view.recommendedImageRectHeight )
					<< "\n    maxImageRectWidth:               " << ToString( view.maxImageRectWidth )
					<< "\n    maxImageRectHeight:              " << ToString( view.maxImageRectHeight )
					<< "\n    recommendedSwapchainSampleCount: " << ToString( view.recommendedSwapchainSampleCount )
					<< "\n    maxSwapchainSampleCount:         " << ToString( view.maxSwapchainSampleCount );
			}

			XR_CHECK_ERRV( xrEnumerateEnvironmentBlendModes( _xrInstance, _xrSystemId, type, 0, OUT &count, null ));

			blend_modes.resize( count );
			XR_CHECK_ERRV( xrEnumerateEnvironmentBlendModes( _xrInstance, _xrSystemId, type, count, OUT &count, OUT blend_modes.data() ));

			str << "\n  Blend modes:";
			for (auto mode : blend_modes)
			{
				str << "\n    ";
				switch_enum( mode )
				{
					case XR_ENVIRONMENT_BLEND_MODE_OPAQUE :			str << "OPAQUE";  break;
					case XR_ENVIRONMENT_BLEND_MODE_ADDITIVE :		str << "ADDITIVE";  break;
					case XR_ENVIRONMENT_BLEND_MODE_ALPHA_BLEND :	str << "ALPHA_BLEND";  break;
					case XR_ENVIRONMENT_BLEND_MODE_MAX_ENUM :		break;
				}
				switch_end
			}
		}
		AE_LOGI( str );
	}

/*
=================================================
	_LogSwapchainFormats
=================================================
*/
	void  OpenXRDeviceInitializer::_LogSwapchainFormats () C_NE___
	{
		uint	count = 0;
		XR_CHECK_ERRV( xrEnumerateSwapchainFormats( _xrSession, 0, OUT &count, null ));

		Array<int64_t>		formats;
		formats.resize( count );

		XR_CHECK_ERRV( xrEnumerateSwapchainFormats( _xrSession, count, OUT &count, OUT formats.data() ));

		String	str {"Swapchain formats:"};
		for (auto fmt : formats)
		{
			str << "\n  " << ToString( Graphics::AEEnumCast( VkFormat(fmt) ));
		}
		AE_LOGI( str );
	}


#	define XRFEATS_FN_IMPL
#	include "xr_features.h"
#	undef  XRFEATS_FN_IMPL

} // AE::App

#endif // AE_ENABLE_OPENXR
