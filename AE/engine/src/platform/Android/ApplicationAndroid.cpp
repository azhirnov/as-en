// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_ANDROID
# include "graphics_rhi/Vulkan/VSwapchain.h"
# include "platform/Android/FileSystemAndroid.h"
# include "platform/Android/ApplicationAndroid.h"

// must be implemented in client code
extern "C" JNIEXPORT jint  JNI_OnLoad   (JavaVM* vm, void *);
extern "C" JNIEXPORT void  JNI_OnUnload (JavaVM* vm, void *);

// to trigger link errors
namespace {
	static const size_t	s_JNI_ptr = size_t(&JNI_OnLoad) + size_t(&JNI_OnUnload);
}

namespace AE::Base {
	extern "C" void AE_DLL_EXPORT Android_SetIsUnderDebugger (bool value);
}

namespace AE::App
{
	INTERNAL_LINKAGE( Unique<ApplicationAndroid>  s_AndApp )

/*
=================================================
	GetApp
=================================================
*/
namespace {
	ND_ static ApplicationAndroid&  GetApp () __NE___
	{
		ASSERT( s_AndApp );
		return *s_AndApp;
	}
}
/*
=================================================
	_GetAppInstance
=================================================
*/
	ApplicationAndroid*  ApplicationAndroid::_GetAppInstance () __NE___
	{
		return s_AndApp.get();
	}

/*
=================================================
	constructor
=================================================
*/
	ApplicationAndroid::ApplicationAndroid (Unique<IAppListener> listener) __NE___ :
		ApplicationBase{ RVRef(listener) }
	{
		Unused( s_JNI_ptr );
	}

/*
=================================================
	destructor
=================================================
*/
	ApplicationAndroid::~ApplicationAndroid () __NE___
	{
		_OnDestroy();

		//ASSERT( _hwCamera.release().use_count() <= 1 );
	}

/*
=================================================
	CreateWindow
=================================================
*/
	WindowPtr  ApplicationAndroid::CreateWindow (WndListenerPtr listener, const WindowDesc &desc, IInputActions* dstActions) __NE___
	{
		CHECK_ERR( listener );

		DRC_EXLOCK( _stCheck );

		WindowAndroid*	activity = null;

		if ( desc.androidWndId != UMax )
		{
			// search by id
			for (auto& [id, wnd] : _andWindows)
			{
				if ( desc.androidWndId == id )
				{
					activity = wnd.get();
					break;
				}
			}
		}
		else
		{
			// find any window
			for (auto& [id, wnd] : _andWindows)
			{
				if ( not wnd->_listener )
				{
					activity = wnd.get();
					break;
				}
			}
		}

		CHECK_ERR( activity != null );
		CHECK_ERR( not activity->_listener );  // already attached to engine window

		activity->_Init( RVRef(listener), desc, dstActions );
		return WindowPtr{activity};
	}

/*
=================================================
	OpenStorage
=================================================
*/
	RC<IVirtualFileStorage>  ApplicationAndroid::OpenStorage (EAppStorage type) __NE___
	{
		switch_enum( type )
		{
			case EAppStorage::Builtin :
			{
				auto	fs = MakeRC<FileSystemAndroid>();
				CHECK_ERR( fs->Create( _paths->jniAssetMngr, "" ));
				return fs;
			}

			case EAppStorage::Cache :
			{
				Path	dir = _paths->internalCache;
				CHECK_ERR( not dir.empty() );
				return VFS::VirtualFileStorageFactory::CreateDynamicFolder( dir );
			}

			case EAppStorage::ExternalCache :
			{
				Path	dir = _paths->externalCache;
				CHECK_ERR( not dir.empty() );
				return VFS::VirtualFileStorageFactory::CreateDynamicFolder( dir );
			}
		}
		switch_end
		return null;
	}

/*
=================================================
	GetStoragePath
=================================================
*/
	Path  ApplicationAndroid::GetStoragePath (EAppStorage type) __NE___
	{
		switch_enum( type )
		{
			case EAppStorage::Builtin :			return {};	// not supported, use 'OpenStorage()'
			case EAppStorage::Cache :			return _paths->internalCache;
			case EAppStorage::ExternalCache :	return _paths->externalCache;
		}
		switch_end
		return {};
	}

/*
=================================================
	GetMonitors
=================================================
*/
	IApplication::MonitorsView_t  ApplicationAndroid::GetMonitors (bool update) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _stCheck );		// for compatibility
		Unused( update );

		return MonitorsView_t{ &_displayInfo, 1 };
	}

	IApplication::MonitorsView_t  ApplicationAndroid::GetCachedMonitors () C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _stCheck );		// for compatibility

		return MonitorsView_t{ &_displayInfo, 1 };
	}

