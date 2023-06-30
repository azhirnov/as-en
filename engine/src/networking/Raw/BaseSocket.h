// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/Raw/IpAddress.h"

namespace AE::Networking
{

    //
    // Base Socket
    //

    class BaseSocket
    {
    // types
    protected:
        #ifdef AE_PLATFORM_WINDOWS
            enum class Socket_t : usize { Unknown = ~usize(0) };
            using Bytes_t       = int;

        #elif defined(AE_PLATFORM_UNIX_BASED)
            enum class Socket_t : int { Unknown = -1 };
            using Bytes_t       = ssize;

        #else
            #error Unsupported platform!
        #endif

        struct _Config
        {
            Bytes   sendBufferSize      = 1_Mb;
            Bytes   receiveBufferSize   = 1_Mb;
        };


    // variables
    protected:
        Socket_t    _handle = Default;


    // methods
    public:
        BaseSocket ()                                   __NE___;
        ~BaseSocket ()                                  __NE___;

            void  Close ()                              __NE___;

        ND_ bool  IsOpen ()                             C_NE___ { return _handle != Default; }

            bool  GetAddress (OUT IpAddress &outAddr)   C_NE___;

    protected:
        ND_ bool  _SetNonBlocking ()                    __NE___;
        ND_ bool  _SetSendBufferSize (Bytes size)       __NE___;
        ND_ bool  _SetReceiveBufferSize (Bytes size)    __NE___;
    };

} // AE::Networking
