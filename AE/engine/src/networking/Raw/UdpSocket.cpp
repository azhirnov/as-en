// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Raw/UdpSocket.h"
#include "networking/Raw/PlatformSpecific.cpp.h"

namespace AE::Networking
{

/*
=================================================
    Open
=================================================
*/
    bool  UdpSocket::Open (ushort port, const Config &cfg) __NE___
    {
        CHECK_ERR( not IsOpen() );

        _stat   = Default;
        _handle = BitCast<Socket_t>(::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ));

        if_unlikely( _handle == Default )
        {
            CATCH( PlatformUtils::CheckNetworkError( "socket() failed with error: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

        // bind to port
        {
            sockaddr_in     sock_addr   = {};
            sock_addr.sin_family        = AF_INET;
            sock_addr.sin_addr.s_addr   = INADDR_ANY;
            sock_addr.sin_port          = HostToNetwork( port );

            if_unlikely( ::bind( BitCast<NativeSocket_t>(_handle), Cast<sockaddr>(&sock_addr), sizeof(sock_addr) ) != 0 )
            {
                CATCH( PlatformUtils::CheckNetworkError( "Failed to bind UDP socket to port ("s << ToString(port) << "): ", SourceLoc_Current(), ELogLevel::Info ));
                return false;
            }
        }

        CHECK_ERR( _SetSendBufferSize( cfg.sendBufferSize ));
        CHECK_ERR( _SetReceiveBufferSize( cfg.receiveBufferSize ));
        CHECK_ERR( _SetNonBlocking() );

        return true;
    }

/*
=================================================
    Send
=================================================
*/
    bool  UdpSocket::Send (const IpAddress &addr, const void* data, const Bytes dataSize, OUT Bytes &outSent) C_NE___
    {
        ASSERT( data != null );
        ASSERT( dataSize > 0 );
        ASSERT( addr.IsValid() );

        outSent = 0_b;

        if_unlikely( not IsOpen() )
            return false;

        sockaddr_in     dst_Addr = {};
        dst_Addr.sin_family         = AF_INET;
        dst_Addr.sin_addr.s_addr    = HostToNetwork( addr.Address() );
        dst_Addr.sin_port           = HostToNetwork( addr.Port() );

        const Bytes_t   sent = ::sendto( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0, Cast<sockaddr>(&dst_Addr), sizeof(dst_Addr) );

        if_unlikely( sent < 0 )
        {
            const auto  err = PlatformUtils::GetNetworkErrorCode();

            if_likely( IsWouldBlock( err ))
                return true;

            CATCH( PlatformUtils::CheckNetworkError( err, "Failed to write to socket: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

        _stat.sentData      += sent;
        _stat.sentPackets   ++;

        outSent = Bytes{ulong(sent)};
        return true;
    }

/*
=================================================
    Receive
=================================================
*/
    bool  UdpSocket::Receive (OUT IpAddress &addr, OUT void* data, const Bytes dataSize, OUT Bytes &outReceived) C_NE___
    {
        ASSERT( data != null );
        ASSERT( dataSize > 0 );

        addr        = Default;
        outReceived = 0_b;

        if_unlikely( not IsOpen() )
            return false;

        sockaddr_in     src_addr;
        socklen_t       addr_len = sizeof(src_addr);
        const Bytes_t   received = ::recvfrom( BitCast<NativeSocket_t>(_handle), Cast<NativeSoketSendBuf_t>(data), int(dataSize), 0, OUT Cast<sockaddr>(&src_addr), OUT &addr_len );

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

        _stat.recvData      += received;
        _stat.recvPackets   ++;

        addr = IpAddress{ NetworkToHost(src_addr.sin_addr.s_addr), NetworkToHost(src_addr.sin_port) };

        outReceived = Bytes{ulong(received)};
        return true;
    }


} // AE::Networking
