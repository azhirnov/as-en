// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/BaseAppV1/DefaultAppV1.h"

#include "platform/Android/AndroidCommon.h"
#include "platform/GLFW/GLFWCommon.h"
#include "platform/WinAPI/WinAPICommon.h"
#include "platform/OpenVR/OpenVRCommon.h"

#ifdef AE_ENABLE_AUDIO
# include "audio/Public/IAudioSystem.h"
#endif

// Enable it if you have very rare bug with synchs.
// When Vulkan validation reports error put breakpoint in 'log.clear();' and check 'log' content.
#define ENABLE_SYNC_LOG		0
#if ENABLE_SYNC_LOG
#	include "VulkanSyncLog.h"
#endif

namespace AE::AppV1
{
	using namespace AE::Threading;
	using namespace AE::Graphics;

/*
=================================================
	constructor
=================================================
*/
	AppCoreV1::AppCoreV1 (const AppConfig &cfg, RC<IBaseApp> impl) __NE___ :
		_device{ True{"enable info log"} },
		_impl{ RVRef(impl) },
		_config{ cfg }
	{
		CHECK( _config.graphics.maxFrames <= _config.graphics.swapchain.minImageCount );

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		ConstCast(_config).enableNetwork = true;
	  #endif

		TaskScheduler::InstanceCtor::Create();
		VFS::VirtualFileSystem::InstanceCtor::Create();
	  #ifdef AE_ENABLE_AUDIO
		Audio::IAudioSystem::InstanceCtor::Create();
	  #endif

		CHECK_FATAL( ThreadMngr::SetupThreads( _config.threading,
											   _config.threading.mask,
											   _config.threading.maxThreads,
											   True{"bind threads to physical cores"},
											   OUT _allowProcessInMain ));

		if ( _config.enableNetwork )
			CHECK_FATAL( Networking::SocketService::Instance().Initialize() );

	  #ifdef AE_ENABLE_AUDIO
		if ( _config.enableAudio )
			CHECK_FATAL( AudioSystem().Initialize() );
	  #endif
	}

/*
=================================================
	destructor
=================================================
*/
	AppCoreV1::~AppCoreV1 () __NE___
	{
		_windows.clear();

		for (; Scheduler().ProcessTasks( _allowProcessInMain, EThreadSeed(usize(this) & 0xF) );) {}

		Scheduler().Release();

		if ( _config.enableNetwork )
			Networking::SocketService::Instance().Deinitialize();

	  #ifdef AE_ENABLE_AUDIO
		if ( _config.enableAudio )
			AudioSystem().Deinitialize();
		Audio::IAudioSystem::InstanceCtor::Destroy();
	  #endif

		VFS::VirtualFileSystem::InstanceCtor::Destroy();
		TaskScheduler::InstanceCtor::Destroy();
	}

/*
=================================================
	BeforeWndUpdate
=================================================
*/
	void  AppCoreV1::BeforeWndUpdate (IApplication &) __NE___
	{
		PROFILE_ONLY({
			auto	profiler = Scheduler().GetProfiler();
			if ( profiler )
				profiler->EndNonTaskWork( this, "NativeApp" );
		})


		Ptr<IWindow>	active_wnd;
		Ptr<IVRDevice>	active_vr;

		for (auto& wnd : _windows) {
			if ( wnd->GetState() == IWindow::EState::Focused )
				active_wnd = wnd.get();
		}

		if ( _vrDevice and _vrDevice->GetState() == IWindow::EState::Focused )
			active_vr = _vrDevice.get();

		_impl->WaitFrame( GetMainThreadMask(), active_wnd, active_vr );
	}

/*
=================================================
	AfterWndUpdate
=================================================
*/
	void  AppCoreV1::AfterWndUpdate (IApplication &app) __NE___
	{
		_impl->RenderFrame();

		#if ENABLE_SYNC_LOG
		# ifdef AE_ENABLE_VULKAN
		{
			String	log;
			VulkanSyncLog::GetLog( OUT log );
			log.clear();
		}
		# elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		{
			String	log;
			_device.GetSyncLog( OUT log );
			log.clear();
		}
		# endif
		#endif

		PROFILE_ONLY({
			auto	profiler = Scheduler().GetProfiler();
			if ( profiler )
				profiler->BeginNonTaskWork( this, "NativeApp" );
		})

		// terminate application if main window has been closed
		if_unlikely( _windows.empty() or _windows[0]->GetState() == IWindow::EState::Destroyed )
		{
			if ( _vrDevice )
				_vrDevice.reset();

			app.Terminate();
		}
	}

/*
=================================================
	OnStart
=================================================
*/
	void  AppCoreV1::OnStart (IApplication &app) __NE___
	{
		CHECK_FATAL( _InitGraphics( app ));
	}

/*
=================================================
	_OnStartImpl
=================================================
*/
	bool  AppCoreV1::_OnStartImpl (IApplication &app) __NE___
	{
		// create window
		{
			auto	wnd = app.CreateWindow( MakeUnique<AppCoreV1::WindowEventListener>( _impl, *this ), _config.window );
			CHECK_ERR( wnd );
			_windows.emplace_back( RVRef(wnd) );
		}

		// create second window
		#if 0
		{
			WindowDesc	desc = _config.window;
			desc.title << "-2";

			auto	wnd = app.CreateWindow( MakeUnique<AppCoreV1::WindowEventListener>( _impl, *this ), desc, &_windows[0]->InputActions() );
			CHECK_ERR( wnd );
			_windows.emplace_back( RVRef(wnd) );
		}
		#endif

		// create VR device
		if ( _config.enableVR and _windows.size() >= 1 )
		{
			const auto	CreateVR = [this, &app] (IVRDevice::EDeviceType type) -> bool
			{{
				_vrDevice = app.CreateVRDevice( MakeUnique<AppCoreV1::VRDeviceEventListener>( _impl, *this ), &_windows[0]->InputActions(), type );
				return _vrDevice and _vrDevice->CreateRenderSurface( _config.vr );
			}};

			ASSERT( not _config.vrDevices.empty() );

			for (auto type : _config.vrDevices) {
				if ( CreateVR( type )) break;
			}

			if ( _vrDevice )
			{
				IVRDevice::Settings		settings;
				settings.cameraClipPlanes	= { 0.1f, 100.f };

				Unused( _vrDevice->Setup( settings ));
			}
		}

		return true;
	}

