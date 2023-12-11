// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Address and port in host order.
    Use HostToNetwork() to convert to network order.
    Use NetworkToHost() to convert from network order before creating IpAddress object.
*/

#pragma once

#include "networking/Common.h"

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
            uint    p1 : 8;     // .xxx..
            uint    p0 : 8;     // xxx...
        };
        StaticAssert( sizeof(IPv4Bits) == 4, "IPv4Bits must have 32 bits" );


    // variables
    private:
        union {
            IPv4Bits    _addressBits;
            uint        _address    = 0;
        };
        ushort          _port       = 0;


    // methods
    public:
        IpAddress ()                                    __NE___ {}
        IpAddress (uint addr, ushort port)              __NE___ : _address{addr}, _port{port} {}

        ND_ bool    IsValid ()                          C_NE___ { return (_address != 0) | (_port != 0); }
        ND_ uint    Address ()                          C_NE___ { return _address; }
        ND_ ushort  Port ()                             C_NE___ { return _port; }

            void    SetPort (ushort value)              __NE___ { _port = value; }

        ND_ bool    operator == (const IpAddress &rhs)  C_NE___ { return (_address == rhs._address) & (_port == rhs._port); }
        ND_ bool    operator <  (const IpAddress &rhs)  C_NE___ { return _address == rhs._address ? _port < rhs._port : _address < rhs._address; }

        ND_ String  ToString ()                         C_Th___;
        ND_ String  ToHostName (Bool optIsTCP = True{}) C_Th___;

            void    ToNative (OUT AnyTypeRef addr)      C_NE___;


        ND_ static IpAddress  FromServiceUDP (NtStringView hostName, NtStringView serviceName)  __NE___;
        ND_ static IpAddress  FromServiceTCP (NtStringView hostName, NtStringView serviceName)  __NE___;

        ND_ static IpAddress  FromHostPortUDP (NtStringView hostName, ushort port)              __NE___;
        ND_ static IpAddress  FromHostPortTCP (NtStringView hostName, ushort port)              __NE___;

        // allow external connection
        ND_ static IpAddress  FromLocalPortUDP (ushort port)                                    __NE___;
        ND_ static IpAddress  FromLocalPortTCP (ushort port)                                    __NE___;

        // only local connection
        ND_ static IpAddress  FromLocalhostUDP (ushort port)                                    __NE___;
        ND_ static IpAddress  FromLocalhostTCP (ushort port)                                    __NE___;

        ND_ static IpAddress  FromNative (AnyTypeCRef addr)                                     __NE___;
    };


} // AE::Networking
