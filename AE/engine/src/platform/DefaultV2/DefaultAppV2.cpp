// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/DefaultV2/DefaultAppV2.h"

#include "platform/Android/AndroidCommon.h"
#include "platform/GLFW/GLFWCommon.h"
#include "platform/WinAPI/WinAPICommon.h"
#include "platform/OpenVR/OpenVRCommon.h"

#define ENABLE_SYNC_LOG     0
#if ENABLE_SYNC_LOG
#   include "VulkanSyncLog.h"
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
    AppCore::AppCore () __NE___
    {
    }

/*
=================================================
    destructor
=================================================
*/
    AppCore::~AppCore () __NE___
    {
        _mainLoop.Write( Default );
    }

/*
=================================================
    OpenView
=================================================
*/
    void  AppCore::OpenView (const ViewModeName &name) __NE___
    {
        RC<IViewMode>       mode = _CreateViewMode( name );
        Ptr<IInputActions>  input;

        if ( not mode )
            return;

        {
            auto    main_loop = _mainLoop.WriteNoLock();
            EXLOCK( main_loop );

            main_loop->mode = mode;
            input = main_loop->input;
        }

        //_SetInputMode( input, mode->GetInputMode() );

        // TODO: UI animation
    }

/*
=================================================
    OnStart
=================================================
*/
    bool  AppCore::OnStart (IApplication &app) __NE___
    {
        return  _InitVFS( app )     and
                _LoadInputActions();
    }

/*
=================================================
    _InitVFS
----
    This is default implementation and can be overriden.
=================================================
*/
    bool  AppCore::_InitVFS (IApplication &app) __NE___
    {
      #ifdef AE_PLATFORM_ANDROID
        auto    assets = app.OpenBuiltinStorage();
        CHECK_ERR( assets );

        RC<RDataSource> ds;
        CHECK_ERR( assets->Open( OUT ds, VFS::FileName{"resources.bin"} )); 

        auto    storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( RVRef(ds) );
      #else
        Unused( app );
        auto    storage = VFS::VirtualFileStorageFactory::CreateStaticArchive( "resources.bin" );
      #endif

        CHECK_ERR( storage );
        CHECK_ERR( GetVFS().AddStorage( storage ));
        return true;
    }

/*
=================================================
    _LoadInputActions
=================================================
*/
    bool  AppCore::_LoadInputActions () __NE___
    {
        // load input actions
        _inputActionsData = MakeRC<MemRStream>();

        auto    file = GetVFS().OpenAsStream( VFS::FileName{"controls"} );
        CHECK_ERR( file );
        CHECK_ERR( _inputActionsData->LoadRemaining( *file ));

        return true;
    }

/*
=================================================
    OnSurfaceCreated
=================================================
*/
    bool  AppCore::OnSurfaceCreated (IOutputSurface &surface) __NE___
    {
        CHECK_ERR( surface.IsInitialized() );

        OpenView( ViewModeName{"splash"} );
        return true;
    }

/*
=================================================
    _InitInputActions
=================================================
*/
    void  AppCore::_InitInputActions (IInputActions &ia) __NE___
    {
        CHECK_ERRV( _inputActionsData );

        MemRefRStream   stream{ _inputActionsData->GetData() };

        // may be switch between 'glfw'/'winapi' and 'openvr'/'openxr' platforms
        CHECK_ERRV( ia.LoadSerialized( stream ));

        if (RC<IViewMode> mode = _mainLoop->mode)
            CHECK( ia.SetMode( mode->GetInputMode() ));
    }

/*
=================================================
    _SetInputMode
=================================================
*/
    AsyncTask  AppCore::_SetInputMode (Ptr<IInputActions> input, InputModeName mode) __NE___
    {
        if ( input == null )
            return null;

        return Scheduler().Run<AsyncTaskFn>(
                    Tuple{  [input, mode] () { Unused( input->SetMode( mode )); },
                            "AppCore::SetInputMode",
                            ETaskQueue::Main },
                    Tuple{} );
    }

