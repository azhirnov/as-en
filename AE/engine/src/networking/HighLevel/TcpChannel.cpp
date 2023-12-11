// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/TcpChannel.h"
#include "networking/HighLevel/Client.h"
#include "networking/HighLevel/Server.h"

namespace AE::Networking
{
namespace {
    static constexpr auto   c_ChannelType = EChannel::Reliable;
}

/*
=================================================
    _MsgHeader
=================================================
*/
    struct TcpChannel::_MsgHeader
    {
        // TODO
    //  uint            _magic  : 8;
    //  uint            _size   : 11;
    //  uint            _msgId  : 12;

        ushort          magic;
        Byte16u         size;
        CSMessageUID    msgId;

        _MsgHeader () __NE___ = default;

        _MsgHeader (Byte16u size, CSMessageUID msgId) __NE___ :
            magic{ _magicByte },
            size{ size },
            msgId{ msgId }
        {}
    };

/*
=================================================
    SendQueue::ResetQueue
=================================================
*/
    inline void  TcpChannel::SendQueue::ResetQueue () __NE___
    {
        queue       = Default;
        queueLast   = Default;
    }

/*
=================================================
    ReceiveQueue::ResetQueue
=================================================
*/
    inline void  TcpChannel::ReceiveQueue::ResetQueue () __NE___
    {
        queue.clear();
    }

/*
=================================================
    constructor
=================================================
*/
    TcpChannel::TcpChannel (RC<MessageFactory> mf, RC<IAllocator> alloc) __NE___ :
        _msgFactory{ RVRef(mf) },
        _allocator{ RVRef(alloc) }
    {
        StaticAssert( sizeof(_MsgHeader) == 6 );

        _toSend  .storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
        _received.storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
    }

/*
=================================================
    destructor
=================================================
*/
    TcpChannel::~TcpChannel () __NE___
    {
        _toSend  .storage.Dealloc( _allocator.get() );
        _received.storage.Dealloc( _allocator.get() );
    }

/*
=================================================
    _IsValid
=================================================
*/
    bool  TcpChannel::_IsValid () C_NE___
    {
        return  _msgFactory         and
                _allocator          and
                _toSend.storage     and
                _received.storage;
    }

/*
=================================================
    _SendMessages
=================================================
*/
    void  TcpChannel::_SendMessages (TcpSocket &socket, INOUT bool &isDisconnected) __NE___
    {
        if_unlikely( _toSend.queue.empty() )
            return;

        auto&       storage         = _toSend.storage;
        Bytes       encoded         = _toSend.encoded;      // offset in 'storage' to the end of encoded messages
        Bytes       pending;                                // offset in 'storage' to begin of data which can be sent
        auto        last_failed_it  = _toSend.queue.end();

        const auto  SendData        = [&socket, &storage, &encoded, &pending, &isDisconnected] (bool sendAll = false)
        {{
            //DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(pending), encoded ));

            bool    retry = true;
            for (uint i = 0; retry & (encoded > pending) & (i < _maxAttemptsToSend); ++i)
            {
                        retry       = sendAll;
                auto    [err, sent] = socket.Send( storage.Ptr( pending ), encoded - pending );

                BEGIN_ENUM_CHECKS();
                switch ( err )
                {
                    case_likely SocketSendError::Sent :
                    {
                        ASSERT( sent > 0 );
                        pending += sent;
                        break;
                    }

                    case SocketSendError::NotSent :
                    case SocketSendError::ResourceTemporarilyUnavailable :
                        ThreadUtils::Sleep_500us();     // TODO: optimize
                        break; // skip

                    case SocketSendError::_Error :
                    case SocketSendError::UDP_MessageTooLong :
                        // unused

                    case SocketSendError::NoSocket :
                    case SocketSendError::NotConnected :
                    case SocketSendError::ConnectionResetByPeer :
                    case SocketSendError::UnknownError :
                    case SocketSendError::PermissionDenied :
                    default :
                        isDisconnected = true;
                        return;
                }
                END_ENUM_CHECKS();
            }

            if ( pending != encoded )
            {
                MemMove( OUT storage.Ptr(), storage.Ptr( pending ), encoded - pending );    // TODO: optimize
                //DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(), encoded - pending ));
            }

            encoded -= pending;
            pending  = 0_b;
        }};


        for (auto it = _toSend.queue.begin(); (it != _toSend.queue.end()) & (not isDisconnected);)
        {
            if_unlikely( encoded + sizeof(_MsgHeader) >= storage.Size() )
                SendData();

            auto*           hdr_ptr     = storage.Ptr( encoded );
            const Bytes     msg_off     = encoded + sizeof(_MsgHeader);
            DataEncoder     enc         { storage.Ptr( msg_off ), storage.End() };
            auto            err         = (*it)->Serialize( enc );

            BEGIN_ENUM_CHECKS();
            switch ( err )
            {
                case_likely CSMessage::EncodeError::OK :
                {
                    encoded = storage.Size() - enc.RemainingSize();

                    _MsgHeader  header { encoded - msg_off, (*it)->UniqueId() };

                    ASSERT( Bytes{header.size} <= _maxMsgSize );
                    MemCopy( OUT hdr_ptr, &header, Sizeof(header) );

                    ++it;
                    break;
                }

                // skip message
                case CSMessage::EncodeError::Failed :
                    _OnEncodingError( *it );
                    ++it;
                    break;

                // send and try again
                case CSMessage::EncodeError::NoMemory :
                {
                    // skip on second try
                    if ( last_failed_it == it )
                    {
                        _OnEncodingError( *it );
                        ++it;
                        break;
                    }

                    SendData();
                    last_failed_it = it;
                    break;
                }
            }
            END_ENUM_CHECKS();
        }

        SendData( true );
        ASSERT( isDisconnected or pending == 0 );

        isDisconnected |= (pending > 0);

        _toSend.encoded = encoded;
    }

/*
=================================================
    _ReceiveMessages
=================================================
*/
    void  TcpChannel::_ReceiveMessages (TcpSocket &socket, const FrameUID frameId, const EClientLocalID clientId, INOUT bool &isDisconnected) __NE___
    {
        auto&   storage     = _received.storage;
        Bytes   received    = _received.received;   // offset in 'storage' to the end of received data
        Bytes   decoded;                            // offset in 'storage' to the begin of data which can be decoded, if enough size
        auto&   allocator   = _msgFactory->GetAllocator( frameId );
        bool    retry       = true;

        for (uint i = 0; retry & (i < _maxAttemptsToReceive); ++i)
        {
                    retry       = false;
            auto    [err, recv] = socket.Receive( OUT storage.Ptr( received ), storage.Size() - received );

            BEGIN_ENUM_CHECKS();
            switch ( err )
            {
                case_likely SocketReceiveError::Received :
                {
                    ASSERT( recv > 0 );
                    received += recv;

                    DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(decoded), received ));

                    for (; sizeof(_MsgHeader) <= (received - decoded);)
                    {
                        ASSERT( received >= decoded );

                        _MsgHeader  header; MemCopy( OUT &header, storage.Ptr( decoded ), Sizeof(header) );

                        if_unlikely( (Bytes{header.size} + sizeof(header)) > (received - decoded) )
                        {
                            ASSERT( (received - decoded) < Sizeof(header)   or
                                    Bytes{header.size} <= _maxMsgSize       or
                                    header.magic == _magicByte );

                            retry = true;
                            break;  // not enough data
                        }

                        DataDecoder     des{ storage.Ptr( decoded + sizeof(header) ), header.size, allocator };

                        ASSERT( Bytes{header.size} <= _maxMsgSize );
                        ASSERT( header.magic == _magicByte );

                        decoded += sizeof(header) + header.size;

                        if_unlikely( header.magic != _magicByte )
                            continue;  // invalid header - skip

                        // cache optimization:
                        // allocate chunk before allocating the message.
                        auto&   dst = _received.queue( CSMessage::UnpackGroupID( header.msgId ));

                        if_unlikely( dst.last.empty() )
                        {
                            dst.last = dst.first.AddChunk( allocator, NetConfig::MsgPerChunk );
                            if_unlikely( dst.last.empty() )
                                break;  // out of memory
                        }

                        if_unlikely( dst.last->IsFull() )
                        {
                            dst.last = dst.last.AddChunk( allocator, NetConfig::MsgPerChunk );
                            if_unlikely( dst.last.empty() )
                                break;  // out of memory
                        }

                        // create & decode message
                        CSMessagePtr    msg;
                        if_likely( _msgFactory->DeserializeMsg( frameId, header.msgId, clientId, OUT msg, des ))
                        {
                            ASSERT( des.IsComplete() );
                            dst.last->emplace_back( msg );
                        }
                        else
                        {
                            // TODO:
                            //  out of memory - try again in next frame
                            //  data corruption - skip

                            //_OnDecodingError( header.msgId );
                            break;
                        }
                    }

                    // free space for large data
                    if ( decoded > 0 )
                    {
                        ASSERT( received >= decoded );
                        if ( received != decoded )
                        {
                            MemMove( OUT storage.Ptr(), storage.Ptr( decoded ), received - decoded );   // TODO: optimize
                            DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(), received - decoded ));
                        }
                        received    -= decoded;
                        decoded     = 0_b;
                        retry       = true;
                    }
                    break;
                }

                case SocketReceiveError::NotReceived :
                case SocketReceiveError::ResourceTemporarilyUnavailable :
                    break; // skip

                case SocketReceiveError::_Error :
                    // unused

                case SocketReceiveError::ConnectionResetByPeer :
                case SocketReceiveError::ConnectionRefused :
                case SocketReceiveError::NotConnected :
                case SocketReceiveError::NoSocket :
                case SocketReceiveError::UnknownError :
                default :
                    isDisconnected = true;
                    break;
            }
            END_ENUM_CHECKS();
        }

        CHECK_MSG( decoded == 0, "Received data is not complete" );
        _received.received = received;
    }

