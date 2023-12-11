// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/IpAddress.h"
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
    ND_ static bool  GetAddressFromHostAndService (const char* hostName, const char* serviceName, Bool isUDP,
                                                   OUT uint &outIpv4Addr, OUT ushort &outPort) __NE___
    {
        outIpv4Addr = 0;
        outPort     = 0;

        addrinfo*   out_info = null;
        {
            addrinfo    hints   = {};
        //  hints.ai_flags      = AI_PASSIVE;
            hints.ai_family     = AF_INET;
            hints.ai_socktype   = isUDP ? SOCK_DGRAM : SOCK_STREAM;
            hints.ai_protocol   = isUDP ? IPPROTO_UDP : IPPROTO_TCP;

            if_unlikely( ::getaddrinfo( hostName, serviceName, &hints, OUT &out_info ) != 0 )
            {
                if ( hostName == null )     hostName    = "";
                if ( serviceName == null )  serviceName = "";

                NET_CHECK2( "Failed to get address info for host '"s << hostName << "' and service '" << serviceName << "': " );
                return false;
            }
        }

        bool    result = false;

        for (addrinfo* info = out_info; info != null; info = info->ai_next)
        {
            if ( info->ai_family == AF_INET and info->ai_addrlen == sizeof(sockaddr_in) )
            {
                auto*   addr_v4 = Cast<sockaddr_in>( info->ai_addr );

                outIpv4Addr = NetworkToHost( addr_v4->sin_addr.s_addr );
                outPort     = NetworkToHost( addr_v4->sin_port );

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
    ToNative
=================================================
*/
    void  IpAddress::ToNative (OUT AnyTypeRef outAddr) C_NE___
    {
        auto&   addr = outAddr.As< sockaddr_in >();

        addr.sin_family         = AF_INET;
        addr.sin_port           = HostToNetwork( _port );
        addr.sin_addr.s_addr    = HostToNetwork( _address );
        ZeroMem( addr.sin_zero );
    }

/*
=================================================
    ToString
=================================================
*/
    String  IpAddress::ToString () C_Th___
    {
        return  Base::ToString( _addressBits.p0 ) << '.' << Base::ToString( _addressBits.p1 ) << '.' <<
                Base::ToString( _addressBits.p2 ) << '.' << Base::ToString( _addressBits.p3 ) << ':' <<
                Base::ToString( _port );
    }

/*
=================================================
    ToHostName
=================================================
*/
    String  IpAddress::ToHostName (Bool optIsTCP) C_Th___
    {
        sockaddr_in addr;
        ToNative( OUT addr );

        char    host    [NI_MAXHOST];
        char    service [NI_MAXSERV];

        if_unlikely( ::getnameinfo( Cast<sockaddr>(&addr), sizeof(addr), OUT host, NI_MAXHOST, OUT service, NI_MAXSERV, (optIsTCP ? 0 : NI_DGRAM) ) != 0 )
            return Default;

        String  str;
        str << host << ':' << service;
        return str;
    }

/*
=================================================
    FromServiceUDP
=================================================
*/
    IpAddress  IpAddress::FromServiceUDP (NtStringView hostName, NtStringView serviceName) __NE___
    {
        IpAddress   result;
        CHECK( GetAddressFromHostAndService( hostName.c_str(), serviceName.c_str(), True{"UDP"}, OUT result._address, OUT result._port ));
        return result;
    }

/*
=================================================
    FromHostPortUDP
=================================================
*/
    IpAddress  IpAddress::FromHostPortUDP (NtStringView hostName, ushort port) __NE___
    {
        IpAddress   result;
        CHECK( GetAddressFromHostAndService( hostName.c_str(), null, True{"UDP"}, OUT result._address, OUT result._port ));
        ASSERT( result._port == 0 );
        result._port = port;
        return result;
    }

/*
=================================================
    FromLocalPortUDP
=================================================
*/
    IpAddress  IpAddress::FromLocalPortUDP (ushort port) __NE___
    {
        return FromHostPortUDP( "0.0.0.0", port );
    }

/*
=================================================
    FromLocalhostUDP
=================================================
*/
    IpAddress  IpAddress::FromLocalhostUDP (ushort port) __NE___
    {
        return FromHostPortUDP( "localhost", port );
    }

/*
=================================================
    FromServiceTCP
=================================================
*/
    IpAddress  IpAddress::FromServiceTCP (NtStringView hostName, NtStringView serviceName) __NE___
    {
        IpAddress   result;
        CHECK( GetAddressFromHostAndService( hostName.c_str(), serviceName.c_str(), False{"TCP"}, OUT result._address, OUT result._port ));
        return result;
    }

/*
=================================================
    FromHostPortTCP
=================================================
*/
    IpAddress  IpAddress::FromHostPortTCP (NtStringView hostName, ushort port) __NE___
    {
        IpAddress   result;
        CHECK( GetAddressFromHostAndService( hostName.c_str(), null, False{"TCP"}, OUT result._address, OUT result._port ));
        ASSERT( result._port == 0 );
        result._port = port;
        return result;
    }

/*
=================================================
    FromLocalPortTCP
=================================================
*/
    IpAddress  IpAddress::FromLocalPortTCP (ushort port) __NE___
    {
        return FromHostPortTCP( "0.0.0.0", port );
    }

/*
=================================================
    FromLocalhostTCP
=================================================
*/
    IpAddress  IpAddress::FromLocalhostTCP (ushort port) __NE___
    {
        return FromHostPortTCP( "localhost", port );
    }

/*
=================================================
    FromNative
=================================================
*/
    IpAddress  IpAddress::FromNative (AnyTypeCRef inAddr) __NE___
    {
        const auto&     addr = inAddr.As< sockaddr_in >();
        ASSERT( addr.sin_family == AF_INET );

        return IpAddress{ NetworkToHost(addr.sin_addr.s_addr), NetworkToHost(addr.sin_port) };
    }


} // AE::Networking
