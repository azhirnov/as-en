// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Raw/BaseSocket.h"
#include "networking/Raw/PlatformSpecific.cpp.h"

namespace AE::Networking
{

/*
=================================================
    constructor
=================================================
*/
    BaseSocket::BaseSocket () __NE___
    {
    #if defined(AE_WINDOWS_SOCKET)
        STATIC_ASSERT( sizeof(SOCKET) == sizeof(Socket_t) );
        STATIC_ASSERT( Socket_t(INVALID_SOCKET) == Socket_t::Unknown );

    #elif defined(AE_UNIX_SOCKET)
        STATIC_ASSERT( sizeof(int) == sizeof(Socket_t) );
        STATIC_ASSERT( Socket_t(-1) == Socket_t::Unknown );

    #else
        #error Unsupported platform!
    #endif
    }

/*
=================================================
    destructor
=================================================
*/
    BaseSocket::~BaseSocket () __NE___
    {
        Close();
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
                ::closesocket( BitCast<NativeSocket_t>(_handle) );

            #elif defined(AE_PLATFORM_EMSCRIPTEN)
                ::shutdown( BitCast<NativeSocket_t>(_handle), SHUT_RDWR );

            #elif defined(AE_UNIX_SOCKET)
                ::close( BitCast<NativeSocket_t>(_handle) );

            #else
                #error Unsupported platform!
            #endif
        }
        _handle = Default;
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
        DWORD   non_blocking = 1;
        if_unlikely( ::ioctlsocket( BitCast<NativeSocket_t>(_handle), FIONBIO, &non_blocking ) != 0 )
        {
            CATCH( PlatformUtils::CheckNetworkError( "Failed to set socket non-blocking mode: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

    #elif defined(AE_UNIX_SOCKET)
        int     non_blocking = 1;
        if_unlikely( ::fcntl( BitCast<NativeSocket_t>(_handle), F_SETFL, O_NONBLOCK, non_blocking ) < 0 )
        {
            CATCH( PlatformUtils::CheckNetworkError( "Failed to set socket non-blocking mode: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

    #else
        #error Unsupported platform!
    #endif
        return true;
    }

/*
=================================================
    _SetSendBufferSize
=================================================
*/
    bool  BaseSocket::_SetSendBufferSize (const Bytes inSize) __NE___
    {
        ASSERT( IsOpen() );

        const int   size = int(inSize);

        // The total per-socket buffer space reserved for sends.
        if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_SNDBUF, Cast<NativeSocketOpPtr_t>(&size), sizeof(size) ) != 0 )
        {
            CATCH( PlatformUtils::CheckNetworkError( "Failed to set socket send buffer size: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }
        return true;
    }

/*
=================================================
    _SetReceiveBufferSize
=================================================
*/
    bool  BaseSocket::_SetReceiveBufferSize (const Bytes inSize) __NE___
    {
        ASSERT( IsOpen() );

        const int   size = int(inSize);

        // Specifies the total per-socket buffer space reserved for receives.
        if_unlikely( ::setsockopt( BitCast<NativeSocket_t>(_handle), SOL_SOCKET, SO_RCVBUF, Cast<NativeSocketOpPtr_t>(&size), sizeof(size) ) != 0 )
        {
            CATCH( PlatformUtils::CheckNetworkError( "Failed to set socket receive buffer size: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }
        return true;
    }

/*
=================================================
    GetAddress
=================================================
*/
    bool  BaseSocket::GetAddress (OUT IpAddress &outAddr) C_NE___
    {
        CHECK_ERR( IsOpen() );

        sockaddr_in addr;
        socklen_t   addr_len = sizeof(addr);

        if_unlikely( ::getsockname( BitCast<NativeSocket_t>(_handle), OUT Cast<sockaddr>(&addr), OUT &addr_len ) != 0 )
        {
            CATCH( PlatformUtils::CheckNetworkError( "Failed to get socket address: ", SourceLoc_Current(), ELogLevel::Info ));
            return false;
        }

        outAddr = IpAddress{ NetworkToHost(addr.sin_addr.s_addr), NetworkToHost(addr.sin_port) };
        return true;
    }


} // AE::Networking

