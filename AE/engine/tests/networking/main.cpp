// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

extern void UnitTest_UDP ();
extern void UnitTest_TCP ();
extern void UnitTest_TcpChannel ();
extern void UnitTest_UdpChannel ();
extern void UnitTest_AsyncCSMessageProducer ();
extern void UnitTest_SocketDep ();


TEST_ENTRY()
{
	BEGIN_TEST();

	RUN_TEST( UnitTest_UDP );
	RUN_TEST( UnitTest_TCP );

	RUN_TEST( UnitTest_AsyncCSMessageProducer );
	RUN_TEST( UnitTest_SocketDep );

	RUN_TEST( UnitTest_TcpChannel );
	//RUN_TEST( UnitTest_UdpChannel );

	AE_LOGI( "Tests.Network finished" );
	return 0;
}
