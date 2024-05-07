// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Public/IApplication.h"

using namespace AE::App;



#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_Platform (IApplication &app, IWindow &wnd)
{
	AE_LOGI( "Tests.Platform finished" );
	return 0;
}
#else

	extern void UnitTest_Monitor ();
	extern void Test_Application ();
	extern void Test_Input ();

	Unique<IApplication::IAppListener>  AE_OnAppCreated ()
	{
		AE::Base::StaticLogger::LoggerDbgScope log{};

		UnitTest_Monitor();
		Test_Application();
		Test_Input();

		AE_LOGI( "Tests.Platform finished" );
		std::exit(0);
	}

	void  AE_OnAppDestroyed ()
	{
		// do nothing
	}

#endif
