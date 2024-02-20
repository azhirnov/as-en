// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: no

    Input:
        - Receive data and accumulate it in data stream ('_received.storage').
        - Try to extract message from data stream and add it to message queue ('_received.queue').
        - User can access the message queue by calling 'Receive()'.
        - In the next 'ProcessMessages()' call received messages will be discarded.

    Output:
        - Add messages to queue ('_toSend.queue').
        - Try to encode messages to the data stream ('_toSend.storage').
        - Send messages to server/client.

    Reliability:
        - All messages must be encoded in 'ProcessMessages()'.
            - It is safe to reuse message memory with the same 'frameId.Index()'.
            - Encoded data may not completely sent.
            - If can not encode or send all messages then 'MsgQueueStatistic::incompleteOutput' will be increased.
            - If in previous call not an all messages was sent or encode and user increase the 'frameId' then client(s) will be disconnected.

        - Not an all messages will be decoded in 'ProcessMessages()'.
            - Used the allocator from 'MessageFactory', if it runs out of memory, then decoding will stop.
*/

#pragma once

#include "networking/HighLevel/IChannel.h"
#include "networking/LowLevel/TcpSocket.h"

namespace AE::Networking
{

    //
    // Base TCP Channel
    //
    class TcpChannel : public IChannel
    {
    // types
    protected:
        static constexpr Bytes      _maxMsgSize             = NetConfig::TCP_MaxMsgSize;
    private:
        static constexpr char       _magicByte              = '\x1A';
        static constexpr uint       _maxAttemptsToReceive   = 4;
        static constexpr uint       _maxAttemptsToSend      = 4;

        struct _MsgHeader;

        struct SendQueue
        {
            MsgList_t               queue;
            MsgList_t               pendingFirst;
            MsgList_t               pendingLast;
            DynUntypedStorage       storage;
            Bytes                   encoded;

            void  FlushPendingQueue ()  __NE___;
        };

        struct ReceiveQueue
        {
            MsgQueueMap_t           queue;
            DynUntypedStorage       storage;
            Bytes                   received;

            void  ResetQueue ()         __NE___;
        };


    // variables
    protected:
        TcpSocket               _socket;
        RC<MessageFactory>      _msgFactory;

        SendQueue               _toSend;
        ReceiveQueue            _received;
        FrameUID                _lastFrameId;

        RC<IAllocator>          _allocator;


    // methods
    public:

      // IChannel //
        void            Send (MsgList_t)                                            __NE_OV;
        MsgQueueMap_t&  Receive ()                                                  __NE_OV { return _received.queue; }


    protected:
        TcpChannel (RC<MessageFactory>  mf,
                    RC<IAllocator>      alloc)                                      __NE___;

        void  _SendMessages (TcpSocket &, MsgListIter_t&, EClientLocalID, bool &)   __NE___;
        void  _ReceiveMessages (TcpSocket &, FrameUID, EClientLocalID, bool &)      __NE___;

        void  _OnEncodingError (CSMessagePtr)                                       C_NE___;
        void  _OnDecodingError (CSMessageUID)                                       C_NE___;

        ND_ bool  _IsValid ()                                                       C_NE___;

        static void  _ValidateMsgStream (const void *ptr, Bytes size)               __NE___;
    };
//-----------------------------------------------------------------------------



    //
    // TCP Reliable Ordered Client Channel
    //
    class TcpClientChannel final : public TcpChannel
    {
    // types
    public:
        class ClientAPI
        {
            friend class BaseClient;
            ND_ static RC<IChannel>  Create (RC<MessageFactory> mf, RC<IAllocator>, RC<IServerProvider>, Bool, StringView dbgName) __NE___;
        };

    private:
        enum class EStatus : ubyte
        {
            Disconnected,
            Connecting,
            Connected,
            Failed,
        };


    // variables
    private:
        MsgListIter_t           _lastSentMsg;       // encoded but not sent

        const bool              _reliable;
        EStatus                 _status             = EStatus::Disconnected;
        ushort                  _serverIndex        = 0;

        IpAddress               _serverAddress;
        RC<IServerProvider>     _serverProvider;


    // methods
    public:
        ~TcpClientChannel ()                                __NE_OV;

      // IChannel //
        void  ProcessMessages (FrameUID, INOUT MsgQueueStatistic &) __NE_OV;
        bool  DisconnectClient (EClientLocalID)                     __NE_OV { return false; }
        void  DisconnectClientsWithIncompleteMsgQueue ()            __NE_OV {}
        bool  IsConnected ()                                        C_NE_OV { return _status == EStatus::Connected; }

    private:
        TcpClientChannel (RC<MessageFactory>    mf,
                                  RC<IServerProvider>   serverProvider,
                                  RC<IAllocator>        alloc,
                                  Bool                  reliable)   __NE___;
        void  _Reconnect ()                                         __NE___;
        void  _ProcessMessages (FrameUID, MsgQueueStatistic &)      __NE___;

        ND_ bool  _IsValid ()                                       C_NE___;
    };



    //
    // TCP Reliable Ordered Server Channel
    //
    class TcpServerChannel final : public TcpChannel
    {
    // types
    public:
        class ServerAPI
        {
            friend class BaseServer;
            ND_ static RC<IChannel>  Create (RC<MessageFactory> mf, RC<IAllocator>, RC<IClientListener>,
                                             ushort port, uint maxConnections, Bool, StringView dbgName) __NE___;
        };

    private:
        static constexpr uint   _maxClients = NetConfig::TCP_Reliable_MaxClients;

        struct Client
        {
            EClientLocalID      id;
            Bytes16u            received;
            TcpSocket           socket;
            MsgListIter_t       lastSentMsg;    // encoded but not sent
            DynUntypedStorage   encodedStorage;
            Bytes               encoded;
        };

        using ClientIdx_t       = ubyte;
        using ClientPool_t      = StaticArray< Client, _maxClients >;
        using ClientPoolBits_t  = BitSet< _maxClients >;
        using ClientAddrMap_t   = FixedMap< IpAddress, ClientIdx_t, _maxClients >;
        using UniqueClientId_t  = FixedSet< EClientLocalID, _maxClients >;


    // variables
    private:
        const bool              _reliable;

        ClientPool_t            _clientPool;
        ClientPoolBits_t        _poolBits;

        ClientAddrMap_t         _clientAddrMap;
        DynUntypedStorage       _tempStorage;       // size: '_maxMsgSize * _maxClients'

        RC<IClientListener>     _listener;

        DEBUG_ONLY(
            UniqueClientId_t    _uniqueClientId;)


    // methods
    public:
        ~TcpServerChannel ()                                __NE_OV;

      // IChannel //
        void  ProcessMessages (FrameUID, INOUT MsgQueueStatistic &) __NE_OV;
        bool  DisconnectClient (EClientLocalID)                     __NE_OV;
        void  DisconnectClientsWithIncompleteMsgQueue ()            __NE_OV;
        bool  IsConnected ()                                        C_NE_OV { return true; }

    private:
        TcpServerChannel (RC<MessageFactory>    mf,
                                  RC<IClientListener>   listener,
                                  RC<IAllocator>        alloc,
                                  Bool                  reliable)   __NE___;

        ND_ bool  _IsValid ()                                       C_NE___;
            void  _Disconnect (uint idx)                            __NE___;
            void  _CheckNewConnections ()                           __NE___;
            void  _UpdateClients (FrameUID, MsgQueueStatistic &)    __NE___;
    };


} // AE::Networking
