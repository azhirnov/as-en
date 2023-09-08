// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

using namespace AE;
using namespace AE::Base;

extern void UnitTest_SphericalCubeMath ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_GeometryTools ()
#else
int main ()
#endif
{
    AE::Base::StaticLogger::LoggerDbgScope log{};

    UnitTest_SphericalCubeMath();

    AE_LOGI( "Tests.GeometryTools finished" );
    return 0;
}
