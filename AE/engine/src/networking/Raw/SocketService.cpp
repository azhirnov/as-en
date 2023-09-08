// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Raw/SocketService.h"
#include "networking/Raw/PlatformSpecific.cpp.h"

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
            CATCH( PlatformUtils::CheckNetworkError( "WSAStartup failed: ", SourceLoc_Current(), ELogLevel::Info ));
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

} // AE::Networking
