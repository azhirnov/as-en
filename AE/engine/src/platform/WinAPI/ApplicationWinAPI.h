// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/WinAPI/WinAPICommon.h"

#ifdef AE_WINAPI_WINDOW
# include "platform/Private/ApplicationBase.h"
# include "platform/WinAPI/WindowWinAPI.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

    //
    // WinAPI Application
    //

    class ApplicationWinAPI final : public ApplicationBase
    {
    // types
    private:
        using Window        = WeakPtr< WindowWinAPI >;
        using WindowArray_t = FixedArray< Window, PlatformConfig::MaxWindows >;


    // variables
    private:
        RecursiveMutex      _windowsGuard;      // TODO: can be removed
        WindowArray_t       _windows;

        void *              _instance;      // HMODULE
        String              _className;

        Monitors_t          _cachedMonitors;

        Locales_t           _locales;


    // methods
    public:

        StringView  GetClassName ()                                                     C_NE___ { return _className; }
        void*       GetHInstance ()                                                     C_NE___ { return _instance; }

        static int  Run (Unique<IAppListener>)                                          __NE___;


    // IApplication //
        WindowPtr   CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)   __NE_OV;

        Monitors_t  GetMonitors (bool update = false)                                   __NE_OV;

        StringView  GetApiName ()                                                       C_NE_OV { return "winapi"; }

        Locales_t   GetLocales ()                                                       C_NE_OV { return _locales; }

        RC<IVirtualFileStorage> OpenStorage (EAppStorage)                               __NE_OV { return null; }

        ArrayView<const char*>  GetVulkanInstanceExtensions ()                          __NE_OV;


    private:
        explicit ApplicationWinAPI (Unique<IAppListener>)                               __NE___;
        ~ApplicationWinAPI ()                                                           __NE___;

        ND_ bool  _RegisterClass ()                                                     __NE___;
            void  _MainLoop ()                                                          __NE___;

            void  _UpdateMonitors (OUT Monitors_t &)                                    C_NE___;
            void  _GetLocales (OUT Locales_t &)                                         C_NE___;
    };


} // AE::App

#endif // AE_WINAPI_WINDOW
