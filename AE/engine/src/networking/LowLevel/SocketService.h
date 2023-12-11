// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/LowLevel/UdpSocket.h"
#include "networking/LowLevel/TcpSocket.h"

namespace AE::Networking
{

    //
    // Socket Service
    //

    class SocketService final : public Noncopyable
    {
    // variables
    private:
        bool        _initialized    = false;


    // methods
    public:
        ND_ static SocketService&  Instance ()                                      __NE___;

        ND_ bool  Initialize ()                                                     __NE___;
            bool  Deinitialize ()                                                   __NE___;

        ND_ bool  GetSelfIPAddress (const IpAddress &host, OUT IpAddress &self)     C_NE___;
        ND_ bool  GetSelfIPAddress (const IpAddress6 &host, OUT IpAddress6 &self)   C_NE___;

    private:
        SocketService ()                                                            __NE___;
        ~SocketService ()                                                           __NE___;

        template <typename N, typename A>
        ND_ bool  _GetSelfIPAddress (const A &host, OUT A &self)                    C_NE___;
    };


} // AE::Networking
