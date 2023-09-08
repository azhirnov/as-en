// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/GLFW/GLFWCommon.h"

#ifdef AE_ENABLE_GLFW
# include "platform/Public/IApplication.h"
# include "platform/GLFW/WindowGLFW.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

    //
    // GLFW Application
    //

    class ApplicationGLFW final : public ApplicationBase
    {
    // types
    private:
        using Window        = WeakPtr< WindowGLFW >;
        using WindowArray_t = FixedArray< Window, PlatformConfig::MaxWindows >;


    // variables
    private:
        RecursiveMutex      _windowsGuard;      // TODO: can be removed
        WindowArray_t       _windows;

        Monitors_t          _cachedMonitors;

        Locales_t           _locales;


    // methods
    public:
        static int   Run (Unique<IAppListener>)                                         __NE___;
        static bool  GetMonitorInfo (GLFWmonitor*, OUT Monitor &)                       __NE___;


    // IApplication //
        WindowPtr   CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)   __NE_OV;

        Monitors_t  GetMonitors (bool update = false)                                   __NE_OV;

        StringView  GetApiName ()                                                       C_NE_OV { return "glfw"; }

        Locales_t   GetLocales ()                                                       C_NE_OV { return _locales; }

        RC<IVirtualFileStorage> OpenBuiltinStorage ()                                   __NE_OV { return null; }

        ArrayView<const char*>  GetVulkanInstanceExtensions ()                          __NE_OV;


    private:
        explicit ApplicationGLFW (Unique<IAppListener>)                                 __NE___;
        ~ApplicationGLFW ()                                                             __NE___;

        void  _MainLoop ()                                                              __NE___;

        void  _UpdateMinitors (OUT Monitors_t &)                                        C_NE___;
        void  _GetLocales (OUT Locales_t &)                                             C_NE___;
    };


} // AE::App

#endif // AE_ENABLE_GLFW
