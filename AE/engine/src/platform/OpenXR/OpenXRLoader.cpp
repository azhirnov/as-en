// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_OPENXR
# include "platform/OpenXR/OpenXRLoader.h"
# include "openxr/openxr_loader_negotiation.h"

# ifdef AE_PLATFORM_ANDROID
#	define XR_USE_PLATFORM_ANDROID
#	include "platform/Android/GameAppAndroid.h"
#	include "platform/Android/ApplicationAndroid.h"
#	include "openxr/openxr_platform.h"
# endif

# ifdef XR_NO_PROTOTYPES
namespace AE::App
{
	#if 1
	# define XR_LOG( _msg_ )	static struct XrLogCallOnce { XrLogCallOnce() { AE_LOG_DBG( _msg_ ); } } log
	#else
	# define XR_LOG( _msg_ )	AE_LOG_DBG( _msg_ )
	#endif

	#define XRLOADER_STAGE_FNPOINTER
	# include "openxr_fn_lib.h"
	# include "openxr_fn_inst.h"
	#undef  XRLOADER_STAGE_FNPOINTER

	#define XRLOADER_STAGE_DUMMYFN
	# include "openxr_fn_lib.h"
	# include "openxr_fn_inst.h"
	#undef  XRLOADER_STAGE_DUMMYFN

	PFN_xrGetInstanceProcAddr  OpenXRInstanceFn::_var_xrGetInstanceProcAddr = null;

	XRAPI_ATTR static XrResult XRAPI_CALL Dummy_xrGetInstanceProcAddr (XrInstance, const char *, PFN_xrVoidFunction* fn) { XR_LOG( "used dummy function 'xrGetInstanceProcAddr'" );  *fn = null;  return XR_RESULT_MAX_ENUM;  }


/*
=================================================
	OpenXRLib
=================================================
*/
namespace {
	struct OpenXRLib
	{
		Library						module;
		XrInstance					instance			= Default;
		PFN_xrGetInstanceProcAddr	getInstanceProcAddr	= null;
		int							refCounter			= 0;

