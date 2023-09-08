// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_WINAPI_WINDOW
# include "base/Platforms/WindowsHeader.h"
# include "platform/WinAPI/WindowWinAPI.h"
# include "platform/WinAPI/ApplicationWinAPI.h"

namespace AE::App
{

/*
=================================================
    constructor
=================================================
*/
    WindowWinAPI::WindowWinAPI (ApplicationWinAPI &app, Unique<IWndListener> listener, IInputActions* dstActions) __NE___ :
        WindowBase{ app, RVRef(listener) },
        _input{ InputActionsBase::GetQueue( dstActions )}
    {}

/*
=================================================
    destructor
=================================================
*/
    WindowWinAPI::~WindowWinAPI () __NE___
    {
        DRC_EXLOCK( _drCheck );
        _Destroy();
    }

/*
=================================================
    _GetApp
=================================================
*/
    forceinline ApplicationWinAPI&  WindowWinAPI::_GetApp () C_NE___
    {
        return static_cast< ApplicationWinAPI &>( _app );
    }

/*
=================================================
    _Create
=================================================
*/
    bool  WindowWinAPI::_Create (const WindowDesc &desc) __NE___
    {
        DRC_EXLOCK( _drCheck );
        DRC_EXLOCK( _app.GetSingleThreadCheck() );
        CHECK_ERR( _wnd == null );

        const auto      monitors    = _app.GetMonitors();
        Monitor const*  cur_mon     = null;

        for (auto& mon : monitors)
        {
            if ( mon.id == desc.monitorId )
            {
                cur_mon = &mon;
                break;
            }
        }

        if ( cur_mon == null )
            cur_mon = &monitors.front();

        DWORD       wnd_style       = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        DWORD       wnd_ext_style   = WS_EX_APPWINDOW;
        uint2 const scr_res         = uint2{cur_mon->region.pixels.Size()};
        uint2       wnd_size;
        int2        wnd_pos;

        bool        resizable       = false;
        bool        borderless      = false;
        bool        fullscreen      = false;
        bool        always_on_top   = false;

        BEGIN_ENUM_CHECKS();
        switch ( desc.mode )
        {
            case EWindowMode::Resizable :           resizable = true;                           break;
            case EWindowMode::NonResizable :                                                    break;
            case EWindowMode::Borderless :          borderless = true;                          break;
            case EWindowMode::FullscreenWindow :    borderless = true;  always_on_top = true;   break;
            case EWindowMode::Fullscreen :          fullscreen = true;                          break;
            case EWindowMode::_Count :
            default :                                                                           break;
        }
        END_ENUM_CHECKS();

        if ( resizable )
            wnd_style |= WS_OVERLAPPEDWINDOW;
        else
        if ( borderless ) {
            wnd_style |= WS_POPUP | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX;
        }else
            wnd_style |= WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;

        if ( fullscreen or always_on_top )
        {
            // setup for fullscreen
            wnd_style       |= WS_POPUP;
            wnd_ext_style   |= WS_EX_TOPMOST;
            wnd_pos         = cur_mon->region.pixels.LeftTop();
            wnd_size        = scr_res;
        }
        else
        {
            // setup windowed
            RECT    wnd_rect{ 0, 0, int(desc.size.x), int(desc.size.y) };
            ::AdjustWindowRectEx( INOUT &wnd_rect, wnd_style, FALSE, wnd_ext_style );   // win2000

            wnd_size    = Min( desc.size, uint2{ wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top });
            wnd_pos     = cur_mon->workArea.pixels.Center() - int2{wnd_size} / 2;
        }

        _wndMode = desc.mode;

        _wnd = ::CreateWindowExA( wnd_ext_style,                                        // win2000
                                  NtStringView{_GetApp().GetClassName()}.c_str(),
                                  desc.title.c_str(),
                                  wnd_style,
                                  wnd_pos.x,
                                  wnd_pos.y,
                                  wnd_size.x,
                                  wnd_size.y,
                                  null,
                                  null,
                                  HINSTANCE(_GetApp().GetHInstance()),
                                  null );
        CHECK_ERR( _wnd != null );

        _surfaceSize    = desc.size;
        _cursorHandle   = ::LoadCursorW( null, IDC_ARROW ); // win2000
        _windowPos      = wnd_pos;

        _input.SetMonitor( _surfaceSize, GetMonitor() );
        CHECK_ERR( _input.Register( BitCast<HWND>(_wnd) ));

        ::SetCursor( BitCast<HCURSOR>( _cursorHandle ));    // win2000

        struct Utils {
            static LRESULT CALLBACK MsgProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
            {
                auto*   wp = BitCast<WindowWinAPI *>(::GetWindowLongPtrA( hWnd, GWLP_USERDATA ));   // win2000
                if_likely( wp != null )
                {
                    ASSERT( wp->_wnd == hWnd );
                    return LRESULT{wp->_ProcessMessage( uMsg, wParam, lParam )};
                }
                else
                    return ::DefWindowProcA( hWnd, uMsg, wParam, lParam );
            }
        };
        ::SetWindowLongPtrA( BitCast<HWND>(_wnd), GWLP_USERDATA, BitCast<LONG_PTR>( this ));            // win2000
        ::SetWindowLongPtrA( BitCast<HWND>(_wnd), GWLP_WNDPROC,  BitCast<LONG_PTR>( &Utils::MsgProc )); // win2000

        // can call 'Close()' and '_wnd' will be null

        _SetStateV2( EState::Created );
        _SetStateV2( EState::Started );
        _SetStateV2( EState::InForeground );

        _ShowWindow( EVisibility::VisibleFocused );

        return true;
    }

/*
=================================================
    _ShowWindow
=================================================
*/
    void  WindowWinAPI::_ShowWindow (EVisibility value) C_NE___
    {
        if_unlikely( _wnd == null )
            return;

        HWND    hwnd = BitCast<HWND>(_wnd)

        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case EVisibility::VisibleFocused :
                ::SetForegroundWindow( hwnd );              // win2000
                ::ShowWindow( hwnd, SW_SHOWNA );            // win2000
                break;

            case EVisibility::VisibleUnfocused :
                ::ShowWindow( hwnd, SW_SHOWNOACTIVATE );    // win2000
                break;

            case EVisibility::Invisible :
                ::ShowWindow( hwnd, SW_FORCEMINIMIZE );     // win2000
                break;

            default :
                DBG_WARNING( "unknown visibility flags" );
        }
        END_ENUM_CHECKS();
    }

