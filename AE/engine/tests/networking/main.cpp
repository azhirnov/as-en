// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_UDP ();
extern void UnitTest_TCP ();
extern void UnitTest_TcpChannel ();
extern void UnitTest_UdpChannel ();
extern void UnitTest_AsyncCSMessageProducer ();


#ifdef AE_PLATFORM_ANDROID
extern "C" AE_DLL_EXPORT int Tests_Networking (const char* path)
#else
int main (const int argc, char* argv[])
#endif
{
	BEGIN_TEST();

	UnitTest_UDP();
	UnitTest_TCP();

	UnitTest_AsyncCSMessageProducer();

	UnitTest_TcpChannel();
	//UnitTest_UdpChannel();

	AE_LOGI( "Tests.Network finished" );
	return 0;
}
