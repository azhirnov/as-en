// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
	static void  TCP_Test1 ()
	{
		LocalSocketMngr	mngr;

		const char	send_data1[] = "12346ewiofdklijnskdn";
		const char	send_data2[] = "woihfdifkhjnwopefjseoivnj";
		const Bytes	send_data_size = Sizeof(send_data1) + Sizeof(send_data2);

		TcpSocket	server;
		TEST( server.Listen( 4000 ));
		TEST( server.IsOpen() );

		StdThread	listener{ [&] ()
			{
				Array<char>	recv_data;
				for (uint i = 0; i < 100; ++i)
				{
					IpAddress	addr;
					Bytes		recv;
					char		buf[128];

					TcpSocket	client;
					if ( client.Accept( server, OUT addr ))
					{
						AE_LOGI( "TCP: connected to "s << addr.ToString() );

						if ( client.Receive( OUT buf, Sizeof(buf), OUT recv ) and recv > 0 )
						{
							recv_data.insert( recv_data.end(), buf, buf + recv );

							AE_LOGI( "TCP: received "s << ToString(recv) );

							Bytes	sent;
							if ( not client.Send( buf, recv, OUT sent ) or sent != recv )
							{
								AE_LOGI( "TCP: failed to send back" );
							}
						}
					}
					
					if ( ArraySizeOf(recv_data) >= send_data_size )
						break;

					ThreadUtils::Sleep( milliseconds{100} );
				}

				TEST( ArraySizeOf(recv_data) >= send_data_size );
				TEST( ArrayView<char>{send_data1} == ArrayView<char>{recv_data}.section( 0, CountOf(send_data1) ));
				TEST( ArrayView<char>{send_data2} == ArrayView<char>{recv_data}.section( CountOf(send_data1), CountOf(send_data2) ));
			}};

		TcpSocket	client;
		TEST( client.Connect( IpAddress::FromHostPortTCP( "localhost", 4000 ) ));
		TEST( client.IsOpen() );

		Bytes	sent;
		TEST( client.Send( send_data1, Sizeof(send_data1), OUT sent ));
		TEST( sent == Sizeof(send_data1) );
		
		TEST( client.Send( send_data2, Sizeof(send_data2), OUT sent ));
		TEST( sent == Sizeof(send_data2) );

		Bytes	recv;
		char	buf[128];
		for (; client.Receive( OUT buf, Sizeof(buf), OUT recv ) and recv == 0;) {}

		TEST( recv == send_data_size );
		TEST( ArrayView<char>{send_data1} == ArrayView<char>{buf}.section( 0, CountOf(send_data1) ));
		TEST( ArrayView<char>{send_data2} == ArrayView<char>{buf}.section( CountOf(send_data1), CountOf(send_data2) ));

		listener.join();

		client.Close();
	}
}


extern void UnitTest_TCP ()
{
	TCP_Test1();

	TEST_PASSED();
}