/*
=================================================
    _Destroy
=================================================
*/
    void  WindowWinAPI::_Destroy () __NE___
    {
        if ( _wnd == null )
            return;

        _input.Unregister();

        ::SetWindowLongPtrA( BitCast<HWND>(_wnd), GWLP_WNDPROC, BitCast<LONG_PTR>(&::DefWindowProcA) ); // win2000

        ::DestroyWindow( BitCast<HWND>(_wnd) ); // win2000
        _wnd = null;

        _cursorHandle = null;   // TODO: release handle ?

        _DestroyListener();
    }

/*
=================================================
    _Update
=================================================
*/
    bool  WindowWinAPI::_Update () __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_unlikely( _requestQuit )
            return false;

        _input.Update( _app.GetTimeSinceStart() );

        _LockAndHideCursor( _input.RequiresLockAndHideCursor() );

        if_likely( _listener )
            _listener->OnUpdate( *this );

        return true;
    }

/*
=================================================
    _LockAndHideCursor
=================================================
*/
    void  WindowWinAPI::_LockAndHideCursor (bool value) __NE___
    {
        if_unlikely( _lockAndHideCursor != value )
        {
            _lockAndHideCursor = value;

            if ( _lockAndHideCursor )
            {
                ::ClipCursor( null );   // win2000
                ::SetCursor( null );    // win2000
            }
            else
            {
                _ClipCursor();
                ::SetCursor( BitCast<HCURSOR>( _cursorHandle ));    // win2000
            }
        }

        // move cursor to center
        if ( _lockAndHideCursor )
        {
            const int2  center = _windowPos + int2(_surfaceSize) / 2;

            ::SetCursorPos( center.x, center.y );   // win2000

            _input.CursorPosChanged( float2{center} );
        }
    }

/*
=================================================
    _ClipCursor
=================================================
*/
    void  WindowWinAPI::_ClipCursor () __NE___
    {
        if ( _lockAndHideCursor )
        {
            RECT    client_rect = {};
            ::GetClientRect( BitCast<HWND>(_wnd), OUT &client_rect );   // win2000

            ::ClipCursor( &client_rect );   // win2000
        }
    }

