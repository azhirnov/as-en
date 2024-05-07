// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_AsyncMutex ();
extern void UnitTest_Barrier ();
extern void UnitTest_Coroutine ();
extern void UnitTest_Promise ();
extern void UnitTest_Semaphore ();
extern void UnitTest_TaskDeps ();
extern void UnitTest_TaskUsage ();

extern void UnitTest_LfChunkList ();
extern void UnitTest_LfIndexedPool ();
extern void UnitTest_LfStaticPool ();
extern void UnitTest_LfStaticIndexedPool ();
extern void UnitTest_LfTaskQueue ();

extern void UnitTest_LfFixedBlockAllocator3 ();
extern void UnitTest_LfLinearAllocator ();
extern void UnitTest_LfStaticBlockAllocator ();

extern void UnitTest_SpinLock ();
extern void UnitTest_Synchronized ();

extern void UnitTest_AsyncDataSource (const Path &curr);
extern void UnitTest_TsSharedMem ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_Threading (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_AsyncDataSource( curr );

	UnitTest_TsSharedMem();

	UnitTest_LfChunkList();
	UnitTest_LfIndexedPool();
	UnitTest_LfStaticPool();
	UnitTest_LfStaticIndexedPool();
	UnitTest_LfTaskQueue();

	UnitTest_LfFixedBlockAllocator3();
	UnitTest_LfLinearAllocator();
	UnitTest_LfStaticBlockAllocator();

	UnitTest_SpinLock();
	UnitTest_Synchronized();
	UnitTest_Barrier();
	UnitTest_Semaphore();

	UnitTest_TaskDeps();
	UnitTest_TaskUsage();
	UnitTest_AsyncMutex ();
	UnitTest_Promise();
	UnitTest_Coroutine();

	AE_LOGI( "Tests.Threading finished" );
	return 0;
}
