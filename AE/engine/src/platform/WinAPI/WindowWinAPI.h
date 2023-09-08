// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/WinAPI/WinAPICommon.h"

#ifdef AE_WINAPI_WINDOW
# include "platform/Private/WindowBase.h"
# include "platform/Private/WindowSurface.h"
# include "platform/WinAPI/InputActionsWinAPI.h"

namespace AE::App
{

    //
    // WinAPI Window
    //

    class WindowWinAPI final : public WindowBase
    {
        friend class ApplicationWinAPI;

    // types
    private:
        enum class EVisibility
        {
            VisibleFocused,
            VisibleUnfocused,
            Invisible,
        };


    // variables
    private:
        void*               _wnd                = null;     // HWND
        uint2               _surfaceSize;
        int2                _windowPos;
        bool                _requestQuit        = false;
        bool                _lockAndHideCursor  = false;
        void*               _cursorHandle       = null;     // HCURSOR
        EWindowMode         _wndMode            = Default;

        InputActionsWinAPI  _input;


    // methods
    public:
        ~WindowWinAPI ()                                                    __NE_OV;

    // IWindow //
        void        Close ()                                                __NE_OV;

        uint2       GetSurfaceSize ()                                       C_NE_OV;
        Monitor     GetMonitor ()                                           C_NE_OV;

        IInputActions&  InputActions ()                                     __NE_OV { return _input; }
        NativeWindow    GetNative ()                                        C_NE_OV;

        void  SetSize (const uint2 &size)                                   __NE_OV;
        void  SetPosition (const int2 &pos)                                 __NE_OV;
        void  SetPosition (Monitor::ID monitor, const int2 &pos)            __NE_OV;
        void  SetTitle (NtStringView title)                                 __NE_OV;
        void  SetFocus ()                                                   C_NE_OV;


    private:
        explicit WindowWinAPI (ApplicationWinAPI &, Unique<IWndListener>, IInputActions*) __NE___;

        ND_ ApplicationWinAPI&  _GetApp ()                                  C_NE___;

        ND_ bool  _Create (const WindowDesc &desc)                          __NE___;
            void  _Destroy ()                                               __NE___;
        ND_ bool  _Update ()                                                __NE___;
        ND_ ssize _ProcessMessage (uint uMsg, usize wParam, ssize lParam)   __NE___;
            void  _UpdateDescription ()                                     __NE___;
            void  _ShowWindow (EVisibility value)                           C_NE___;

            void  _LockAndHideCursor (bool value)                           __NE___;
            void  _ClipCursor ()                                            __NE___;
    };


} // AE::App

#endif // AE_WINAPI_WINDOW