		ND_ static OpenXRLib&  Instance () __NE___
		{
			static OpenXRLib	lib;
			return lib;
		}
	};
}
/*
=================================================
	InitializeAndroidLoader
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
namespace {
	static bool  InitializeAndroidLoader (const OpenXRLib &xrLib)
	{
		PFN_xrInitializeLoaderKHR	initialize_loader = null;
		if ( xrLib.getInstanceProcAddr != null )
		{
			XrResult  err = xrLib.getInstanceProcAddr( XR_NULL_HANDLE, "xrInitializeLoaderKHR", OUT Cast<PFN_xrVoidFunction>(&initialize_loader) );
			XR_CHECK_ERR( err );
			AE_LOG_DBG( "call xrInitializeLoaderKHR() obtained from xrGetInstanceProcAddr()" );
		}
		else
		{
			CHECK_ERR( xrLib.module.GetProcAddr( "xrInitializeLoaderKHR", OUT initialize_loader ));
			AE_LOG_DBG( "call xrInitializeLoaderKHR() before xrNegotiateLoaderRuntimeInterface" );
		}

		XrLoaderInitInfoAndroidKHR	init_info = {};
		init_info.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;

		CHECK_ERR( OpenXRLoader::GetAndroidVMAndActivity( OUT init_info.applicationVM, OUT init_info.applicationContext ));

		XR_CHECK_ERR( initialize_loader( reinterpret_cast<XrLoaderInitInfoBaseHeaderKHR *>(&init_info) ));
		return true;
	}
}
#endif // AE_PLATFORM_ANDROID
/*
=================================================
	LoadPicoVR
=================================================
*/
#ifdef AE_ENABLE_PICO_OPENXR
namespace {
	static bool  LoadPicoVR (OpenXRLib &xrLib)
	{
		FileRStream	file{"/vendor/etc/openxr/1/active_runtime.json"};
		if ( not file.IsOpen() )
			return false;

		String	str;
		CHECK_ERR( file.Read( file.RemainingSize(), OUT str ));
		AE_LOG_DBG( str );

		const StringView	substr = "\"library_path\": \"";

		usize	begin = str.find( substr );
		CHECK_ERR( begin < str.size() );

		begin += substr.size();
		usize	end = str.find( "\",", begin );
		CHECK_ERR( end < str.size() );

		StringView	lib_name = SubStringBE( str, begin, end );
		AE_LOG_DBG( "XR lib: '"s << lib_name << "'" );

		CHECK_ERR( xrLib.module.Load( lib_name ));

		return InitializeAndroidLoader( xrLib );
	}
}
#endif // AE_ENABLE_PICO_OPENXR
/*
=================================================
	Initialize
----
	must be externally synchronized!
=================================================
*/
	bool  OpenXRLoader::Initialize (NtStringView libName) __NE___
	{
		OpenXRLib&	lib = OpenXRLib::Instance();

		if ( lib.module and lib.refCounter > 0 )
		{
			++lib.refCounter;
			return true;
		}

		if ( not libName.empty() )
			Unused( lib.module.Load( libName ));

	  #ifdef AE_PLATFORM_WINDOWS
		if ( not lib.module  )
			Unused( lib.module.Load( "openxr_loader.dll" ));

		if ( not lib.module  )
		{
			String	path;
			if ( WindowsUtils::ReadRegistry( R"(SOFTWARE\Khronos\OpenXR\1)", "ActiveRuntime", OUT path ))
				Unused( lib.module.Load( Path{path}.replace_extension(".dll") ));
		}
	  #endif

	  #if 0 //defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID)
		if ( not lib.module  )
			Unused( lib.module.Load( "libopenxr_loader.so" ));
	  #endif

	  #ifdef AE_ENABLE_PICO_OPENXR
		if ( not lib.module  )
			LoadPicoVR( lib );
	  #endif

		if ( not lib.module  )
			return false;

	  #ifndef AE_PLATFORM_ANDROID
		// write library path to log
		AE_LOG_DBG( "OpenXR library path: \""s << ToString(lib.module.GetPath()) << '"' );
	  #endif

		OpenXRInstanceFn::_var_xrGetInstanceProcAddr = &Dummy_xrGetInstanceProcAddr;

		bool	direct_load = false;

		// all global functions can be loaded using 'xrGetInstanceProcAddr', so we need to import only this function address.
		if_unlikely( not lib.module.GetProcAddr( "xrGetInstanceProcAddr", OUT lib.getInstanceProcAddr ))
		{
			if ( PFN_xrNegotiateLoaderRuntimeInterface negotiate;
				 lib.module.GetProcAddr( "xrNegotiateLoaderRuntimeInterface", OUT negotiate ))
			{
				XrNegotiateLoaderInfo		loader_info = {};
				loader_info.structType			= XR_LOADER_INTERFACE_STRUCT_LOADER_INFO;
				loader_info.structVersion		= XR_LOADER_INFO_STRUCT_VERSION;
				loader_info.structSize			= sizeof(loader_info);
				loader_info.minInterfaceVersion	= 1;
				loader_info.maxInterfaceVersion	= XR_CURRENT_LOADER_RUNTIME_VERSION;
				loader_info.minApiVersion		= XR_MAKE_VERSION( 1, 0, 0 );
				loader_info.maxApiVersion		= XR_MAKE_VERSION( 1, 0x3ff, 0xfff );

				XrNegotiateRuntimeRequest	runtime_req = {};
				runtime_req.structType		= XR_LOADER_INTERFACE_STRUCT_RUNTIME_REQUEST;
				runtime_req.structVersion	= XR_RUNTIME_INFO_STRUCT_VERSION;
				runtime_req.structSize		= sizeof(runtime_req);

				AE_LOG_DBG( "call xrNegotiateLoaderRuntimeInterface()" );
				XrResult err = negotiate( &loader_info, INOUT &runtime_req );

				if ( XR_SUCCEEDED(err) and runtime_req.getInstanceProcAddr != null )
				{
					lib.getInstanceProcAddr = runtime_req.getInstanceProcAddr;
					direct_load				= true;

					//CHECK( LoadXRLayers( lib ));

					AE_LOGW( "OpenXR driver is loaded directly, validation layers are not attached.\n"
							 "To fix this: use 'openxr_loader' shared library." );
				}
				else
				{
					AE_LOGI( "Failed to negotiate OpenXR api layer." );
					return false;
				}
			}
			else
			{
				AE_LOGI( "'xrGetInstanceProcAddr' is not found" );
				return false;
			}
		}

	  #ifdef AE_PLATFORM_ANDROID
		CHECK_ERR( InitializeAndroidLoader( lib ));
	  #endif

		OpenXRInstanceFn::_var_xrGetInstanceProcAddr = lib.getInstanceProcAddr;

		++lib.refCounter;

		// it is allowed to use null instance handle in 'xrGetInstanceProcAddr'.
		const auto	Load =	[&lib] (OUT auto& outResult, const char* procName, auto dummy)
							{{
								using FN = decltype(dummy);
								PFN_xrVoidFunction	temp = null;
								if ( lib.getInstanceProcAddr( null, procName, OUT &temp ) == XR_SUCCESS )
									outResult = BitCast<FN>( temp );
								else
									outResult = dummy;
							}};

		#define XRLOADER_STAGE_GETADDRESS
		#include "openxr_fn_lib.h"
		#undef  XRLOADER_STAGE_GETADDRESS

		CHECK_ERR( OpenXRInstanceFn::_var_xrCreateInstance != &Dummy_xrCreateInstance );
		CHECK_ERR( OpenXRInstanceFn::_var_xrEnumerateInstanceExtensionProperties != &Dummy_xrEnumerateInstanceExtensionProperties );

		if ( direct_load )
		{
			//OpenXRInstanceFn::_var_xrEnumerateApiLayerProperties = &EnumerateApiLayerProperties;
		}
		return true;
	}

/*
=================================================
	LoadInstance
----
	must be externally synchronized!
	warning: multiple instances are not supported!
=================================================
*/
	bool  OpenXRLoader::LoadInstance (XrInstance instance) __NE___
	{
		OpenXRLib&	lib = OpenXRLib::Instance();

		ASSERT( instance != Default );
		ASSERT( lib.instance == Default or lib.instance == instance );

		if ( lib.getInstanceProcAddr == null )
			return false;

		if ( lib.instance == instance )
			return true;	// functions already loaded for this instance

		lib.instance = instance;

		const auto	Load =	[&lib] (OUT auto& outResult, const char* procName, auto dummy)
							{{
								using FN = decltype(dummy);
								PFN_xrVoidFunction	temp = null;
								if ( lib.getInstanceProcAddr( lib.instance, procName, OUT &temp ) == XR_SUCCESS )
									outResult = BitCast<FN>( temp );
								else
									outResult = dummy;
							}};

		#define XRLOADER_STAGE_GETADDRESS
		#include "openxr_fn_inst.h"
		#undef  XRLOADER_STAGE_GETADDRESS

		return true;
	}

/*
=================================================
	ResetInstance
----
	must be externally synchronized!
=================================================
*/
	void  OpenXRLoader::ResetInstance () __NE___
	{
		OpenXRLib&	lib = OpenXRLib::Instance();
		ASSERT( lib.instance != Default );

		lib.instance = Default;

		const auto	Load =	[] (OUT auto& outResult, const char *, auto dummy) {
								outResult = dummy;
							};

		#define XRLOADER_STAGE_GETADDRESS
		#include "openxr_fn_inst.h"
		#undef  XRLOADER_STAGE_GETADDRESS
	}

/*
=================================================
	Unload
----
	must be externally synchronized!
=================================================
*/
	void  OpenXRLoader::Unload () __NE___
	{
		OpenXRLib&	lib = OpenXRLib::Instance();

		ASSERT( lib.refCounter > 0 );

		if ( (--lib.refCounter) != 0 )
			return;

		lib.module.Unload();
		lib.instance			= null;
		lib.getInstanceProcAddr	= null;

		const auto	Load =	[] (OUT auto& outResult, const char *, auto dummy)
							{{
								outResult = dummy;
							}};

		#define XRLOADER_STAGE_GETADDRESS
		#include "openxr_fn_lib.h"
		#include "openxr_fn_inst.h"
		#undef  XRLOADER_STAGE_GETADDRESS

		OpenXRInstanceFn::_var_xrGetInstanceProcAddr = &Dummy_xrGetInstanceProcAddr;
	}

/*
=================================================
	IsLoaded
----
	must be externally synchronized!
=================================================
*/
	bool  OpenXRLoader::IsLoaded () __NE___
	{
		OpenXRLib&	lib = OpenXRLib::Instance();

		return bool{lib.module};
	}

} // AE::App

