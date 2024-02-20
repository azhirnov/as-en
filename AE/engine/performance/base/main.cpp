// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void PerfTest_HashSet ();
extern void PerfTest_HashMap ();
extern void PerfTest_MinSleepTime ();
extern void PerfTest_Utf8 ();
extern void PerfTest_FindSubString ();
extern void PerfTest_LogicOp ();

#ifdef AE_PLATFORM_ANDROID
extern int PerformanceTests_Base ()
#else
int main ()
#endif
{
    #ifndef AE_RELEASE
    //  return 0; // skip
    #endif

    AE::Base::StaticLogger::LoggerDbgScope log{};

    //PerfTest_HashSet();
    //PerfTest_HashMap();
    //PerfTest_MinSleepTime();
    //PerfTest_Utf8();
    //PerfTest_FindSubString();
    //PerfTest_LogicOp();

    AE_LOGI( "PerformanceTests.Base finished" );
    return 0;
}
