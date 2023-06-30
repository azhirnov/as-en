// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void UnitTest_Console ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_HuLang ()
#else
int main ()
#endif
{
    AE::Base::StaticLogger::LoggerDbgScope log{};

    UnitTest_Console();

    AE_LOGI( "Tests.HuLang finished" );
    return 0;
}