# else
namespace AE::App
{
/*
=================================================
	InitializeAndroidLoader
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
namespace {
	static bool  InitializeAndroidLoader ()
	{
		PFN_xrInitializeLoaderKHR	initialize_loader = null;
		XR_CHECK_ERR( xrGetInstanceProcAddr( XR_NULL_HANDLE, "xrInitializeLoaderKHR", OUT Cast<PFN_xrVoidFunction>(&initialize_loader) ));

		XrLoaderInitInfoAndroidKHR	init_info = {};
		init_info.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;

		CHECK_ERR( OpenXRLoader::GetAndroidVMAndActivity( OUT init_info.applicationVM, OUT init_info.applicationContext ));

		XR_CHECK_ERR( initialize_loader( reinterpret_cast<XrLoaderInitInfoBaseHeaderKHR *>(&init_info) ));
		return true;
	}
}
#endif // AE_PLATFORM_ANDROID
/*
=================================================
	OpenXRLoader
=================================================
*/
	bool  OpenXRLoader::Initialize (NtStringView) __NE___
	{
		#ifdef AE_PLATFORM_ANDROID
			CHECK_ERR( InitializeAndroidLoader() );
		#endif

		return true;
	}

	bool  OpenXRLoader::LoadInstance (XrInstance)	__NE___	{ return true; }
	void  OpenXRLoader::ResetInstance ()			__NE___	{}
	void  OpenXRLoader::Unload ()					__NE___	{}
	bool  OpenXRLoader::IsLoaded ()					__NE___	{ return true; }

} // AE::App
# endif // XR_NO_PROTOTYPES
//-----------------------------------------------------------------------------