/*
=================================================
    _ProcessMessage
=================================================
*/
    ssize  WindowWinAPI::_ProcessMessage (uint uMsg, usize wParam, ssize lParam) __NE___
    {
        DRC_EXLOCK( _drCheck );

        if_likely( not _requestQuit )
        {
            switch ( uMsg )
            {
                case WM_KILLFOCUS :     _SetStateV2( EState::InForeground );                            break;
                case WM_ACTIVATEAPP :   _SetStateV2( wParam ? EState::Focused : EState::InBackground ); break;
                case WM_SETFOCUS :      _SetStateV2( EState::Focused );                                 break;

                case WM_DISPLAYCHANGE :
                {
                    // In window mode there is a bug with incorrect render area.
                    // Force to update render area by changing window position.
                    // Borderless and fullscreen windows doesn't have this bug. 
                    if ( AnyEqual( _wndMode, EWindowMode::NonResizable, EWindowMode::Resizable ))
                    {
                        _windowPos.x += (IsEven( _windowPos.x ) ? +1 : -1);

                        ::SetWindowPos( BitCast<HWND>(_wnd), HWND_TOP,          // win2000
                                        _windowPos.x, _windowPos.y, 0, 0,
                                        SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOCOPYBITS );
                    }
                    _UpdateDescription();
                    break;
                }

                case WM_WINDOWPOSCHANGED :
                {
                    auto*   wndpos  = BitCast<WINDOWPOS*>(usize(lParam));
                    _windowPos      = int2{ wndpos->x, wndpos->y };
                    _UpdateDescription();
                    return 0;   // WM_SIZE and WM_MOVE will not send
                }

                // TODO
                //case WM_DPICHANGED :

                case WM_SETCURSOR :
                {
                    if ( LOWORD(lParam) == HTCLIENT and _lockAndHideCursor )
                    {
                        ::SetCursor( null );
                        return 1;
                    }
                    break;
                }

                case WM_SYSCOMMAND :
                {
                    if ( wParam == SC_SCREENSAVE or wParam == SC_MONITORPOWER )
                        return 1;

                    if ( wParam == SC_RESTORE )
                        _UpdateDescription();

                    break;
                }

                //case WM_ACTIVATE :
                case WM_NCACTIVATE:
                case WM_NCPAINT:
                {
                    if ( AnyEqual( _wndMode, EWindowMode::Borderless, EWindowMode::FullscreenWindow ))
                        return 1;

                    break;
                }

                case WM_QUIT :
                case WM_CLOSE :
                {
                    Close();
                    break;
                }

                default :
                {
                    // from docs:
                    // WM_INPUT with RIM_INPUT: The application must call DefWindowProc so the system can perform cleanup.

                    Unused( _input.ProcessMessage( uMsg, wParam, lParam, _app.GetTimeSinceStart(), _HasFocus() ));
                    break;
                }
            }
        }

        return ::DefWindowProcA( BitCast<HWND>(_wnd), uMsg, wParam, lParam );
    }

/*
=================================================
    _UpdateDescription
=================================================
*/
    void  WindowWinAPI::_UpdateDescription () __NE___
    {
        RECT    client_rect = {};
        ::GetClientRect( BitCast<HWND>(_wnd), OUT &client_rect );   // win2000

        if ( _lockAndHideCursor )
        {
            ::ClipCursor( &client_rect );   // win2000
        }

        const uint2     new_size { client_rect.right - client_rect.left, client_rect.bottom - client_rect.top };

        if ( Any( new_size != _surfaceSize ) and All( IsNotZero( new_size )) )
        {
            _surfaceSize = new_size;

            _surface.ResizeSwapchain();

            _input.SetMonitor( _surfaceSize, GetMonitor() );

            if_likely( _listener )
                _listener->OnResize( *this, _surfaceSize );
        }
    }

/*
=================================================
    GetSurfaceSize
=================================================
*/
    uint2  WindowWinAPI::GetSurfaceSize () C_NE___
    {
        DRC_EXLOCK( _app.GetSingleThreadCheck() );
        DRC_EXLOCK( _drCheck );

        return _surfaceSize;
    }

