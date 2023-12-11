// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/ThreadManager.h"
#include "UnitTest_Common.h"

namespace
{
    template <typename Address>
    static void  TCP_Test_IPv ()
    {
        LocalSocketMngr mngr;

        const char  send_data1[] = "12346ewiofdklijnskdn";
        const char  send_data2[] = "woihfdifkhjnwopefjseoivnj";
        const Bytes send_data_size = Sizeof(send_data1) + Sizeof(send_data2);

        TcpSocket   server;
        TEST( server.Listen( Address::FromLocalhostTCP(4000) ));
        TEST( server.IsOpen() );

        StdThread   listener{ [&] ()
            {{
                Array<char> recv_data;
                for (uint i = 0; i < 100; ++i)
                {
                    Address     addr;
                    char        buf[128];

                    TcpSocket   client;
                    if ( client.Accept( server, OUT addr ))
                    {
                        AE_LOGI( "TCP: connected to "s << addr.ToString() );

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
                    }

                    if ( ArraySizeOf(recv_data) >= send_data_size )
                        break;

                    ThreadUtils::MilliSleep( milliseconds{100} );
                }

                TEST( ArraySizeOf(recv_data) >= send_data_size );
                TEST( ArrayView<char>{send_data1} == ArrayView<char>{recv_data}.section( 0, CountOf(send_data1) ));
                TEST( ArrayView<char>{send_data2} == ArrayView<char>{recv_data}.section( CountOf(send_data1), CountOf(send_data2) ));
            }}};

        TcpSocket   client;
        TEST( client.Connect( Address::FromHostPortTCP( "localhost", 4000 ) ));
        TEST( client.IsOpen() );

        {
            auto [err, sent] = client.Send( send_data1, Sizeof(send_data1) );

            TEST( err == SocketSendError::Sent );
            TEST( sent == Sizeof(send_data1) );
        }{
            auto [err, sent] = client.Send( send_data2, Sizeof(send_data2) );

            TEST( err == SocketSendError::Sent );
            TEST( sent == Sizeof(send_data2) );
        }

        char    buf[128];
        Bytes   total_recv;
        for (;;)
        {
            auto [err, recv] = client.Receive( OUT buf, Sizeof(buf) );

            total_recv += recv;

            if ( err == SocketReceiveError::Received or err > SocketReceiveError::_Error )
                break;
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
