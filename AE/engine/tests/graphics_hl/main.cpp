// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/IApplication.h"

using namespace AE;
using namespace AE::App;
using namespace AE::Threading;

extern void UnitTest_FormattedText ();
extern void UnitTest_UI_Layouts ();
extern void Test_DrawTests (IApplication &, IWindow &);


static void  UnitTests ()
{
	UnitTest_FormattedText();
	UnitTest_UI_Layouts();
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
		WndListener (IApplication &app)				__NE___ : _app{app} {}

		void OnStateChanged (IWindow &, EState)		__NE_OV {}
		void OnSurfaceDestroyed (IWindow &)			__NE_OV {}

		void OnSurfaceCreated (IWindow &wnd)		__NE_OV
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
		AppListener ()								__NE___
		{
			TaskScheduler::InstanceCtor::Create();

			TaskScheduler::Config	cfg;
			cfg.mainThreadCoreId	= ECpuCoreId(0);

			CHECK_FATAL( Scheduler().Setup( cfg ));
		}

		~AppListener ()								__NE_OV
		{
			Scheduler().Release();
			TaskScheduler::InstanceCtor::Destroy();
		}

		void  OnStart (IApplication &app)			__NE_OV
		{
			UnitTests();

			_window = app.CreateWindow( MakeUnique<WndListener>( app ), Default );
			CHECK_FATAL( _window );
		}

		void  BeforeWndUpdate (IApplication &)		__NE_OV {}

		void  AfterWndUpdate (IApplication &app)	__NE_OV
		{
			if ( _window and _window->GetState() == IWindow::EState::Destroyed )
				app.Terminate();
		}

		void  OnStop (IApplication &)				__NE_OV {}
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
