// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

extern void Test_BC_Decode (Executor &ex);
extern void Test_BC_Encode (Executor &ex);

extern void Test_ETC_Decode (Executor &ex);
extern void Test_ETC_Encode (Executor &ex);


TEST_ENTRY()
{
	BEGIN_TEST();

	Executor	ex;
	CHECK_FATAL( ex.Initialize() );

	RUN_TEST( Test_BC_Decode, ex );
	RUN_TEST( Test_ETC_Decode, ex );

//	RUN_TEST( Test_BC_Encode, ex );
//	RUN_TEST( Test_ETC_Encode, ex );

	ex.Deinitialize();

	AE_LOGI( "Tests.BlockCompression finished" );

	// Don't check for memleaks because of
	// false positive in glslang when used dynamic allocation in static variable.
	StaticLogger::Deinitialize( false );

	return 0;
}

