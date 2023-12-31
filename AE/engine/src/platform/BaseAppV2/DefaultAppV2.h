// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IApplication.h"
#include "platform/BaseAppV1/AppConfig.h"
#include "platform/BaseAppV2/IViewMode.h"

#if defined(AE_ENABLE_VULKAN)
#   include "graphics/Vulkan/VDevice.h"

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
        };
        using MainLoopDataSync  = Threading::DRCSynchronized< MainLoopData >;

        using ViewModePtr       = RC<IViewMode>;
        using ViewModeName      = App::ViewModeName;


    // variables
    protected:
        MainLoopDataSync    _mainLoop;
        RC<MemRStream>      _inputActionsData;

        // TODO: UI


    // methods
    public:
        AppCore ()                                                                                  __NE___;
        ~AppCore ()                                                                                 __NE_OV;

    // user api

        // Open view by name.
        //  Thread-safe: main thread only
        //
            void  OpenView (const ViewModeName &)                                                   __NE___;

        ND_ Ptr<IInputActions>  GetInputActions ()                                                  C_NE___ { return _mainLoop->input; }
        ND_ Ptr<IOutputSurface> GetOutputSurface ()                                                 C_NE___ { return _mainLoop->output; }


    // for AppCoreV2
        ND_ virtual bool  OnStart (IApplication &)                                                  __NE___;


    // for DefaultIWndListener & DefaultVRDeviceListener
        ND_         bool  OnSurfaceCreated (IOutputSurface &)                                       __NE___;

                    void  StartRendering (Ptr<IInputActions>, Ptr<IOutputSurface>, IWindow::EState) __NE___;
                    void  StopRendering (Ptr<IOutputSurface>)                                       __NE___;

                    void  WaitFrame (const Threading::EThreadArray &)                               __NE___;
            virtual void  RenderFrame ()                                                            __NE___;


    protected:
        ND_ bool  _InitVFS (IApplication &)                                                         __NE___;
        ND_ bool  _LoadInputActions ()                                                              __NE___;

            void  _InitInputActions (IInputActions &)                                               __NE___;

            static  AsyncTask   _SetInputMode (Ptr<IInputActions>, InputModeName)                   __NE___;

        ND_ virtual ViewModePtr _CreateViewMode (const ViewModeName &)                              __NE___ = 0;
    };



    //
    // Application Event Listener
    //
    class AppCoreV2 final : public IApplication::IAppListener
    {
    // types
    private:
        class WindowEventListener;
        class VRDeviceEventListener;

    public:
        using AppCoreCtor_t = Function< RC<AppCore> () >;


    // variables
    private:
      #if defined(AE_ENABLE_VULKAN)
        Graphics::VDeviceInitializer    _device;

      #elif defined(AE_ENABLE_METAL)
        Graphics::MDeviceInitializer    _device;

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        Graphics::RDeviceInitializer    _device;

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
        AppCoreV2 (const AppConfig &, RC<AppCore>)      __NE___;
        AppCoreV2 (const AppConfig &, AppCoreCtor_t)    __NE___;
        ~AppCoreV2 ()                                   __NE_OV;

        ND_ AppConfig const&    Config ()                       C_NE___ { return _config; }
        //ND_ AppCore &         GetBaseApp ()                   __NE___ { return *_core; }
        ND_ auto const&         GetMainThreadMask ()            C_NE___ { return _allowProcessInMain; }


    // IAppListener //
        void  OnStart (IApplication &)                          __NE_OV;
        void  OnStop  (IApplication &)                          __NE_OV;

        void  BeforeWndUpdate (IApplication &)                  __NE_OV;
        void  AfterWndUpdate (IApplication &)                   __NE_OV;


    private:
        ND_ bool  _CreateWindow (IApplication &);
        ND_ bool  _InitGraphics (IApplication &);
            void  _DestroyGraphics ();
    };



    //
    // Window Event Listener
    //
    class AppCoreV2::WindowEventListener final : public IWindow::IWndListener
    {
    // variables
    private:
        RC<AppCore>     _core;
        AppCoreV2 &     _app;


    // methods
    public:
        WindowEventListener (RC<AppCore> core, AppCoreV2 &app) __NE___ :
            _core{RVRef(core)}, _app{app}
        {}


    // IWndListener //
        void  OnStateChanged (IWindow &, EState)    __NE_OV;

        void  OnSurfaceCreated (IWindow &)          __NE_OV;
        void  OnSurfaceDestroyed (IWindow &)        __NE_OV;
    };



    //
    // VR Device Listener
    //
    class AppCoreV2::VRDeviceEventListener final : public IVRDevice::IVRDeviceEventListener
    {
    // variables
    private:
        RC<AppCore>     _core;
        AppCoreV2 &     _app;


    // methods
    public:
        VRDeviceEventListener (RC<AppCore> core, AppCoreV2 &app) __NE___ :
            _core{RVRef(core)}, _app{app}
        {}


    // IVRDeviceEventListener //
        void  OnStateChanged (IVRDevice &, EState state)    __NE_OV;
    };


} // AE::AppV2
