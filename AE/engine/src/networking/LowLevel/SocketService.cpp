// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "networking/LowLevel/SocketService.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

namespace AE::Networking
{
namespace {
/*
=================================================
	GetIPRoute
=================================================
*/
#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS)
	ND_ static bool  GetIPRoute (StringView prefix, OUT IpAddress &outAddr) __NE___
	{
		String	cmd = "ip route";
		String	output;

		CHECK_ERR( UnixProcess::Execute( cmd, OUT output, UnixProcess::EFlags::ReadOutput | UnixProcess::EFlags::NoWindow ));

		AE_LOGW( "IP route: '"s << output << "'" );

		const StringView	def {" src "};

		usize	pos = output.find( def );
		if ( pos == String::npos )
			return false;

		pos += def.size();
		usize	end = output.find( ' ', pos );

		outAddr = IpAddress::FromServiceUDP( SubStringBE( output, pos, end ), "0" );
		return true;
	}

	ND_ static bool  GetIPRoute (StringView prefix, OUT IpAddress6 &) __NE___
	{
		String	cmd = "ip -6 route";
		return false;
	}

#endif // AE_PLATFORM_LINUX or AE_PLATFORM_MACOS
} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	SocketService::SocketService () __NE___
	{
	}

/*
=================================================
	destructor
=================================================
*/
	SocketService::~SocketService () __NE___
	{
		Deinitialize();
	}

/*
=================================================
	Instance
=================================================
*/
	bool  SocketService::Initialize () __NE___
	{
		if ( _initialized )
			return true;

	#if defined(AE_WINDOWS_SOCKET)
		WSADATA		wsa_data;
		const WORD	req_ver	= MAKEWORD(2, 2);
		const int	error	= ::WSAStartup( req_ver, OUT &wsa_data );

		if_unlikely( error != NO_ERROR )
		{
			NET_CHECK2( "WSAStartup failed: " );
			return false;
		}
		_initialized = true;

	#elif defined(AE_UNIX_SOCKET)
		_initialized = true;

	#else
	#	error Unsupported platform!
	#endif

		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	bool  SocketService::Deinitialize () __NE___
	{
		if ( not _initialized )
			return true;

		_initialized = false;

	#if defined(AE_WINDOWS_SOCKET)
		::WSACleanup();

	#elif defined(AE_UNIX_SOCKET)
	#else
	#	error Unsupported platform!
	#endif
		return true;
	}

/*
=================================================
	Instance
=================================================
*/
	SocketService&  SocketService::Instance () __NE___
	{
		static SocketService	network;
		return network;
	}

/*
=================================================
	GetSelfIPAddress
----
	How to use:
	set router or any internet DNS address to 'host' and get your local address.
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	bool  SocketService::_GetSelfIPAddress (const AddressType &hostAddr, OUT AddressType &selfAddr) C_NE___
	{
		constexpr bool	is_ipv6 = IsSame< AddressType, IpAddress6 >;

		NativeSocket_t	socket = ::socket( (is_ipv6 ? AF_INET6 : AF_INET), SOCK_DGRAM, 0 );

		if ( BitCast<BaseSocket::Socket_t>(socket) == Default )
			return false;

		bool			result	= false;
		NativeAddress	host_addr;
		hostAddr.ToNative( OUT AnyTypeRef{ host_addr });

		if ( ::connect( socket, Cast<sockaddr>(&host_addr), sizeof(host_addr) ) == 0 )
		{
			NativeAddress	self_addr;
			socklen_t		self_addr_len = sizeof(self_addr);

			if ( ::getsockname( socket, OUT Cast<sockaddr>(&self_addr), OUT &self_addr_len ) == 0 )
			{
				result		= true;
				selfAddr	= AddressType::FromNative( self_addr );
			}
			else
				NET_CHECK2( "getsockname() failed: " );
		}
		else
			NET_CHECK2( "connect() failed: " );

	  #if defined(AE_WINDOWS_SOCKET)
		::closesocket( socket );
	  #elif defined(AE_UNIX_SOCKET)
		::close( socket );
	  #else
		#error Unsupported platform!
	  #endif

		return result;
	}

	bool  SocketService::GetSelfIPAddress (const IpAddress &host, OUT IpAddress &self) C_NE___
	{
		return _GetSelfIPAddress< sockaddr_in >( host, OUT self );
	}

	bool  SocketService::GetSelfIPAddress (const IpAddress6 &host, OUT IpAddress6 &self) C_NE___
	{
		return _GetSelfIPAddress< sockaddr_in6 >( host, OUT self );
	}

/*
=================================================
	GetRouterIPAddress
=================================================
*/
	bool  SocketService::GetRouterIPAddress (OUT IpAddress &outAddr) C_NE___
	{
	#ifdef AE_PLATFORM_WINDOWS
		String	cmd = "ipconfig";
		String	output;

		using EFlags = OSProcess::EFlags;
		CHECK_ERR( WindowsProcess::Execute( cmd, OUT output, EFlags::UseCommandPrompt | EFlags::ReadOutput | EFlags::NoWindow ));

		for (usize pos = 0; pos < output.size();)
		{
			pos = output.find( "Default Gateway", pos );
			if ( pos == String::npos )
				break;

			usize	new_line	= output.find( "\r\n", pos );
			usize	ip_begin	= output.find( ": ",  pos );

			if ( ip_begin < new_line	and
				 ip_begin + 2 < new_line )
			{
				outAddr = IpAddress::FromServiceUDP( SubStringBE( output, ip_begin+2, new_line ), "0" );
				return true;
			}
			pos = new_line;
		}
		return false;

	#elif defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS)
		return GetIPRoute( "default via ", outAddr );

	#elif defined(AE_PLATFORM_ANDROID)
		CHECK_ERR( _cb.getRouterIPAddress != null );
		return _cb.getRouterIPAddress( _cb.userData, OUT outAddr );

	#else
		return false;
	#endif
	}

/*
=================================================
	GetSelfLocalIPAddress
=================================================
*/
	bool  SocketService::GetSelfLocalIPAddress (OUT IpAddress &self) C_NE___
	{
	#if defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_MACOS)
		return GetIPRoute( " src ", OUT self );

	#else
		IpAddress	router_ip;
		return	GetRouterIPAddress( OUT router_ip )  and
				GetSelfIPAddress( router_ip, OUT self );
	#endif
	}

/*
=================================================
	GetSelfGlobalIPAddress
=================================================
*/
	bool  SocketService::GetSelfGlobalIPAddress (OUT IpAddress &self, IpAddress httpServerAddr, nanoseconds timeout) C_NE___
	{
		self = Default;

		TcpSocket::Config	cfg;
		cfg.nonBlocking		= true;

		TcpSocket	tcp;
		if ( not tcp.Connect( httpServerAddr, cfg ))
		{
			// can't connect
			return false;
		}

		const auto	end_time = HighResClock::now() + timeout;

		// request
		for (;;)
		{
			static constexpr char	request[] =
				"GET /?format=text HTTP/1.1\r\n"
				"Host: api.ipify.org\r\n"
				"Connection: close\r\n\r\n";

			auto	[err, size] = tcp.Send( request, Sizeof(request) );

			if ( err >= SocketSendError::_Error or HighResClock::now() > end_time )
			{
				// connection lost
				return false;
			}

			if ( err == SocketSendError::Sent and size == Sizeof(request) )
				break;
		}

		// response
		for (;;)
		{
			char	buf [4096];
			auto	[err, size] = tcp.Receive( OUT buf, Sizeof(buf) );

			if ( err >= SocketReceiveError::_Error or HighResClock::now() > end_time )
			{
				// connection lost
				return false;
			}

			if ( err == SocketReceiveError::Received )
			{
				StringView	div{ "\r\n\r\n" };
				StringView	str{ buf, usize{size} };

				usize	pos = str.find( div );
				if ( pos < str.size() )
				{
					self = IpAddress::FromHostPortTCP( str.substr( pos + div.size() ), 0 );
					return true;
				}
				break;
			}
		}

		// failed to parse response
		return false;
	}

/*
=================================================
	SetCallbacks
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
	void  SocketService::SetCallbacks (const Callbacks &cb) __NE___
	{
		_cb = cb;
	}
#endif

} // AE::Networking
