// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/HighLevel/Client.h"

namespace AE::Networking
{

    //
    // Default Server Provider v1
    //

    class DefaultServerProviderV1 final : public IServerProvider
    {
        IpAddress   _addr4;

    public:
        explicit DefaultServerProviderV1 (const IpAddress &addr4)       __NE___ : _addr4{addr4} {}

        void  GetAddress (EChannel, uint, Bool, OUT IpAddress &addr)    __NE_OV { addr = _addr4; }
        void  GetAddress (EChannel, uint, Bool, OUT IpAddress6 &)       __NE_OV {}
    };



    //
    // Default Server Provider v2
    //

    class DefaultServerProviderV2 final : public IServerProvider
    {
        Array< IpAddress >  _addr4;

    public:
        explicit DefaultServerProviderV2 (ArrayView<IpAddress> addr4)       __NE___ : _addr4{addr4} {}

        void  GetAddress (EChannel, uint idx, Bool, OUT IpAddress &addr)    __NE_OV { addr = _addr4[ idx % _addr4.size() ]; }
        void  GetAddress (EChannel, uint,     Bool, OUT IpAddress6 &)       __NE_OV {}
    };


} // AE::Networking
