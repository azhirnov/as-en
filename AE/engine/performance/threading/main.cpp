// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void  PerfTest_AsyncMutex ();
extern void  PerfTest_AsyncFile ();
extern void  PerfTest_TaskSystem ();

#ifdef AE_PLATFORM_ANDROID
extern int PerformanceTests_Threading ()
#else
int main ()
#endif
{
	#ifndef AE_RELEASE
	//	return 0; // skip
	#endif

	AE::Base::StaticLogger::LoggerDbgScope log{};

	PerfTest_AsyncFile();
	//PerfTest_AsyncMutex();
	//PerfTest_TaskSystem();

	AE_LOGI( "PerformanceTests.Threading finished" );
	return 0;
}
