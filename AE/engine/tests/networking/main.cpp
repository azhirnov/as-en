// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Common.h"

extern void UnitTest_UDP ();
extern void UnitTest_TCP ();
extern void UnitTest_TcpChannel ();
extern void UnitTest_UdpChannel ();
extern void UnitTest_AsyncCSMessageProducer ();


#ifdef AE_PLATFORM_ANDROID
extern int Test_Networking ()
#else
int main ()
#endif
{
    AE::Base::StaticLogger::LoggerDbgScope log{};

    UnitTest_UDP();
    UnitTest_TCP();

    UnitTest_AsyncCSMessageProducer();

    UnitTest_TcpChannel();
    //UnitTest_UdpChannel();

    AE_LOGI( "Tests.Network finished" );
    return 0;
}
