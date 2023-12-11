// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/BaseAppV2/DefaultAppV2.h"
#include "graphics/RenderGraph/RenderGraphImpl.h"

#include "platform/Android/AndroidCommon.h"
#include "platform/GLFW/GLFWCommon.h"
#include "platform/WinAPI/WinAPICommon.h"
#include "platform/OpenVR/OpenVRCommon.h"

// Enable it if you have very rare bug with synchs.
// When Vulkan validation reports error put breakpoint in 'log.clear();' and check 'log' content.
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
        auto    assets = app.OpenStorage( EAppStorage::Builtin );
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

        GetVFS().MakeImmutable();
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

        auto    file = GetVFS().Open<RStream>( VFS::FileName{"controls"} );
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
    void  AppCore::WaitFrame (const Threading::EThreadArray &threads) __NE___
    {
        CHECK( GraphicsScheduler().WaitNextFrame( threads, AE::DefaultTimeout ));
    }

/*
=================================================
    RenderFrame
=================================================
*/
    void  AppCore::RenderFrame () __NE___
    {
        Ptr<IInputActions>      input;
        Ptr<IOutputSurface>     output;
        RC<IViewMode>           mode;
        auto&                   rts     = GraphicsScheduler();
        auto                    rg      = rts.GetRenderGraphPtr();

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

        const auto  frame_id = rts.GetFrameId();
        CHECK_ERRV( rts.BeginFrame() );

        if ( rg )
            rg->OnBeginFrame( frame_id );

        AsyncTask   proc_input  = mode->Update( input->ReadInput( frame_id ), Default );
        // 'proc_input' can be null

        AsyncTask   draw_task   = mode->Draw( output, { proc_input });
        CHECK( draw_task );

        AsyncTask   end_frame   = rts.EndFrame( Tuple{ draw_task });

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
    AppCoreV2::AppCoreV2 (const AppConfig &cfg, AppCoreCtor_t ctor) __NE___ :
        _device{ True{"enable info log"} },
        _config{ cfg }
    {
        CHECK( _config.graphics.maxFrames <= _config.graphics.swapchain.minImageCount );

        TaskScheduler::InstanceCtor::Create();
        VFS::VirtualFileSystem::InstanceCtor::Create();

        _core = ctor();
        CHECK_FATAL( _core );

        CHECK_FATAL( ThreadMngr::SetupThreads( _config.threading,
                                               _config.threading.mask,
                                               _config.threading.maxThreads,
                                               True{"bind threads to physical cores"},
                                               OUT _allowProcessInMain ));

        if ( _config.enableNetwork )
            CHECK_FATAL( Networking::SocketService::Instance().Initialize() );
    }

    AppCoreV2::AppCoreV2 (const AppConfig &cfg, RC<AppCore> core) __NE___ :
        AppCoreV2{ cfg, [core](){ return core; }}
    {}

/*
=================================================
    destructor
=================================================
*/
    AppCoreV2::~AppCoreV2 () __NE___
    {
        _core = null;
        _windows.clear();
        _vrDevice = null;

        Scheduler().Release();

        if ( _config.enableNetwork )
            Networking::SocketService::Instance().Deinitialize();

        VFS::VirtualFileSystem::InstanceCtor::Destroy();
        TaskScheduler::InstanceCtor::Destroy();
    }

/*
=================================================
    BeforeWndUpdate
=================================================
*/
    void  AppCoreV2::BeforeWndUpdate (IApplication &) __NE___
    {
        PROFILE_ONLY({
            auto    profiler = Scheduler().GetProfiler();
            if ( profiler )
                profiler->EndNonTaskWork( this, "NativeApp" );
        })

        _core->WaitFrame( GetMainThreadMask() );
    }

/*
=================================================
    AfterWndUpdate
=================================================
*/
    void  AppCoreV2::AfterWndUpdate (IApplication &app) __NE___
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
    void  AppCoreV2::OnStart (IApplication &app) __NE___
    {
        CHECK_FATAL( _core->OnStart( app ));

        CHECK_FATAL( _InitGraphics( app ));
        CHECK_FATAL( _CreateWindow( app ));
    }

/*
=================================================
    _CreateWindow
=================================================
*/
    bool  AppCoreV2::_CreateWindow (IApplication &app)
    {
        // create window
        {
            auto    wnd = app.CreateWindow( MakeUnique<AppCoreV2::WindowEventListener>( _core, *this ), _config.window );
            CHECK_ERR( wnd );
            _windows.emplace_back( RVRef(wnd) );
        }

        // create VR device
        if ( _config.enableVR )
        {
            const auto  CreateVR = [this, &app] (IVRDevice::EDeviceType type) -> bool
            {{
                _vrDevice = app.CreateVRDevice( MakeUnique<AppCoreV2::VRDeviceEventListener>( _core, *this ), &_windows[0]->InputActions(), type );
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
    void  AppCoreV2::OnStop (IApplication &) __NE___
    {
        if ( _core )
        {
            _core->WaitFrame( GetMainThreadMask() );
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
    bool  AppCoreV2::_InitGraphics (IApplication &app)
    {
        if_unlikely( _device.IsInitialized() )
            return true;

        RenderTaskScheduler::InstanceCtor::Create( _device );

      #ifdef AE_ENABLE_VULKAN
        CHECK_ERR( _device.Init( _config.graphics, app.GetVulkanInstanceExtensions() ));

        #if ENABLE_SYNC_LOG
        {
            FlatHashMap<VkQueue, String>    qnames;

            for (auto& q : _device.GetQueues()) {
                qnames.emplace( q.handle, q.debugName );
            }
            VulkanSyncLog::Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
            VulkanSyncLog::Enable();
        }
        #endif

      #elif defined(AE_ENABLE_METAL) and defined(AE_ENABLE_REMOTE_GRAPHICS)
        CHECK_ERR( _device.Init( _config.graphics ));

      #else
      # error not implemented
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
    void  AppCoreV2::_DestroyGraphics ()
    {
        if_unlikely( not _device.IsInitialized() )
            return;

        #if ENABLE_SYNC_LOG
            VulkanSyncLog::Deinitialize( INOUT _device.EditDeviceFnTable() );
        #endif

        Unused( GraphicsScheduler().WaitAll( AE::DefaultTimeout )); // TODO ???

        RenderTaskScheduler::InstanceCtor::Destroy();

        CHECK_ERRV( _device.DestroyLogicalDevice() );
        CHECK_ERRV( _device.DestroyInstance() );
    }

/*
=================================================
    OnStateChanged
=================================================
*/
    void  AppCoreV2::WindowEventListener::OnStateChanged (IWindow &wnd, EState state) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( state )
        {
            case EState::InForeground :
            case EState::Focused :
                _core->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), state );
                break;

            case EState::InBackground :
            case EState::Stopped :
            case EState::Destroyed :
                _core->StopRendering( &wnd.GetSurface() );
                _core->WaitFrame( _app.GetMainThreadMask() );
                break;

            case EState::Created :
            case EState::Unknown :
            case EState::Started :
                break;

            default :
                DBG_WARNING( "unsupported window state" );
                break;
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    OnSurfaceCreated
=================================================
*/
    void  AppCoreV2::WindowEventListener::OnSurfaceCreated (IWindow &wnd) __NE___
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
    void  AppCoreV2::WindowEventListener::OnSurfaceDestroyed (IWindow &) __NE___
    {
        _core->StopRendering( null );
        _core->WaitFrame( _app.GetMainThreadMask() );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    OnStateChanged
=================================================
*/
    void  AppCoreV2::VRDeviceEventListener::OnStateChanged (IVRDevice &vr, EState state) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( state )
        {
            case EState::InForeground :
            case EState::Focused :
                _core->StartRendering( &vr.InputActions(), &vr.GetSurface(), state );
                break;

            case EState::InBackground :
            case EState::Stopped :
            case EState::Destroyed :
                _core->StopRendering( &vr.GetSurface() );
                _core->WaitFrame( _app.GetMainThreadMask() );
                break;

            case EState::Created :
            case EState::Unknown :
            case EState::Started :
                break;

            default :
                DBG_WARNING( "unsupported VR state" );
                break;
        }
        END_ENUM_CHECKS();
    }


} // AE::AppV2
