// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_Serialization ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_Serializing ()
#else
int main ()
#endif
{
    AE::Base::StaticLogger::LoggerDbgScope log{};

    UnitTest_Serialization();

    AE_LOGI( "Tests.Serializing finished" );
    return 0;
}
