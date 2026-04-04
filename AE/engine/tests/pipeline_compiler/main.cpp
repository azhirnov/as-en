// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void  UnitTest_StructType ();

extern void  UnitTest_DSLayout_GLSL ();
extern void  UnitTest_DSLayout_HLSL ();
extern void  UnitTest_DSLayout_MSL ();

extern void  UnitTest_PipelineLayout_GLSL ();
extern void  UnitTest_PipelineLayout_HLSL ();
extern void  UnitTest_PipelineLayout_MSL ();

extern void  UnitTest_VertexBufferInput_GLSL ();
extern void  UnitTest_VertexBufferInput_HLSL ();
extern void  UnitTest_VertexBufferInput_MSL ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_StructType );

	RUN_TEST( UnitTest_DSLayout_GLSL );
	RUN_TEST( UnitTest_DSLayout_HLSL );
	RUN_TEST( UnitTest_DSLayout_MSL );

	RUN_TEST( UnitTest_PipelineLayout_GLSL );
	RUN_TEST( UnitTest_PipelineLayout_HLSL );
	RUN_TEST( UnitTest_PipelineLayout_MSL );

	RUN_TEST( UnitTest_VertexBufferInput_GLSL );
	RUN_TEST( UnitTest_VertexBufferInput_HLSL );
	RUN_TEST( UnitTest_VertexBufferInput_MSL );

	AE_LOGI( "Tests.PipelineCompiler finished" );

	// Don't check for memleaks because of
	// false positive in glslang when used dynamic allocation in static variable.
	StaticLogger::Deinitialize( false );

	return 0;
}
