// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: no
*/

#pragma once

#include "networking/HighLevel/IChannel.h"
#include "networking/LowLevel/UdpDbgSocket.h"

namespace AE::Networking
{

    //
    // UDP Unreliable Unordered Channel
    //

    class UdpUnreliable : public IChannel
    {
    // types
    protected:
        #if 0 //def AE_DEBUG
        using Socket_t  = UdpDbgSocket;
        #else
        using Socket_t  = UdpSocket;
        #endif

        static constexpr ushort     _magic          = 0;    // TODO
        static constexpr Bytes      _maxPacketSize  {508};

        struct _PacketHeader;
        struct _MsgHeader;

        struct QueueAndStorage
        {
            Array< CSMessagePtr >   queue;
            DynUntypedStorage       storage;
        };


    // variables
    protected:
        Socket_t                _socket;
        RC<MessageFactory>      _msgFactory;

        QueueAndStorage         _output;
        Array< CSMessagePtr >   _inputQueue;

        RC<IAllocator>          _allocator;


    // methods
    public:
        ~UdpUnreliable ()                                               __NE_OV;

      // IChannel //
        void            Send (MsgList_t)                                __NE_OV;
        MsgQueueMap_t&  Receive ()                                      __NE_OV { return *Cast<MsgQueueMap_t>(null); }

        bool  DisconnectClient (EClientLocalID)                         __NE_OV { return false; }
        bool  IsConnected ()                                            C_NE_OV { return false; }


    protected:
        UdpUnreliable (RC<MessageFactory>   mf,
                       RC<IAllocator>       alloc)                      __NE___;

        template <typename Address>
        void  _SendMessages (const Address &, QueueAndStorage &, bool &)C_NE___;

    //  void  _ReceiveMessages (QueueAndStorage &, FrameUID, bool &)    C_NE___;

        void  _OnEncodingError (CSMessagePtr)                           C_NE___;
        void  _OnDecodingError (CSMessageUID)                           C_NE___;
    };



    //
    // UDP Client Channel
    //

    class UdpUnreliableClientChannel final : public UdpUnreliable
    {
    // types
    public:
        class ClientAPI
        {
            friend class BaseClient;
            ND_ static RC<IChannel>  Create (RC<MessageFactory> mf, RC<IAllocator>,
                                             RC<IServerProvider>, ushort port) __NE___;
        };


    // variables
    private:
        IpAddress               _serverAddress;
        uint                    _serverIndex        = 0;
        RC<IServerProvider>     _serverProvider;


    // methods
    public:
        ~UdpUnreliableClientChannel ()                                  __NE_OV;

      // IChannel //
        void  ProcessMessages (FrameUID)                                __NE_OV;

    private:
        UdpUnreliableClientChannel (RC<MessageFactory>  mf,
                                    RC<IServerProvider> serverProvider,
                                    RC<IAllocator>      alloc)          __NE___;
        void  _Reconnect ()                                             __NE___;
    };



    //
    // UDP Server Channel
    //

    class UdpUnreliableServerChannel final : public UdpUnreliable
    {
    // types
    public:
        class ServerAPI
        {
            friend class BaseServer;
            ND_ static RC<IChannel>  Create (RC<MessageFactory> mf, RC<IAllocator>,
                                             RC<IClientListener>, ushort port) __NE___;
        };

    private:
        static constexpr uint   _maxClients = 64;

        struct Client
        {
            DynUntypedStorage       storage;
        };
        using Clients_t = FixedMap< IpAddress, Client, _maxClients >;


    // variables
    private:
    //  Clients_t               _clients;
        RC<IClientListener>     _listener;


    // methods
    public:
        ~UdpUnreliableServerChannel ()                                  __NE_OV;

      // IChannel //
        void  ProcessMessages (FrameUID)                                __NE_OV;

    private:
        UdpUnreliableServerChannel (RC<MessageFactory>  mf,
                                    RC<IClientListener> listener,
                                    RC<IAllocator>      alloc)          __NE___;
    };


} // AE::Networking
