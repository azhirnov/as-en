// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Serialization ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_Serialization );

	AE_LOGI( "Tests.Serializing finished" );
	return 0;
}
