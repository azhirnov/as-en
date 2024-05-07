// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/TcpSocket.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

namespace AE::Networking
{

/*
=================================================
	Accept
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	inline bool  TcpSocket::_Accept (const TcpSocket &other, OUT AddressType &clientAddr) __NE___
	{
		CHECK_ERR( not IsOpen() );

		if_unlikely( not other.IsOpen() )
			return false;

		NativeAddress	src_addr;
		socklen_t		addr_len = sizeof(src_addr);

		_handle = BitCast<Socket_t>(::accept( BitCast<NativeSocket_t>(other._handle), OUT Cast<sockaddr>(&src_addr), OUT &addr_len ));

		if_likely( _handle == Default )
			return false;	// no clients	// TODO: check errors

	  #ifdef AE_UNIX_SOCKET
		// On Linux, the new socket returned by accept() does not inherit file status flags such as O_NONBLOCK and O_ASYNC from the listening socket.
		// This behavior differs from the canonical BSD sockets implementation.
		if ( other.IsNonBlocking() )
			CHECK_ERR( _SetNonBlocking() );

		if ( other.IsNoDelay() )
			CHECK_ERR( _SetNoDelay() );
	  #endif

		clientAddr = AddressType::FromNative( AnyTypeCRef{ src_addr });
		ASSERT( clientAddr.IsValid() );

		DEBUG_ONLY(
			if constexpr( IsSameTypes< AddressType, IpAddress6 >){
				_dbgAddr6	= clientAddr;
				_isIPv6		= true;
			}else{
				_dbgAddr4	= clientAddr;
				_isIPv6		= false;
			})
		return true;
	}

	bool  TcpSocket::Accept (const TcpSocket &other, OUT IpAddress &clientAddr) __NE___
	{
		return _Accept< sockaddr_in >( other, OUT clientAddr );
	}

	bool  TcpSocket::Accept (const TcpSocket &other, OUT IpAddress6 &clientAddr) __NE___
	{
		return _Accept< sockaddr_in6 >( other, OUT clientAddr );
	}

/*
=================================================
	Listen
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	bool  TcpSocket::_Listen (const AddressType &addr, const Config &cfg) __NE___
	{
		constexpr bool	is_ipv6 = IsSameTypes< AddressType, IpAddress6 >;

		ASSERT( addr.IsValid() );
		CHECK_ERR( _Create( cfg, Bool{is_ipv6} ));

		// binding
		{
			const int		opt			= cfg.reuseAddress ? 1 : 0;
			NativeAddress	sock_addr;
			addr.ToNative( OUT AnyTypeRef{ sock_addr });

			if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_REUSEADDR, Cast<NativeSocketOptPtr_t>(&opt), sizeof(opt) ))
			{
				NET_CHECK2( "TCP("s << GetDebugName() << ") failed to set 'reuse_address' flag" );
			}

			if_unlikely( ::bind( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&sock_addr), sizeof(sock_addr) ) != 0 )
			{
				NET_CHECK2( "TCP("s << GetDebugName() << ") failed to bind socket to port ("s << ToString(addr.Port()) << "): " );
				Close();
				return false;
			}

			if_unlikely( ::listen( BitCast<NativeSocket_t>(_handle), (cfg.maxConnections == UMax ? SOMAXCONN : cfg.maxConnections) ) != 0 )
			{
				NET_CHECK2( "TCP("s << GetDebugName() << ") failed to listen on port ("s << ToString(addr.Port()) << "): " );
				Close();
				return false;
			}
		}

		if ( cfg.nonBlocking )
			CHECK_ERR( _SetNonBlocking() );

		if ( cfg.noDelay )
			CHECK_ERR( _SetNoDelay() );

		DEBUG_ONLY(
			if constexpr( is_ipv6 ){
				_dbgAddr6	= addr;
				_isIPv6		= true;
			}else{
				_dbgAddr4	= addr;
				_isIPv6		= false;
			})
		return true;
	}

	bool  TcpSocket::Listen (const IpAddress &addr, const Config &cfg) __NE___
	{
		return _Listen< sockaddr_in >( addr, cfg );
	}

	bool  TcpSocket::Listen (const IpAddress6 &addr, const Config &cfg) __NE___
	{
		return _Listen< sockaddr_in6 >( addr, cfg );
	}

/*
=================================================
	Connect
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	inline bool  TcpSocket::_Connect (const AddressType &addr, const Config &cfg) __NE___
	{
		constexpr bool	is_ipv6 = IsSameTypes< AddressType, IpAddress6 >;

		ASSERT( addr.IsValid() );
		CHECK_ERR( _Create( cfg, Bool{is_ipv6} ));

		// connect
		{
			NativeAddress	peer_addr;
			addr.ToNative( OUT AnyTypeRef{ peer_addr });

			if_unlikely( ::connect( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&peer_addr), sizeof(peer_addr) ) != 0 )
			{
				NET_CHECK2( "TCP("s << GetDebugName() << ") failed to connect socket to address ("s << addr.ToString() << "): " );
				Close();
				return false;
			}
		}

		// set non-blocking after 'connect()' otherwise 'connect()' will return EAGAIN or EINPROGRESS until 3-way handshake is in progress.
		if ( cfg.nonBlocking )
			CHECK_ERR( _SetNonBlocking() );

		if ( cfg.noDelay )
			CHECK_ERR( _SetNoDelay() );

		DEBUG_ONLY(
			if constexpr( is_ipv6 ){
				_dbgAddr6	= addr;
				_isIPv6		= true;
			}else{
				_dbgAddr4	= addr;
				_isIPv6		= false;
			})
		return true;
	}

	bool  TcpSocket::Connect (const IpAddress &addr, const Config &cfg) __NE___
	{
		ASSERT( addr.Address() != 0 );
		return _Connect< sockaddr_in >( addr, cfg );
	}

	bool  TcpSocket::Connect (const IpAddress6 &addr, const Config &cfg) __NE___
	{
		return _Connect< sockaddr_in6 >( addr, cfg );
	}

/*
=================================================
	AsyncConnect
----
	use 'ConnectionStatus()' to check is it connected or failed.
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	inline bool  TcpSocket::_AsyncConnect (const AddressType &addr, const Config &cfg) __NE___
	{
		constexpr bool	is_ipv6 = IsSameTypes< AddressType, IpAddress6 >;

		ASSERT( addr.IsValid() );
		CHECK_ERR( _Create( cfg, Bool{is_ipv6} ));

		if ( cfg.nonBlocking )
			CHECK_ERR( _SetNonBlocking() );

		if ( cfg.noDelay )
			CHECK_ERR( _SetNoDelay() );

		// connect
		{
			NativeAddress	peer_addr;
			addr.ToNative( OUT AnyTypeRef{ peer_addr });

			const int	res = ::connect( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&peer_addr), sizeof(peer_addr) );
			const auto	err = PlatformUtils::GetNetworkErrorCode();

			if_unlikely( res != 0 and not IsInProgress( err ))
			{
				NET_CHECK( err, "TCP("s << GetDebugName() << ") failed to connect socket to address ("s << addr.ToString() << "): " );
				Close();
				return false;
			}
		}

		DEBUG_ONLY(
			if constexpr( is_ipv6 ){
				_dbgAddr6	= addr;
				_isIPv6		= true;
			}else{
				_dbgAddr4	= addr;
				_isIPv6		= false;
			})
		return true;
	}

	bool  TcpSocket::AsyncConnect (const IpAddress &addr, const Config &cfg) __NE___
	{
		ASSERT( addr.Address() != 0 );
		return _AsyncConnect< sockaddr_in >( addr, cfg );
	}

	bool  TcpSocket::AsyncConnect (const IpAddress6 &addr, const Config &cfg) __NE___
	{
		return _AsyncConnect< sockaddr_in6 >( addr, cfg );
	}

/*
=================================================
	_Create
=================================================
*/
	bool  TcpSocket::_Create (const Config &cfg, Bool useIPv6) __NE___
	{
		CHECK_ERR( not IsOpen() );

		_handle = BitCast<Socket_t>(::socket( (useIPv6 ? AF_INET6 : AF_INET), SOCK_STREAM, IPPROTO_TCP ));

		if_unlikely( _handle == Default )
		{
			NET_CHECK2( "TCP("s << GetDebugName() << ") socket() failed with error: " );
			return false;
		}

		DEBUG_ONLY( _isIPv6 = useIPv6; )

		CHECK_ERR( _SetSendBufferSize( cfg.sendBufferSize ));
		CHECK_ERR( _SetReceiveBufferSize( cfg.receiveBufferSize ));
		return true;
	}

/*
=================================================
	_SetNoDelay
=================================================
*/
	bool  TcpSocket::_SetNoDelay () __NE___
	{
		ASSERT( IsOpen() );

		int		option = 1;

		if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), IPPROTO_TCP, TCP_NODELAY, Cast<NativeSocketOptPtr_t>(&option), sizeof(option) ) != 0 )
		{
			NET_CHECK2( "TCP("s << GetDebugName() << ") failed to set socket no-delay mode: " );
			return false;
		}
		return true;
	}

