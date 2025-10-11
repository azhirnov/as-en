// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "../tests/shared/UnitTest_Shared.h"

extern void  UnitTest_LLama ();
extern void  UnitTest_RemoteLLama ();


extern const char*  GetGGMLModelName()
{
	static const char	c_ModelName[] = R"(path/to/llm)";
	return c_ModelName;
}


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_LangModel (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_LLama();
	UnitTest_RemoteLLama();

	AE_LOGI( "Tests.LangModel finished" );
	return 0;
}
