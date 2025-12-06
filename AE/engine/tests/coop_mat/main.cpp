// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Executor.h"

extern void Test_CoopMat (Executor &ex);
extern void Test_CoopVec (Executor &ex);
extern void Test_CoopVecTraining (Executor &ex);
extern void Test_IntDotProduct (Executor &ex);
extern void Test_MLPTraining (Executor &ex);


int main (const int argc, char* argv[])
{
	BEGIN_TEST();

	Executor	ex;
	CHECK_FATAL( ex.Initialize() );

	Test_CoopMat( ex );
	Test_CoopVec( ex );
	Test_CoopVecTraining( ex );
	Test_IntDotProduct( ex );
	Test_MLPTraining( ex );

	ex.Deinitialize();

	AE_LOGI( "Tests.CoopMat finished" );

	// Don't check for memleaks because of
	// false positive in glslang when used dynamic allocation in static variable.
	StaticLogger::Deinitialize( false );

	return 0;
}
