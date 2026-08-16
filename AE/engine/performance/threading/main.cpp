// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

extern void  PerfTest_AsyncMutex ();
extern void  PerfTest_AsyncFile (const AE::Base::Path &curr);
extern void  PerfTest_TaskSystemCoro ();
extern void  PerfTest_TaskOrder1 ();
extern void  PerfTest_TaskOrder2 ();

extern void  PerfTest_MtAllocator ();

extern void  PerfTest_Raw_Atomic ();
extern void  PerfTest_Raw_ThreadWakeUp ();


TEST_ENTRY()
{
	BEGIN_TEST();

	CHECK( PlatformUtils::SetSystemSleepState( ESystemSleepState::DontSleep_AllowTurnDisplayOff ));

	RUN_TEST( PerfTest_AsyncFile, curr );
	RUN_TEST( PerfTest_AsyncMutex );
	RUN_TEST( PerfTest_TaskSystemCoro );

	RUN_TEST( PerfTest_TaskOrder1 );
	RUN_TEST( PerfTest_TaskOrder2 );

	//RUN_TEST( PerfTest_MtAllocator );

	//RUN_TEST( PerfTest_Raw_ThreadWakeUp );
	//RUN_TEST( PerfTest_Raw_Atomic );

	AE_LOGI( "PerformanceTests.Threading finished" );
	return 0;
}