/*
=================================================
    StartRendering
=================================================
*/
    void  AppCore::StartRendering (Ptr<IInputActions> input, Ptr<IOutputSurface> output, IWindow::EState state) __NE___
    {
        ASSERT( bool{input} == bool{output} );

        const bool  focused = (state == IWindow::EState::Focused);
        bool        ia_changed;

        {
            auto    main_loop = _mainLoop.WriteNoLock();
            EXLOCK( main_loop );

            if ( not focused and main_loop->output != null )
                return;

            ia_changed = (main_loop->input != input) and (input != null);

            main_loop->input  = input;
            main_loop->output = output;
        }

        if ( ia_changed )
            _InitInputActions( *input );
    }

/*
=================================================
    StopRendering
=================================================
*/
    void  AppCore::StopRendering (Ptr<IOutputSurface> output) __NE___
    {
        auto    main_loop = _mainLoop.WriteNoLock();
        EXLOCK( main_loop );

        if ( output == null or main_loop->output == output )
            main_loop->output = null;
    }

/*
=================================================
    WaitFrame
=================================================
*/
    void  AppCore::WaitFrame (const EThreadArray &threads) __NE___
    {
        AsyncTask   task;
        std::swap( task, _mainLoop->endFrame );

        for (;;)
        {
            if ( task == null or task->IsFinished() )
                break;

            Scheduler().ProcessTasks( threads, 0 );

            Scheduler().DbgDetectDeadlock();
        }
    }

