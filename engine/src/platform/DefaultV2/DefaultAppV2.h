// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IApplication.h"
#include "platform/DefaultV1/AppConfig.h"
#include "platform/DefaultV2/IViewMode.h"

#if defined(AE_ENABLE_VULKAN)
#   include "graphics/Vulkan/VDevice.h"
#   include "VulkanSyncLog.h"

#elif defined(AE_ENABLE_METAL)
#   include "graphics/Metal/MDevice.h"

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   include "graphics/Remote/RDevice.h"

#else
#   error not implemented
#endif


namespace AE::Base { class MemRStream; }

namespace AE::AppV2
{
    using namespace AE::App;
    using AppConfig = AE::AppV1::AppConfig;


    //
    // Application Core
    //
    class AppCore : public EnableRC<AppCore>
    {
    // types
    protected:
        struct MainLoopData
        {
            Ptr<IInputActions>      input;      // lifetime is same as Window/VRDevice lifetime
            Ptr<IOutputSurface>     output;     // lifetime is same as Window/VRDevice lifetime
            RC<IViewMode>           mode;
            AsyncTask               endFrame;
        };
        using MainLoopDataSync  = Threading::Synchronized< SharedMutex, MainLoopData >;

        using ViewModePtr       = RC<IViewMode>;
        using ViewModeName      = App::ViewModeName;


    // variables
    protected:
        MainLoopDataSync    _mainLoop;
        RC<MemRStream>      _inputActionsData;

        // TODO: UI


    // methods
    public:
        AppCore ()                                                                  __NE___;
        ~AppCore ()                                                                 __NE_OV;

        // user api
            void  OpenView (const ViewModeName &)                                   __NE___;

        ND_ Ptr<IInputActions>  GetInputActions ()                                  C_NE___ { return _mainLoop->input; }
        ND_ Ptr<IOutputSurface> GetOutputSurface ()                                 C_NE___ { return _mainLoop->output; }


        // for DefaultAppListener
        ND_ virtual bool  OnStart (IApplication &)                                  __NE___;


        // for DefaultIWndListener & DefaultVRDeviceListener
        ND_ bool  OnSurfaceCreated (IOutputSurface &)                               __NE___;
            void  SurfaceDestroyed ()                                               __NE___;

            void  InitInputActions (IInputActions &)                                __NE___;

            void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>)          __NE___;
            void  StopRendering ()                                                  __NE___;

            void  WaitFrame (const Threading::EThreadArray &)                       __NE___;
            void  RenderFrame ()                                                    __NE___;


    protected:
        ND_ bool  _InitVFS (IApplication &)                                         __NE___;
        ND_ bool  _LoadInputActions ()                                              __NE___;

        ND_ virtual AsyncTask   _DrawFrame ()                                       __NE___;
            static  AsyncTask   _SetInputMode (Ptr<IInputActions>, InputModeName)   __NE___;

        ND_ virtual ViewModePtr _CreateViewMode (const ViewModeName &)              __NE___ = 0;
    };



    //
    // Application Event Listener
    //
    class DefaultAppListener final : public IApplication::IAppListener
    {
    // types
    private:
        using AppCoreCtor_t = Function< RC<AppCore> () >;


    // variables
    private:
      #if defined(AE_ENABLE_VULKAN)
        Graphics::VDeviceInitializer    _vulkan;

      #elif defined(AE_ENABLE_METAL)
        Graphics::MDeviceInitializer    _metal;

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        Graphics::RDeviceInitializer    _remote;

      #else
      # error not implemented
      #endif

        Array<WindowPtr>            _windows;
        VRDevicePtr                 _vrDevice;
        RC<AppCore>                 _core;

        const AppConfig             _config;
        Threading::EThreadArray     _allowProcessInMain;


    // methods
    public:
        DefaultAppListener (const AppConfig &, RC<AppCore>)     __NE___;
        DefaultAppListener (const AppConfig &, AppCoreCtor_t)   __NE___;
        ~DefaultAppListener ()                                  __NE_OV;

        ND_ AppConfig const&    Config ()                       C_NE___ { return _config; }
        //ND_ AppCore &         GetBaseApp ()                   __NE___ { return *_core; }


    // IAppListener //
        void  OnStart (IApplication &)                          __NE_OV;
        void  OnStop  (IApplication &)                          __NE_OV;

        void  BeforeWndUpdate (IApplication &)                  __NE_OV;
        void  AfterWndUpdate (IApplication &)                   __NE_OV;


    private:
        ND_ bool  _CreateWindow (IApplication &);
        ND_ bool  _InitGraphics (IApplication &);

      #if defined(AE_ENABLE_VULKAN)
        ND_ bool  _CreateVulkan (IApplication &app);
        ND_ bool  _DestroyVulkan ();

      #elif defined(AE_ENABLE_METAL)
        ND_ bool  _CreateMetal (IApplication &app);
        ND_ bool  _DestroyMetal ();

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        ND_ bool  _CreateRemoteGraphics (IApplication &app);
        ND_ bool  _DestroyRemoteGraphics ();

      #else
      # error not implemented
      #endif
    };



    //
    // Window Event Listener
    //
    class DefaultIWndListener final : public IWindow::IWndListener
    {
    // variables
    private:
        RC<AppCore>             _core;
        DefaultAppListener &    _app;


    // methods
    public:
        DefaultIWndListener (RC<AppCore> core, DefaultAppListener &app) __NE___ :
            _core{RVRef(core)}, _app{app}
        {}


    // IWndListener //
        void  OnStateChanged (IWindow &, EState)    __NE_OV;
        void  OnUpdate (IWindow &)                  __NE_OV {}

        void  OnResize (IWindow &, const uint2 &)   __NE_OV {}
        void  OnSurfaceCreated (IWindow &)          __NE_OV;
        void  OnSurfaceDestroyed (IWindow &)        __NE_OV;
    };



    //
    // VR Device Listener
    //
    class DefaultVRDeviceListener final : public IVRDevice::IVRDeviceEventListener
    {
    // variables
    private:
        RC<AppCore>             _core;
        DefaultAppListener &    _app;


    // methods
    public:
        DefaultVRDeviceListener (RC<AppCore> core, DefaultAppListener &app) __NE___ :
            _core{RVRef(core)}, _app{app}
        {}


    // IVRDeviceEventListener //
        void  OnUpdate (IVRDevice &)                        __NE_OV {}
        void  OnStateChanged (IVRDevice &, EState state)    __NE_OV;
    };


} // AE::AppV2
