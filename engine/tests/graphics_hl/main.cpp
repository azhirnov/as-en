// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/IApplication.h"
#include "graphics/Public/ResourceManager.h"

using namespace AE::App;
using namespace AE::Threading;

extern void UnitTest_FormattedText ();
extern void Test_DrawTests (IApplication &, IWindow &);


static void  UnitTests ()
{
}

static void  GraphicsTests (IApplication &app, IWindow &wnd)
{
	Test_DrawTests( app, wnd );
}


#ifdef AE_PLATFORM_ANDROID
extern int Test_GraphicsHL (IApplication &app, IWindow &wnd)
{
	UnitTests();
	GraphicsTests( app, wnd );

	AE_LOGI( "Tests.GraphicsHL finished" );
	return 0;
}
#else

	class WndListener final : public IWindow::IWndListener
	{
	private:
		IApplication&	_app;

	public:
		WndListener (IApplication &app) : _app{app} {}
		~WndListener () override {}
		
		void OnStateChanged (IWindow &, EState) override {}
		void OnResize (IWindow &, const uint2 &) override {}
		void OnUpdate (IWindow &) override {}
		void OnSurfaceDestroyed (IWindow &) override {}

		void OnSurfaceCreated (IWindow &wnd) override
		{
			GraphicsTests( _app, wnd );

			wnd.Close();
			AE_LOGI( "Tests.GraphicsHL finished" );
		}
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
			UnitTests();
			
			_window = app.CreateWindow( MakeUnique<WndListener>( app ), Default );
			CHECK_FATAL( _window );
		}
		
		void  BeforeWndUpdate (IApplication &) override {}

		void  AfterWndUpdate (IApplication &app) override
		{
			if ( _window and _window->GetState() == IWindow::EState::Destroyed )
				app.Terminate();
		}

		void  OnStop (IApplication &) override {}
	};

	Unique<IApplication::IAppListener>  AE_OnAppCreated ()
	{
		AE::Base::StaticLogger::InitDefault();

		return MakeUnique<AppListener>();
	}

	void  AE_OnAppDestroyed ()
	{
		AE::Base::StaticLogger::Deinitialize(true);
	}

#endif // AE_PLATFORM_ANDROID
