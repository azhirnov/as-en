// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_ArchiveStorage (const Path &curr);
extern void UnitTest_NetworkStorage (const Path &curr);


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_ArchiveStorage, curr );
	RUN_TEST( UnitTest_NetworkStorage, curr );

	AE_LOGI( "Tests.VFS finished" );
	return 0;
}
