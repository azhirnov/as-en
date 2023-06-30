// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/InPlace.h"
#include "base/DataSource/FileStream.h"
#include "threading/TaskSystem/TaskScheduler.h"
#include "../shared/UnitTest_Shared.h"

#if defined(AE_ENABLE_GLFW)
#   define ENABLE_TEST  1
#   include "platform/GLFW/ApplicationGLFW.h"
    using Application_t = AE::App::ApplicationGLFW;

#elif defined(AE_PLATFORM_WINDOWS)
#   define ENABLE_TEST  1
#   include "platform/WinAPI/ApplicationWinAPI.h"
    using Application_t = AE::App::ApplicationWinAPI;

#elif defined(AE_PLATFORM_ANDROID)
#   define ENABLE_TEST  0

#endif

#if ENABLE_TEST

using namespace AE::App;
using namespace AE::Threading;

namespace
{
    class WndListener final : public IWindow::IWndListener
    {
    public:
        void OnStateChanged (IWindow &, EState)     __NE_OV {}
        void OnSurfaceCreated (IWindow &)           __NE_OV {}
        void OnSurfaceDestroyed (IWindow &)         __NE_OV {}
        void OnUpdate (IWindow &wnd)                __NE_OV { wnd.Close(); }
        void OnResize (IWindow &, const uint2 &)    __NE_OV {}
    };


    class AppListener final : public IApplication::IAppListener
    {
    private:
        WindowPtr   _window;

    public:
        AppListener ()
        {
            TaskScheduler::CreateInstance();

            TaskScheduler::Config   cfg;
            CHECK_FATAL( Scheduler().Setup( cfg ));
        }

        ~AppListener () __NE_OV
        {
            Scheduler().Release();
            TaskScheduler::DestroyInstance();
        }

        void  OnStart (IApplication &app) __NE_OV
        {
            WindowDesc  desc;

            _window = app.CreateWindow( MakeUnique<WndListener>(), desc );
            TEST( _window );

            TEST( FileSystem::FindAndSetCurrent( "platform/bindings", 5 ));

            const auto  path        = FileSystem::CurrentPath() / (String{app.GetApiName()} << ".as");
            const auto  ansi_path   = ToString( path );

            FileRStream file {path};
            TEST( file.IsOpen() );

            auto&   input = _window->InputActions();
            Unused( input );

            //TEST( input.LoadFromScript( file, SourceLoc{ ansi_path, 0 }));
            //TEST( input.SetMode( InputModeName{"UI"} ));
        }

        void  OnStop (IApplication &) __NE_OV
        {
            _window = null;
        }

        void  BeforeWndUpdate (IApplication &) __NE_OV
        {}

        void  AfterWndUpdate (IApplication &app) __NE_OV
        {
            if ( _window and _window->GetState() == IWindow::EState::Destroyed )
                app.Terminate();
        }
    };
}


extern void Test_Input ()
{
    Application_t::Run( MakeUnique<AppListener>() );

    TEST_PASSED();
}

#endif // ENABLE_TEST
