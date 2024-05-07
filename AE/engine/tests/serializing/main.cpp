// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Serialization ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_Serializing (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_Serialization();

	AE_LOGI( "Tests.Serializing finished" );
	return 0;
}
