// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_WINAPI_WINDOW
# include "platform/WinAPI/ApplicationWinAPI.h"
# include "base/Platforms/WindowsHeader.h"

namespace AE::App
{

/*
=================================================
    constructor
=================================================
*/
    ApplicationWinAPI::ApplicationWinAPI (Unique<IAppListener> listener) __NE___ :
        ApplicationBase{ RVRef(listener) }
    {
      #if AE_PLATFORM_TARGET_VERSION_MAJOR >= 8
        if ( auto* fn = BitCast<decltype(&::SetProcessDpiAwareness)>(WindowsUtils::_SetProcessDpiAwarenessFn()) )
        {
            CHECK( fn( PROCESS_SYSTEM_DPI_AWARE  ) == S_OK );   // win8.1
        }
      #endif

        _UpdateMinitors( OUT _cachedMonitors );
        _GetLocales( OUT _locales );

        DEBUG_ONLY(
            for (auto& m : _cachedMonitors) {
                m.Print();
            })
    }

/*
=================================================
    destructor
=================================================
*/
    ApplicationWinAPI::~ApplicationWinAPI () __NE___
    {
        DRC_EXLOCK( _stCheck );

        EXLOCK( _windowsGuard );

        // windows must be destroyed before destroying app
        for (auto& wnd : _windows)
        {
            CHECK( not wnd.lock() );
        }
        _windows.clear();

        if ( not _className.empty() and _instance != null )
            ::UnregisterClassA( _className.c_str(), BitCast<HMODULE>(_instance) );  // win2000
    }

/*
=================================================
    CreateWindow
=================================================
*/
    WindowPtr  ApplicationWinAPI::CreateWindow (WndListenerPtr listener, const WindowDesc &desc, IInputActions* dstActions) __NE___
    {
        DRC_EXLOCK( _stCheck );

        CHECK_ERR( _isRunning.load() );
        CHECK_ERR( listener );

        SharedPtr<WindowWinAPI>     wnd{ new WindowWinAPI{ *this, RVRef(listener), dstActions }};
        CHECK_ERR( wnd->_Create( desc ));

        {
            EXLOCK( _windowsGuard );
            _windows.push_back( wnd );
        }
        return wnd;
    }

/*
=================================================
    _RegisterClass
=================================================
*/
    bool  ApplicationWinAPI::_RegisterClass () __NE___
    {
        DRC_EXLOCK( _stCheck );

        _className.reserve( 64 );

        const HMODULE   instance = BitCast<HMODULE>( _instance );

        for (uint i = 0; i < 10; ++i)
        {
            _className << "AE Class " << char('0' + i);

            WNDCLASSA   tmp = {};
            bool        ret = ::GetClassInfoA( instance, _className.c_str(), OUT &tmp ) != FALSE;   // win2000

            if ( not ret )
                break;

            _className.clear();
        }
        CHECK_ERR( not _className.empty() );

        WNDCLASSEXA     window_class = {};
        window_class.cbSize         = sizeof(window_class);
        window_class.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc    = &::DefWindowProcA;
        window_class.cbClsExtra     = 0;
        window_class.cbWndExtra     = 0;
        window_class.hInstance      = instance;
        window_class.hbrBackground  = HBRUSH(COLOR_WINDOW+1); //(HBRUSH) COLOR_WINDOWFRAME;
        window_class.hIcon          = HICON(null);
        window_class.hCursor        = ::LoadCursor( null, IDC_ARROW );
        window_class.lpszMenuName   = null;
        window_class.lpszClassName  = _className.c_str();
        window_class.hIconSm        = HICON(null);

        CHECK_ERR( ::RegisterClassExA( OUT &window_class ) != 0 );  // win2000
        return true;
    }

/*
=================================================
    GetMonitors
----
    TODO: CapabilitiesRequestAndCapabilitiesReply
=================================================
*/
    ApplicationWinAPI::Monitors_t  ApplicationWinAPI::GetMonitors (bool update) __NE___
    {
        DRC_EXLOCK( _stCheck );

        if_unlikely( _cachedMonitors.empty() or update )
        {
            _UpdateMinitors( OUT _cachedMonitors );
        }
        return _cachedMonitors;
    }

/*
=================================================
    _SetMinitorNames
=================================================
*/
    void  ApplicationWinAPI::_SetMinitorNames (INOUT Monitors_t &monitors) C_NE___
    {
        Array<DISPLAYCONFIG_PATH_INFO>  paths;
        Array<DISPLAYCONFIG_MODE_INFO>  modes;
        const UINT32                    flags   = QDC_ONLY_ACTIVE_PATHS;
        UINT32                          path_count, mode_count;

        CHECK_ERRV( ::GetDisplayConfigBufferSizes( flags, OUT &path_count, &mode_count ) == ERROR_SUCCESS );    // winvista
        CHECK_ERRV( monitors.size() == path_count );

        paths.resize( path_count );
        modes.resize( mode_count );

        CHECK_ERRV( ::QueryDisplayConfig( flags, INOUT &path_count, OUT paths.data(), INOUT &mode_count, OUT modes.data(), null ) == ERROR_SUCCESS );   // win7

        paths.resize( path_count );
        modes.resize( mode_count );

        for (usize i = 0 ; i < paths.size(); ++i)
        {
            const auto& src = paths[i].targetInfo;

            DISPLAYCONFIG_TARGET_DEVICE_NAME    target_name = {};
            target_name.header.adapterId    = src.adapterId;
            target_name.header.id           = src.id;
            target_name.header.type         = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
            target_name.header.size         = sizeof(target_name);

            CHECK_ERRV( ::DisplayConfigGetDeviceInfo( INOUT &target_name.header ) == ERROR_SUCCESS );   // winvista

            auto&   dst = monitors[i];

            dst.freq        = (src.refreshRate.Numerator + src.refreshRate.Denominator/2) / src.refreshRate.Denominator;
            dst.isExternal  = AnyBits( target_name.outputTechnology, DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL );

            if ( target_name.flags.friendlyNameFromEdid )
                dst.name = Monitor::Name_t{ToString( WStringView{target_name.monitorFriendlyDeviceName} )};
        }
    }

/*
=================================================
    _UpdateMinitors
=================================================
*/
    void  ApplicationWinAPI::_UpdateMinitors (OUT Monitors_t &outMonitors) C_NE___
    {
        outMonitors.clear();

        struct Params
        {
            ApplicationWinAPI const&    app;
            Monitors_t &                monitors;
        } params{ *this, outMonitors };

        MONITORENUMPROC EnumMonitor = [] (HMONITOR hMonitor, HDC, LPRECT, LPARAM dwData) -> BOOL
        {{
            //ApplicationWinAPI*    app     = BitCast<Params *>( dwData )->app;
            Monitors_t&         monitors    = BitCast<Params *>( dwData )->monitors;
            MONITORINFOEXA      info        = {};

            info.cbSize = sizeof(info);
            CHECK( ::GetMonitorInfoA( hMonitor, OUT &info ) != FALSE );

            bool        has_freq    = false;
            bool        has_dpi     = false;
            const bool  is_primary  = AllBits( info.dwFlags, MONITORINFOF_PRIMARY );
            Monitor     disp;

            disp.native = BitCast<Monitor::NativeMonitor_t>( hMonitor );
            disp.ppi    = float2{96.f};
            disp.id     = Monitor::ID(monitors.size());

            // get monitor info
            if ( DWORD mon_count = 0;
                 ::GetNumberOfPhysicalMonitorsFromHMONITOR( hMonitor, OUT &mon_count ) != FALSE )   // winvista
            {
                FixedArray< PHYSICAL_MONITOR, 8 >   win_monitors;
                win_monitors.resize( mon_count );

                if ( ::GetPhysicalMonitorsFromHMONITOR( hMonitor, DWORD(win_monitors.size()), OUT win_monitors.data() ) != FALSE )  // winvista
                {
                    for (usize i = 0; (not has_freq) & (i < win_monitors.size()); ++i)
                    {
                        MC_TIMING_REPORT    timing = {};
                        if ( ::GetTimingReport( win_monitors[i].hPhysicalMonitor, OUT &timing ) != FALSE )  // winvista
                        {
                            disp.freq   = timing.dwVerticalFrequencyInHZ;   // TODO
                            has_freq    = true;
                        }
                    }
                    ::DestroyPhysicalMonitors( DWORD(win_monitors.size()), win_monitors.data() );   // winvista
                }
            }

            if ( not has_freq )
            {
                HDC hdc_screen = ::GetDC( null );
                disp.freq   = ::GetDeviceCaps( hdc_screen, VREFRESH );  // win2000
                has_freq    = true;
                ::ReleaseDC( null, hdc_screen );
            }

            disp.workArea   = PixelsRectI(RectI{ info.rcWork.left,    info.rcWork.top,    info.rcWork.right,    info.rcWork.bottom    });
            disp.region     = PixelsRectI(RectI{ info.rcMonitor.left, info.rcMonitor.top, info.rcMonitor.right, info.rcMonitor.bottom });
            disp.orient     = disp.IsHorizontal() ? Monitor::EOrientation::Landscape : Monitor::EOrientation::Portrait;

          #if AE_PLATFORM_TARGET_VERSION_MAJOR >= 8
            if ( auto* fn = BitCast<decltype(&::GetDpiForMonitor)>(WindowsUtils::_GetDpiForMonitorFn()) )
            {
                uint2   dpi;
                if ( fn( hMonitor, MDT_RAW_DPI, OUT &dpi.x, OUT &dpi.y ) == S_OK )  // win8.1
                {
                    disp.ppi = float2(dpi);
                    has_dpi  = true;
                }
            }
          #endif

            if ( not has_dpi and is_primary )
            {
                // TODO
                //disp.SetPhysicalSize( self->_ScreenPhysicalSize() );
                //disp.SetPixelsPerInch( disp.CalcPixelsPerInch() );
                //has_dpi = true;
            }

            disp.physicalSize   = Meters2f{ disp.RegionSize() / disp.ppi * Monitor::_MetersInInch() };
            disp.name           = info.szDevice;

            monitors.push_back( disp );
            return TRUE;
        }};

        CHECK( ::EnumDisplayMonitors( null, null, EnumMonitor, BitCast<LPARAM>(&params) ) != FALSE );   // win2000

        _SetMinitorNames( outMonitors );
    }

/*
=================================================
    _GetLocales
=================================================
*/
    void  ApplicationWinAPI::_GetLocales (OUT Locales_t &outLocales) C_NE___
    {
        outLocales.clear();

        Array<String>   locales_str;
        if ( WindowsUtils::GetLocales( OUT locales_str ))
        {
            const usize     cnt = Min( outLocales.size(), locales_str.size() );
            for (usize i = 0; i < cnt; ++i) {
                outLocales.push_back( LocaleName{locales_str[i]} );
            }
        }
    }

/*
=================================================
    GetVulkanInstanceExtensions
=================================================
*/
    ArrayView<const char*>  ApplicationWinAPI::GetVulkanInstanceExtensions () __NE___
    {
        static const char*  exts[] = { "VK_KHR_win32_surface" };
        return exts;
    }

/*
=================================================
    _MainLoop
=================================================
*/
    void  ApplicationWinAPI::_MainLoop () __NE___
    {
        _instance = ::GetModuleHandleA( LPCSTR(null) );
        CHECK_ERRV( _RegisterClass() );

        if_likely( _listener )
            _listener->OnStart( *this );

        // main loop
        for (; _isRunning.load();)
        {
            ApplicationBase::_BeforeUpdate();

            // process messages for all windows
            for (MSG msg; ::PeekMessageA( OUT &msg, null, 0, 0, PM_REMOVE ) != FALSE; )
            {
                //::TranslateMessage( &msg );
                ::DispatchMessageA( &msg );
            }


            bool    wnd_is_empty;
            {
                EXLOCK( _windowsGuard );

                for (usize i = 0; i < _windows.size();)
                {
                    // _ProcessMessages() will return 'false' if window is closed
                    if_likely( auto wnd = _windows[i].lock(); wnd and wnd->_Update() )
                        ++i;
                    else
                        _windows.fast_erase( i );
                }

                wnd_is_empty = _windows.empty();
            }

            ApplicationBase::_AfterUpdate();

            if_unlikely( wnd_is_empty )
            {
                ThreadUtils::Sleep( milliseconds{1} );
            }
        }

        ApplicationBase::_Destroy();
    }

/*
=================================================
    Run
=================================================
*/
    int  ApplicationWinAPI::Run (Unique<IAppListener> listener) __NE___
    {
        int res = 0;
        {
            AE::App::ApplicationWinAPI  app{ RVRef(listener) };
            app._MainLoop();
        }

        AE_OnAppDestroyed();
        return res;
    }

} // AE::App


/*
=================================================
    AE_AppEntry
=================================================
*/
extern int  AE_AppEntry ()
{
    return AE::App::ApplicationWinAPI::Run( AE_OnAppCreated() );
}

extern int  main ()
{
    return AE::App::ApplicationWinAPI::Run( AE_OnAppCreated() );
}

# ifdef AE_PLATFORM_WINDOWS
extern int  WinMain (HINSTANCE  hInstance,
                     HINSTANCE  hPrevInstance,
                     LPSTR      lpCmdLine,
                     int        nShowCmd)
{
    AE::Base::Unused( hInstance, hPrevInstance, lpCmdLine, nShowCmd );

    return AE::App::ApplicationWinAPI::Run( AE_OnAppCreated() );
}
# endif

#endif // AE_WINAPI_WINDOW