/*
=================================================
	IsNoDelay
=================================================
*/
	bool  TcpSocket::IsNoDelay () C_NE___
	{
		ASSERT( IsOpen() );

		int			option	= 0;
		socklen_t	len		= sizeof(option);

		if_likely( ::getsockopt( BitCast<NativeSocket_t>(_handle), IPPROTO_TCP, TCP_NODELAY, OUT Cast<NativeSocketOptPtr_t>(&option), INOUT &len ) == 0 )
		{
			return option == 1;
		}
		return false;
	}

/*
=================================================
	Send
=================================================
*/
	auto  TcpSocket::Send (const void* data, const Bytes dataSize) C_NE___ -> Tuple< SocketSendError, Bytes >
	{
		ASSERT( data != null or dataSize > 0 );

		if_unlikely( not IsOpen() )
			return Tuple{ SocketSendError::NoSocket, 0_b };

		const Byte_t	sent = ::send( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0 );

		if_unlikely( sent < 0 )
		{
			const auto	err = PlatformUtils::GetNetworkErrorCode();
			DEBUG_ONLY( if ( ShouldPrintError( err )) NET_CHECK( err, "TCP("s << GetDebugName() << ") failed to write to socket ("s << _GetLastAddress() << "): " );)
			return Tuple{ TranslateSocketSendError( err ), 0_b };
		}
		return Tuple{ (sent ? SocketSendError::Sent : SocketSendError::NotSent), Bytes{ulong(sent)} };
	}