/*
=================================================
    RenderFrame
=================================================
*/
    void  AppCore::RenderFrame () __NE___
    {
        _mainLoop->endFrame = null;

        Ptr<IInputActions>      input;
        Ptr<IOutputSurface>     output;
        RC<IViewMode>           mode;
        auto&                   rts     = RenderTaskScheduler();

        {
            auto    main_loop = _mainLoop.ReadNoLock();
            SHAREDLOCK( main_loop );

            input   = main_loop->input;
            output  = main_loop->output;
            mode    = main_loop->mode;
        }

        if_unlikely( input == null               or
                     output == null              or
                     not output->IsInitialized() or
                     mode == null )
            return;

        const auto  frame_id    = rts.GetFrameId();

        AsyncTask   proc_input  = mode->Update( input->ReadInput( frame_id ), Default );
        // 'proc_input' can be null

        AsyncTask   begin_frame = rts.BeginFrame();
        if_unlikely( begin_frame->IsInterrupted() )
            return;

        AsyncTask   draw_task   = mode->Draw( output, { begin_frame, proc_input });
        CHECK( draw_task );

        AsyncTask   end_frame   = rts.EndFrame( Tuple{ draw_task });

        input->NextFrame( frame_id.Next() );

        _mainLoop->endFrame = end_frame;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    DefaultAppListener::DefaultAppListener (const AppConfig &cfg, AppCoreCtor_t ctor) __NE___ :
        #if defined(AE_ENABLE_VULKAN)
            _vulkan{ True{"enable info log"} },
        #elif defined(AE_ENABLE_METAL)
            _metal{ True{"enable info log"} },
        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            _remote{ True{"enable info log"} },
        #else
        #   error not implemented
        #endif
        _config{ cfg }
    {
        CHECK( _config.graphics.maxFrames <= _config.graphics.swapchain.minImageCount );

        TaskScheduler::CreateInstance();
        VFS::VirtualFileSystem::CreateInstance();

        // create render task scheduler
      #if defined(AE_ENABLE_VULKAN)
        VRenderTaskScheduler::CreateInstance( _vulkan );

      #elif defined(AE_ENABLE_METAL)
        MRenderTaskScheduler::CreateInstance( _metal );

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        RRenderTaskScheduler::CreateInstance( _remote );

      #else
      # error not implemented
      #endif

        _core = ctor();
        CHECK_FATAL( _core );

        CHECK_FATAL( ThreadMngr::SetupThreads( _config.threading,
                                               _config.threading.mask,
                                               null,
                                               _config.threading.maxThreads,
                                               OUT _allowProcessInMain ));
    }

    DefaultAppListener::DefaultAppListener (const AppConfig &cfg, RC<AppCore> core) __NE___ :
        DefaultAppListener{ cfg, [core](){ return core; }}
    {}

/*
=================================================
    destructor
=================================================
*/
    DefaultAppListener::~DefaultAppListener () __NE___
    {
        _core = null;
        _windows.clear();
        _vrDevice = null;

        Scheduler().Release();
        VFS::VirtualFileSystem::DestroyInstance();
        TaskScheduler::DestroyInstance();
    }

/*
=================================================
    BeforeWndUpdate
=================================================
*/
    void  DefaultAppListener::BeforeWndUpdate (IApplication &) __NE___
    {
        PROFILE_ONLY({
            auto    profiler = Scheduler().GetProfiler();
            if ( profiler )
                profiler->EndNonTaskWork( this, "NativeApp" );
        })

        _core->WaitFrame( _allowProcessInMain );
    }

/*
=================================================
    AfterWndUpdate
=================================================
*/
    void  DefaultAppListener::AfterWndUpdate (IApplication &app) __NE___
    {
        _core->RenderFrame();

        #if ENABLE_SYNC_LOG
        {
            String  log;
            VulkanSyncLog::GetLog( OUT log );
            log.clear();
        }
        #endif

        PROFILE_ONLY({
            auto    profiler = Scheduler().GetProfiler();
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
=================================================
*/
    void  DefaultAppListener::OnStart (IApplication &app) __NE___
    {
        CHECK_FATAL( _core->OnStart( app ));

        CHECK_FATAL( _InitGraphics( app ));
        CHECK_FATAL( _CreateWindow( app ));
    }

/*
=================================================
    _InitGraphics
=================================================
*/
    bool  DefaultAppListener::_InitGraphics (IApplication &app)
    {
      #if defined(AE_ENABLE_VULKAN)
        return _CreateVulkan( app );

      #elif defined(AE_ENABLE_METAL)
        return _CreateMetal( app );

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        return _CreateRemoteGraphics( app );

      #else
      # error not implemented
      #endif
    }

/*
=================================================
    _CreateWindow
=================================================
*/
    bool  DefaultAppListener::_CreateWindow (IApplication &app)
    {
        // create window
        {
            auto    wnd = app.CreateWindow( MakeUnique<DefaultIWndListener>( _core, *this ), _config.window );
            CHECK_ERR( wnd );
            _windows.emplace_back( RVRef(wnd) );
        }

        // create VR device
        if ( _config.enableVR )
        {
            const auto  CreateVR = [this, &app] (IVRDevice::EDeviceType type) -> bool
            {{
                _vrDevice = app.CreateVRDevice( MakeUnique<DefaultVRDeviceListener>( _core, *this ), &_windows[0]->InputActions(), type );
                return _vrDevice and _vrDevice->CreateRenderSurface( _config.vr );
            }};

            for (auto type : _config.vrDevices) {
                if ( CreateVR( type )) break;
            }
        }

        return true;
    }

/*
=================================================
    OnStop
=================================================
*/
    void  DefaultAppListener::OnStop (IApplication &) __NE___
    {
        if ( _core )
        {
            _core->WaitFrame( _allowProcessInMain );
            _core = null;
        }
        _windows.clear();
        _vrDevice = null;

      #if defined(AE_ENABLE_VULKAN)
        Unused( _DestroyVulkan() );

      #elif defined(AE_ENABLE_METAL)
        Unused( _DestroyMetal() );

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        Unused( _DestroyRemoteGraphics() );

      #else
      # error not implemented
      #endif
    }
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
/*
=================================================
    _CreateVulkan
=================================================
*/
    bool  DefaultAppListener::_CreateVulkan (IApplication &app)
    {
        if_unlikely( _vulkan.IsInitialized() )
            return true;

        CHECK_ERR( _vulkan.Init( _config.graphics, app.GetVulkanInstanceExtensions() ));
        CHECK_ERR( _vulkan.CheckConstantLimits() );
        CHECK_ERR( _vulkan.CheckExtensions() );

        #if ENABLE_SYNC_LOG
        {
            FlatHashMap<VkQueue, String>    qnames;

            for (auto& q : _vulkan.GetQueues()) {
                qnames.emplace( q.handle, q.debugName );
            }
            VulkanSyncLog::Initialize( INOUT _vulkan.EditDeviceFnTable(), RVRef(qnames) );
            VulkanSyncLog::Enable();
        }
        #endif

        CHECK_ERR( RenderTaskScheduler().Initialize( _config.graphics ));
        return true;
    }

/*
=================================================
    _DestroyVulkan
=================================================
*/
    bool  DefaultAppListener::_DestroyVulkan ()
    {
        if_unlikely( not _vulkan.IsInitialized() )
            return true;

        #if ENABLE_SYNC_LOG
            VulkanSyncLog::Deinitialize( INOUT _vulkan.EditDeviceFnTable() );
        #endif

        Unused( RenderTaskScheduler().WaitAll() );  // TODO ???

        VRenderTaskScheduler::DestroyInstance();

        CHECK_ERR( _vulkan.DestroyLogicalDevice() );
        CHECK_ERR( _vulkan.DestroyInstance() );
        return true;
    }
//-----------------------------------------------------------------------------



#elif defined(AE_ENABLE_METAL)
/*
=================================================
    _CreateMetal
=================================================
*/
    bool  DefaultAppListener::_CreateMetal (IApplication &)
    {
        if_unlikely( _metal.IsInitialized() )
            return true;

        CHECK_ERR( _metal.Init( _config.graphics ));
        CHECK_ERR( _metal.CheckConstantLimits() );
        CHECK_ERR( _metal.CheckExtensions() );

        CHECK_ERR( RenderTaskScheduler().Initialize( _config.graphics ));
        return true;
    }

/*
=================================================
    _DestroyMetal
=================================================
*/
    bool  DefaultAppListener::_DestroyMetal ()
    {
        if_unlikely( not _metal.IsInitialized() )
            return true;

        Unused( RenderTaskScheduler().WaitAll() );  // TODO ???

        MRenderTaskScheduler::DestroyInstance();

        CHECK_ERR( _metal.DestroyLogicalDevice() );
        return true;
    }
//-----------------------------------------------------------------------------



#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
/*
=================================================
    _CreateRemoteGraphics
=================================================
*/
    bool  DefaultAppListener::_CreateRemoteGraphics (IApplication &)
    {
        if_unlikely( _remote.IsInitialized() )
            return true;

        //CHECK_ERR( _remote.Init( _config.graphics ));
        CHECK_ERR( _remote.CheckConstantLimits() );
        CHECK_ERR( _remote.CheckExtensions() );

        CHECK_ERR( RenderTaskScheduler().Initialize( _config.graphics ));
        return true;
    }

/*
=================================================
    _DestroyRemoteGraphics
=================================================
*/
    bool  DefaultAppListener::_DestroyRemoteGraphics ()
    {
        if_unlikely( not _remote.IsInitialized() )
            return true;

        Unused( RenderTaskScheduler().WaitAll() );  // TODO ???

        RRenderTaskScheduler::DestroyInstance();

        //CHECK_ERR( _remote.DestroyLogicalDevice() );
        return true;
    }
//-----------------------------------------------------------------------------

#else
#   error not implemented
#endif



/*
=================================================
    OnStateChanged
=================================================
*/
    void  DefaultIWndListener::OnStateChanged (IWindow &wnd, EState state) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( state )
        {
            case EState::InForeground :
            case EState::Focused :          _core->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), state ); break;

            case EState::InBackground :
            case EState::Stopped :
            case EState::Destroyed :        _core->StopRendering( &wnd.GetSurface() );                              break;

            case EState::Created :
            case EState::Unknown :
            case EState::Started :                                                                                  break;

            default :                       DBG_WARNING( "unsupported window state" );                              break;
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    OnSurfaceCreated
=================================================
*/
    void  DefaultIWndListener::OnSurfaceCreated (IWindow &wnd) __NE___
    {
        // create render surface
        CHECK_FATAL( wnd.CreateRenderSurface( _app.Config().graphics.swapchain ));

        _core->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), IWindow::EState::InForeground );

        CHECK_FATAL( _core->OnSurfaceCreated( wnd.GetSurface() ));
    }

/*
=================================================
    OnSurfaceDestroyed
=================================================
*/
    void  DefaultIWndListener::OnSurfaceDestroyed (IWindow &) __NE___
    {
        _core->StopRendering( null );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    OnStateChanged
=================================================
*/
    void  DefaultVRDeviceListener::OnStateChanged (IVRDevice &vr, EState state) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( state )
        {
            case EState::InForeground :
            case EState::Focused :          _core->StartRendering( &vr.InputActions(), &vr.GetSurface(), state );   break;

            case EState::InBackground :
            case EState::Stopped :
            case EState::Destroyed :        _core->StopRendering( &vr.GetSurface() );                               break;

            case EState::Created :
            case EState::Unknown :
            case EState::Started :                                                                                  break;

            default :                       DBG_WARNING( "unsupported VR state" );                                  break;
        }
        END_ENUM_CHECKS();
    }


} // AE::AppV2
