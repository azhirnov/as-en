// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void UnitTest_ArchiveStorage ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_VFS ()
#else
int main ()
#endif
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	UnitTest_ArchiveStorage();

	AE_LOGI( "Tests.VFS finished" );
	return 0;
}
