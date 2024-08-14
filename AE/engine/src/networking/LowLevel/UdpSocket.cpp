// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/UdpSocket.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

namespace AE::Networking
{

/*
=================================================
	Open
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	bool  UdpSocket::_Open (const AddressType &addr, const Config &cfg) __NE___
	{
		ASSERT( addr.IsValid() );
		CHECK_ERR( not IsOpen() );

		constexpr bool	is_ipv6 = IsSameTypes< AddressType, IpAddress6 >;

		_handle = BitCast<Socket_t>(::socket( (is_ipv6 ? AF_INET6 : AF_INET), SOCK_DGRAM, IPPROTO_UDP ));

		if_unlikely( _handle == Default )
		{
			NET_CHECK2( "UDP("s << GetDebugName() << ") socket() failed with error: " );
			return false;
		}

		// bind to port
		{
			NativeAddress	sock_addr = {};
			addr.ToNative( OUT AnyTypeRef{ sock_addr });

			if_unlikely( ::bind( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&sock_addr), sizeof(sock_addr) ) != 0 )
			{
				NET_CHECK2( "UDP("s << GetDebugName() << ") failed to bind socket to port ("s << ToString(addr.Port()) << "): " );
				Close();
				return false;
			}
		}

		CHECK_ERR( _SetSendBufferSize( cfg.sendBufferSize ));
		CHECK_ERR( _SetReceiveBufferSize( cfg.receiveBufferSize ));

		if ( cfg.nonBlocking )
			CHECK_ERR( _SetNonBlocking() );

		return true;
	}

	bool  UdpSocket::Open (const IpAddress &addr, const Config &cfg) __NE___
	{
		return _Open< sockaddr_in >( addr, cfg );
	}

	bool  UdpSocket::Open (const IpAddress6 &addr, const Config &cfg) __NE___
	{
		return _Open< sockaddr_in6 >( addr, cfg );
	}

/*
=================================================
	Send
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	auto  UdpSocket::_Send (const AddressType &addr, const void* data, Bytes dataSize) C_NE___ -> Tuple< SocketSendError, Bytes >
	{
		ASSERT( (data != null) and (dataSize > 0) );
		ASSERT( addr.IsValid() );

		if_unlikely( not IsOpen() )
			return Tuple{ SocketSendError::NoSocket, 0_b };

		NativeAddress	dst_Addr = {};
		addr.ToNative( OUT AnyTypeRef{ dst_Addr });

		const Byte_t	sent = ::sendto( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0, Cast<sockaddr>(&dst_Addr), sizeof(dst_Addr) );

		if_unlikely( sent < 0 )
		{
			const auto	err = PlatformUtils::GetNetworkErrorCode();
			DEBUG_ONLY( if ( ShouldPrintError( err )) NET_CHECK( err, "UDP("s << GetDebugName() << ") failed to write to socket ("s << addr.ToString() << "): " );)
			return Tuple{ TranslateSocketSendError( err ), 0_b };
		}

		return Tuple{ (sent ? SocketSendError::Sent : SocketSendError::NotSent), Bytes{ulong(sent)} };
	}

	auto  UdpSocket::Send (const IpAddress &addr, const void* data, const Bytes dataSize) C_NE___ -> Tuple< SocketSendError, Bytes >
	{
		ASSERT( addr.Address() != 0 );
		return _Send< sockaddr_in >( addr, data, dataSize );
	}

	auto  UdpSocket::Send (const IpAddress6 &addr, const void* data, const Bytes dataSize) C_NE___ -> Tuple< SocketSendError, Bytes >
	{
		return _Send< sockaddr_in6 >( addr, data, dataSize );
	}

/*
=================================================
	Receive
=================================================
*/
	template <typename NativeAddress, typename AddressType>
	auto  UdpSocket::_Receive (OUT AddressType &addr, OUT void* data, Bytes dataSize) C_NE___ -> Tuple< SocketReceiveError, Bytes >
	{
		ASSERT( (data != null) and (dataSize > 0) );

		addr = Default;

		if_unlikely( not IsOpen() )
			return Tuple{ SocketReceiveError::NoSocket, 0_b };

		NativeAddress	src_addr;
		socklen_t		addr_len = sizeof(src_addr);
		const Byte_t	received = ::recvfrom( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0, OUT Cast<sockaddr>(&src_addr), OUT &addr_len );

		if_unlikely( received < 0 )
		{
			const auto	err = PlatformUtils::GetNetworkErrorCode();
			DEBUG_ONLY( if ( ShouldPrintError( err )) NET_CHECK( err, "UDP("s << GetDebugName() << ") failed to read from socket: " );)
			return Tuple{ TranslateSocketReceiveError( err ), 0_b };
		}

		addr = AddressType::FromNative( AnyTypeCRef{ src_addr });
		ASSERT( addr.IsValid() );

		return Tuple{ (received ? SocketReceiveError::Received : SocketReceiveError::NotReceived), Bytes{ulong(received)} };
	}

	auto  UdpSocket::Receive (OUT IpAddress &addr, OUT void* data, const Bytes dataSize) C_NE___ -> Tuple< SocketReceiveError, Bytes >
	{
		return _Receive< sockaddr_in >( OUT addr, OUT data, dataSize );
	}

	auto  UdpSocket::Receive (OUT IpAddress6 &addr, OUT void* data, const Bytes dataSize) C_NE___ -> Tuple< SocketReceiveError, Bytes >
	{
		return _Receive< sockaddr_in6 >( OUT addr, OUT data, dataSize );
	}

} // AE::Networking
