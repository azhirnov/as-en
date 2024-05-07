// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_ArchiveStorage (const Path &curr);
extern void UnitTest_NetworkStorage (const Path &curr);


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_VFS (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_ArchiveStorage( curr );
	UnitTest_NetworkStorage( curr );

	AE_LOGI( "Tests.VFS finished" );
	return 0;
}
