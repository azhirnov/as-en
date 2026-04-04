// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
	
#define AE_ROUTER_IPv4				(Networking::IpAddress::FromServiceUDP( "192.168.0.1", "8080" ))
#ifdef AE_PLATFORM_APPLE
# define AE_ROUTER_IPv6				(Networking::IpAddress6::FromServiceUDP( "TODO%en0", "80" ))
#else
# define AE_ROUTER_IPv6				(Networking::IpAddress6::FromServiceUDP( "TODO", "" ))
#endif

#define AE_GOOGLE_DNS_IPv4			(Networking::IpAddress::FromServiceUDP( "8.8.8.8", "53" ))
#define AE_GOOGLE_DNS_IPv6			(Networking::IpAddress6::FromServiceUDP( "2001:4860:4860::8888", "53" ))


// Remove Graphics
#define AE_RMG_PORT					3000
#define AE_RMG_IPv4					(Networking::IpAddress::FromHostPortTCP( "TODO", AE_RMG_PORT ))


// cicd
#define AE_CICD_PORT				3001
#define AE_CICD_ANDROID_SERVER		(Networking::IpAddress::FromHostPortTCP( "TODO", AE_CICD_PORT ))


// screen share
#define AE_SCREENSHARE_PORT			3002
#define AE_SCREENSHARE_IPv4			(Networking::IpAddress::FromHostPortTCP( "TODO", AE_SCREENSHARE_PORT ))


// LLM server
#define AE_LLM_SERVER_PORT			3003
#define AE_LLM_SERVER_IPv4			(Networking::IpAddress::FromHostPortTCP( "TODO", AE_LLM_SERVER_PORT ))


// VFS server
#define AE_VFS_SERVER_PORT			3004
#define AE_VFS_SERVER_FOLDER		"TODO"


// Android Remote Control
#define AE_ANDREMCTRL_SERVER_PORT	3005
#define AE_ANDREMCTRL_SERVER_IPv4	(Networking::IpAddress::FromHostPortTCP( "TODO", AE_ANDREMCTRL_SERVER_PORT ))


// Android Launcher
#define AE_ANDLAUNCHER_UDP_CLIENT_PORT	3010


// for tests
#define AE_TEST_TCP_PORT_1			3006
#define AE_TEST_TCP_PORT_2			3007
#define AE_TEST_UDP_PORT_1			3008
#define AE_TEST_UDP_PORT_2			3009

#define AE_TEST_IPv4				"TODO"
