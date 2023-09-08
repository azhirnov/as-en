// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Address and port in host order.
    Use HostToNetwork() to convert to network order.
    Use NetworkToHost() to convert from network order before creating IpAddress object.
*/

#pragma once

#include "networking/Networking.pch.h"

namespace AE::Networking
{
    using namespace AE::Base;


    //
    // IPv4 Address
    //

    class IpAddress
    {
    // types
    private:
        struct IPv4Bits
        {
            uint    p3 : 8;     // ...xxx
            uint    p2 : 8;     // ..xxx.
            uint    p1 : 8;     // .xxx.
            uint    p0 : 8;     // xxx.
        };


    // variables
    private:
        union {
            IPv4Bits    _addressBits;
            uint        _address    = 0;
        };
        ushort          _port       = 0;


    // methods
    public:
        IpAddress ()                        __NE___ {}
        IpAddress (uint addr, ushort port)  __NE___ : _address{addr}, _port{port} {}
        ~IpAddress ()                       __NE___ {}

        ND_ bool    IsValid ()              C_NE___ { return (_address != 0) & (_port != 0); }
        ND_ uint    Address ()              C_NE___ { return _address; }
        ND_ ushort  Port ()                 C_NE___ { return _port; }

        ND_ String  ToString ()             C_Th___;

        ND_ static IpAddress  FromServiceUDP (NtStringView hostName, NtStringView serviceName)  __NE___;
        ND_ static IpAddress  FromServiceTCP (NtStringView hostName, NtStringView serviceName)  __NE___;

        ND_ static IpAddress  FromHostPortUDP (NtStringView hostName, ushort port)              __NE___;
        ND_ static IpAddress  FromHostPortTCP (NtStringView hostName, ushort port)              __NE___;

        ND_ static IpAddress  FromLocalhostUDP (ushort port)                                    __NE___;
        ND_ static IpAddress  FromLocalhostTCP (ushort port)                                    __NE___;
    };


} // AE::Networking
