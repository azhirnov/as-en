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
    // IPv6 Address
    //

    class IpAddress6
    {
    // types
    private:
        struct IPv6Bits
        {
            ushort  p0;     // xxxx:::::::
            ushort  p1;     // :xxxx::::::
            ushort  p2;     // ::xxxx:::::
            ushort  p3;     // :::xxxx::::
            ushort  p4;     // ::::xxxx:::
            ushort  p5;     // :::::xxxx::
            ushort  p6;     // ::::::xxxx:
            ushort  p7;     // :::::::xxxx
        };
        StaticAssert( sizeof(IPv6Bits) == 16, "IPv6Bits must have 128 bits" );


    // variables
    private:
        IPv6Bits    _address    = {};   // in network order
        ushort      _port       = 0;
        ushort      _scopeId    = 0;


    // methods
    public:
        IpAddress6 ()                                           __NE___ {}
        IpAddress6 (ushort p0, ushort p1, ushort p2, ushort p3,
                    ushort p4, ushort p5, ushort p6, ushort p7,
                    ushort port, ushort scopeId)                __NE___;

        ND_ bool    IsValid ()                                  C_NE___;
        ND_ ushort  Port ()                                     C_NE___ { return _port; }

            void    SetPort (ushort value)                      __NE___ { _port = value; }

        ND_ bool    operator == (const IpAddress6 &rhs)         C_NE___ { return MemEqual( *this, rhs ); }
        ND_ bool    operator <  (const IpAddress6 &rhs)         C_NE___ { return MemLess( *this, rhs ); }

        ND_ String  ToString ()                                 C_Th___;

            void    ToNative (OUT AnyTypeRef addr)              C_NE___;


        ND_ static IpAddress6  FromServiceUDP (NtStringView hostName, NtStringView serviceName) __NE___;
        ND_ static IpAddress6  FromServiceTCP (NtStringView hostName, NtStringView serviceName) __NE___;

        ND_ static IpAddress6  FromHostPortUDP (NtStringView hostName, ushort port)             __NE___;
        ND_ static IpAddress6  FromHostPortTCP (NtStringView hostName, ushort port)             __NE___;

        // allow external connection
        ND_ static IpAddress6  FromLocalPortUDP (ushort port)                                   __NE___;
        ND_ static IpAddress6  FromLocalPortTCP (ushort port)                                   __NE___;

        // only local connection
        ND_ static IpAddress6  FromLocalhostUDP (ushort port)                                   __NE___;
        ND_ static IpAddress6  FromLocalhostTCP (ushort port)                                   __NE___;

        ND_ static IpAddress6  FromNative (AnyTypeCRef addr)                                    __NE___;
        ND_ static IpAddress6  FromString (StringView addr)                                     __NE___;
    };


} // AE::Networking