/*
=================================================
	GetVulkanInstanceExtensions
=================================================
*/
	ArrayView<const char*>  ApplicationAndroid::GetVulkanInstanceExtensions () __NE___
	{
		return Graphics::VSwapchain::GetInstanceExtensions();
	}

/*
=================================================
	Terminate
=================================================
*/
	void  ApplicationAndroid::Terminate () __NE___
	{
		DRC_EXLOCK( _stCheck );

		_isRunning.store( false );

		for (auto& obj_wnd : _andWindows)
		{
			obj_wnd.second->Close();
		}
	}

/*
=================================================
	BeforeUpdate
=================================================
*/
	void  ApplicationAndroid::BeforeUpdate () __NE___
	{
		DRC_EXLOCK( _stCheck );

		ApplicationBase::_BeforeUpdate();
	}

/*
=================================================
	AfterUpdate
=================================================
*/
	void  ApplicationAndroid::AfterUpdate () __NE___
	{
		DRC_EXLOCK( _stCheck );

		for (usize i = 0; i < _andWindows.size();)
		{
			if_likely( _andWindows[i].second->_wndState != IWindow::EState::Destroyed )
			{
				++i;
			}
			else
			{
				_andWindows.fast_erase( i );
				_windows.fast_erase( i );
			}
		}

		ApplicationBase::_AfterUpdate();

		if_unlikely( _andWindows.empty() )
		{
			_OnDestroy();
		}
	}

/*
=================================================
	_OnDestroy
=================================================
*/
	void  ApplicationAndroid::_OnDestroy () __NE___
	{
		DRC_EXLOCK( _drCheck );

		ApplicationBase::_Destroy();

		_java.application		= Default;
		_java.assetManager		= Default;
		_paths->jniAssetMngr	= null;

		//_methods.createWindow	= Default;

		// windows must be destroyed before destroying app
		for (auto& obj_wnd : _andWindows)
		{
			CHECK( obj_wnd.second->_wndState == IWindow::EState::Destroyed );
			CHECK( obj_wnd.second.use_count() == 1 );
		}

		_andWindows.clear();
		_windows.clear();
	}

/*
=================================================
	_AddAndroidWindow
=================================================
*/
	ApplicationAndroid::WinID  ApplicationAndroid::_AddAndroidWindow (SharedPtr<WindowAndroid> wnd) __NE___
	{
		_AddWindow( RVRef(wnd) );

		WinID	id = _andWindows.back().first;

		if ( not _started and _listener )
		{
			_started = true;
			_listener->OnStart( *this );
		}

		return id;
	}

/*
=================================================
	_AddWindow
=================================================
*/
	void  ApplicationAndroid::_AddWindow (SharedPtr<WindowBase> wnd) __NE___
	{
		WinID	id = _windowCounter;
		_andWindows.emplace_back( id, Cast<WindowAndroid>(wnd) );
		_windows.push_back( wnd );

		++_windowCounter;

		ASSERT_Eq( _andWindows.size(), _windows.size() );
	}

/*
=================================================
	SetRotation
=================================================
*/
	void  ApplicationAndroid::SetRotation (int orientation) __NE___
	{
		DRC_EXLOCK( _drCheck );

		enum {
			ROTATION_0	 = 0x00,
			ROTATION_90	 = 0x01,
			ROTATION_180 = 0x02,
			ROTATION_270 = 0x03,
		};
		switch ( orientation )
		{
			case ROTATION_0   :		_displayInfo.orient = Monitor::EOrientation::Deg_0;		break;
			case ROTATION_90  :		_displayInfo.orient = Monitor::EOrientation::Deg_90;	break;
			case ROTATION_180 :		_displayInfo.orient = Monitor::EOrientation::Deg_180;	break;
			case ROTATION_270 :		_displayInfo.orient = Monitor::EOrientation::Deg_270;	break;
		}
	}

/*
=================================================
	ShowToast
=================================================
*/
	void  ApplicationAndroid::ShowToast (NtStringView msg, bool longTime) __NE___
	{
		DRC_EXLOCK( _stCheck );
		DRC_EXLOCK( _drCheck );

		_methods.showToast( Java::JavaString{ msg }.Get(), longTime );
	}

