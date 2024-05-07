// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_SphericalCubeMath ();
extern void UnitTest_Triangulation ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_GeometryTools (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_SphericalCubeMath();
	UnitTest_Triangulation();

	AE_LOGI( "Tests.GeometryTools finished" );
	return 0;
}
