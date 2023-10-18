// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    State chain:
        Created -> Destroyed
        Created -> Started -> Stopped -> Destroyed
        Created -> Started -> InForeground -> InBackground -> Stopped -> Destroyed
        Created -> Started -> InForeground -> Focused -> InBackground -> Stopped -> Destroyed
*/

#pragma once

#include "graphics/Public/SwapchainDesc.h"
#include "graphics/Public/NativeWindow.h"

#include "platform/Public/Monitor.h"
#include "platform/Public/InputActions.h"

namespace AE::App
{
    using Graphics::NativeWindow;


    enum class EWindowMode : ubyte
    {
        Resizable,
        NonResizable,
        Borderless,

        FullscreenWindow,       // borderless, always on top
        Fullscreen,

        _Count,
        Unknown         = Resizable,
    };

    ND_ constexpr bool  EWindowMode_IsFullscreen (EWindowMode value) __NE___ { return value >= EWindowMode::FullscreenWindow; }


    //
    // Window Description
    //

    struct WindowDesc
    {
        String          title;
        uint2           size        = {640, 480};
        Monitor::ID     monitorId   = Default;
        EWindowMode     mode        = Default;

        WindowDesc () __NE___ {}
    };



    //
    // Window interface
    //

    class IWindow
    {
    // types
    public:
        class IWndListener
        {
        // types
        public:
            enum class EState
            {
                Unknown,
                Created,
                Started,
                InForeground,   // Unfocused ?
                Focused,
                InBackground,
                Stopped,
                Destroyed,
            };

        // interface
        public:
            virtual ~IWndListener ()                                        __NE___ {}

            virtual void  OnSurfaceCreated (IWindow &wnd)                   __NE___ = 0;
            virtual void  OnSurfaceDestroyed (IWindow &wnd)                 __NE___ = 0;

            virtual void  OnStateChanged (IWindow &wnd, EState state)       __NE___ = 0;
        };

        using EState = IWndListener::EState;


    // interface
    public:
        virtual ~IWindow ()                                                         __NE___ {}

        // Close window.
        //   Thread safe: no
        //
        virtual void  Close ()                                                      __NE___ = 0;

        // Return surface size.
        //   Note: window size with border will be greater then surface size.
        //   Thread safe: main thread only
        //
        ND_ virtual uint2  GetSurfaceSize ()                                        C_NE___ = 0;

        // Returns current state.
        //   Thread safe: no
        //
        ND_ virtual EState  GetState ()                                             C_NE___ = 0;

        // Returns current monitor for window.
        //   Thread safe: main thread only
        //
        ND_ virtual Monitor  GetMonitor ()                                          C_NE___ = 0;

        // Returns window native handles.
        //   Thread safe: yes
        //
        ND_ virtual NativeWindow  GetNative ()                                      C_NE___ = 0;

        // Returns input actions class.
        //   Thread safe: yes
        //
        ND_ virtual IInputActions&  InputActions ()                                 __NE___ = 0;

        // Returns current window mode.
        //   Thread safe: no
        //
        ND_ virtual EWindowMode  GetCurrentMode ()                                  C_NE___ = 0;


    // surface api

        // Create or recreate rendering surface (swapchain).
        //   Thread safe: main thread only, must be synchronized with 'GetSurface()'
        //
        ND_ virtual bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc) __NE___ = 0;

        // Returns render surface reference.
        // Surface must be successfully created using 'CreateRenderSurface()'.
        //   Thread safe: must be synchronized with 'CreateRenderSurface()'
        //
        ND_ virtual IOutputSurface&  GetSurface ()                                  __NE___ = 0;


    // desktop only

        // Set focus to the window.
        //   Thread safe: main thread only
        //
        virtual void  SetFocus ()                                                   C_NE___ = 0;

        // Set surface size.
        //   Thread safe: main thread only
        //
        virtual void  SetSize (const uint2 &size)                                   __NE___ = 0;

        // Set window position.
        //   Thread safe: main thread only
        //
        virtual void  SetPosition (const int2 &pos)                                 __NE___ = 0;
        virtual void  SetPosition (Monitor::ID monitor, const int2 &pos)            __NE___ = 0;

        // Set window title.
        //   Thread safe: main thread only
        //
        virtual void  SetTitle (NtStringView title)                                 __NE___ = 0;

        // Set window mode windowed/fullscreen.
        //   Thread safe: main thread only
        //
        ND_ virtual bool  SetMode (EWindowMode mode, Monitor::ID monitor = Default) __NE___ = 0;
    };


} // AE::App
