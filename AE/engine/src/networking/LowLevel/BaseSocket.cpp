// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/BaseSocket.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

namespace AE::Networking
{

#if defined(AE_WINDOWS_SOCKET)
	StaticAssert( sizeof(SOCKET) == sizeof(BaseSocket::Socket_t) );
	StaticAssert( BaseSocket::Socket_t(INVALID_SOCKET) == BaseSocket::Socket_t::Unknown );

#elif defined(AE_UNIX_SOCKET)
	StaticAssert( sizeof(int) == sizeof(BaseSocket::Socket_t) );
	StaticAssert( BaseSocket::Socket_t(-1) == BaseSocket::Socket_t::Unknown );

#else
#	error Unsupported platform!
#endif

/*
=================================================
	constructor
=================================================
*/
	BaseSocket::BaseSocket (BaseSocket &&other) __NE___ :
		_handle{ other._handle }
		DEBUG_ONLY(, _dbgName{ RVRef(other._dbgName) })
	{
		other._handle = Default;
	}

/*
=================================================
	Close
=================================================
*/
	void  BaseSocket::Close () __NE___
	{
		if ( _handle != Default )
		{
			#if defined(AE_WINDOWS_SOCKET)
				::shutdown( BitCast<NativeSocket_t>(_handle), SD_BOTH );	// for TCP
				::closesocket( BitCast<NativeSocket_t>(_handle) );

			#elif defined(AE_PLATFORM_EMSCRIPTEN)
				::shutdown( BitCast<NativeSocket_t>(_handle), SHUT_RDWR );

			#elif defined(AE_UNIX_SOCKET)
				::shutdown( BitCast<NativeSocket_t>(_handle), SHUT_RDWR );	// for TCP
				::close( BitCast<NativeSocket_t>(_handle) );

			#else
				#error Unsupported platform!
			#endif
		}
		_handle = Default;
	}

/*
=================================================
	FastClose
=================================================
*/
	void  BaseSocket::FastClose () __NE___
	{
		if ( _handle != Default )
		{
			struct linger		so_linger;
			so_linger.l_onoff	= 1;
			so_linger.l_linger	= 0;
			::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_LINGER, Cast<NativeSocketOptPtr_t>(&so_linger), sizeof(so_linger) );

			Close();
		}
	}

/*
=================================================
	BlockingClose
=================================================
*/
	void  BaseSocket::BlockingClose () __NE___
	{
		if ( _handle != Default )
		{
			Unused( _SetBlocking() );
			Close();
		}
	}

/*
=================================================
	_SetBlocking
=================================================
*/
	bool  BaseSocket::_SetBlocking () __NE___
	{
		ASSERT( IsOpen() );

	#if defined(AE_WINDOWS_SOCKET)
		DWORD	non_blocking = 0;
		if_unlikely( ::ioctlsocket( BitCast<NativeSocket_t>(_handle), FIONBIO, &non_blocking ) != 0 )	// win8.1
		{
			NET_CHECK2( "Failed to set socket blocking mode: " );
			return false;
		}

	#elif defined(AE_UNIX_SOCKET)
		int		non_blocking = 0;
		if_unlikely( ::fcntl( BitCast<NativeSocket_t>(_handle), F_SETFL, O_NONBLOCK, non_blocking ) < 0 )
		{
			NET_CHECK2( "Failed to set socket non-blocking mode: " );
			return false;
		}

	#else
		#error Unsupported platform!
	#endif
		return true;
	}

/*
=================================================
	_SetNonBlocking
=================================================
*/
	bool  BaseSocket::_SetNonBlocking () __NE___
	{
		ASSERT( IsOpen() );

	#if defined(AE_WINDOWS_SOCKET)
		DWORD	non_blocking = 1;
		if_unlikely( ::ioctlsocket( BitCast<NativeSocket_t>(_handle), FIONBIO, &non_blocking ) != 0 )	// win8.1
		{
			NET_CHECK2( "Failed to set socket non-blocking mode: " );
			return false;
		}

	#elif defined(AE_UNIX_SOCKET)
		int		non_blocking = 1;
		if_unlikely( ::fcntl( BitCast<NativeSocket_t>(_handle), F_SETFL, O_NONBLOCK, non_blocking ) < 0 )
		{
			NET_CHECK2( "Failed to set socket non-blocking mode: " );
			return false;
		}

	#else
		#error Unsupported platform!
	#endif
		return true;
	}

