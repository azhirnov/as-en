// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Perf_Common.h"

extern void Perf_Hierarchy ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Perf_ECS (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
  #if 1 //def AE_RELEASE
	BEGIN_TEST();

	Unused( PlatformUtils::SetSystemSleepState( ESystemSleepState::DisplayAlwaysOn ));

	Perf_Hierarchy();

	AE_LOGI( "PerformanceTests.ECS finished" );

  #else
  # ifdef AE_PLATFORM_ANDROID
	Unused( path );
  # else
	Unused( argc, argv );
  # endif
  #endif

	return 0;
}