/*
=================================================
	native_OnCreate
=================================================
*/
	void JNICALL ApplicationAndroid::native_OnCreate (JNIEnv* env, jclass, jobject appCtx, jobject assetMngr, jboolean isUnderDebugger) __NE___
	{
		auto&	app = GetApp();
		DRC_EXLOCK( app._drCheck );

		JavaEnv	je{ env };

		app._java.application		= JavaObj{ appCtx, je };
		app._java.assetManager		= JavaObj{ assetMngr, je };
		app._paths->jniAssetMngr	= AAssetManager_fromJava( env, app._java.assetManager.Get() );

		app._java.application.Method( "ShowToast",			OUT app._methods.showToast );
		app._java.application.Method( "IsNetworkConnected",	OUT app._methods.isNetworkConnected );
		//app._java.application.Method( "CreateWindow",		OUT app._methods.createWindow );

		Base::Android_SetIsUnderDebugger( isUnderDebugger );
		ASSERT( isUnderDebugger == AndroidUtils::IsUnderDebugger() );

		if ( isUnderDebugger )
		{
			AE_LOGW( ">>> Enabled Android debugging <<<" );
			StaticLogger::AddLogger( ILogger::CreateBreakOnError() );
		}
	}

/*
=================================================
	native_SetDirectories
=================================================
*/
	void JNICALL ApplicationAndroid::native_SetDirectories (JNIEnv*, jclass,
															jstring internalAppData, jstring internalCache,
															jstring externalAppData, jstring externalCache) __NE___
	{
		auto&	app = GetApp();
		DRC_EXLOCK( app._drCheck );

		app._paths->internalAppData	= Path{JavaString{ internalAppData }};
		app._paths->internalCache	= Path{JavaString{ internalCache }};
		app._paths->externalAppData	= Path{JavaString{ externalAppData }};
		app._paths->externalCache	= Path{JavaString{ externalCache }};
	}

/*
=================================================
	native_SetDisplayInfo
=================================================
*/
	void JNICALL  ApplicationAndroid::native_SetDisplayInfo (JNIEnv* env, jclass,
															 jint minWidth, jint minHeight,
															 jint maxWidth, jint maxHeight,
															 float dpi, jint refreshRate, jint orientation,
															 float avrLum, float maxLum, float minLum,
															 jintArray cutoutRects, const jint cutoutRectCount) __NE___
	{
		auto&	app = GetApp();
		DRC_EXLOCK( app._drCheck );

		auto&	disp = app._displayInfo;

		disp.workArea.pixels	= RectI{ 0, 0, minWidth, minHeight };
		disp.region.pixels		= RectI{ 0, 0, maxWidth, maxHeight };
		disp.ppi				= dpi;
		disp.physicalSize		= disp._CalculatePhysicalSize();
		disp.freq				= refreshRate;
		app.SetRotation( orientation );

		disp.hdr.luminance.avr	= HDRConfig::Luminance_t{ avrLum };
		disp.hdr.luminance.max	= HDRConfig::Luminance_t{ maxLum };
		disp.hdr.luminance.min	= HDRConfig::Luminance_t{ minLum };

		if ( cutoutRectCount > 0 )
		{
			ASSERT( IsMultipleOf( cutoutRectCount, 4 ));

			JavaArray<jint>	cutout_rects { cutoutRects, True{"readOnly"}, JavaEnv{env} };

			for (jint i = 0; i+3 < cutoutRectCount; i += 4)
			{
				auto&	dst			= disp.cutout.emplace_back();
				dst.pixels.left		= cutout_rects[i+0];
				dst.pixels.top		= cutout_rects[i+1];
				dst.pixels.right	= cutout_rects[i+2];
				dst.pixels.bottom	= cutout_rects[i+3];
			}
		}

		DEBUG_ONLY( disp.Print() );
	}

/*
=================================================
	native_SetSystemInfo
=================================================
*/
	void JNICALL  ApplicationAndroid::native_SetSystemInfo (JNIEnv*, jclass, jstring iso3Lang0, jstring iso3Lang1) __NE___
	{
		auto&	app = GetApp();
		DRC_EXLOCK( app._drCheck );

		app._locales.clear();
		app._locales.push_back( LocaleName{ JavaString{ iso3Lang0 }.c_str() });
		app._locales.push_back( LocaleName{ JavaString{ iso3Lang1 }.c_str() });
	}

