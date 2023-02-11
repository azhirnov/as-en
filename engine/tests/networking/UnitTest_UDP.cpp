// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
	static void  UDP_Test1 ()
	{
		LocalSocketMngr	mngr;

		const char	send_data[] = "12346ewiofdklijnskdn";

		StdThread	listener{ [&] ()
			{
				UdpSocket	sock2;
				TEST( sock2.Open( 4000 ));
				TEST( sock2.IsOpen() );

				Array<char>	recv_data;
				for (uint i = 0; i < 100; ++i)
				{
					IpAddress	addr;
					Bytes		recv;
					char		buf[128];

					if ( sock2.Receive( OUT addr, buf, Sizeof(buf), OUT recv ) and recv > 0 )
					{
						recv_data.insert( recv_data.end(), buf, buf + recv );

						AE_LOGI( "Received "s << ToString(recv) << " from " << addr.ToString() );
					}
					
					if ( recv_data.size() >= sizeof(send_data) )
						break;

					ThreadUtils::Sleep( milliseconds{100} );
				}

				TEST( sizeof(send_data) == recv_data.size() );
				TEST( ArrayView<char>{send_data} == recv_data );
			}};

		UdpSocket	sock1;
		TEST( sock1.Open( 3000 ));
		TEST( sock1.IsOpen() );

		Bytes	sent;
		TEST( sock1.Send( IpAddress::FromHostPortUDP( "localhost", 4000 ), send_data, Sizeof(send_data), OUT sent ));
		TEST( sent == Sizeof(send_data) );

		listener.join();

		sock1.Close();
	}
}


extern void UnitTest_UDP ()
{
	UDP_Test1();

	TEST_PASSED();
}
