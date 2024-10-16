// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
	static const ushort		c_Port = 4001;


	template <typename Address>
	static void  TCP_Test_IPv ()
	{
		LocalSocketMngr	mngr;

		const char	send_data1[] = "12346ewiofdklijnskdn";
		const char	send_data2[] = "woihfdifkhjnwopefjseoivnj";
		const Bytes	send_data_size = Sizeof(send_data1) + Sizeof(send_data2);

		Threading::Barrier	sync {2};

		StdThread	listener{ [&] ()
			{{
				TcpSocket	server;
				TEST( server.Listen( Address::FromLocalhostTCP(c_Port) ));
				TEST( server.IsOpen() );

				sync.Wait();
				sync.Wait();

				Address		addr;
				TcpSocket	client;

				Array<char>	recv_data;
				for (uint i = 0; i < 1'000; ++i)	// max: 100s
				{
					if ( not client.IsOpen() )
					{
						if ( not client.Accept( server, OUT addr ))
						{
							ThreadUtils::MilliSleep( milliseconds{100} );
							continue;
						}
						AE_LOGI( "TCP: connected to "s << addr.ToString() );
					}

					if ( client.IsOpen() )
					{
						char	buf[128];
						auto [err1, recv] = client.Receive( OUT buf, Sizeof(buf) );

						if ( err1 == SocketReceiveError::Received )
						{
							TEST( recv > 0 );
							recv_data.insert( recv_data.end(), buf, buf + recv );

							AE_LOGI( "TCP: received "s << ToString(recv) );

							auto [err2, sent] = client.Send( buf, recv );

							if ( err2 > SocketSendError::_Error or sent != recv )
							{
								AE_LOGI( "TCP: failed to send back" );
							}
						}
						ThreadUtils::MilliSleep( milliseconds{100} );
					}

					if ( ArraySizeOf(recv_data) >= send_data_size )
						break;

					ThreadUtils::MilliSleep( milliseconds{100} );
				}

				TEST( ArraySizeOf(recv_data) >= send_data_size );
				TEST( ArrayView<char>{send_data1} == ArrayView<char>{recv_data}.section( 0, CountOf(send_data1) ));
				TEST( ArrayView<char>{send_data2} == ArrayView<char>{recv_data}.section( CountOf(send_data1), CountOf(send_data2) ));

				server.Close();
			}}};

		sync.Wait();

		TcpSocket	client;
		TEST( client.Connect( Address::FromHostPortTCP( "localhost", c_Port ) ));
		TEST( client.IsOpen() );

		sync.Wait();

		{
			auto [err, sent] = client.Send( send_data1, Sizeof(send_data1) );

			TEST( err == SocketSendError::Sent );
			TEST( sent == Sizeof(send_data1) );
		}{
			auto [err, sent] = client.Send( send_data2, Sizeof(send_data2) );

			TEST( err == SocketSendError::Sent );
			TEST( sent == Sizeof(send_data2) );
		}

		char	buf[128];
		Bytes	total_recv;
		for (; total_recv < send_data_size;)
		{
			auto [err, recv] = client.Receive( OUT buf, Sizeof(buf) );

			total_recv += recv;

			TEST( err < SocketReceiveError::_Error );
		}

		TEST( total_recv == send_data_size );
		TEST( ArrayView<char>{send_data1} == ArrayView<char>{buf}.section( 0, CountOf(send_data1) ));
		TEST( ArrayView<char>{send_data2} == ArrayView<char>{buf}.section( CountOf(send_data1), CountOf(send_data2) ));

		listener.join();

		client.Close();
	}

	static void  TCP_Test1 ()
	{
		TCP_Test_IPv< IpAddress >();
	}

	static void  TCP_Test2 ()
	{
		TCP_Test_IPv< IpAddress6 >();
	}
	//-----------------------------------------------------
}


extern void UnitTest_TCP ()
{
	TCP_Test1();
	TCP_Test2();

	TEST_PASSED();
}