/*
=================================================
	native_SetSystemInfo
=================================================
*/
	void JNICALL  ApplicationAndroid::native_EnableCamera (JNIEnv*, jclass) __NE___
	{
		auto&	app = GetApp();
		DRC_EXLOCK( app._drCheck );

		/*if ( not app._hwCamera.load() )
		{
			auto	camera = MakeRC<HwCameraAndroid>();
			CHECK_ERRV( camera->Initialize() );

			app._hwCamera.store( RVRef(camera) );
			app._listener->OnEvent( app, EAppEvent::CameraEnabled );
		}*/
	}

/*
=================================================
	OnJniLoad
=================================================
*/
	jint ApplicationAndroid::OnJniLoad (JavaVM* vm) __NE___
	{
		using namespace AE::Java;

		JNIEnv* env;
		if ( vm->GetEnv( OUT reinterpret_cast<void**>(&env), JavaEnv::Version ) != JNI_OK )
			return -1;

		JavaEnv::SetVM( vm );

		const int	argc	= 1;
		const char*	argv[]	= { "" };

		s_AndApp.reset( new ApplicationAndroid{ AE_OnAppCreated( argc, argv )});

		AE_LOGI( "Started java application" );

		// register application native methods
		{
			JavaClass	app_class{ "AE/engine/BaseApplication" };
			CHECK_ERR( app_class, -1 );

			app_class.RegisterStaticMethod( "native_OnCreate",			&ApplicationAndroid::native_OnCreate );
			app_class.RegisterStaticMethod( "native_SetDirectories",	&ApplicationAndroid::native_SetDirectories );
			app_class.RegisterStaticMethod( "native_SetDisplayInfo",	&ApplicationAndroid::native_SetDisplayInfo );
			app_class.RegisterStaticMethod( "native_SetSystemInfo",		&ApplicationAndroid::native_SetSystemInfo );
			app_class.RegisterStaticMethod( "native_EnableCamera",		&ApplicationAndroid::native_EnableCamera );
		}

		// register activity native methods
		{
			JavaClass	wnd_class{ "AE/engine/BaseActivity" };
			CHECK_ERR( wnd_class, -1 );

			wnd_class.RegisterStaticMethod( "native_OnCreate",				&WindowAndroid::native_OnCreate );
			wnd_class.RegisterStaticMethod( "native_OnDestroy",				&WindowAndroid::native_OnDestroy );
			wnd_class.RegisterStaticMethod( "native_OnStart",				&WindowAndroid::native_OnStart );
			wnd_class.RegisterStaticMethod( "native_OnStop",				&WindowAndroid::native_OnStop );
			wnd_class.RegisterStaticMethod( "native_OnEnterForeground",		&WindowAndroid::native_OnEnterForeground );
			wnd_class.RegisterStaticMethod( "native_OnEnterBackground",		&WindowAndroid::native_OnEnterBackground );
			wnd_class.RegisterStaticMethod( "native_SurfaceChanged",		&WindowAndroid::native_SurfaceChanged );
			wnd_class.RegisterStaticMethod( "native_SurfaceDestroyed",		&WindowAndroid::native_SurfaceDestroyed );
			wnd_class.RegisterStaticMethod( "native_Update",				&WindowAndroid::native_Update );
			wnd_class.RegisterStaticMethod( "native_OnKey",					&WindowAndroid::native_OnKey );
			wnd_class.RegisterStaticMethod( "native_OnTouch",				&WindowAndroid::native_OnTouch );
			wnd_class.RegisterStaticMethod( "native_OnOrientationChanged",	&WindowAndroid::native_OnOrientationChanged );
			wnd_class.RegisterStaticMethod( "native_UpdateSensor",			&WindowAndroid::native_UpdateSensor );
			wnd_class.RegisterStaticMethod( "native_SendBatteryStat1",		&WindowAndroid::native_SendBatteryStat1 );
			wnd_class.RegisterStaticMethod( "native_SendBatteryStat2",		&WindowAndroid::native_SendBatteryStat2 );
		}

		return JavaEnv::Version;
	}

/*
=================================================
	OnJniUnload
=================================================
*/
	void ApplicationAndroid::OnJniUnload (JavaVM* vm) __NE___
	{
		using namespace AE::Java;

		s_AndApp.reset();

		AE_OnAppDestroyed();

		JavaEnv::SetVM( null );
	}

} // AE::App

#endif // AE_PLATFORM_ANDROID
