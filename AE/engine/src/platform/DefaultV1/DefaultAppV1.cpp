// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/DefaultV1/DefaultAppV1.h"

#include "platform/Android/AndroidCommon.h"
#include "platform/GLFW/GLFWCommon.h"
#include "platform/WinAPI/WinAPICommon.h"
#include "platform/OpenVR/OpenVRCommon.h"

#define ENABLE_SYNC_LOG     0
#if ENABLE_SYNC_LOG
#   include "VulkanSyncLog.h"
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
    DefaultAppListener::DefaultAppListener (const AppConfig &cfg, RC<IBaseApp> impl) __NE___ :
        #if defined(AE_ENABLE_VULKAN)
            _vulkan{ True{"enable info log"} },
        #elif defined(AE_ENABLE_METAL)
            _metal{ True{"enable info log"} },
        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            _remote{ True{"enable info log"} },
        #else
        #   error not implemented
        #endif
        _impl{ RVRef(impl) },
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

        CHECK_FATAL( ThreadMngr::SetupThreads( _config.threading,
                                               _config.threading.mask,
                                               null,
                                               _config.threading.maxThreads,
                                               OUT _allowProcessInMain ));
    }

/*
=================================================
    destructor
=================================================
*/
    DefaultAppListener::~DefaultAppListener () __NE___
    {
        _windows.clear();

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

        _impl->WaitFrame( _allowProcessInMain );
    }

/*
=================================================
    AfterWndUpdate
=================================================
*/
    void  DefaultAppListener::AfterWndUpdate (IApplication &app) __NE___
    {
        _impl->RenderFrame();

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
      #if defined(AE_ENABLE_VULKAN)
        CHECK_FATAL( _CreateVulkan( app ));

      #elif defined(AE_ENABLE_METAL)
        CHECK_FATAL( _CreateMetal( app ));

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        CHECK_FATAL( _CreateRemoteGraphics( app ));

      #else
      # error not implemented
      #endif
    }

/*
=================================================
    _OnStartImpl
=================================================
*/
    bool  DefaultAppListener::_OnStartImpl (IApplication &app) __NE___
    {
        // create window
        {
            auto    wnd = app.CreateWindow( MakeUnique<DefaultIWndListener>( _impl, *this ), _config.window );
            CHECK_ERR( wnd );
            _windows.emplace_back( RVRef(wnd) );
        }

        // create second window
        #if 0
        {
            WindowDesc  desc = _config.window;
            desc.title << "-2";

            auto    wnd = app.CreateWindow( MakeUnique<DefaultIWndListener>( _impl, *this ), desc, &_windows[0]->InputActions() );
            CHECK_ERR( wnd );
            _windows.emplace_back( RVRef(wnd) );
        }
        #endif

        // create VR device
        if ( _config.enableVR and _windows.size() >= 1 )
        {
            const auto  CreateVR = [this, &app] (IVRDevice::EDeviceType type) -> bool
            {{
                _vrDevice = app.CreateVRDevice( MakeUnique<DefaultVRDeviceListener>( _impl, *this ), &_windows[0]->InputActions(), type );
                return _vrDevice and _vrDevice->CreateRenderSurface( _config.vr );
            }};

            ASSERT( not _config.vrDevices.empty() );

            for (auto type : _config.vrDevices) {
                if ( CreateVR( type )) break;
            }

            if ( _vrDevice )
            {
                IVRDevice::Settings     settings;
                settings.cameraClipPlanes   = { 0.1f, 100.f };

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
    void  DefaultAppListener::OnStop (IApplication &) __NE___
    {
        if ( _impl )
        {
            _impl->WaitFrame( _allowProcessInMain );
            _impl = null;
        }
        _windows.clear();

      #if defined(AE_ENABLE_VULKAN)
        _DestroyVulkan();

      #elif defined(AE_ENABLE_METAL)
        _DestroyMetal();

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        _DestroyRemoteGraphics();

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
            case EState::Focused :          _impl->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), state ); break;

            case EState::InBackground :
            case EState::Stopped :
            case EState::Destroyed :        _impl->StopRendering( &wnd.GetSurface() );                              break;

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

        _impl->StartRendering( &wnd.InputActions(), &wnd.GetSurface(), EState::InForeground );

        CHECK_FATAL( _impl->OnSurfaceCreated( wnd.GetSurface() ));
    }

/*
=================================================
    OnSurfaceDestroyed
=================================================
*/
    void  DefaultIWndListener::OnSurfaceDestroyed (IWindow &) __NE___
    {
        _impl->StopRendering( null );
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
            case EState::Focused :          _impl->StartRendering( &vr.InputActions(), &vr.GetSurface(), state );   break;

            case EState::InBackground :
            case EState::Stopped :
            case EState::Destroyed :        _impl->StopRendering( &vr.GetSurface() );                               break;

            case EState::Created :
            case EState::Unknown :
            case EState::Started :                                                                                  break;

            default :                       DBG_WARNING( "unsupported VR state" );                                  break;
        }
        END_ENUM_CHECKS();
    }


} // AE::AppV1
