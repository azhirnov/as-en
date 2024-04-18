// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/IpAddress6.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

namespace AE::Networking
{
namespace
{
/*
=================================================
	GetAddressFromHostAndService
=================================================
*/
	template <typename IPv6Addr>
	ND_ static bool  GetAddressFromHostAndService (const char* hostName, const char* serviceName, Bool isUDP,
												   OUT IPv6Addr &outIpv6Addr, OUT ushort &outPort, OUT ushort &outScopeId) __NE___
	{
		ZeroMem( OUT outIpv6Addr );
		outPort = 0;

		addrinfo*	out_info = null;
		{
			addrinfo	info	= {};
			info.ai_family		= AF_INET6;
			info.ai_socktype	= isUDP ? SOCK_DGRAM : SOCK_STREAM;
			info.ai_protocol	= isUDP ? IPPROTO_UDP : IPPROTO_TCP;

			if_unlikely( ::getaddrinfo( hostName, serviceName, &info, OUT &out_info ) != 0 )
			{
				if ( hostName == null )		hostName	= "";
				if ( serviceName == null )	serviceName	= "";

				NET_CHECK2( "Failed to get address info for host '"s << hostName << "' and service '" << serviceName << "': " );
				return false;
			}
		}

		bool	result = false;

		for (addrinfo* info = out_info; info != null; info = info->ai_next)
		{
			if ( info->ai_family == AF_INET6 and info->ai_addrlen == sizeof(sockaddr_in6) )
			{
				auto*	addr_v6 = Cast<sockaddr_in6>( info->ai_addr );

				std::memcpy( OUT &outIpv6Addr, &addr_v6->sin6_addr, sizeof(outIpv6Addr) );
				outPort		= NetworkToHost( addr_v6->sin6_port );
				outScopeId	= ushort(addr_v6->sin6_scope_id);

				result = true;
				break;
			}
		}

		if ( out_info != null )
			::freeaddrinfo( out_info );

		return result;
	}

} // namespace


/*
=================================================
	IsValid
=================================================
*/
	bool  IpAddress6::IsValid () C_NE___
	{
		return	((_address.p0 | _address.p1 | _address.p2 | _address.p3 | _address.p4 | _address.p5 | _address.p6 | _address.p7) != 0) and
				(_port != 0);
	}

/*
=================================================
	ToNative
=================================================
*/
	void  IpAddress6::ToNative (OUT AnyTypeRef outAddr) C_NE___
	{
		auto&	addr = outAddr.As< sockaddr_in6 >();

		addr.sin6_family	= AF_INET6;
		addr.sin6_port		= HostToNetwork( _port );
		addr.sin6_flowinfo	= 0;
		addr.sin6_scope_id	= _scopeId;

		std::memcpy( OUT &addr.sin6_addr, &_address, sizeof(_address) );
		StaticAssert( sizeof(addr.sin6_addr) == sizeof(_address) );
	}

/*
=================================================
	ToString
=================================================
*/
	String  IpAddress6::ToString () C_Th___
	{
		String	str;
		str << '[';
		if ( _address.p0 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p0 ));	str << ':';
		if ( _address.p1 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p1 ));	str << ':';
		if ( _address.p2 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p2 ));	str << ':';
		if ( _address.p3 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p3 ));	str << ':';
		if ( _address.p4 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p4 ));	str << ':';
		if ( _address.p5 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p5 ));	str << ':';
		if ( _address.p6 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p6 ));	str << ':';
		if ( _address.p7 != 0 )	str << Base::ToString<16>( NetworkToHost( _address.p7 ));
		if ( _scopeId != 0 )	str << '%' << Base::ToString<10>( _scopeId );
		str << ']';
		str << ':' << Base::ToString<10>( _port );
		return str;
	}

/*
=================================================
	FromServiceUDP
=================================================
*/
	IpAddress6  IpAddress6::FromServiceUDP (NtStringView hostName, NtStringView serviceName) __NE___
	{
		IpAddress6	result;
		CHECK( GetAddressFromHostAndService( hostName.c_str(), serviceName.c_str(), True{"UDP"}, OUT result._address, OUT result._port, OUT result._scopeId ));
		return result;
	}

/*
=================================================
	FromHostPortUDP
=================================================
*/
	IpAddress6  IpAddress6::FromHostPortUDP (NtStringView hostName, ushort port) __NE___
	{
		IpAddress6	result;
		CHECK( GetAddressFromHostAndService( hostName.c_str(), null, True{"UDP"}, OUT result._address, OUT result._port, OUT result._scopeId ));
		ASSERT( result._port == 0 );
		result._port = port;
		return result;
	}

/*
=================================================
	FromLocalPortUDP
=================================================
*/
	IpAddress6  IpAddress6::FromLocalPortUDP (ushort port) __NE___
	{
		return FromHostPortUDP( "::", port );
	}

/*
=================================================
	FromLocalhostUDP
=================================================
*/
	IpAddress6  IpAddress6::FromLocalhostUDP (ushort port) __NE___
	{
		return FromHostPortUDP( "::1", port );
	}

