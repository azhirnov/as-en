// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "pch/Base.h"
#include "threading/TaskSystem/TaskScheduler.h"
#include "../tests/shared/UnitTest_Shared.h"

#if defined(AE_ENABLE_GLFW)
#	define ENABLE_TEST
#	include "platform/GLFW/ApplicationGLFW.h"
	using Application_t = AE::App::ApplicationGLFW;

#  ifdef AE_PLATFORM_WINDOWS
#	include "platform/WinAPI/SerializableInputActionsWinAPI.h"
	using EInputType	= AE::App::SerializableInputActionsWinAPI::EInputType;
#  endif

#elif defined(AE_PLATFORM_WINDOWS)
#	define ENABLE_TEST
#	include "platform/WinAPI/ApplicationWinAPI.h"
#	include "platform/WinAPI/SerializableInputActionsWinAPI.h"
	using Application_t = AE::App::ApplicationWinAPI;
	using EInputType	= AE::App::SerializableInputActionsWinAPI::EInputType;
#endif


#ifdef ENABLE_TEST

using namespace AE::App;
using namespace AE::Threading;

namespace
{
	class AppListener final : public IApplication::IAppListener
	{
	private:
		Unique<ISendInput>	_sendInput;

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
			_sendInput = app.CreateInputSender();
			CHECK_FATAL( _sendInput );

			const bool	down	= true;
			const bool	up		= false;

			#if 0
				CHECK( _sendInput->SetCursorPos( int2{ 1352, 24 }));

				CHECK( _sendInput->SetKeyState( ushort(EInputType::MouseBtn0), down ));
				CHECK( _sendInput->SetKeyState( ushort(EInputType::MouseBtn0), up ));
			#endif

			#if 0
				CHECK( _sendInput->SetCursorPos( int2{ 2076, 512 }));

				CHECK( _sendInput->SetKeyState( ushort(EInputType::MouseBtn0), down ));
				CHECK( _sendInput->SetKeyState( ushort(EInputType::MouseBtn0), up ));

				CHECK( _sendInput->ClipboardPut( u8"test clipboard" ));

				CHECK( _sendInput->SetKeyState( ushort(EInputType::Key_L_Control), down ));
				CHECK( _sendInput->SetKeyState( ushort(EInputType::Key_V), down ));

				CHECK( _sendInput->SetKeyState( ushort(EInputType::Key_L_Control), up ));
				CHECK( _sendInput->SetKeyState( ushort(EInputType::Key_V), up ));
			#endif

			CHECK( _sendInput->SetKeyState( ushort(EInputType::Key_L_Win), down ));
			CHECK( _sendInput->SetKeyState( ushort(EInputType::Key_L_Win), up ));
		}

		void  OnStop (IApplication &) __NE_OV
		{
		}

		void  BeforeWndUpdate (IApplication &) __NE_OV
		{}

		void  AfterWndUpdate (IApplication &app) __NE_OV
		{
			app.Terminate();
		}
	};
}


extern void Test_SendInput ()
{
	Application_t::Run( MakeUnique<AppListener>() );

	TEST_PASSED();
}

#endif // ENABLE_TEST
