// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Raw/IpAddress.h"
#include "networking/Raw/PlatformSpecific.cpp.h"

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
            addrinfo    info    = {};
            info.ai_family      = AF_INET;
            info.ai_socktype    = isUDP ? SOCK_DGRAM : SOCK_STREAM;
            info.ai_protocol    = isUDP ? IPPROTO_UDP : IPPROTO_TCP;

            if_unlikely( ::getaddrinfo( hostName, serviceName, &info, OUT &out_info ) != 0 )
            {
                if ( hostName == null )     hostName    = "";
                if ( serviceName == null )  serviceName = "";

                CATCH( PlatformUtils::CheckNetworkError(
                            "Failed to get address info for host '"s << hostName << "' and service '" << serviceName << "': ",
                            SourceLoc_Current(), ELogLevel::Info ));
                return false;
            }
        }

        IpAddress   result;

        if ( out_info != null and out_info->ai_family == AF_INET )
        {
            const auto* ptr         = Cast<ubyte>( out_info->ai_addr->sa_data );
            ushort      port        = (ushort(ptr[0]) << 0) | (ushort(ptr[1]) << 16);
            uint        addr        = (uint(ptr[2]) << 0) | (uint(ptr[3]) << 8) | (uint(ptr[4]) << 16) | (uint(ptr[5]) << 24);
                        outIpv4Addr = NetworkToHost( addr );
                        outPort     = NetworkToHost( port );
        }

        if ( out_info != null )
            ::freeaddrinfo( out_info );

        return true;
    }
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
        result._port = port;
        return result;
    }

/*
=================================================
    FromLocalhostUDP
=================================================
*/
    IpAddress  IpAddress::FromLocalhostUDP (ushort port) __NE___
    {
        return IpAddress{ 0x7F'00'00'01, port };
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
        result._port = port;
        return result;
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


} // AE::Networking
