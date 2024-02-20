// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "networking/HighLevel/MessageFactory.h"

namespace AE::Networking
{

    //
    // Default Client/Server Message Consumer
    //

    class DefaultCSMessageConsumer final : public ICSMessageConsumer
    {
    // types
    private:
        using Fn = Function< void (ChunkList<const CSMessagePtr>) >;


    // variables
    private:
        const Fn                _fn;
        const CSMessageGroupID  _groupId;


    // methods
    public:
        DefaultCSMessageConsumer (Fn fn, CSMessageGroupID id)   __NE___ : _fn{RVRef(fn)}, _groupId{id} {}

        CSMessageGroupID  GetGroupID ()                         C_NE_OV { return _groupId; }
        void  Consume (ChunkList<const CSMessagePtr> msgList)   __NE_OV { _fn( msgList ); }
    };


} // AE::Networking
