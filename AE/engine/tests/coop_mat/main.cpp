// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Executor.h"

extern void Test_CoopMat (Executor &ex);
extern void Test_CoopVec (Executor &ex);
extern void Test_CoopVecTraining (Executor &ex);
extern void Test_IntDotProduct (Executor &ex);
extern void Test_MLPTraining (Executor &ex);


TEST_ENTRY()
{
	BEGIN_TEST();

	Executor	ex;
	CHECK_FATAL( ex.Initialize() );

	RUN_TEST( Test_CoopMat, ex );
	RUN_TEST( Test_CoopVec, ex );
	RUN_TEST( Test_CoopVecTraining, ex );
	RUN_TEST( Test_IntDotProduct, ex );
	RUN_TEST( Test_MLPTraining, ex );

	ex.Deinitialize();

	AE_LOGI( "Tests.CoopMat finished" );

	// Don't check for memleaks because of
	// false positive in glslang when used dynamic allocation in static variable.
	StaticLogger::Deinitialize( false );

	return 0;
}
