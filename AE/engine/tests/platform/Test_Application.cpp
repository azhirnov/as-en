// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Containers/InPlace.h"
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
	InPlace<Array<int>>		events;


	class WndListener final : public IWindow::IWndListener
	{
	public:
		void OnStateChanged (IWindow &, EState state) __NE_OV
		{
			switch ( state )
			{
				case EState::Created :		events->push_back( 2 );		break;
				case EState::Started :		events->push_back( 3 );		break;
				case EState::InForeground :	events->push_back( 5 );		break;
				case EState::Focused :		events->push_back( 6 );		break;
				case EState::InBackground :	events->push_back( 7 );		break;
				case EState::Stopped :		events->push_back( 9 );		break;
				case EState::Destroyed :	events->push_back( 10 );	break;
			}
		}

		void OnSurfaceCreated (IWindow &) __NE_OV
		{
			events->push_back( 4 );
		}

		void OnSurfaceDestroyed (IWindow &) __NE_OV
		{
			events->push_back( 8 );
		}
	};


	class AppListener final : public IApplication::IAppListener
	{
	private:
		WindowPtr	_window;
		uint		_counter	= 0;

	public:
		AppListener () __NE___
		{
			TaskScheduler::InstanceCtor::Create();

			TaskScheduler::Config	cfg;
			CHECK_FATAL( Scheduler().Setup( cfg ));
		}

		~AppListener () __NE_OV
		{
			Scheduler().Release();
			TaskScheduler::InstanceCtor::Destroy();
		}

		void  OnStart (IApplication &app) __NE_OV
		{
			events->push_back( 1 );

			auto	monitors = app.GetMonitors();
			TEST( monitors.size() >= 1 );

			WindowDesc	desc;
			desc.monitorId	= Monitor::ID(1);	// try to show window on second monitor

			_window = app.CreateWindow( MakeUnique<WndListener>(), desc );
			TEST( _window );
		}

		void  OnStop (IApplication &) __NE_OV
		{
			_window = null;

			events->push_back( 11 );
		}

		void  BeforeWndUpdate (IApplication &) __NE_OV
		{}

		void  AfterWndUpdate (IApplication &app) __NE_OV
		{
			if ( ++_counter > 1000 )
				app.Terminate();
		}
	};
}


extern void Test_Application ()
{
	events.Create();

	Application_t::Run( MakeUnique<AppListener>() );

	TEST( ArrayView<int>{*events}.section( 0, 6 ) == Array<int>{ 1, 2, 3, 4, 5, 6 });

	TEST( ArrayView<int>{*events}.section( 6, UMax ) == Array<int>{ 5, 7, 8, 9, 10, 11 });

	events.Destroy();

	TEST_PASSED();
}

#endif // ENABLE_TEST
