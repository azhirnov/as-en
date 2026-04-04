// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_SphericalCubeMath ();
extern void UnitTest_Triangulation ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_SphericalCubeMath );
	RUN_TEST( UnitTest_Triangulation );

	AE_LOGI( "Tests.GeometryTools finished" );
	return 0;
}