/*
=================================================
    _OnEncodingError / _OnDecodingError
=================================================
*/
    inline void  TcpChannel::_OnEncodingError (CSMessagePtr) C_NE___
    {
        AE_LOG_DBG( "OnEncodingError" );
    }

    inline void  TcpChannel::_OnDecodingError (CSMessageUID) C_NE___
    {
        AE_LOG_DBG( "OnDecodingError" );
    }

/*
=================================================
    Send
=================================================
*/
    void  TcpChannel::Send (MsgList_t msgList) __NE___
    {
        if_unlikely( _toSend.queue.empty() )
            _toSend.queue = msgList;

        _toSend.queueLast.Append( msgList );
        _toSend.queueLast.MoveToLast();
    }

/*
=================================================
    _ValidateMsgStream
=================================================
*/
    void  TcpChannel::_ValidateMsgStream (const void *ptr, Bytes size) __NE___
    {
        Bytes   offset;

        for (;;)
        {
            _MsgHeader  header;

            if ( offset + Sizeof(header) > size )
                break;

            MemCopy( OUT &header, ptr + offset, Sizeof(header) );

            ASSERT( header.magic == _magicByte );
            ASSERT( header.size <= _maxMsgSize );

            if ( offset + Sizeof(header) + header.size > size )
                break;

            offset += Sizeof(header) + header.size;
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ProcessMessages
=================================================
*/
    void  TcpClientChannel::ProcessMessages (FrameUID frameId) __NE___
    {
        BEGIN_ENUM_CHECKS();
        switch ( _status )
        {
            case_likely EStatus::Connected :
                _ProcessMessages( frameId );
                break;

            // TODO: custom timeout
            case EStatus::Connecting :
            {
                switch ( _socket.ConnectionStatus() )
                {
                    case TcpSocket::EStatus::Connected :
                        AE_LOGI( "Connected client to TCP server: "s << _serverAddress.ToString() );
                        _status = EStatus::Connected;
                        break;

                    case TcpSocket::EStatus::Connecting :
                        break;

                    case TcpSocket::EStatus::Failed :
                    case TcpSocket::EStatus::NoSocket :
                    default :
                        _status = EStatus::Failed;
                        break;
                }
                break;
            }

            case EStatus::Failed :
                ++_serverIndex;

            case EStatus::Disconnected :
            default :
                AE_LOGI( "Client disconnected, try to reconnect or try another server..." );
                _Reconnect();
                break;
        }
        END_ENUM_CHECKS();
    }

    void  TcpClientChannel::_ProcessMessages (FrameUID frameId) __NE___
    {
        _received.ResetQueue();

        bool    disconnected = false;

        _ReceiveMessages( _socket, frameId, Default, INOUT disconnected );
        _SendMessages( _socket, INOUT disconnected );

        _toSend.ResetQueue();

        if ( disconnected )
        {
            AE_LOGI( "Client disconnected, try to reconnect or try another server..." );
            _Reconnect();
        }
    }

/*
=================================================
    _Reconnect
=================================================
*/
    void  TcpClientChannel::_Reconnect () __NE___
    {
        _socket.Close();

        _status             = EStatus::Disconnected;
        _toSend.encoded     = 0_b;
        _received.received  = 0_b;

        _serverProvider->GetAddress( c_ChannelType, _serverIndex, True{"TCP"}, OUT _serverAddress );

        if ( _socket.AsyncConnect( _serverAddress ))
        {
            AE_LOGI( "Try connecting client to TCP server: "s << _serverAddress.ToString() );
            _status = EStatus::Connecting;
        }
        else
        {
            AE_LOGI( "Invalid server address: "s << _serverAddress.ToString() );
            ++_serverIndex;
        }
    }

/*
=================================================
    ClientAPI::Create
=================================================
*/
    RC<IChannel>  TcpClientChannel::ClientAPI::Create (RC<MessageFactory> mf, RC<IAllocator> alloc, RC<IServerProvider> serverProvider) __NE___
    {
        CHECK_ERR( mf );
        CHECK_ERR( alloc );
        CHECK_ERR( serverProvider );

        RC<TcpClientChannel>    result{new TcpClientChannel{ RVRef(mf), RVRef(serverProvider), RVRef(alloc) }};

        CHECK_ERR( result->_IsValid() );

        DEBUG_ONLY( result->_socket.SetDebugName( "TCP client" );)

        result->_Reconnect();

        return result;
    }

/*
=================================================
    constructor
=================================================
*/
    TcpClientChannel::TcpClientChannel (RC<MessageFactory>  mf,
                                        RC<IServerProvider> serverProvider,
                                        RC<IAllocator>      alloc) __NE___ :
        TcpChannel{ RVRef(mf), RVRef(alloc) },
        _serverProvider{ RVRef(serverProvider) }
    {}

/*
=================================================
    destructor
=================================================
*/
    TcpClientChannel::~TcpClientChannel () __NE___
    {}

/*
=================================================
    _IsValid
=================================================
*/
    bool  TcpClientChannel::_IsValid () C_NE___
    {
        return  TcpChannel::_IsValid()  and
                _serverProvider;
    }

/*
=================================================
    IsConnected
=================================================
*/
    bool  TcpClientChannel::IsConnected () C_NE___
    {
        return _socket.IsOpen();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ProcessMessages
=================================================
*/
    void  TcpServerChannel::ProcessMessages (const FrameUID frameId) __NE___
    {
        if_unlikely( not _socket.IsOpen() )
            return;

        _received.ResetQueue();

        for (uint i = 0; i < _maxClients; ++i)
        {
            IpAddress   addr;
            TcpSocket   client;

            if_likely( not client.Accept( _socket, OUT addr ))
                break;

            if ( auto client_id = _listener->OnClientConnected( c_ChannelType, addr );  client_id != Default )
            {
                const int   idx = BitScanForward( ~_poolBits.to_ullong() );

                if_unlikely( idx < 0 or idx >= int(_maxClients) )
                {
                    _listener->OnClientDisconnected( c_ChannelType, client_id );
                    break;
                }

                // save client
                _poolBits.set( idx );

                auto&   dst = _clientPool[idx];
                dst.id      = client_id;
                Reconstruct( OUT dst.socket, RVRef(client) );

                dst.socket.KeepAlive();

                CHECK( _clientAddrMap.insert_or_assign( addr, ClientIdx_t(idx) ).first );

                ASSERT( _uniqueClientId.insert( client_id ).second );
                ASSERT( _uniqueClientId.size() == _clientAddrMap.size() );
            }
        }

        for (uint idx : BitIndexIterate( _poolBits ))
        {
            auto&   client          = _clientPool[idx];
            void*   storage         = _tempStorage.Ptr( idx * _maxMsgSize );
            bool    disconnected    = false;

            // receive message from client
            {
                // restore data which is not yet decoded
                if ( client.received > 0 )
                {
                    MemCopy( OUT _received.storage.Ptr(), storage, client.received );
                    _received.received  = client.received;
                    client.received     = 0_b;
                }

                _ReceiveMessages( client.socket, frameId, client.id, INOUT disconnected );

                // Too much data, disconnect client to avoid message stream corruption.
                if_unlikely( _received.received > _maxMsgSize )
                {
                    AE_LOG_DBG( "input message stream overflow, client will be disconnected" );
                    disconnected = true;
                }else

                // Save data which can not be decoded, must be less than single message size.
                if_unlikely( _received.received > 0 )
                {
                    MemCopy( OUT storage, _received.storage.Ptr(), _received.received );

                    client.received     = _received.received;
                    _received.received  = 0_b;
                }
            }

            // send messages to client
            if_likely( not disconnected )
            {
                _toSend.encoded = 0_b;

                _SendMessages( client.socket, INOUT disconnected );

                if_unlikely( _toSend.encoded > 0 )
                {
                    AE_LOG_DBG( "output message stream overflow, client will be disconnected" );
                    disconnected = true;
                }
            }

            if_unlikely( disconnected )
                _Disconnect( idx );
        }

        _toSend.ResetQueue();
    }

/*
=================================================
    ServerAPI::Create
=================================================
*/
    RC<IChannel>  TcpServerChannel::ServerAPI::Create (RC<MessageFactory> mf, RC<IAllocator> alloc,
                                                       RC<IClientListener> listener,
                                                       ushort port, uint maxConnections) __NE___
    {
        CHECK_ERR( mf );
        CHECK_ERR( alloc );
        CHECK_ERR( listener );

        RC<TcpServerChannel>    result  {new TcpServerChannel{ RVRef(mf), RVRef(listener), RVRef(alloc) }};
        TcpSocket::Config       cfg;    cfg.maxConnections = maxConnections;

        CHECK_ERR( result->_IsValid() );

        DEBUG_ONLY( result->_socket.SetDebugName( "TCP server" );)
        CHECK_ERR( result->_socket.Listen( IpAddress::FromLocalPortTCP(port), cfg ));

        AE_LOGI( "Started TCP server on port: "s << ToString(port) );
        return result;
    }

/*
=================================================
    constructor
=================================================
*/
    TcpServerChannel::TcpServerChannel (RC<MessageFactory>  mf,
                                        RC<IClientListener> listener,
                                        RC<IAllocator>      alloc) __NE___ :
        TcpChannel{ RVRef(mf), RVRef(alloc) },
        _listener{ RVRef(listener) }
    {
        _tempStorage.Alloc( _maxMsgSize * _maxClients, DefaultAllocatorAlign, _allocator.get() );
    }

/*
=================================================
    destructor
=================================================
*/
    TcpServerChannel::~TcpServerChannel () __NE___
    {
        _tempStorage.Dealloc( _allocator.get() );
    }

/*
=================================================
    _IsValid
=================================================
*/
    bool  TcpServerChannel::_IsValid () C_NE___
    {
        return  TcpChannel::_IsValid()  and
                _tempStorage            and
                _listener;
    }

/*
=================================================
    DisconnectClient
=================================================
*/
    bool  TcpServerChannel::DisconnectClient (EClientLocalID id) __NE___
    {
        for (uint idx : BitIndexIterate( _poolBits ))
        {
            if_unlikely( _clientPool[idx].id == id )
            {
                _Disconnect( idx );
                return true;
            }
        }
        return false;
    }

    void  TcpServerChannel::_Disconnect (const uint idx) __NE___
    {
        auto&       client  = _clientPool[idx];
        const auto  id      = client.id;

        client.id       = Default;
        client.received = 0_b;
        client.socket.Close();

        _poolBits.reset( idx );

        for (ubyte i : _clientAddrMap.GetValueArray())
        {
            if ( i == idx )
            {
                auto    it = _clientAddrMap.begin() + i;
                ASSERT( it->second == idx );

                _clientAddrMap.EraseByIter( it );
                break;
            }
        }

        ASSERT( _uniqueClientId.erase( id ));
        _listener->OnClientDisconnected( c_ChannelType, id );

        AE_LOG_DBG( "client ("s << ToString<16>(uint(id)) << ") disconnected" );
    }


} // AE::Networking