/*
=================================================
	FromServiceTCP
=================================================
*/
	IpAddress6  IpAddress6::FromServiceTCP (NtStringView hostName, NtStringView serviceName) __NE___
	{
		IpAddress6	result;
		CHECK( GetAddressFromHostAndService( hostName.c_str(), serviceName.c_str(), False{"TCP"}, OUT result._address, OUT result._port, OUT result._scopeId ));
		return result;
	}

/*
=================================================
	FromHostPortTCP
=================================================
*/
	IpAddress6  IpAddress6::FromHostPortTCP (NtStringView hostName, ushort port) __NE___
	{
		IpAddress6	result;
		CHECK( GetAddressFromHostAndService( hostName.c_str(), null, False{"TCP"}, OUT result._address, OUT result._port, OUT result._scopeId ));
		ASSERT( result._port == 0 );
		result._port = port;
		return result;
	}

/*
=================================================
	FromLocalPortTCP
=================================================
*/
	IpAddress6  IpAddress6::FromLocalPortTCP (ushort port) __NE___
	{
		return FromHostPortTCP( "::", port );
	}

/*
=================================================
	FromLocalhostTCP
=================================================
*/
	IpAddress6  IpAddress6::FromLocalhostTCP (ushort port) __NE___
	{
		return FromHostPortTCP( "::1", port );
	}

/*
=================================================
	FromNative
=================================================
*/
	IpAddress6  IpAddress6::FromNative (AnyTypeCRef inAddr) __NE___
	{
		const auto&		addr = inAddr.As< sockaddr_in6 >();
		IpAddress6		result;

		ASSERT( addr.sin6_family == AF_INET6 );

		std::memcpy( OUT &result._address, &addr.sin6_addr, sizeof(result._address) );
		StaticAssert( sizeof(addr.sin6_addr) == sizeof(_address) );

		result._port 	= NetworkToHost( addr.sin6_port );
		result._scopeId	= ushort(addr.sin6_scope_id);

		return result;
	}

/*
=================================================
	FromString
----
	Supported formats:
		xxxx:xxxx:::
		xxxx:xxxx:::%scope
		::1
		[xxxx:xxxx:::]:port
=================================================
*/
	IpAddress6  IpAddress6::FromString (StringView addr) __NE___
	{
		const auto	ParseUShortHex = [] (INOUT usize &pos, StringView addr) -> ushort
		{{
			uint	x = 0;
			for (uint j = 0; (j < 4) and (pos < addr.size()); ++j, ++pos)
			{
				const char	c = addr[pos];

				if ( (c >= '0') and (c <= '9') )	(x <<= 4) |= uint(c - '0');			else
				if ( (c >= 'a') and (c <= 'f') )	(x <<= 4) |= uint(c - 'a' + 0xA);	else
				if ( (c >= 'A') and (c <= 'F') )	(x <<= 4) |= uint(c - 'A' + 0xA);	else
													break;
			}
			return ushort(x);
		}};

		const auto	ParseUShort = [] (INOUT usize &pos, StringView addr) -> ushort
		{{
			uint	x = 0;
			for (uint j = 0; (j < 4) and (pos < addr.size()); ++j, ++pos)
			{
				const char	c = addr[pos];
				if ( (c >= '0') and (c <= '9') )	(x *= 10) += uint(c - '0');	else
													break;
			}
			return ushort(x);
		}};

		IpAddress6	result;
		usize		pos		= 0;
		uint		i		= 0;

		if ( pos < addr.size() and addr[pos] == '[' )
			++pos;

		for (ushort* part = &result._address.p0; i < 8; ++i, ++part)
		{
			*part = NetworkToHost( ParseUShortHex( INOUT pos, addr ));

			if ( (pos < addr.size()) and (i < 7) )
			{
				const char	c = addr[pos];
				ASSERT( c == ':' );

				if_unlikely( c != ':' )
					break;

				++pos;
			}
		}

		// read scope
		if ( pos < addr.size() and addr[pos] == '%' )
		{
			++pos;
			result._scopeId = ParseUShort( INOUT pos, addr );
		}

		// read port
		if ( pos+1 < addr.size() and addr[pos] == ']' and addr[pos+1] == ':' )
		{
			pos += 2;
			result._port = ParseUShort( INOUT pos, addr );
		}

		ASSERT( pos == addr.size() );
		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	IpAddress6::IpAddress6 (ushort p0, ushort p1, ushort p2, ushort p3,
							ushort p4, ushort p5, ushort p6, ushort p7,
							ushort port, ushort scopeId) __NE___
	{
		_address.p0	= NetworkToHost( p0 );
		_address.p1	= NetworkToHost( p1 );
		_address.p2	= NetworkToHost( p2 );
		_address.p3	= NetworkToHost( p3 );
		_address.p4	= NetworkToHost( p4 );
		_address.p5	= NetworkToHost( p5 );
		_address.p6	= NetworkToHost( p6 );
		_address.p7	= NetworkToHost( p7 );
		_port		= port;
		_scopeId	= scopeId;
	}

/*
=================================================
	CalcHash / CalcHashOfAddress
=================================================
*/
	HashVal  IpAddress6::CalcHash () C_NE___
	{
		return HashOf( static_cast< void const *>(this), sizeof(*this) );
	}

	HashVal  IpAddress6::CalcHashOfAddress () C_NE___
	{
		return HashOf( static_cast< void const *>(&_address), sizeof(_address) );
	}


} // AE::Networking
