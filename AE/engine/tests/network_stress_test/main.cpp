// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "pch/Networking.h"
#include "../tests/shared/UnitTest_Shared.h"

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
	IpAddress	addr;
	CHECK( SocketService::Instance().GetSelfLocalIPAddress( OUT addr ));
	return addr;
}

static IpAddress6  GetSelfIPv6AddressFromRouter ()
{
	IpAddress6	addr;
	CHECK( SocketService::Instance().GetSelfIPAddress( AE_ROUTER_IPv6, OUT addr ));
	return addr;
}

static IpAddress  GetSelfIPv4AddressFromGoogleDNS ()
{
	IpAddress	addr;
	CHECK( SocketService::Instance().GetSelfIPAddress( AE_GOOGLE_DNS_IPv4, OUT addr ));
	return addr;
}

static IpAddress6  GetSelfIPv6AddressFromGoogleDNS ()
{
	IpAddress6	addr;
	CHECK( SocketService::Instance().GetSelfIPAddress( AE_GOOGLE_DNS_IPv6, OUT addr ));
	return addr;
}


TEST_ENTRY()
{
	BEGIN_TEST();

	auto&	mngr = SocketService::Instance();
	CHECK_ERR( mngr.Initialize() );

	static constexpr ushort	TCP_port		= AE_TEST_TCP_PORT_1;
	static constexpr ushort	UDP_port		= AE_TEST_UDP_PORT_1;
	static constexpr ushort	UDP_port2		= AE_TEST_UDP_PORT_2;
	static constexpr char	IP_address[]	= AE_TEST_IPv4;

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
		StdThread	tcp_thread{ [](){ TcpServerV4( IpAddress::FromLocalPortTCP( TCP_port )); }};
		StdThread	udp_thread{ [](){ UdpServerV4( IpAddress::FromLocalPortUDP( UDP_port )); }};

		tcp_thread.join();
		udp_thread.join();
	#endif

	// server TCP/UDP IPv6
	#if 0
		IpAddress6	addr = GetSelfIPv6AddressFromRouter();

		StdThread	tcp_thread{ [addr]() mutable { addr.SetPort( TCP_port );  TcpServerV6( addr ); }};
		StdThread	udp_thread{ [addr]() mutable { addr.SetPort( UDP_port );  UdpServerV6( addr ); }};

		tcp_thread.join();
		udp_thread.join();
	#endif
	//-------------------------------------------


	// TCP client/server IPv4
	#if 0
		StdThread	server_thread{ [](){ TcpServerV4( IpAddress::FromLocalhostTCP( TCP_port )); }};
		StdThread	client_thread{ [](){ TcpClientV4( IpAddress::FromLocalhostTCP( TCP_port )); }};

		server_thread.join();
		client_thread.join();
	#endif

	// TCP client/server IPv6
	#if 0
		IpAddress6	addr = GetSelfIPv6AddressFromRouter();	addr.SetPort( TCP_port );

		StdThread	server_thread{ [addr](){ TcpServerV6( addr ); }};
		StdThread	client_thread{ [addr](){ TcpClientV6( addr ); }};

		server_thread.join();
		client_thread.join();
	#endif
	//-------------------------------------------


	// UDP client/server IPv4
	#if 0
		StdThread	server_thread{ [](){ UdpServerV4( IpAddress::FromLocalhostUDP( UDP_port )); }};
		StdThread	client_thread{ [](){ UdpClientV4( IpAddress::FromLocalhostUDP( UDP_port ), IpAddress::FromLocalhostUDP( UDP_port2 )); }};

		server_thread.join();
		client_thread.join();
	#endif

	// UDP client/server IPv6
	#if 0
		IpAddress6	addr = GetSelfIPv6AddressFromRouter();	addr.SetPort( UDP_port );

		StdThread	server_thread{ [addr](){ UdpServerV6( addr ); }};
		StdThread	client_thread{ [addr](){ UdpClientV6( addr, IpAddress6::FromLocalhostUDP( UDP_port2 )); }};

		server_thread.join();
		client_thread.join();
	#endif
	//-------------------------------------------


	// UDP client
	#if 0
		UdpClientV4( IpAddress::FromHostPortUDP( IP_address, UDP_port ));
	#endif
	#if 0
		UdpClientV6( IpAddress6::FromHostPortUDP( IP_address, UDP_port ));
	#endif

	// TCP client
	#if 0
		TcpClientV4( IpAddress::FromHostPortTCP( IP_address, TCP_port ));
	#endif
	//-------------------------------------------


	#if 1
		StdThread	server_thread{ [](){ TcpMsgServerV4( TCP_port ); }};
		StdThread	client_thread{ [](){ TcpMsgClientV4( {	IpAddress::FromHostPortTCP( IP_address, TCP_port )
														 }); }};

		server_thread.join();
		client_thread.join();
	#endif
	#if 0
		TcpMsgServerV4( TCP_port );
	#endif
	#if 0
		TcpMsgClientV4({ IpAddress::FromHostPortTCP( IP_address, TCP_port )});
	#endif
	//-------------------------------------------


	mngr.Deinitialize();
	return 0;
}
