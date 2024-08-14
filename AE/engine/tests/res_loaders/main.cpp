// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_AEImage ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_ResourceLoaders (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_AEImage();

	AE_LOGI( "Tests.ResourceLoaders finished" );
	return 0;
}
