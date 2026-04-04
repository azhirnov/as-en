// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Archetype ();
extern void UnitTest_EntityPool ();
extern void UnitTest_Registry ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_Archetype );
	RUN_TEST( UnitTest_EntityPool );
	RUN_TEST( UnitTest_Registry );

	AE_LOGI( "Tests.ECS finished" );
	return 0;
}
