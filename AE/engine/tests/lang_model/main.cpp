// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../tests/shared/UnitTest_Shared.h"

extern void  UnitTest_LLama ();
extern void  UnitTest_RemoteLLama ();
extern void  UnitTest_StableDiffusion (const Path &dstFolder);


extern const char*  GetGGMLModelName()
{
	static const char	c_ModelName[] = R"(path/to/llm)";
	return c_ModelName;
}


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_LLama );
	RUN_TEST( UnitTest_RemoteLLama );
	RUN_TEST( UnitTest_StableDiffusion, curr );

	AE_LOGI( "Tests.LangModel finished" );
	return 0;
}