/*
=================================================
	IsNonBlocking
=================================================
*/
	bool  BaseSocket::IsNonBlocking () C_NE___
	{
	#if defined(AE_WINDOWS_SOCKET)
		// TODO
		return true;

	#elif defined(AE_UNIX_SOCKET)
		int		non_blocking = ::fcntl( BitCast<NativeSocket_t>(_handle), F_GETFL, 0 );

		if_unlikely( non_blocking < 0 )
			NET_CHECK2( "Failed to set socket non-blocking mode: " );

		return (non_blocking & O_NONBLOCK) == O_NONBLOCK;

	#else
	//	#error Unsupported platform!
	#endif
	}

/*
=================================================
	_SetSendBufferSize
=================================================
*/
	bool  BaseSocket::_SetSendBufferSize (const Bytes inSize) __NE___
	{
		ASSERT( IsOpen() );

		const int	size = int(inSize);

		// The total per-socket buffer space reserved for sends.
		if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_SNDBUF, Cast<NativeSocketOptPtr_t>(&size), sizeof(size) ) != 0 )
		{
			NET_CHECK2( "Failed to set socket send buffer size: " );
			return false;
		}
		return true;
	}

/*
=================================================
	GetSendBufferSize
=================================================
*/
	Bytes  BaseSocket::GetSendBufferSize () C_NE___
	{
		ASSERT( IsOpen() );

		int			buf_size	= 0;
		socklen_t	len			= sizeof(buf_size);

		if_likely( ::getsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_SNDBUF, OUT Cast<NativeSocketOptPtr_t>(&buf_size), INOUT &len ) == 0 )
			return Bytes{uint(buf_size)};

		return 0_b;
	}

/*
=================================================
	_SetReceiveBufferSize
=================================================
*/
	bool  BaseSocket::_SetReceiveBufferSize (const Bytes inSize) __NE___
	{
		ASSERT( IsOpen() );

		const int	size = int(inSize);

		// Specifies the total per-socket buffer space reserved for receives.
		if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_RCVBUF, Cast<NativeSocketOptPtr_t>(&size), sizeof(size) ) != 0 )
		{
			NET_CHECK2( "Failed to set socket receive buffer size: " );
			return false;
		}
		return true;
	}

/*
=================================================
	GetReceiveBufferSize
=================================================
*/
	Bytes  BaseSocket::GetReceiveBufferSize () C_NE___
	{
		ASSERT( IsOpen() );

		int			buf_size	= 0;
		socklen_t	len			= sizeof(buf_size);

		if_likely( ::getsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_RCVBUF, OUT Cast<NativeSocketOptPtr_t>(&buf_size), INOUT &len ) == 0 )
			return Bytes{uint(buf_size)};

		return 0_b;
	}

/*
=================================================
	GetAddress
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	bool  BaseSocket::_GetAddress (OUT AddressType &outAddr) C_NE___
	{
		CHECK_ERR( IsOpen() );

		NativeAddress	addr;
		socklen_t		addr_len = sizeof(addr);

		if_unlikely( ::getsockname( BitCast<NativeSocket_t>(_handle), OUT Cast<sockaddr>(&addr), OUT &addr_len ) != 0 )
		{
			NET_CHECK2( "Failed to get socket address: " );
			return false;
		}

		outAddr = AddressType::FromNative( AnyTypeCRef{ addr });
		ASSERT( outAddr.IsValid() );

		return true;
	}

	bool  BaseSocket::GetAddress (OUT IpAddress &outAddr) C_NE___
	{
		return _GetAddress< sockaddr_in >( outAddr );
	}

	bool  BaseSocket::GetAddress (OUT IpAddress6 &outAddr) C_NE___
	{
		return _GetAddress< sockaddr_in6 >( outAddr );
	}


} // AE::Networking

