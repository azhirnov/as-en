// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Networking.h"

using namespace AE;
using namespace AE::Networking;

namespace {
    static constexpr uint   MsgSize = 1024;
}


extern void  UdpServerV6 (const IpAddress6 &serverAddr)
{
    UdpSocket   server;
    CHECK_ERRV( server.Open( serverAddr ));
    CHECK_ERRV( server.IsOpen() );

    {
        IpAddress6  addr;
        CHECK_ERRV( server.GetAddress( addr ));
        AE_LOGI( "UDP6 server: started in "s << addr.ToString() );
    }

    char    buf [MsgSize];

    for (;;)
    {
        IpAddress6  addr;
        auto        [err, recv] = server.Receive( OUT addr, buf, Sizeof(buf) );

        if ( recv > 0 )
        {
            AE_LOGI( "UDP6 server: received "s << ToString(recv) << " from " << addr.ToString() );

            auto    [err2, sent] = server.Send( addr, buf, recv );
            if ( sent > 0 )
                AE_LOGI( "UDP6 server: sent "s << ToString(sent) );

            continue;
        }
        ThreadUtils::MilliSleep( milliseconds{100} );
    }
}


extern void  UdpClientV6 (const IpAddress6 &serverAddr, const IpAddress6 &clientAddr)
{
    UdpSocket   client;
    CHECK_ERRV( client.Open( clientAddr ));
    CHECK_ERRV( client.IsOpen() );

    Array<char>     rnd_data;
    for (uint i = 0; i < MsgSize; ++i) {
        rnd_data.push_back( char(::rand() & 0x7F) );
    }

    char    buf [MsgSize];

    for (;;)
    {
        usize   size = ::rand() % rnd_data.size();
        if ( size == 0 )
            continue;

        auto    [err, sent] = client.Send( serverAddr, rnd_data.data(), Bytes{size} );
        if ( sent > 0 )
            AE_LOGI( "UDP6 client: sent "s << ToString(sent) << " to " << serverAddr.ToString() );

        IpAddress6  addr2;
        auto    [err2, recv] = client.Receive( OUT addr2, OUT buf, Sizeof(buf) );
        if ( recv > 0 )
            AE_LOGI( "UDP6 client: received "s << ToString(recv) << " from " << addr2.ToString() );

        ThreadUtils::MilliSleep( milliseconds{50} );
    }
}
