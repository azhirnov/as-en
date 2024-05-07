// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_RectPacker ();
extern void UnitTest_RectPackerSTB ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_AtlasTools (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	//UnitTest_RectPacker();
	UnitTest_RectPackerSTB();

	AE_LOGI( "Tests.AtlasTools finished" );
	return 0;
}
