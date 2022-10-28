// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

using namespace AE;
using namespace AE::Base;


extern void UnitTest_RectPacker ();
extern void UnitTest_RectPackerSTB ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_AtlasTools ()
#else
int main ()
#endif
{
	AE::Base::StaticLogger::LoggerDbgScope log{};

	//UnitTest_RectPacker();
	UnitTest_RectPackerSTB();
	
	AE_LOGI( "Tests.AtlasTools finished" );
	return 0;
}