namespace AE::App
{
/*
=================================================
	GetAndroidVMAndActivity
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
	bool  OpenXRLoader::GetAndroidVMAndActivity (OUT void* &vm, OUT void* &activity) __NE___
	{
		vm			= null;
		activity	= null;

		if ( GameAppAndroid*  game_app = GameAppAndroid::_GetAppInstance())
		{
			android_app*	and_app = static_cast<android_app*>(game_app->_GetAndroidApp());
			CHECK_ERR( and_app != null );

			vm			= and_app->activity->vm;
			activity	= and_app->activity->clazz;

			AE_LOG_DBG( "used GameAppAndroid" );
		}
		else
		if ( ApplicationAndroid*  app = ApplicationAndroid::_GetAppInstance())
		{
			SharedPtr<WindowAndroid>	wnd = app->_GetAppWindow( 0 );
			CHECK_ERR( wnd );

			vm			= JavaEnv::GetJavaVM();
			activity	= wnd->GetNative().activity;

			AE_LOG_DBG( "used ApplicationAndroid" );
		}

		CHECK_ERR( vm != null );
		CHECK_ERR( activity != null );

		return true;
	}
#endif // AE_PLATFORM_ANDROID
/*
=================================================
	_CheckXrErrors
=================================================
*/
	bool  OpenXRInstanceFn::_CheckXrErrors (const XrResult errCode, const char* xrcall, const char* func, const SourceLoc &loc) __NE___
	{
		if_likely( errCode == XR_SUCCESS )
			return true;

	  #ifdef AE_ENABLE_LOGS
		TRY{
			String	msg;
			msg.reserve( 256 );
			msg << "OpenXR error: ";

			switch ( errCode )
			{
				#define CASE( _code_ )	case _code_ :	msg << AE_TOSTRING( _code_ ); break;
				CASE( XR_TIMEOUT_EXPIRED )
				CASE( XR_SESSION_LOSS_PENDING )
				CASE( XR_EVENT_UNAVAILABLE )
				CASE( XR_SPACE_BOUNDS_UNAVAILABLE )
				CASE( XR_SESSION_NOT_FOCUSED )
				CASE( XR_FRAME_DISCARDED )
				CASE( XR_ERROR_VALIDATION_FAILURE )
				CASE( XR_ERROR_RUNTIME_FAILURE )
				CASE( XR_ERROR_OUT_OF_MEMORY )
				CASE( XR_ERROR_API_VERSION_UNSUPPORTED )
				CASE( XR_ERROR_INITIALIZATION_FAILED )
				CASE( XR_ERROR_FUNCTION_UNSUPPORTED )
				CASE( XR_ERROR_FEATURE_UNSUPPORTED )
				CASE( XR_ERROR_EXTENSION_NOT_PRESENT )
				CASE( XR_ERROR_LIMIT_REACHED )
				CASE( XR_ERROR_SIZE_INSUFFICIENT )
				CASE( XR_ERROR_HANDLE_INVALID )
				CASE( XR_ERROR_INSTANCE_LOST )
				CASE( XR_ERROR_SESSION_RUNNING )
				CASE( XR_ERROR_SESSION_NOT_RUNNING )
				CASE( XR_ERROR_SESSION_LOST )
				CASE( XR_ERROR_SYSTEM_INVALID )
				CASE( XR_ERROR_PATH_INVALID )
				CASE( XR_ERROR_PATH_COUNT_EXCEEDED )
				CASE( XR_ERROR_PATH_FORMAT_INVALID )
				CASE( XR_ERROR_PATH_UNSUPPORTED )
				CASE( XR_ERROR_LAYER_INVALID )
				CASE( XR_ERROR_LAYER_LIMIT_EXCEEDED )
				CASE( XR_ERROR_SWAPCHAIN_RECT_INVALID )
				CASE( XR_ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED )
				CASE( XR_ERROR_ACTION_TYPE_MISMATCH )
				CASE( XR_ERROR_SESSION_NOT_READY )
				CASE( XR_ERROR_SESSION_NOT_STOPPING )
				CASE( XR_ERROR_TIME_INVALID )
				CASE( XR_ERROR_REFERENCE_SPACE_UNSUPPORTED )
				CASE( XR_ERROR_FILE_ACCESS_ERROR )
				CASE( XR_ERROR_FILE_CONTENTS_INVALID )
				CASE( XR_ERROR_FORM_FACTOR_UNSUPPORTED )
				CASE( XR_ERROR_FORM_FACTOR_UNAVAILABLE )
				CASE( XR_ERROR_API_LAYER_NOT_PRESENT )
				CASE( XR_ERROR_CALL_ORDER_INVALID )
				CASE( XR_ERROR_GRAPHICS_DEVICE_INVALID )
				CASE( XR_ERROR_POSE_INVALID )
				CASE( XR_ERROR_INDEX_OUT_OF_RANGE )
				CASE( XR_ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED )
				CASE( XR_ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED )
				CASE( XR_ERROR_NAME_DUPLICATED )
				CASE( XR_ERROR_NAME_INVALID )
				CASE( XR_ERROR_ACTIONSET_NOT_ATTACHED )
				CASE( XR_ERROR_ACTIONSETS_ALREADY_ATTACHED )
				CASE( XR_ERROR_LOCALIZED_NAME_DUPLICATED )
				CASE( XR_ERROR_LOCALIZED_NAME_INVALID )
				CASE( XR_ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING )
				CASE( XR_ERROR_RUNTIME_UNAVAILABLE )
				CASE( XR_ERROR_EXTENSION_DEPENDENCY_NOT_ENABLED )
				CASE( XR_ERROR_PERMISSION_INSUFFICIENT )
				CASE( XR_ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR )
				CASE( XR_ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR )
				#undef CASE

				default :	msg << "unknown (" << ToString(int(errCode)) << ')';  break;
			}

			// TODO: xrResultToString

			msg << ", in " << xrcall << ", function: " << func;
			AE_LOGE( msg, loc );
		}
		CATCH_ALL()
	  #else
		Unused( vkcall, func, loc );
	  #endif

		return false;
	}

} // AE::App

#endif // AE_ENABLE_OPENXR
