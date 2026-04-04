// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Array ();
extern void UnitTest_Class ();
extern void UnitTest_MathFunc ();
extern void UnitTest_String ();
extern void UnitTest_Exceptions ();
extern void UnitTest_Preprocessor ();
extern void UnitTest_Fn ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_Array );
	RUN_TEST( UnitTest_Class );
	RUN_TEST( UnitTest_MathFunc );
	RUN_TEST( UnitTest_String );
	RUN_TEST( UnitTest_Exceptions );
	RUN_TEST( UnitTest_Preprocessor );
	RUN_TEST( UnitTest_Fn );

	// TODO: multithreading test

	AE_LOGI( "Tests.ScriptBinding finished" );
	return 0;
}
