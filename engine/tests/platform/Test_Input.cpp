// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/InPlace.h"
#include "base/DataSource/FileStream.h"
#include "threading/TaskSystem/TaskScheduler.h"
#include "../shared/UnitTest_Shared.h"

#if defined(AE_ENABLE_GLFW)
#	define ENABLE_TEST	1
#	include "platform/GLFW/ApplicationGLFW.h"
	using Application_t = AE::App::ApplicationGLFW;

#elif defined(AE_PLATFORM_WINDOWS)
#	define ENABLE_TEST	1
#	include "platform/WinAPI/ApplicationWinAPI.h"
	using Application_t = AE::App::ApplicationWinAPI;
	
#elif defined(AE_PLATFORM_ANDROID)
#	define ENABLE_TEST	0

#endif

#if ENABLE_TEST

using namespace AE::App;
using namespace AE::Threading;

namespace
{
	class WndListener final : public IWindow::IWndListener
	{
	public:
		void OnStateChanged (IWindow &, EState) override {}
		void OnSurfaceCreated (IWindow &) override {}
		void OnSurfaceDestroyed (IWindow &) override {}
		void OnUpdate (IWindow &wnd) override { wnd.Close(); }
		void OnResize (IWindow &, const uint2 &) override {}
	};


	class AppListener final : public IApplication::IAppListener
	{
	private:
		WindowPtr	_window;

	public:
		AppListener ()
		{
			TaskScheduler::CreateInstance();
			Scheduler().Setup( 1 );
		}

		~AppListener () override
		{
			Scheduler().Release();
			TaskScheduler::DestroyInstance();
		}

		void  OnStart (IApplication &app) override
		{
			WindowDesc	desc;

			_window = app.CreateWindow( MakeUnique<WndListener>(), desc );
			TEST( _window );
			
			TEST( FileSystem::FindAndSetCurrent( "platform/bindings", 5 ));

			const auto	path		= FileSystem::CurrentPath() / (String{app.GetApiName()} << ".as");
			const auto	ansi_path	= ToString( path );

			FileRStream	file {path};
			TEST( file.IsOpen() );

			auto&	input = _window->InputActions();

			//TEST( input.LoadFromScript( file, SourceLoc{ ansi_path, 0 }));
			//TEST( input.SetMode( InputModeName{"UI"} ));
		}

		void  OnStop (IApplication &) override
		{
			_window = null;
		}
			
		void  BeforeWndUpdate (IApplication &) override
		{}

		void  AfterWndUpdate (IApplication &app) override
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