/*
=================================================
	OnStop
=================================================
*/
	void  AppCoreV1::OnStop (IApplication &) __NE___
	{
		if ( _impl )
		{
			_impl->WaitFrame( GetMainThreadMask(), null, null );
			_impl = null;
		}
		_windows.clear();

		_DestroyGraphics();
	}

/*
=================================================
	_InitGraphics
=================================================
*/
	bool  AppCoreV1::_InitGraphics (IApplication &app) __NE___
	{
		if_unlikely( _device.IsInitialized() )
			return true;

		RenderTaskScheduler::InstanceCtor::Create( _device );

	  #ifdef AE_ENABLE_VULKAN
		CHECK_ERR( _device.Init( _config.graphics, app.GetVulkanInstanceExtensions() ));

		#if ENABLE_SYNC_LOG
		{
			FlatHashMap<VkQueue, String>	qnames;

			for (auto& q : _device.GetQueues()) {
				qnames.emplace( q.handle, q.debugName );
			}
			VulkanSyncLog::Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
			VulkanSyncLog::Enable();
		}
		#endif

	  #elif defined(AE_ENABLE_METAL)
		Unused( app );
		CHECK_ERR( _device.Init( _config.graphics ));

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Unused( app );
		CHECK_ERR( _device.Init( _config.graphics ));

		#if ENABLE_SYNC_LOG
			_device.EnableSyncLog( true );
		#endif

	  #else
	  #	error not implemented
	  #endif

		CHECK_ERR( _device.CheckConstantLimits() );
		CHECK_ERR( _device.CheckExtensions() );

		CHECK_ERR( GraphicsScheduler().Initialize( _config.graphics ));
		return true;
	}

/*
=================================================
	_DestroyGraphics
=================================================
*/
	void  AppCoreV1::_DestroyGraphics () __NE___
	{
		if_unlikely( not _device.IsInitialized() )
			return;

		Unused( GraphicsScheduler().WaitAll( AE::DefaultTimeout ));	// TODO ???

		RenderTaskScheduler::InstanceCtor::Destroy();

		#if ENABLE_SYNC_LOG
			VulkanSyncLog::Deinitialize( INOUT _device.EditDeviceFnTable() );
		#endif

		CHECK_ERRV( _device.DestroyLogicalDevice() );
		CHECK_ERRV( _device.DestroyInstance() );
	}

/*
=================================================
	OnStateChanged
=================================================
*/
	void  AppCoreV1::WindowEventListener::OnStateChanged (IWindow &wnd, EState state) __NE___
	{
		switch_enum( state )
		{
			case EState::InForeground :
			case EState::Focused :
				_impl->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), state );
				break;

			case EState::InBackground :
			case EState::Stopped :
			case EState::Destroyed :
				_impl->StopRendering( &wnd.GetSurface() );
				_impl->WaitFrame( _app.GetMainThreadMask(), null, null );
				break;

			case EState::Created :
			case EState::Unknown :
			case EState::Started :
				break;

			default :
				DBG_WARNING( "unsupported window state" );
				break;
		}
		switch_end
	}

/*
=================================================
	OnSurfaceCreated
=================================================
*/
	void  AppCoreV1::WindowEventListener::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		// create render surface
		CHECK_FATAL( wnd.CreateRenderSurface( _app.GetConfig().graphics.swapchain ));

		_impl->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), EState::InForeground );

		CHECK_FATAL( _impl->OnSurfaceCreated( wnd ));
	}

/*
=================================================
	OnSurfaceDestroyed
=================================================
*/
	void  AppCoreV1::WindowEventListener::OnSurfaceDestroyed (IWindow &) __NE___
	{
		_impl->StopRendering( null );
		_impl->WaitFrame( _app.GetMainThreadMask(), null, null );

		// 'WaitFrame' may not process any task, so we need to process them here too.
		Unused( Scheduler().ProcessTasks( _app.GetMainThreadMask(), Scheduler().GetDefaultSeed() ));

		Unused( GraphicsScheduler().WaitAll( AE::DefaultTimeout ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	OnStateChanged
=================================================
*/
	void  AppCoreV1::VRDeviceEventListener::OnStateChanged (IVRDevice &vr, EState state) __NE___
	{
		switch_enum( state )
		{
			case EState::InForeground :
			case EState::Focused :
				_impl->StartRendering( &vr.InputActions(), &vr.GetSurface(), state );
				break;

			case EState::InBackground :
			case EState::Stopped :
			case EState::Destroyed :
				_impl->StopRendering( &vr.GetSurface() );
				_impl->WaitFrame( _app.GetMainThreadMask(), null, null );
				break;

			case EState::Created :
			case EState::Unknown :
			case EState::Started :
				break;

			default :
				DBG_WARNING( "unsupported VR state" );
				break;
		}
		switch_end
	}


} // AE::AppV1
