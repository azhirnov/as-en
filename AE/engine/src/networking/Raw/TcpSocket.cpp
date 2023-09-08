// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Raw/TcpSocket.h"
#include "networking/Raw/PlatformSpecific.cpp.h"

namespace AE::Networking
{

/*
=================================================
    Accept
=================================================
*/
    bool  TcpSocket::Accept (const TcpSocket &other, OUT IpAddress &clientAddr) __NE___
    {
        CHECK_ERR( not IsOpen() );

        if_unlikely( not other.IsOpen() )
            return false;

        sockaddr_in src_addr;
        socklen_t   addr_len = sizeof(src_addr);

        _handle = BitCast<Socket_t>(::accept( BitCast<NativeSocket_t>(other._handle), OUT Cast<sockaddr>(&src_addr), OUT &addr_len ));

        if_likely( _handle == Default )
            return false;   // no clients

        clientAddr = IpAddress{ NetworkToHost(src_addr.sin_addr.s_addr), NetworkToHost(src_addr.sin_port) };
        return true;
    }

/*
=================================================
    Listen
=================================================
*/
    bool  TcpSocket::Listen (ushort port, const Config &cfg) __NE___
    {
        CHECK_ERR( _Create( cfg ));

        // binding
        {
            sockaddr_in     sock_addr   = {};
            sock_addr.sin_family        = AF_INET;
            sock_addr.sin_addr.s_addr   = INADDR_ANY;
            sock_addr.sin_port          = HostToNetwork( port );

            if_unlikely( ::bind( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&sock_addr), sizeof(sock_addr) ) != 0 )
            {
                CATCH( PlatformUtils::CheckNetworkError( "Failed to bind TCP socket to port ("s << ToString(port) << "): ", SourceLoc_Current(), ELogLevel::Info ));
                return false;
            }

            if_unlikely( ::listen( BitCast<NativeSocket_t>(_handle), (cfg.maxConnections == UMax ? SOMAXCONN : cfg.maxConnections) ) != 0 )
            {
                CATCH( PlatformUtils::CheckNetworkError( "Failed to listen on TCP socket: ", SourceLoc_Current(), ELogLevel::Info ));
                return false;
            }
        }

        CHECK_ERR( _SetNonBlocking() );
        CHECK_ERR( _SetNoDelay() );
        return true;
    }

/*
=================================================
    Connect
=================================================
*/
    bool  TcpSocket::Connect (const IpAddress &addr, const Config &cfg) __NE___
    {
        CHECK_ERR( _Create( cfg ));

        // connect
        {
            sockaddr_in     dst_Addr    = {};
            dst_Addr.sin_family         = AF_INET;
            dst_Addr.sin_addr.s_addr    = HostToNetwork( addr.Address() );
            dst_Addr.sin_port           = HostToNetwork( addr.Port() );

            if_unlikely( ::connect( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&dst_Addr), sizeof(dst_Addr) ) != 0 )
            {
                CATCH( PlatformUtils::CheckNetworkError( "Failed to connect TCP socket to address ("s << addr.ToString() << "): ",
                                                         SourceLoc_Current(), ELogLevel::Info ));
                return false;
            }
        }

        CHECK_ERR( _SetNonBlocking() );
        CHECK_ERR( _SetNoDelay() );
        return true;
    }

/*
=================================================
    _Create
=================================================
*/
    bool  TcpSocket::_Create (const Config &cfg) __NE___
    {
        CHECK_ERR( not IsOpen() );

        _handle = BitCast<Socket_t>(::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ));

        if_unlikely( _handle == Default )
        {
            CATCH( PlatformUtils::CheckNetworkError( "socket() failed with error: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

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

        int     option = 1;

        if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), IPPROTO_TCP, TCP_NODELAY, Cast<NativeSocketOpPtr_t>(&option), sizeof(option) ) != 0 )
        {
            CATCH( PlatformUtils::CheckNetworkError( "Failed to set socket no-delay mode: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }
        return true;
    }

/*
=================================================
    Send
=================================================
*/
    bool  TcpSocket::Send (const void* data, Bytes dataSize, OUT Bytes &outSent) C_NE___
    {
        ASSERT( data != null );
        ASSERT( dataSize > 0 );

        outSent = 0_b;

        if_unlikely( not IsOpen() )
            return false;

        const Bytes_t   sent = ::send( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0 );

        if_unlikely( sent < 0 )
        {
            const auto  err = PlatformUtils::GetNetworkErrorCode();

            if_likely( IsWouldBlock( err ))
                return true;

            CATCH( PlatformUtils::CheckNetworkError( err, "Failed to write to socket: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

        outSent = Bytes{ulong(sent)};
        return true;
    }

/*
=================================================
    Receive
=================================================
*/
    bool  TcpSocket::Receive (OUT void* data, Bytes dataSize, OUT Bytes &outReceived) C_NE___
    {
        ASSERT( data != null );
        ASSERT( dataSize > 0 );

        outReceived = 0_b;

        if_unlikely( not IsOpen() )
            return false;

        const Bytes_t   received = ::recv( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0 );

        if_unlikely( received < 0 )
        {
            const auto  err = PlatformUtils::GetNetworkErrorCode();

            if_likely( IsWouldBlock( err ))
                return true;

            if ( IsForcedlyClosed( err ))
                return false;

            CATCH( PlatformUtils::CheckNetworkError( err, "Failed to read from socket: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

        outReceived = Bytes{ulong(received)};
        return true;
    }


} // AE::Networking

