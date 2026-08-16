// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

extern void UnitTest_RectPacker ();
extern void UnitTest_RectPackerSTB ();


TEST_ENTRY()
{
	BEGIN_TEST();

	//RUN_TEST( UnitTest_RectPacker );
	RUN_TEST( UnitTest_RectPackerSTB );

	AE_LOGI( "Tests.AtlasTools finished" );
	return 0;
}
