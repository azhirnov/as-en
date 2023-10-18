// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_GLFW
# include "base/Platforms/WindowsHeader.h"
# include "platform/GLFW/ApplicationGLFW.h"

# include "GLFW/glfw3.h"
# include "GLFW/glfw3native.h"

namespace AE::App
{

/*
=================================================
    GLFW_ErrorCallback
=================================================
*/
namespace {
    static void  GLFW_ErrorCallback (int code, const char* msg) __NE___
    {
        AE_LOGE( "GLFW error: " + ToString( code ) + ", msg: \"" + msg + "\"" );
    }
}

/*
=================================================
    constructor
=================================================
*/
    ApplicationGLFW::ApplicationGLFW (Unique<IAppListener> listener) __NE___ :
        ApplicationBase{ RVRef(listener) }
    {
      #ifndef AE_CFG_RELEASE
        glfwSetErrorCallback( &GLFW_ErrorCallback );
      #else
        Unused( &GLFW_ErrorCallback );
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
    ApplicationGLFW::~ApplicationGLFW () __NE___
    {
        DRC_EXLOCK( _stCheck );
        EXLOCK( _windowsGuard );

        // windows must be destroyed before destroying app
        for (auto& wnd : _windows)
        {
            CHECK( not wnd.lock() );
        }

        glfwTerminate();
    }

/*
=================================================
    CreateWindow
=================================================
*/
    WindowPtr  ApplicationGLFW::CreateWindow (WndListenerPtr listener, const WindowDesc &desc, IInputActions* dstActions) __NE___
    {
        DRC_EXLOCK( _stCheck );

        CHECK_ERR( _isRunning.load() );
        CHECK_ERR( listener );

        SharedPtr<WindowGLFW>   wnd{ new WindowGLFW{ *this, RVRef(listener), dstActions }};
        CHECK_ERR( wnd->_Create( desc ));

        {
            EXLOCK( _windowsGuard );
            _windows.push_back( wnd );
        }
        return wnd;
    }

/*
=================================================
    GetMonitors
=================================================
*/
    ApplicationGLFW::Monitors_t  ApplicationGLFW::GetMonitors (bool update) __NE___
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
    _UpdateMinitors
=================================================
*/
    void  ApplicationGLFW::_UpdateMinitors (OUT Monitors_t &outMonitors) C_NE___
    {
        outMonitors.clear();

        int             count;
        GLFWmonitor**   monitors = glfwGetMonitors( OUT &count );

        if ( monitors == null or count <= 0 )
            return;

        outMonitors.resize( count );

        for (int i = 0; i < count; ++i)
        {
            GetMonitorInfo( monitors[i], OUT outMonitors[i] );

            outMonitors[i].id = Monitor::ID(i);
        }
    }

/*
=================================================
    _GetLocales
=================================================
*/
    void  ApplicationGLFW::_GetLocales (OUT Locales_t &outLocales) C_NE___
    {
        outLocales.clear();

    #ifdef AE_PLATFORM_WINDOWS

        Array<String>   locales_str;
        if ( WindowsUtils::GetLocales( OUT locales_str ))
        {
            const usize     cnt = Min( outLocales.size(), locales_str.size() );
            for (usize i = 0; i < cnt; ++i) {
                outLocales.push_back( LocaleName{locales_str[i]} );
            }
        }
    #endif
    }

/*
=================================================
    GetMonitorInfo
=================================================
*/
    bool  ApplicationGLFW::GetMonitorInfo (GLFWmonitor* ptr, OUT Monitor &result) __NE___
    {
        CHECK_ERR( ptr != null );

        int2    pos;
        int2    size;
        glfwGetMonitorWorkarea( ptr, OUT &pos.x, OUT &pos.y, OUT &size.x, OUT &size.y );

        int2    mpos;
        glfwGetMonitorPos( ptr, OUT &mpos.x, OUT &mpos.y );

        int2    size_mm;
        glfwGetMonitorPhysicalSize( ptr, OUT &size_mm.x, OUT &size_mm.y );

        result.workArea     = PixelsRectI{ RectI{ pos, pos + size }};
        result.physicalSize = Meters2f{ float2(size_mm) * 1.0e-3f };

        if ( const GLFWvidmode* mode = glfwGetVideoMode( ptr ))
        {
            result.region   = PixelsRectI{ RectI{ mpos, mpos + int2{ mode->width, mode->height }}};
            result.freq     = mode->refreshRate;
        }
        else
        {
            result.region   = result.workArea;
        }

        // calc PPI
        result.ppi = result.RegionSize() / result.physicalSize.meters * Monitor::_MetersInInch();

        result.name = glfwGetMonitorName( ptr );    // TODO: returns 'Generic PnP Monitor'

        // set native handle
        #if defined(AE_PLATFORM_WINDOWS)
            POINT   pt = { pos.x+1, pos.y+1 };
            result.native = BitCast<Monitor::NativeMonitor_t>( ::MonitorFromPoint( pt, MONITOR_DEFAULTTONULL ));

        #elif defined(AE_PLATFORM_LINUX)
            result.native = BitCast<Monitor::NativeMonitor_t>( ::glfwGetX11Monitor( ptr ));

        #elif defined(AE_PLATFORM_MACOS)
            result.native = BitCast<Monitor::NativeMonitor_t>( ::glfwGetCocoaMonitor( ptr ));

        #else
        #   error no implementation for current platform!
        #endif

        return true;
    }

/*
=================================================
    GetVulkanInstanceExtensions
=================================================
*/
    ArrayView<const char*>  ApplicationGLFW::GetVulkanInstanceExtensions () __NE___
    {
        uint            required_extension_count = 0;
        const char **   required_extensions      = glfwGetRequiredInstanceExtensions( OUT &required_extension_count );

        return ArrayView<const char*>{ required_extensions, required_extension_count };
    }

/*
=================================================
    _MainLoop
=================================================
*/
    void  ApplicationGLFW::_MainLoop () __NE___
    {
        DRC_EXLOCK( _stCheck );

        if_likely( _listener )
            _listener->OnStart( *this );

        for (; _isRunning.load();)
        {
            ApplicationBase::_BeforeUpdate();

            glfwPollEvents();

            bool    wnd_is_empty;
            {
                EXLOCK( _windowsGuard );

                for (usize i = 0; i < _windows.size();)
                {
                    // _ProcessMessages() will return 'false' if window is closed
                    if_likely( auto wnd = _windows[i].lock(); wnd and wnd->_ProcessMessages() )
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
    int  ApplicationGLFW::Run (Unique<IAppListener> listener) __NE___
    {
        int res = 0;
        {
            CHECK_ERR( glfwInit() == GLFW_TRUE, -1 );
            CHECK_ERR( glfwVulkanSupported() == GLFW_TRUE, -1 );

            AE::App::ApplicationGLFW    app{ RVRef(listener) };
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
    return AE::App::ApplicationGLFW::Run( AE_OnAppCreated() );
}

extern int  main ()
{
    return AE::App::ApplicationGLFW::Run( AE_OnAppCreated() );
}

# ifdef AE_PLATFORM_WINDOWS
extern int  WinMain (HINSTANCE  hInstance,
                     HINSTANCE  hPrevInstance,
                     LPSTR      lpCmdLine,
                     int        nShowCmd)
{
    AE::Base::Unused( hInstance, hPrevInstance, lpCmdLine, nShowCmd );

    return AE::App::ApplicationGLFW::Run( AE_OnAppCreated() );
}
# endif

#endif // AE_ENABLE_GLFW