/*
=================================================
	Receive
=================================================
*/
	auto  TcpSocket::Receive (OUT void* data, const Bytes dataSize) C_NE___ -> Tuple< SocketReceiveError, Bytes >
	{
		ASSERT( data != null or dataSize > 0 );

		if_unlikely( not IsOpen() )
			return Tuple{ SocketReceiveError::NoSocket, 0_b };

		const Byte_t	received = ::recv( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0 );

		if_unlikely( received < 0 )
		{
			const auto	err = PlatformUtils::GetNetworkErrorCode();
			DEBUG_ONLY( if ( ShouldPrintError( err )) NET_CHECK( err, "TCP("s << GetDebugName() << ") failed to read from socket ("s << _GetLastAddress() << "): " );)
			return Tuple{ TranslateSocketReceiveError( err ), 0_b };
		}
		return Tuple{ (received ? SocketReceiveError::Received : SocketReceiveError::NotReceived), Bytes{ulong(received)} };
	}

/*
=================================================
	KeepAlive
=================================================
*/
	bool  TcpSocket::KeepAlive (bool enable) __NE___
	{
		ASSERT( IsOpen() );

		const int	i_enable = int(enable);

		if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_KEEPALIVE, Cast<NativeSocketOptPtr_t>(&i_enable), sizeof(i_enable) ) != 0 )
		{
			NET_CHECK2( "TCP("s << GetDebugName() << ") failed to set socket keep alive: " );
			return false;
		}

		return true;
	}

