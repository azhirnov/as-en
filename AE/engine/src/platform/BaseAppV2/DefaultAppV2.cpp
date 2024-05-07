// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/BaseAppV2/DefaultAppV2.h"

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

namespace AE::AppV2
{
	using namespace AE::Threading;
	using namespace AE::Graphics;

/*
=================================================
	constructor
=================================================
*/
	AppCore::AppCore (const AppConfig &cfg) __NE___ :
		_config{ cfg }
	{
	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		ConstCast(_config).enableNetwork = true;
	  #endif

		// TODO: allow to override
		CHECK_FATAL( ThreadMngr::SetupThreads( cfg.threading,
											   cfg.threading.mask,
											   cfg.threading.maxThreads,
											   True{"bind threads to physical cores"},
											   OUT _allowProcessInMain ));
	}

/*
=================================================
	destructor
=================================================
*/
	AppCore::~AppCore () __NE___
	{
		_curState.Write( Default );
	}

/*
=================================================
	OpenView
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppCore::OpenView (ViewModeName::Ref name) __NE___
	{
		RC<IViewMode>	new_view = _CreateViewMode( name );

		if ( not new_view )
			return;

		CurrentState	data;
		{
			auto	state = _curState.WriteLock();
			data = *state;
			state->view = new_view;
		}

		if ( data.view )
			data.view->Close();

		CHECK( data.input->SetMode( new_view->GetInputMode() ));
		CHECK( new_view->Open( data.output ));

		// TODO: UI animation
	}

/*
=================================================
	OpenViewAsync
=================================================
*/
	AsyncTask  AppCore::OpenViewAsync (ViewModeName::Ref name) __NE___
	{
		return MakeTask( [name, app = GetRC()]() { app->OpenView( name ); }, {},
						 "OpenViewAsync",
						 ETaskQueue::Main );
	}

/*
=================================================
	_InitVFS
----
	This is default implementation and can be overriden.
	Thread-safe:  yes
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
	bool  AppCore::_InitVFS (VFS::FileName::Ref archiveInAssets) __NE___
	{
		using namespace AE::VFS;

		auto	assets = GetApplication()->OpenStorage( EAppStorage::Builtin );
		CHECK_ERR( assets );

		RC<RDataSource>	ds;
		CHECK_ERR( assets->Open( OUT ds, archiveInAssets ));

		auto	storage = VirtualFileStorageFactory::CreateStaticArchive( RVRef(ds) );

		CHECK_ERR( storage );
		CHECK_ERR( GetVFS().AddStorage( storage ));

		GetVFS().MakeImmutable();
		return true;
	}
#else
	bool  AppCore::_InitVFS (const Path &archivePath) __NE___
	{
		auto	storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( archivePath );

		CHECK_ERR( storage );
		CHECK_ERR( GetVFS().AddStorage( storage ));

		GetVFS().MakeImmutable();
		return true;
	}
#endif

/*
=================================================
	_InitInputActions
----
	Thread-safe:  main thread only
=================================================
*/
	bool  AppCore::_InitInputActions (IInputActions &ia, VFS::FileName::Ref fname) __NE___
	{
		// file can be cached if switch between desktop and VR is possible

		auto	file = GetVFS().Open<RStream>( fname );
		CHECK_ERR( file );

		MemRefRStream&	stream = *Cast<MemRefRStream>(file.get());
		ASSERT( CastAllowed<MemRefRStream>( file.get() ));

		CHECK_ERR( ia.LoadSerialized( stream ));

		if (RC<IViewMode> view = _curState->view)
			CHECK_ERR( ia.SetMode( view->GetInputMode() ));

		return true;
	}

/*
=================================================
	StartRendering
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppCore::StartRendering (IWindow &wnd, IWindow::EState wndState) __NE___
	{
		_StartRendering( wnd.InputActions(), wnd.GetSurface(), wndState, WindowOrVR_t{&wnd} );
	}

	void  AppCore::StartRendering (IVRDevice &vr, IVRDevice::EState wndState) __NE___
	{
		_StartRendering( vr.InputActions(), vr.GetSurface(), wndState, WindowOrVR_t{&vr} );
	}

	inline void  AppCore::_StartRendering (IInputActions &input, IOutputSurface &output, const IWindow::EState wndState, WindowOrVR_t wndOrVR) __NE___
	{
		const bool	focused			= (wndState == IWindow::EState::Focused);
		const bool	in_foreground	= (wndState >= IWindow::EState::InForeground) and (wndState <= IWindow::EState::Focused);
		bool		ia_changed;
		bool		has_view;

		{
			auto	state = _curState.WriteLock();

			if ( not focused and state->output != null )
				return;

			ia_changed			= (state->input != &input);
			has_view			= output.IsInitialized() and (state->view != null);

			state->input		= &input;
			state->output		= &output;
			state->windowOrVR	= wndOrVR;
		}

		if_unlikely( ia_changed )
			_InputActionsChanged( input );

		if_unlikely( not has_view and in_foreground )
			OpenView( ViewModeName_Initial );
	}

/*
=================================================
	StopRendering
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppCore::StopRendering (Ptr<IOutputSurface> output) __NE___
	{
		auto	state = _curState.WriteLock();

		if ( output == null or state->output == output )
			state->output = null;
	}

/*
=================================================
	WaitFrame
----
	Thread-safe:	main thread only
	Usage:			every frame
=================================================
*/
	void  AppCore::WaitFrame () __NE___
	{
		CHECK( GraphicsScheduler().WaitNextFrame( _allowProcessInMain, AE::DefaultTimeout ));
	}

/*
=================================================
	RenderFrame
----
	Thread-safe:	main thread only
	Usage:			every frame
=================================================
*/
	void  AppCore::RenderFrame () __NE___
	{
		Ptr<IInputActions>		input;
		Ptr<IOutputSurface>		output;
		RC<IViewMode>			view;
		auto&					rts		= GraphicsScheduler();
		auto					rg		= rts.GetRenderGraphPtr();
		const auto				frame_id = rts.GetFrameId();

		{
			auto	state = _curState.ReadLock();

			input	= state->input;
			output	= state->output;
			view	= state->view;
		}

		if_unlikely( input == null				 or
					 output == null				 or
					 not output->IsInitialized() or
					 view == null				 or
					 not rts.BeginFrame() )
		{
			ThreadUtils::Sleep_15ms();
			return;
		}

		if ( rg )
			rg->OnBeginFrame( frame_id );

		AsyncTask	view_task	= view->Update( input->ReadInput( frame_id ), output, Default );
		// 'view_task' can be null

		AsyncTask	end_frame	= rts.EndFrame( Tuple{ view_task });

		if ( rg )
			rg->OnEndFrame();

		input->NextFrame( frame_id.Next() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	AppMainV2::AppMainV2 (AppCoreCtor_t ctor) __NE___ :
		_device{ True{"enable info log"} }
	{
		TaskScheduler::InstanceCtor::Create();
		VFS::VirtualFileSystem::InstanceCtor::Create();
	  #ifdef AE_ENABLE_AUDIO
		Audio::IAudioSystem::InstanceCtor::Create();
	  #endif

		_core = ctor();
		CHECK_FATAL( _core );

		auto&	cfg = _core->Config();

		CHECK( cfg.graphics.maxFrames <= cfg.graphics.swapchain.minImageCount );

		if ( cfg.enableNetwork )
			CHECK_FATAL( Networking::SocketService::Instance().Initialize() );

	  #ifdef AE_ENABLE_AUDIO
		if ( cfg.enableAudio )
			CHECK_FATAL( AudioSystem().Initialize() );
	  #endif
	}

	AppMainV2::AppMainV2 (RC<AppCore> core) __NE___ :
		AppMainV2{ [core](){ return core; }}
	{}

/*
=================================================
	destructor
=================================================
*/
	AppMainV2::~AppMainV2 () __NE___
	{
		const bool	enable_network	= _core ? _core->Config().enableNetwork	: false;
		const bool	enable_audio	= _core ? _core->Config().enableAudio	: false;

		_core = null;
		_windows.clear();
		_vrDevice = null;

		Scheduler().Release();

		if ( enable_network )
			Networking::SocketService::Instance().Deinitialize();

	  #ifdef AE_ENABLE_AUDIO
		if ( enable_audio )
			AudioSystem().Deinitialize();
		Audio::IAudioSystem::InstanceCtor::Destroy();
	  #endif

		VFS::VirtualFileSystem::InstanceCtor::Destroy();
		TaskScheduler::InstanceCtor::Destroy();
	}

/*
=================================================
	BeforeWndUpdate
----
	Thread-safe:	main thread only
	Usage:			every frame
=================================================
*/
	void  AppMainV2::BeforeWndUpdate (IApplication &) __NE___
	{
		PROFILE_ONLY({
			auto	profiler = Scheduler().GetProfiler();
			if ( profiler )
				profiler->EndNonTaskWork( this, "NativeApp" );
		})

		_core->WaitFrame();
	}

/*
=================================================
	AfterWndUpdate
----
	Thread-safe:	main thread only
	Usage:			every frame
=================================================
*/
	void  AppMainV2::AfterWndUpdate (IApplication &app) __NE___
	{
		_core->RenderFrame();

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
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppMainV2::OnStart (IApplication &app) __NE___
	{
		CHECK_FATAL( _InitGraphics( app ));
		CHECK_FATAL( _CreateWindow( app ));
	}

/*
=================================================
	_CreateWindow
=================================================
*/
	bool  AppMainV2::_CreateWindow (IApplication &app) __NE___
	{
		auto&	cfg = _core->Config();

		// create window
		{
			auto	wnd = app.CreateWindow( MakeUnique<AppMainV2::WindowEventListener>( _core ), cfg.window );
			CHECK_ERR( wnd );
			_windows.emplace_back( RVRef(wnd) );
		}

		// create VR device
		if ( cfg.enableVR )
		{
			const auto	CreateVR = [this, &app, &cfg] (IVRDevice::EDeviceType type) -> bool
			{{
				_vrDevice = app.CreateVRDevice( MakeUnique<AppMainV2::VRDeviceEventListener>( _core ), &_windows[0]->InputActions(), type );
				return _vrDevice and _vrDevice->CreateRenderSurface( cfg.vr );
			}};

			for (auto type : cfg.vrDevices) {
				if ( CreateVR( type )) break;
			}
		}

		return true;
	}

/*
=================================================
	OnStop
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppMainV2::OnStop (IApplication &) __NE___
	{
		if ( _core )
		{
			_core->WaitFrame();

			ASSERT( _core.use_count() == 1 );
			_core = null;
		}

		_windows.clear();
		_vrDevice = null;

		_DestroyGraphics();
	}

/*
=================================================
	_InitGraphics
=================================================
*/
	bool  AppMainV2::_InitGraphics (IApplication &app) __NE___
	{
		if_unlikely( _device.IsInitialized() )
			return true;

		RenderTaskScheduler::InstanceCtor::Create( _device );

	  #ifdef AE_ENABLE_VULKAN
		CHECK_ERR( _device.Init( _core->Config().graphics, app.GetVulkanInstanceExtensions() ));

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
		CHECK_ERR( _device.Init( _core->Config().graphics ));

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Unused( app );
		CHECK_ERR( _device.Init( _core->Config().graphics ));

		#if ENABLE_SYNC_LOG
			_device.EnableSyncLog( true );
		#endif

	  #else
	  #	error not implemented
	  #endif

		CHECK_ERR( _device.CheckConstantLimits() );
		CHECK_ERR( _device.CheckExtensions() );

		CHECK_ERR( GraphicsScheduler().Initialize( _core->Config().graphics ));
		return true;
	}

/*
=================================================
	_DestroyGraphics
=================================================
*/
	void  AppMainV2::_DestroyGraphics () __NE___
	{
		if_unlikely( not _device.IsInitialized() )
			return;

		#if ENABLE_SYNC_LOG
			VulkanSyncLog::Deinitialize( INOUT _device.EditDeviceFnTable() );
		#endif

		Unused( GraphicsScheduler().WaitAll( AE::DefaultTimeout ));	// TODO ???

		RenderTaskScheduler::InstanceCtor::Destroy();

		CHECK_ERRV( _device.DestroyLogicalDevice() );
		CHECK_ERRV( _device.DestroyInstance() );
	}

/*
=================================================
	OnStateChanged
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppMainV2::WindowEventListener::OnStateChanged (IWindow &wnd, EState state) __NE___
	{
		switch_enum( state )
		{
			case EState::InForeground :
			case EState::Focused :
				_core->StartRendering( wnd, state );
				break;

			case EState::InBackground :
			case EState::Stopped :
			case EState::Destroyed :
				_core->StopRendering( &wnd.GetSurface() );
				_core->WaitFrame();
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
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppMainV2::WindowEventListener::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		// create render surface
		CHECK_FATAL( wnd.CreateRenderSurface( _core->Config().graphics.swapchain ));

		_core->StartRendering( wnd, wnd.GetState() );
	}

/*
=================================================
	OnSurfaceDestroyed
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppMainV2::WindowEventListener::OnSurfaceDestroyed (IWindow &) __NE___
	{
		_core->StopRendering( null );
		_core->WaitFrame();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	OnStateChanged
----
	Thread-safe:  main thread only
=================================================
*/
	void  AppMainV2::VRDeviceEventListener::OnStateChanged (IVRDevice &vr, EState state) __NE___
	{
		switch_enum( state )
		{
			case EState::InForeground :
			case EState::Focused :
				_core->StartRendering( vr, state );
				break;

			case EState::InBackground :
			case EState::Stopped :
			case EState::Destroyed :
				_core->StopRendering( &vr.GetSurface() );
				_core->WaitFrame();
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


} // AE::AppV2