/*
=================================================
    Close
=================================================
*/
    void  WindowWinAPI::Close () __NE___
    {
        DRC_EXLOCK( _drCheck );

        _requestQuit = true;

        _Destroy();
    }

/*
=================================================
    GetMonitor
=================================================
*/
    Monitor  WindowWinAPI::GetMonitor () C_NE___
    {
        DRC_EXLOCK( _app.GetSingleThreadCheck() );

        HMONITOR    handle      = ::MonitorFromWindow( BitCast<HWND>(_wnd), MONITOR_DEFAULTTONEAREST ); // win2000
        const auto  monitors    = _app.GetMonitors();

        for (auto& mon : monitors)
        {
            if ( handle == mon.native )
                return mon;
        }

        return Default;
    }

/*
=================================================
    GetNative
=================================================
*/
    NativeWindow  WindowWinAPI::GetNative () C_NE___
    {
        DRC_EXLOCK( _drCheck );

        NativeWindow    result;
        result.hinstance    = _GetApp().GetHInstance();
        result.hwnd         = _wnd;
        return result;
    }

/*
=================================================
    SetSize
=================================================
*/
    void  WindowWinAPI::SetSize (const uint2 &size) __NE___
    {
        DRC_EXLOCK( _drCheck );
        DRC_EXLOCK( _app.GetSingleThreadCheck() );

        ASSERT( All( IsNotZero( size )) );

        if_unlikely( _wnd == null )
            return;

        HWND    hwnd            = BitCast<HWND>(_wnd);
        RECT    new_rect        = { 0, 0, int(size.x), int(size.y) };
        DWORD   wnd_style       = ::GetWindowLongA( hwnd, GWL_STYLE );              // win2000
        DWORD   wnd_ext_style   = ::GetWindowLongA( hwnd, GWL_EXSTYLE );            // win2000
        RECT    old_rect;
        ::GetWindowRect( hwnd, OUT &old_rect );                                     // win2000

        ::AdjustWindowRectEx( INOUT &new_rect, wnd_style, FALSE, wnd_ext_style );   // win2000

        ::SetWindowPos( hwnd, HWND_TOP,                                             // win2000
                        old_rect.left, old_rect.top,
                        new_rect.right - new_rect.left, new_rect.bottom - new_rect.top,
                        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOCOPYBITS );
    }

/*
=================================================
    SetPosition
=================================================
*/
    void  WindowWinAPI::SetPosition (const int2 &pos) __NE___
    {
        DRC_EXLOCK( _drCheck );
        DRC_EXLOCK( _app.GetSingleThreadCheck() );

        if_unlikely( _wnd == null )
            return;

        HWND    hwnd    = BitCast<HWND>(_wnd);
        RECT    old_rect;
        ::GetWindowRect( hwnd, OUT &old_rect ); // win2000

        ::SetWindowPos( hwnd, HWND_TOP,         // win2000
                        pos.x, pos.y,
                        old_rect.right - old_rect.left, old_rect.bottom - old_rect.top,
                        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOCOPYBITS );
    }

/*
=================================================
    SetPosition
=================================================
*/
    void  WindowWinAPI::SetPosition (Monitor::ID monitorId, const int2 &pos) __NE___
    {
        DRC_EXLOCK( _app.GetSingleThreadCheck() );

        const auto  monitors = _app.GetMonitors();
        CHECK_ERRV( usize(monitorId) < monitors.size() );

        const auto& monitor = monitors[ usize(monitorId) ];
        CHECK( monitor.id == monitorId );

        SetPosition( monitor.workArea.pixels.LeftTop() + pos );
    }

/*
=================================================
    SetTitle
=================================================
*/
    void  WindowWinAPI::SetTitle (NtStringView title) __NE___
    {
        DRC_EXLOCK( _drCheck );
        DRC_EXLOCK( _app.GetSingleThreadCheck() );

        if_likely( _wnd != null )
        {
            ::SetWindowTextA( BitCast<HWND>(_wnd), title.c_str() ); // win2000
        }
    }

/*
=================================================
    SetFocus
=================================================
*/
    void  WindowWinAPI::SetFocus () C_NE___
    {
        DRC_EXLOCK( _drCheck );
        DRC_EXLOCK( _app.GetSingleThreadCheck() );

        _ShowWindow( EVisibility::VisibleFocused );
    }


} // AE::App

#endif // AE_WINAPI_WINDOW
