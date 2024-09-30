// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

extern void PerfTest_HashSet ();
extern void PerfTest_HashMap ();
extern void PerfTest_MinSleepTime ();
extern void PerfTest_Utf8 ();
extern void PerfTest_FindSubString ();

#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Perf_Base (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
  #ifdef AE_RELEASE
	BEGIN_TEST();

	PerfTest_HashSet();
	PerfTest_HashMap();

	//PerfTest_MinSleepTime();
	//PerfTest_Utf8();
	//PerfTest_FindSubString();
	//PerfTest_LogicOp();

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
