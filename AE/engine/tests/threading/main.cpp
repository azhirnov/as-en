// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_AsyncMutex ();
extern void UnitTest_Coroutine ();
extern void UnitTest_Promise ();
extern void UnitTest_TaskDeps ();
extern void UnitTest_TaskUsage ();

extern void UnitTest_LfChunkList ();
extern void UnitTest_LfIndexedPool ();
extern void UnitTest_LfStaticPool ();
extern void UnitTest_LfStaticIndexedPool ();
extern void UnitTest_LfTaskQueue ();
extern void UnitTest_LfStaticQueue ();

extern void UnitTest_LfFixedBlockAllocator3 ();
extern void UnitTest_LfLinearAllocator ();

extern void UnitTest_SpinLock ();
extern void UnitTest_Synchronized ();

extern void UnitTest_AsyncDataSource ();
extern void UnitTest_TsSharedMem ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_Threading ()
#else
int main ()
#endif
{
    AE::Base::StaticLogger::LoggerDbgScope log{};

    // minimize disk usage for debug build
  #if defined(AE_RELEASE) and not defined(AE_PLATFORM_ANDROID)
    UnitTest_AsyncDataSource();
  #endif

    UnitTest_TsSharedMem();

    UnitTest_LfChunkList();
    UnitTest_LfIndexedPool();
    UnitTest_LfStaticPool();
    UnitTest_LfStaticIndexedPool();
    UnitTest_LfTaskQueue();
    UnitTest_LfStaticQueue();

    UnitTest_LfFixedBlockAllocator3();
    UnitTest_LfLinearAllocator();

    UnitTest_SpinLock();
    UnitTest_Synchronized();

    UnitTest_TaskDeps();
    UnitTest_TaskUsage();
    UnitTest_AsyncMutex ();
    UnitTest_Promise();
    UnitTest_Coroutine();

    AE_LOGI( "Tests.Threading finished" );
    return 0;
}
