// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
    template <typename Address>
    static void  UDP_Test_IPv ()
    {
        LocalSocketMngr mngr;

        const char  send_data[] = "12346ewiofdklijnskdn";

        StdThread   listener{ [&] ()
            {{
                UdpSocket   sock2;
                TEST( sock2.Open( Address::FromLocalhostUDP(4000) ));
                TEST( sock2.IsOpen() );

                Array<char> recv_data;
                for (uint i = 0; i < 100; ++i)
                {
                    Address     addr;
                    char        buf[128];
                    auto        [err, recv] = sock2.Receive( OUT addr, buf, Sizeof(buf) );

                    if ( err == SocketReceiveError::Received )
                    {
                        TEST( recv > 0 );
                        recv_data.insert( recv_data.end(), buf, buf + recv );

                        AE_LOGI( "Received "s << ToString(recv) << " from " << addr.ToString() );
                    }

                    if ( recv_data.size() >= sizeof(send_data) )
                        break;

                    ThreadUtils::MilliSleep( milliseconds{100} );
                }

                TEST( sizeof(send_data) == recv_data.size() );
                TEST( ArrayView<char>{send_data} == recv_data );
            }}};

        UdpSocket   sock1;
        TEST( sock1.Open( Address::FromLocalhostUDP(3000) ));
        TEST( sock1.IsOpen() );

        auto [err, sent] = sock1.Send( Address::FromHostPortUDP( "localhost", 4000 ), send_data, Sizeof(send_data) );

        TEST( err == SocketSendError::Sent );
        TEST( sent == Sizeof(send_data) );

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
