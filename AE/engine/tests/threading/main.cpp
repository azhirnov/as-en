// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_AsyncMutex ();
extern void UnitTest_AsyncDataSource (const Path &curr);
extern void UnitTest_Barrier ();
extern void UnitTest_Promise ();
extern void UnitTest_Semaphore ();
extern void UnitTest_SyncPoint ();
extern void UnitTest_SpinLock ();
extern void UnitTest_Synchronized ();
extern void UnitTest_Task ();

extern void UnitTest_LfChunkList ();
extern void UnitTest_LfIndexedPool ();
extern void UnitTest_LfStaticPool ();
extern void UnitTest_LfStaticIndexedPool ();
extern void UnitTest_LfTaskQueue ();

extern void UnitTest_LfFixedBlockAllocator3 ();
extern void UnitTest_LfLinearAllocator ();
extern void UnitTest_LfStaticBlockAllocator ();

extern void UnitTest_TsSharedMem ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_Task );
	RUN_TEST( UnitTest_Promise );
	RUN_TEST( UnitTest_AsyncDataSource, curr );

	RUN_TEST( UnitTest_TsSharedMem );

	RUN_TEST( UnitTest_LfChunkList );
	RUN_TEST( UnitTest_LfIndexedPool );
	RUN_TEST( UnitTest_LfStaticPool );
	RUN_TEST( UnitTest_LfStaticIndexedPool );
	RUN_TEST( UnitTest_LfTaskQueue );

	RUN_TEST( UnitTest_LfFixedBlockAllocator3 );
	RUN_TEST( UnitTest_LfLinearAllocator );
	RUN_TEST( UnitTest_LfStaticBlockAllocator );

	RUN_TEST( UnitTest_SpinLock );
	RUN_TEST( UnitTest_Synchronized );
	RUN_TEST( UnitTest_Barrier );
	RUN_TEST( UnitTest_Semaphore );

	RUN_TEST( UnitTest_AsyncMutex );
	RUN_TEST( UnitTest_SyncPoint );

	AE_LOGI( "Tests.Threading finished" );
	return 0;
}