/*
=================================================
	IsKeepAlive
=================================================
*/
	bool  TcpSocket::IsKeepAlive () C_NE___
	{
		ASSERT( IsOpen() );

		int			i_enable	= 0;
		socklen_t	len			= sizeof(i_enable);

		if_likely( ::getsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_KEEPALIVE, OUT Cast<NativeSocketOptPtr_t>(&i_enable), INOUT &len ) == 0 )
		{
			return i_enable == 1;
		}
		return false;
	}

/*
=================================================
	GetPeerName
----
	Returns peer address which was previously used in 'Connect()' call.
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	bool  TcpSocket::_GetPeerName (OUT AddressType &outAddr) C_NE___
	{
		constexpr bool	is_ipv6 = IsSameTypes< AddressType, IpAddress6 >;
		ASSERT( _isIPv6 == is_ipv6 );
		Unused( is_ipv6 );

		NativeAddress	addr;
		socklen_t		addr_len = sizeof(addr);

		if_unlikely( ::getpeername( BitCast<NativeSocket_t>(_handle), OUT Cast<sockaddr>(&addr), OUT &addr_len ) != 0 )
		{
			DEBUG_ONLY(
				const auto	err = PlatformUtils::GetNetworkErrorCode();
				if ( not IsNotConnected( err ))
					NET_CHECK( err, "Failed to get peer address: " );
			)
			return false;
		}

		outAddr = AddressType::FromNative( AnyTypeCRef{ addr });
		ASSERT( outAddr.IsValid() );

		return true;
	}

	bool  TcpSocket::GetPeerName (OUT IpAddress &outAddr) C_NE___
	{
		return _GetPeerName< sockaddr_in >( outAddr );
	}

	bool  TcpSocket::GetPeerName (OUT IpAddress6 &outAddr) C_NE___
	{
		return _GetPeerName< sockaddr_in6 >( outAddr );
	}

/*
=================================================
	ConnectionStatus
----
	If not connected 'getpeername()' returns error.
	Non-blocking socket in 'Connect()' will start 3-way handshake, so we need to check if connection is established.
=================================================
*/
	TcpSocket::EStatus  TcpSocket::ConnectionStatus () C_NE___
	{
		if ( not IsOpen() )
			return EStatus::NoSocket;

		const auto	CheckError = [this] (EStatus onSuccess)
		{{
			int			err	= 0;
			socklen_t	len = sizeof(err);

			if ( ::getsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_ERROR, OUT Cast<NativeSocketOptPtr_t>(&err), &len ) == 0 )
			{
				return 	err == 0 ? onSuccess :
						IsInProgress( err ) ? EStatus::Connecting :
						EStatus::Failed;
			}
			return EStatus::Failed;
		}};

		struct timeval wait_time;
		wait_time.tv_sec	= 0;
		wait_time.tv_usec	= 0;

		fd_set	socket_set = {};

	  #ifdef AE_WINDOWS_SOCKET
		socket_set.fd_count		= 1;
		socket_set.fd_array[0]	= BitCast<NativeSocket_t>(_handle);
	  #else
		FD_ZERO( &socket_set );
		FD_SET( BitCast<NativeSocket_t>(_handle), &socket_set );
	  #endif

		int	count = ::select( int(BitCast<NativeSocket_t>(_handle) + 1), &socket_set, &socket_set, null, &wait_time );
		if ( count == 0 )
			return CheckError( EStatus::Connecting );	// select() timeout

		if ( count > 0 )
			return CheckError( EStatus::Connected );

		const auto	err = PlatformUtils::GetNetworkErrorCode();

		if ( IsInProgress( err ))
			return EStatus::Connecting;

		return EStatus::Failed;
	}


} // AE::Networking
