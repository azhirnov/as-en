// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

extern void Perf_Hierarchy ();


TEST_ENTRY()
{
  #if 1 //def AE_RELEASE
	BEGIN_TEST();

	Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));

	RUN_TEST( Perf_Hierarchy );

	AE_LOGI( "PerformanceTests.ECS finished" );

  #else
  # ifdef AE_PLATFORM_ANDROID
	Unused( path );
  # else
	Unused( argc, argv );
  # endif
  #endif

	return 0;
}
