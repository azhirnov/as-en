// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Array ();
extern void UnitTest_Class ();
extern void UnitTest_MathFunc ();
extern void UnitTest_String ();
extern void UnitTest_Exceptions ();
extern void UnitTest_Preprocessor ();
extern void UnitTest_Fn ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_Scripting (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_Array();
	UnitTest_Class();
	UnitTest_MathFunc();
	UnitTest_String();
	UnitTest_Exceptions();
	UnitTest_Preprocessor();
	UnitTest_Fn();

	// TODO: multithreading test

	AE_LOGI( "Tests.ScriptBinding finished" );
	return 0;
}
