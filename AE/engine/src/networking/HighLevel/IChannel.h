// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: no
*/

#pragma once

#include "networking/LowLevel/IpAddress.h"
#include "networking/LowLevel/IpAddress6.h"
#include "networking/HighLevel/MessageFactory.h"

namespace AE::Networking
{
    class IServerProvider;
    class IClientListener;



    //
    // Channel interface
    //

    class IChannel : public EnableRC<IChannel>
    {
    // types
    public:
        using MsgList_t     = ChunkList< CSMessagePtr >;
        using CMsgList_t    = ChunkList< const CSMessagePtr >;

    protected:
        struct MsgQueueByGroup
        {
            MsgList_t   first;
            MsgList_t   last;
        };
        using MsgQueueMap_t = FixedMap< CSMessageGroupID, MsgQueueByGroup, NetConfig::MaxMsgGroupsPerFrame >;


    // interface
    public:
      // user api //
            virtual void            Send (MsgList_t)            __NE___ = 0;
        ND_ virtual MsgQueueMap_t&  Receive ()                  __NE___ = 0;


      // client / server api //
            virtual void  ProcessMessages (FrameUID frameId)    __NE___ = 0;

        ND_ virtual bool  DisconnectClient (EClientLocalID)     __NE___ = 0;

        ND_ virtual bool  IsConnected ()                        C_NE___ = 0;    // TODO: status
    };


} // AE::Networking
