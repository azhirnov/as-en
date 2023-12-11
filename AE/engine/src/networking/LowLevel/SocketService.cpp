// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/LowLevel/SocketService.h"
#include "networking/LowLevel/PlatformSpecific.cpp.h"

namespace AE::Networking
{

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
        WSADATA     wsa_data;
        const WORD  req_ver = MAKEWORD(2, 2);
        const int   error   = ::WSAStartup( req_ver, OUT &wsa_data );

        if_unlikely( error != NO_ERROR )
        {
            NET_CHECK2( "WSAStartup failed: " );
            return false;
        }
        _initialized = true;

    #elif defined(AE_UNIX_SOCKET)
        _initialized = true;

    #else
    #   error Unsupported platform!
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
    #   error Unsupported platform!
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
        static SocketService    network;
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
        constexpr bool  is_ipv6 = IsSameTypes< AddressType, IpAddress6 >;

        NativeSocket_t  socket = ::socket( (is_ipv6 ? AF_INET6 : AF_INET), SOCK_DGRAM, 0 );

        if ( BitCast<BaseSocket::Socket_t>(socket) == Default )
            return false;

        bool            result  = false;
        NativeAddress   host_addr;
        hostAddr.ToNative( OUT AnyTypeRef{ host_addr });

        if ( ::connect( socket, Cast<sockaddr>(&host_addr), sizeof(host_addr) ) == 0 )
        {
            NativeAddress   self_addr;
            socklen_t       self_addr_len = sizeof(self_addr);

            if ( ::getsockname( socket, OUT Cast<sockaddr>(&self_addr), OUT &self_addr_len ) == 0 )
            {
                result      = true;
                selfAddr    = AddressType::FromNative( self_addr );
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


} // AE::Networking
