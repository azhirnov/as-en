// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "platform/Public/Application.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::App;


#ifdef AE_PLATFORM_ANDROID
	TEST_ENTRY()
	{
		AE_LOGI( "Tests.Platform finished" );
		return 0;
	}
#else

	extern void Test_Application ();
	extern void Test_Input ();
	extern void Test_SendInput ();

	Unique<IApplication::IAppListener>  AE_OnAppCreated (const int argc, char const* argv[])
	{
		BEGIN_TEST();

		RUN_TEST( Test_Application );
		RUN_TEST( Test_Input );
		RUN_TEST( Test_SendInput );

		AE_LOGI( "Tests.Platform finished" );
		std::exit(0);
	}

	void  AE_OnAppDestroyed ()
	{
		// do nothing
	}

#endif
