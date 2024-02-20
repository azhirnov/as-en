// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: no
*/

#pragma once

#include "networking/HighLevel/IChannel.h"

namespace AE::Networking
{

    //
    // Client/Server Base
    //

    class ClientServerBase
    {
    // types
    private:
        using Channels_t        = StaticArray< RC<IChannel>, uint(EChannel::_Count) >;
        using MsgList_t         = IChannel::MsgList_t;
        using CMsgList_t        = IChannel::CMsgList_t;
        using MsgQueueStatistic = IChannel::MsgQueueStatistic;

        using MsgProducerMap_t  = StaticArray< Array< RC<ICSMessageProducer> >, uint(EChannel::_Count) >;
        using MsgConsumerMap_t  = FixedMap< CSMessageGroupID, Array<RC<ICSMessageConsumer>>, NetConfig::MaxMsgGroupsPerFrame >;


    // variables
    protected:
        FrameUID                _prevFrameId;
        RC<MessageFactory>      _msgFactory;
        Channels_t              _channels;
        MsgProducerMap_t        _producers;
        MsgConsumerMap_t        _consumers;
        RC<IAllocator>          _allocator;

        DRC_ONLY( Threading::DataRaceCheck  _drCheck;)


    // methods
    public:

        // Add/Remove message producer
        //
        bool  Add (RC<ICSMessageProducer>)                          __NE___;
        void  Remove (const ICSMessageProducer*)                    __NE___;
        void  RemoveAllProducers ()                                 __NE___;


        // Add/Remove message consumer
        //
        bool  Add (RC<ICSMessageConsumer>)                          __NE___;
        void  Remove (const ICSMessageConsumer*)                    __NE___;
        void  RemoveAll (CSMessageGroupID)                          __NE___;
        void  RemoveAllConsumers ()                                 __NE___;


        // Receive and send messages.
        // 'frameId' must be same or +1 to the previous call.
        // Returns statistics of incomplete message queues, if user increase 'frameId' for the next call,
        // it will cause client disconnection for reliable channels.
        //
        ND_ MsgQueueStatistic  Update (FrameUID frameId)            __NE___;


        // Send messages to the specified channel.
        //
        void  Send (EChannel id, MsgList_t)                         __NE___;


    protected:
        ClientServerBase ()                                         __NE___ {}
        virtual ~ClientServerBase ()                                __NE___ {}

        ND_ bool  _Initialize2 (RC<MessageFactory>  mf,
                                RC<IAllocator>      alloc,
                                FrameUID            firstFrameId)   __NE___;
            void  _Deinitialize2 ()                                 __NE___;

        ND_ bool  _IsInitialized2 ()                                C_NE___ { return bool{_msgFactory}; }

        ND_ MessageFactory&  _GetMessageFactory ()                  __NE___ { return *_msgFactory; }
    };


} // AE::Networking
