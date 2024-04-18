// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"
using namespace AE::Base;

extern void  UnitTest_StructType ();
extern void  UnitTest_DSLayout_GLSL ();
extern void  UnitTest_DSLayout_MSL ();
extern void  UnitTest_PipelineLayout_GLSL ();
extern void  UnitTest_PipelineLayout_MSL ();
extern void  UnitTest_VertexBufferInput_GLSL ();
extern void  UnitTest_VertexBufferInput_MSL ();

int main ()
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	UnitTest_StructType();
	UnitTest_DSLayout_GLSL();
	UnitTest_DSLayout_MSL();

	UnitTest_PipelineLayout_GLSL();
	UnitTest_PipelineLayout_MSL();

	UnitTest_VertexBufferInput_GLSL();
	UnitTest_VertexBufferInput_MSL();

	AE_LOGI( "Tests.PipelineCompiler finished" );
	return 0;
}
