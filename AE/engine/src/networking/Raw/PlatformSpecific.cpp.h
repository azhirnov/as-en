// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Networking.pch.h"

#ifdef AE_PLATFORM_WINDOWS
#   define AE_WINDOWS_SOCKET
#   include "base/Platforms/WindowsHeader.h"

#elif defined(AE_EMS_NATIVE_SOCKETS)
    // https://emscripten.org/docs/porting/networking.html
#   include "emscripten/websocket.h"

#elif defined(AE_PLATFORM_UNIX_BASED)
#   define AE_UNIX_SOCKET
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <netinet/in.h>
#   include <netinet/ip.h>
#   include <netinet/tcp.h>

#   ifdef AE_PLATFORM_LINUX
#    include <fcntl.h>
#   endif

#   ifdef AE_PLATFORM_APPLE
#    include <fcntl.h>
#    include <arpa/inet.h>
#   endif

#else
#   error Unsupported platform!
#endif

namespace AE::Networking
{

#if defined(AE_WINDOWS_SOCKET)
    using NativeSocket_t        = SOCKET;
    using NativeSocketOpPtr_t   = char;
    using NativeSoketSendBuf_t  = char;

    ND_ inline bool     IsWouldBlock (uint err)         __NE___ { return err == WSAEWOULDBLOCK; }
    ND_ inline bool     IsForcedlyClosed (uint err)     __NE___ { return err == WSAECONNRESET; }


#elif defined(AE_UNIX_SOCKET)
    using NativeSocket_t        = int;
    using NativeSocketOpPtr_t   = void;
    using NativeSoketSendBuf_t  = void;

    ND_ inline bool     IsWouldBlock (int err)          __NE___ { return err == EWOULDBLOCK; }
    ND_ inline bool     IsForcedlyClosed (int err)      __NE___ { return false; }

#else
#   error Unsupported platform!
#endif

    ND_ inline ushort   HostToNetwork (ushort value)    __NE___ { return htons( value ); }
    ND_ inline uint     HostToNetwork (uint   value)    __NE___ { return htonl( value ); }
    ND_ inline ushort   NetworkToHost (ushort value)    __NE___ { return ntohs( value ); }
    ND_ inline uint     NetworkToHost (uint   value)    __NE___ { return ntohl( value ); }

#ifdef AE_PLATFORM_WINDOWS
    ND_ inline ULONG    HostToNetwork (ULONG  value)    __NE___ { return htonl( value ); }
    ND_ inline ULONG    NetworkToHost (ULONG  value)    __NE___ { return ntohl( value ); }
#endif

} // AE::Networking
