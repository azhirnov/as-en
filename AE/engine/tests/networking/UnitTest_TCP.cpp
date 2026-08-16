// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
	static const ushort		c_Port = AE_TEST_TCP_PORT_1;


	template <typename Address>
	static void  TCP_Test_IPv (Address clientAddr, Address serverAddr)
	{
		const char	send_data1[] = "12346ewiofdklijnskdn";
		const char	send_data2[] = "woihfdifkhjnwopefjseoivnj";
		const Bytes	send_data_size = Sizeof(send_data1) + Sizeof(send_data2);

		Threading::Barrier	sync {2};

		StdThread	listener{ [&] ()
			{{
				TcpSocket	server;
				TEST( server.Listen( serverAddr ));
				TEST( server.IsOpen() );

				sync.Wait();
				sync.Wait();

				Address		addr;
				TcpSocket	client;		// client socket on server side

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

					// receive
					if ( client.IsOpen() )
					{
						char	buf[128];
						auto [err1, recv] = client.Receive( OUT buf, Sizeof(buf) );

						if ( err1 == SocketReceiveError::Received )
						{
							TEST( recv > 0 );
							recv_data.insert( recv_data.end(), buf, buf + recv );

							AE_LOGI( "TCP: received "s << ToString(recv) );

							// send
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

		TcpSocket	client;		// on user side
		TEST( client.Connect( clientAddr ));
		TEST( client.IsOpen() );

		sync.Wait();

		TEST( client.ConnectionStatus() == TcpSocket::EStatus::Connected );

		// send
		{
			auto [err, sent] = client.Send( send_data1, Sizeof(send_data1) );

			TEST( err == SocketSendError::Sent );
			TEST( sent == Sizeof(send_data1) );
		}{
			auto [err, sent] = client.Send( send_data2, Sizeof(send_data2) );

			TEST( err == SocketSendError::Sent );
			TEST( sent == Sizeof(send_data2) );
		}

		// receive
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


	template <typename Address>
	static void  TCP_Test_IP ()
	{
		LocalSocketMngr	mngr;

		// localhost
		{
			auto	client	= Address::FromHostPortTCP( "localhost", c_Port );
			auto	server	= Address::FromLocalhostTCP( c_Port );

			TCP_Test_IPv( client, server );
		}

		// local network
		{
			Address	server_addr;

			if constexpr( IsSame< Address, IpAddress >)
			{
				if ( not mngr->GetSelfLocalIPAddress( OUT server_addr ))
				{
					AE_LOGW( "GetSelfLocalIPAddress failed" );
					return;
				}
			}
			if constexpr( IsSame< Address, IpAddress6 >)
			{
				if ( not mngr->GetSelfIPAddress( AE_ROUTER_IPv6, OUT server_addr ))
				{
					AE_LOGW( "GetSelfIPAddress failed for "s << AE_ROUTER_IPv6.ToString() );
					return;
				}
			}

			auto	client	= server_addr;
			auto	server	= Address::FromLocalPortTCP( c_Port );

			client.SetPort( c_Port );

			TCP_Test_IPv( client, server );
		}
	}


	static void  TCP_Test1 ()
	{
		TCP_Test_IP< IpAddress >();
	}

	static void  TCP_Test2 ()
	{
		TCP_Test_IP< IpAddress6 >();
	}
	//-----------------------------------------------------
}


extern void UnitTest_TCP ()
{
	TCP_Test1();
	TCP_Test2();

	TEST_PASSED();
}
