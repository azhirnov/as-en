// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_ANDROID
# include "platform/Android/GameAppAndroid.h"
# include "graphics_rhi/Vulkan/VSwapchain.h"
# include "platform/Android/FileSystemAndroid.h"
# include "platform/OpenXR/VRSessionOpenXR.h"

namespace AE::Base {
	extern bool  Android_IsUnderDebugger;
}

namespace AE::App
{
	INTERNAL_LINKAGE( Unique<GameAppAndroid>  s_GameApp )

/*
=================================================
	GetApp
=================================================
*/
namespace {
	ND_ static GameAppAndroid&  GetApp () __NE___
	{
		ASSERT( s_GameApp );
		return *s_GameApp;
	}
}
/*
=================================================
	_GetAppInstance
=================================================
*/
	GameAppAndroid*  GameAppAndroid::_GetAppInstance () __NE___
	{
		return s_GameApp.get();
	}

/*
=================================================
	constructor
=================================================
*/
	GameAppAndroid::GameAppAndroid (Unique<IAppListener> listener) __NE___ :
		ApplicationBase{ RVRef(listener) }
	{
	}

/*
=================================================
	destructor
=================================================
*/
	GameAppAndroid::~GameAppAndroid () __NE___
	{
		_OnDestroy();
	}

/*
=================================================
	CreateWindow
=================================================
*/
	WindowPtr  GameAppAndroid::CreateWindow (WndListenerPtr listener, const WindowDesc &, IInputActions* dstActions) __NE___
	{
		CHECK_ERR( listener );
		CHECK_ERR( dstActions == null );

		DRC_EXLOCK( _stCheck );

		CHECK_ERR( _nativeApp == null );
		CHECK_ERR( static_cast<android_app*>(_nativeApp)->window != null );
		CHECK_ERR( not _window );

		_window.reset( new GameWindowAndroid{ *this, RVRef(listener) });
		return _window;
	}

/*
=================================================
	CreateVRSession
=================================================
*/
	WindowPtr  GameAppAndroid::CreateVRSession (WndListenerPtr listener, IInputActions* dstActions, IVRSession::EDeviceType type) __NE___
	{
	#ifdef AE_ENABLE_OPENXR
		CHECK_ERR( listener );
		CHECK_ERR( type == Default or type == IVRSession::EDeviceType::OpenXR );

		DRC_EXLOCK( _stCheck );

		CHECK_ERR( not _window );

		SharedPtr<VRSessionOpenXR>	vr{ new VRSessionOpenXR{ *this, RVRef(listener), dstActions }};
		if ( not vr->Create() )
			return Default;

		_AddWindow( vr );
		_window = vr;

		return _window;

	#else
		return Default;
	#endif
	}

/*
=================================================
	OpenStorage
=================================================
*/
	RC<IVirtualFileStorage>  GameAppAndroid::OpenStorage (EAppStorage type) __NE___
	{
		switch_enum( type )
		{
			case EAppStorage::Builtin :
			{
				auto	fs = MakeRC<FileSystemAndroid>();
				CHECK_ERR( fs->Create( static_cast<android_app*>(_nativeApp)->activity->assetManager, "" ));
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
	Path  GameAppAndroid::GetStoragePath (EAppStorage type) __NE___
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
	IApplication::MonitorsView_t  GameAppAndroid::GetMonitors (bool update) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );
		DRC_EXLOCK( _stCheck );		// for compatibility
		Unused( update );

		return MonitorsView_t{ &_displayInfo, 1 };
	}

	IApplication::MonitorsView_t  GameAppAndroid::GetCachedMonitors () C_NE___
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
	ArrayView<const char*>  GameAppAndroid::GetVulkanInstanceExtensions () __NE___
	{
		return Graphics::VSwapchain::GetInstanceExtensions();
	}

/*
=================================================
	Terminate
=================================================
*/
	void  GameAppAndroid::Terminate () __NE___
	{
		DRC_EXLOCK( _stCheck );

		_isRunning.store( false );

		if ( _window )
		{
			_window->Close();
			_window.reset();
		}
	}

/*
=================================================
	BeforeUpdate
=================================================
*/
	void  GameAppAndroid::BeforeUpdate () __NE___
	{
	}

/*
=================================================
	AfterUpdate
=================================================
*/
	void  GameAppAndroid::AfterUpdate () __NE___
	{

	}

/*
=================================================
	_OnDestroy
=================================================
*/
	void  GameAppAndroid::_OnDestroy () __NE___
	{
		DRC_EXLOCK( _drCheck );

		ApplicationBase::_Destroy();

		_windows.clear();
	}

/*
=================================================
	_MainLoop
=================================================
*/
	void  GameAppAndroid::_MainLoop (void* nativeApp) __NE___
	{
		auto*	androidApp = static_cast<android_app*>(nativeApp);

		_nativeApp = nativeApp;

		_paths.Write( StoragePath{
			Path{ androidApp->activity->internalDataPath },
			Path{},	// TODO
			Path{ androidApp->activity->externalDataPath },
			Path{}	// TODO
		});

		for (;;)
		{
			if ( androidApp->destroyRequested != 0 )
				return;

			for (;;)
			{
				android_poll_source*	source = null;
				int						events = 0;

				const int timeout_ms	=  -1; //(not androidAppState.resumed		and
											//not m_sessionRunning			and
											//androidApp->destroyRequested == 0) ? -1 : 0;

				if ( ALooper_pollOnce( timeout_ms, null, &events, reinterpret_cast<void**>(&source) ) < 0 )
					break;

				if ( source != null )
					source->process( androidApp, source );
			}

			if ( _window )
			{
				_window->ProcessMessages();
			}
		}
	}

/*
=================================================
	_HandleCmd
=================================================
*/
	void  GameAppAndroid::_HandleCmd (void* app, int32_t cmd) __NE___
	{
		auto*	self = Cast<GameAppAndroid>( static_cast<android_app*>(app)->userData );

		switch ( cmd )
		{
			case APP_CMD_START:
			{
				self->_state = EState::Started;
				self->_listener->OnStart( *self );
				
				//if ( self->_window )
				//	self->_window->_SetStateV2( self->_state );
				break;
			}

			case APP_CMD_RESUME:
			{
				self->_state = EState::InForeground;
				
				//if ( self->_window )
				//	self->_window->_SetStateV2( self->_state );
				break;
			}

			case APP_CMD_PAUSE:
			{
				self->_state = EState::InBackground;
				
				//if ( self->_window )
				//	self->_window->_SetStateV2( self->_state );
				break;
			}

			case APP_CMD_STOP:
			{
				self->_state = EState::Stopped;
				
				//if ( self->_window )
				//	self->_window->_SetStateV2( self->_state );

				self->_listener->OnStop( *self );
				break;
			}

			case APP_CMD_DESTROY:
			{
				self->_state = EState::Destroyed;

				if ( self->_window )
				{
					self->_window->Close();
					self->_window.reset();
				}
				break;
			}

			case APP_CMD_INIT_WINDOW:
			{
				break;
			}

			case APP_CMD_TERM_WINDOW:
			{
				if ( self->_window )
				{
					self->_window->Close();
					self->_window.reset();
				}
				break;
			}
		}
	}

} // AE::App

/*
=================================================
	android_main
----
	run in separate thread
	TODO: https://developer.android.com/games/agdk/overview
		https://developer.android.com/jetpack/androidx/releases/games
		https://github.com/android/games-samples/tree/main/agdk
=================================================
*/
void  android_main (android_app* app)
{
	using namespace AE::App;
	using namespace AE::Java;

	JavaEnv::SetVM( app->activity->vm );
	JavaEnv  env;

	s_GameApp.reset( new GameAppAndroid{ AE_OnAppCreated() });
	AE_LOGI( "Started native application" );

	app->userData = s_GameApp.get();
	app->onAppCmd = reinterpret_cast< decltype(android_app::onAppCmd) >(&GameAppAndroid::_HandleCmd);

	s_GameApp->_MainLoop( app );

	app->userData = null;
	s_GameApp.reset();

	AE_OnAppDestroyed();
    JavaEnv::SetVM( null );
}

# include "android_native_app_glue.c"

#endif // AE_PLATFORM_ANDROID
