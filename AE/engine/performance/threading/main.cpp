// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

extern void  PerfTest_AsyncMutex ();
extern void  PerfTest_AsyncFile (const AE::Base::Path &curr);
extern void  PerfTest_TaskSystem ();
extern void  PerfTest_MtAllocator ();

extern void  PerfTest_Raw_Atomic ();
extern void  PerfTest_Raw_ThreadWakeUp ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Perf_Threading (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
  #ifdef AE_RELEASE
	BEGIN_TEST();

	PerfTest_AsyncFile( curr );
	PerfTest_AsyncMutex();
	PerfTest_TaskSystem();

	//PerfTest_MtAllocator();

	//PerfTest_Raw_ThreadWakeUp();
	//PerfTest_Raw_Atomic();

	AE_LOGI( "PerformanceTests.Threading finished" );
  #endif
	return 0;
}
