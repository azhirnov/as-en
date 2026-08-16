// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

extern void UnitTest_AEImage ();
extern void UnitTest_CSVMeshLoader ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_AEImage );
	RUN_TEST( UnitTest_CSVMeshLoader );

	AE_LOGI( "Tests.ResourceLoaders finished" );
	return 0;
}
