// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

extern void PerfTest_HashSet ();
extern void PerfTest_HashMap ();
extern void PerfTest_MinSleepTime ();
extern void PerfTest_Utf8 ();
extern void PerfTest_FindSubString ();
extern void PerfTest_LogicOp ();
extern void PerfTest_SIMD ();
extern void PerfTest_SIMD2 ();
extern void PerfTest_SIMD3 ();
extern void PerfTest_VertSIMD ();
extern void PerfTest_MemCopy ();
extern void PerfTest_CacheSize ();
extern void PerfTest_Compression ();


TEST_ENTRY()
{
  #ifdef AE_RELEASE
	BEGIN_TEST();

	Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));

	RUN_TEST( PerfTest_HashSet );
	RUN_TEST( PerfTest_HashMap );

	RUN_TEST( PerfTest_MinSleepTime );
	RUN_TEST( PerfTest_Utf8 );
	RUN_TEST( PerfTest_FindSubString );
	RUN_TEST( PerfTest_LogicOp );

	RUN_TEST( PerfTest_SIMD );
	RUN_TEST( PerfTest_SIMD2 );
	RUN_TEST( PerfTest_SIMD3 );
	RUN_TEST( PerfTest_VertSIMD );

	RUN_TEST( PerfTest_MemCopy );
	RUN_TEST( PerfTest_CacheSize );

	RUN_TEST( PerfTest_Compression );

	AE_LOGI( "PerformanceTests.Base finished" );

  #else
  # ifdef AE_PLATFORM_ANDROID
	Unused( path );
  # else
	Unused( argc, argv );
  # endif
  #endif

	return 0;
}
