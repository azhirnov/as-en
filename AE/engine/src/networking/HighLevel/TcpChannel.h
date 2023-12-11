// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: no

    Input:
        - Receive data and accumulate it in data stream ('_input.storage').
        - Try to extract message from data stream and add it to message queue ('_input.queue').
        - User can access the message queue by calling 'Receive()'.

    Output:
        - Add messages to queue ('_output.queue').
        - Send messages to server/client.
        - Recycle memory which is allocated for messages and start new frame.
*/

#pragma once

#include "networking/HighLevel/IChannel.h"
#include "networking/LowLevel/TcpSocket.h"

namespace AE::Networking
{

    //
    // TCP Reliable Ordered Channel
    //

    class TcpChannel : public IChannel
    {
    // types
    protected:
        static constexpr Bytes      _maxMsgSize             = NetConfig::TCP_MaxMsgSize;
    private:
        static constexpr ushort     _magicByte              = 0xAE23;
        static constexpr uint       _maxAttemptsToReceive   = 4;
        static constexpr uint       _maxAttemptsToSend      = 8;

        struct _MsgHeader;

        struct SendQueue
        {
            MsgList_t               queue;
            MsgList_t               queueLast;
            DynUntypedStorage       storage;
            Bytes                   encoded;

            void  ResetQueue () __NE___;
        };

        struct ReceiveQueue
        {
            MsgQueueMap_t           queue;
            DynUntypedStorage       storage;
            Bytes                   received;

            void  ResetQueue () __NE___;
        };


    // variables
    protected:
        TcpSocket               _socket;
        RC<MessageFactory>      _msgFactory;

        SendQueue               _toSend;
        ReceiveQueue            _received;

        RC<IAllocator>          _allocator;


    // methods
    public:
        ~TcpChannel ()                                                          __NE_OV;

      // IChannel //
        void            Send (MsgList_t)                                        __NE_OV;
        MsgQueueMap_t&  Receive ()                                              __NE_OV { return _received.queue; }


    protected:
        TcpChannel (RC<MessageFactory>  mf,
                    RC<IAllocator>      alloc)                                  __NE___;

        void  _SendMessages (TcpSocket &, bool &)                               __NE___;
        void  _ReceiveMessages (TcpSocket &, FrameUID, EClientLocalID, bool &)  __NE___;

        void  _OnEncodingError (CSMessagePtr)                                   C_NE___;
        void  _OnDecodingError (CSMessageUID)                                   C_NE___;

        ND_ bool  _IsValid ()                                                   C_NE___;

        static void  _ValidateMsgStream (const void *ptr, Bytes size)           __NE___;
    };



    //
    // TCP Client Channel
    //

    class TcpClientChannel final : public TcpChannel
    {
    // types
    public:
        class ClientAPI
        {
            friend class BaseClient;
            ND_ static RC<IChannel>  Create (RC<MessageFactory> mf, RC<IAllocator>, RC<IServerProvider>) __NE___;
        };

    private:
        enum class EStatus : uint
        {
            Disconnected,
            Connecting,
            Connected,
            Failed,
        };


    // variables
    private:
        EStatus                 _status             = EStatus::Disconnected;
        uint                    _serverIndex        = 0;
        IpAddress               _serverAddress;
        RC<IServerProvider>     _serverProvider;


    // methods
    public:
        ~TcpClientChannel ()                                    __NE_OV;

      // IChannel //
        void  ProcessMessages (FrameUID)                        __NE_OV;
        bool  DisconnectClient (EClientLocalID)                 __NE_OV { return false; }
        bool  IsConnected ()                                    C_NE_OV;

    private:
        TcpClientChannel (RC<MessageFactory>    mf,
                          RC<IServerProvider>   serverProvider,
                          RC<IAllocator>        alloc)          __NE___;
        void  _Reconnect ()                                     __NE___;
        void  _ProcessMessages (FrameUID)                       __NE___;

        ND_ bool  _IsValid ()                                   C_NE___;
    };



    //
    // TCP Server Channel
    //

    class TcpServerChannel final : public TcpChannel
    {
    // types
    public:
        class ServerAPI
        {
            friend class BaseServer;
            ND_ static RC<IChannel>  Create (RC<MessageFactory> mf, RC<IAllocator>, RC<IClientListener>,
                                             ushort port, uint maxConnections) __NE___;
        };

    private:
        static constexpr uint   _maxClients = 16;

        struct Client
        {
            EClientLocalID      id;
            Byte16u             received;
            TcpSocket           socket;
        };

        using ClientIdx_t       = ubyte;
        using ClientPool_t      = StaticArray< Client, _maxClients >;
        using ClientPoolBits_t  = BitSet< _maxClients >;
        using ClientAddrMap_t   = FixedMap< IpAddress, ClientIdx_t, _maxClients >;
        using UniqueClientId_t  = FixedSet< EClientLocalID, _maxClients >;


    // variables
    private:
        ClientPool_t            _clientPool;
        ClientPoolBits_t        _poolBits;

        ClientAddrMap_t         _clientAddrMap;
        DynUntypedStorage       _tempStorage;       // size: '_maxMsgSize * _maxClients'

        RC<IClientListener>     _listener;

        DEBUG_ONLY(
            UniqueClientId_t    _uniqueClientId;)


    // methods
    public:
        ~TcpServerChannel ()                                    __NE_OV;

      // IChannel //
        void  ProcessMessages (FrameUID)                        __NE_OV;
        bool  DisconnectClient (EClientLocalID)                 __NE_OV;
        bool  IsConnected ()                                    C_NE_OV { return true; }

    private:
        TcpServerChannel (RC<MessageFactory>    mf,
                          RC<IClientListener>   listener,
                          RC<IAllocator>        alloc)          __NE___;

        ND_ bool  _IsValid ()                                   C_NE___;
            void  _Disconnect (uint idx)                        __NE___;
    };


} // AE::Networking
