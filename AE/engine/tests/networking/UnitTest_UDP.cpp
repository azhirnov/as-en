// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
	static const ushort		c_Port = 3000;


	template <typename Address>
	static void  UDP_Test_IPv ()
	{
		LocalSocketMngr	mngr;

		const char			send_data[] = "12346ewiofdklijnskdn";
		Threading::Barrier	sync {2};

		StdThread	listener{ [&] ()
			{{
				UdpSocket	sock2;
				TEST( sock2.Open( Address::FromLocalhostUDP(c_Port) ));
				TEST( sock2.IsOpen() );

				sync.Wait();

				Array<char>	recv_data;
				for (uint i = 0; i < 1'000; ++i)	// max: 100s
				{
					Address		addr;
					char		buf[128];
					auto		[err, recv] = sock2.Receive( OUT addr, OUT buf, Sizeof(buf) );

					if ( err == SocketReceiveError::Received )
					{
						TEST( recv > 0 );
						recv_data.insert( recv_data.end(), buf, buf + recv );

						AE_LOGI( "UDP received "s << ToString(recv) << " from " << addr.ToString() );

						auto [err2, sent] = sock2.Send( addr, buf, recv );

						if ( err2 > SocketSendError::_Error or sent != recv )
						{
							AE_LOGI( "UDP: failed to send back" );
						}
					}

					if ( ArraySizeOf(recv_data) >= sizeof(send_data) )
						break;

					ThreadUtils::MilliSleep( milliseconds{100} );
				}

				TEST( sizeof(send_data) == ArraySizeOf(recv_data) );
				TEST( ArrayView<char>{send_data} == recv_data );
			}}};

		UdpSocket	sock1;
		TEST( sock1.Open( Address::FromLocalhostUDP(c_Port+1) ));
		TEST( sock1.IsOpen() );

		sync.Wait();

		{
			auto [err, sent] = sock1.Send( Address::FromHostPortUDP( "localhost", c_Port ), send_data, Sizeof(send_data) );

			TEST( err == SocketSendError::Sent );
			TEST( sent == Sizeof(send_data) );
		}

		char	buf[128];
		Bytes	total_recv;
		for (; total_recv < Sizeof(send_data);)
		{
			Address		addr;
			auto [err, recv] = sock1.Receive( OUT addr, OUT buf, Sizeof(buf) );

			total_recv += recv;

			TEST( err < SocketReceiveError::_Error );
		}
		TEST( sizeof(send_data) == total_recv );
		TEST( ArrayView<char>{send_data} == ArrayView<char>{buf, usize{total_recv}} );

		listener.join();

		sock1.Close();
	}

	static void  UDP_Test1 ()
	{
		UDP_Test_IPv< IpAddress >();
	}

	static void  UDP_Test2 ()
	{
		UDP_Test_IPv< IpAddress6 >();
	}
	//-----------------------------------------------------
}


extern void UnitTest_UDP ()
{
	UDP_Test1();
	UDP_Test2();

	TEST_PASSED();
}
