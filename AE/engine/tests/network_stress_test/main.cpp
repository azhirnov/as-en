// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Networking.h"

using namespace AE;
using namespace AE::Networking;


extern void  UdpServerV4 (const IpAddress &serverAddr);
extern void  UdpClientV4 (const IpAddress &serverAddr, const IpAddress &clientAddr);

extern void  TcpServerV4 (const IpAddress &serverAddr);
extern void  TcpClientV4 (const IpAddress &serverAddr);

extern void  TcpServerV6 (const IpAddress6 &serverAddr);
extern void  TcpClientV6 (const IpAddress6 &serverAddr);

extern void  UdpServerV6 (const IpAddress6 &serverAddr);
extern void  UdpClientV6 (const IpAddress6 &serverAddr, const IpAddress6 &clientAddr);

extern void  TcpMsgServerV4 (ushort port);
extern void  TcpMsgClientV4 (ArrayView<IpAddress> serverAddr);



static IpAddress  GetSelfIPv4AddressFromRouter ()
{
    IpAddress   addr;
    CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress::FromServiceUDP( "192.168.0.1", "8080" ), OUT addr ));
    return addr;
}

static IpAddress6  GetSelfIPv6AddressFromRouter ()
{
    IpAddress6  addr;
    #ifdef AE_PLATFORM_APPLE
        CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress6::FromServiceUDP( "IPv6 addr%en0", "80" ), OUT addr ));
    #else
        CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress6::FromServiceUDP( "IPv6 addr", "" ), OUT addr ));
    #endif
    return addr;
}

static IpAddress  GetSelfIPv4AddressFromGoogleDNS ()
{
    IpAddress   addr;
    CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress::FromServiceUDP( "8.8.8.8", "53" ), OUT addr ));
    return addr;
}

static IpAddress6  GetSelfIPv6AddressFromGoogleDNS ()
{
    IpAddress6  addr;
    CHECK( SocketService::Instance().GetSelfIPAddress( IpAddress6::FromServiceUDP( "2001:4860:4860::8888", "53" ), OUT addr ));
    return addr;
}


int main ()
{
    AE::Base::StaticLogger::LoggerDbgScope  log{};

    auto&   mngr = SocketService::Instance();
    CHECK_ERR( mngr.Initialize() );

    // local IPv4/IPv6 using router address
    #if 0
        AE_LOGI( "Local address iPv4: "s << GetSelfIPv4AddressFromRouter().ToString() );
        AE_LOGI( "Local address iPv6: "s << GetSelfIPv6AddressFromRouter().ToString() );
    #endif

    // local IPv4/IPv6 using Google DNS
    #if 0
        AE_LOGI( "Local address iPv4: "s << GetSelfIPv4AddressFromGoogleDNS().ToString() );
        AE_LOGI( "Local address iPv6: "s << GetSelfIPv6AddressFromGoogleDNS().ToString() );
    #endif
    //-------------------------------------------


    // server TCP/UDP IPv4
    #if 0
        StdThread   tcp_thread{ [](){ TcpServerV4( IpAddress::FromLocalPortTCP( 3000 )); }};
        StdThread   udp_thread{ [](){ UdpServerV4( IpAddress::FromLocalPortUDP( 3001 )); }};

        tcp_thread.join();
        udp_thread.join();
    #endif

    // server TCP/UDP IPv6
    #if 0
        IpAddress6  addr = GetSelfIPv6AddressFromRouter();

        StdThread   tcp_thread{ [addr]() mutable { addr.SetPort( 3000 );  TcpServerV6( addr ); }};
        StdThread   udp_thread{ [addr]() mutable { addr.SetPort( 3001 );  UdpServerV6( addr ); }};

        tcp_thread.join();
        udp_thread.join();
    #endif
    //-------------------------------------------


    // TCP client/server IPv4
    #if 0
        StdThread   server_thread{ [](){ TcpServerV4( IpAddress::FromLocalhostTCP( 3000 )); }};
        StdThread   client_thread{ [](){ TcpClientV4( IpAddress::FromLocalhostTCP( 3000 )); }};

        server_thread.join();
        client_thread.join();
    #endif

    // TCP client/server IPv6
    #if 0
        IpAddress6  addr = GetSelfIPv6AddressFromRouter();  addr.SetPort( 3000 );

        StdThread   server_thread{ [addr](){ TcpServerV6( addr ); }};
        StdThread   client_thread{ [addr](){ TcpClientV6( addr ); }};

        server_thread.join();
        client_thread.join();
    #endif
    //-------------------------------------------


    // UDP client/server IPv4
    #if 0
        StdThread   server_thread{ [](){ UdpServerV4( IpAddress::FromLocalhostUDP( 3000 )); }};
        StdThread   client_thread{ [](){ UdpClientV4( IpAddress::FromLocalhostUDP( 3000 ), IpAddress::FromLocalhostUDP( 3001 )); }};

        server_thread.join();
        client_thread.join();
    #endif

    // UDP client/server IPv6
    #if 0
        IpAddress6  addr = GetSelfIPv6AddressFromRouter();  addr.SetPort( 3000 );

        StdThread   server_thread{ [addr](){ UdpServerV6( addr ); }};
        StdThread   client_thread{ [addr](){ UdpClientV6( addr, IpAddress6::FromLocalhostUDP( 3001 )); }};

        server_thread.join();
        client_thread.join();
    #endif
    //-------------------------------------------


    // UDP client
    #if 0
        UdpClientV4( IpAddress::FromHostPortUDP( "IPv4 addr", 3001 ));
    #endif
    #if 0
        UdpClientV6( IpAddress6::FromHostPortUDP( "IPv4 addr", 3001 ));
    #endif

    // TCP client
    #if 0
        TcpClientV4( IpAddress::FromHostPortTCP( "IPv4 addr", 3000 ));
    #endif
    #if 0
    //  TcpClientV6( IpAddress6{ 0xfe80, 0, 0, 0, 0x8a6, 0xeb9, 0x7c2a, 0x8cab, 3000, 0 });
        TcpClientV6( IpAddress6::FromString( "[IPv6 addr]:3000" ));
    //  TcpClientV6( IpAddress6::FromServiceUDP( "IPv6 addr", "" ));    // error
    #endif
    //-------------------------------------------


    #if 1
        StdThread   server_thread{ [](){ TcpMsgServerV4( 3000 ); }};
        StdThread   client_thread{ [](){ TcpMsgClientV4( {  IpAddress::FromHostPortTCP( "IPv4 addr", 3000 ),
                                                            IpAddress::FromHostPortTCP( "IPv4 addr", 3000 )
                                                         }); }};

        server_thread.join();
        client_thread.join();
    #endif
    //-------------------------------------------


    mngr.Deinitialize();
    return 0;
}
