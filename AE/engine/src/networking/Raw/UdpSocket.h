// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    MTU : 1460 bytes
*/

#pragma once

#include "networking/Raw/BaseSocket.h"

namespace AE::Networking
{

    //
    // UDP Socket
    //

    class UdpSocket final : public BaseSocket
    {
    // types
    public:
        struct Statistic
        {
            Bytes   sentData;
            Bytes   recvData;
            uint    sentPackets = 0;
            uint    recvPackets = 0;
        };

        using Config = BaseSocket::_Config;


    // variables
    private:
        mutable Statistic   _stat;


    // methods
    public:
        UdpSocket ()                                                                                    __NE___ {}

        ND_ bool  Open (ushort port, const Config &cfg = Default)                                       __NE___;

            bool  Send (const IpAddress &addr, const void* data, Bytes dataSize, OUT Bytes &sent)       C_NE___;
            bool  Receive (OUT IpAddress &addr, OUT void* data, Bytes dataSize, OUT Bytes &received)    C_NE___;

        ND_ Statistic const&  Stats ()                                                                  C_NE___ { return _stat; }
    };


} // AE::Networking