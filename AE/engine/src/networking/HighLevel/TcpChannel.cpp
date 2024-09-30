// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/TcpChannel.h"
#include "networking/HighLevel/Client.h"
#include "networking/HighLevel/Server.h"

namespace AE::Networking
{
namespace {
	static constexpr auto	c_TcpChannelType = EChannel::Reliable;
}

/*
=================================================
	_MsgHeader
=================================================
*/
	struct TcpChannel::_MsgHeader
	{
		uint	_magic	: 6;
		uint	_size	: 11;
		uint	_msgId	: NetConfig::CSMessageUID_Bits;

		_MsgHeader () __NE___ = default;

		_MsgHeader (Bytes size, CSMessageUID msgId) __NE___ :
			_magic{ uint(_magicByte) },
			_size{ uint(size) },
			_msgId{ uint(msgId) }
		{
			ASSERT( Size() == size );
			ASSERT( Id() == msgId );
		}

		ND_ Bytes			Size ()		C_NE___	{ return Bytes{_size}; }
		ND_ CSMessageUID	Id ()		C_NE___	{ return CSMessageUID(_msgId); }
		ND_ char			Magic ()	C_NE___	{ return char(_magic); }
	};

/*
=================================================
	SendQueue::FlushPendingQueue
=================================================
*/
	inline void  TcpChannel::SendQueue::FlushPendingQueue () __NE___
	{
		queue			= pendingFirst;
		pendingFirst	= Default;
		pendingLast		= Default;
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
		StaticAssert( sizeof(_MsgHeader) == 4 );
	}

/*
=================================================
	_IsValid
=================================================
*/
	bool  TcpChannel::_IsValid () C_NE___
	{
		return	_msgFactory		and
				_allocator;
	}

/*
=================================================
	_SendMessages
=================================================
*/
	void  TcpChannel::_SendMessages (TcpSocket &socket, MsgListIter_t &lastSendMsg, const EClientLocalID clientId, INOUT bool &isDisconnected) __NE___
	{
		if_unlikely( _toSend.queue.empty() )
			return;

		auto&		storage			= _toSend.storage;
		Bytes		encoded			= _toSend.encoded;		// offset in 'storage' to the end of encoded messages
		Bytes		pending;								// offset in 'storage' to begin of data which can be sent
		const auto	end_it			= _toSend.queue.end();
		auto		last_failed_it	= end_it;

		//         |<---->|- available for Send()
		//   __________________________
		//  |______|______|____________| - storage
		//         ^      ^
		//  pending       encoded

		const auto	SendData		= [&socket, &storage, &encoded, &pending, &isDisconnected] (bool sendAll = false)
		{{
			//DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(pending), encoded ));

			bool	retry = true;
			for (uint i = 0; retry and (encoded > pending) and (i < _maxAttemptsToSend); ++i)
			{
						retry		= sendAll;
				auto	[err, sent]	= socket.Send( storage.Ptr( pending ), encoded - pending );

				switch_enum( err )
				{
					case_likely SocketSendError::Sent :
					{
						ASSERT( sent > 0 );
						pending += sent;
						break;
					}

					case SocketSendError::NotSent :
					case SocketSendError::ResourceTemporarilyUnavailable :
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
				switch_end
			}

			if ( pending != encoded )
			{
				MemMove( OUT storage.Ptr(), storage.Ptr( pending ), encoded - pending );	// TODO: optimize
				//DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(), encoded - pending ));
			}

			encoded -= pending;
			pending  = 0_b;
		}};


		for (auto it = lastSendMsg; (it != end_it) and (not isDisconnected);)
		{
			if ( auto msg_client_id = (*it)->ClientId();  not ((msg_client_id == clientId) or (msg_client_id == Default) or (clientId == Default)) )
				continue;

			if_unlikely( encoded + _maxMsgSize*4 >= storage.Size() )
			{
				SendData( true );

				if_unlikely( encoded + _maxMsgSize*2 >= storage.Size() )
				{
					AE_LOG_DBG( "not enough memory to encode" );

					_toSend.encoded	= encoded;
					lastSendMsg		= it;
					return;
				}
			}

			auto*			hdr_ptr		= storage.Ptr( encoded );
			const Bytes		msg_off		= encoded + sizeof(_MsgHeader);
			DataEncoder		enc			{ storage.Ptr( msg_off ), _maxMsgSize };
			auto			err			= (*it)->Serialize( enc );

			switch_enum( err )
			{
				case_likely CSMessage::EncodeError::OK :
				{
					ASSERT_LE( enc.RemainingSize(), _maxMsgSize );

					_MsgHeader	header { _maxMsgSize - enc.RemainingSize(), (*it)->UniqueId() };

					ASSERT( header.Size() <= _maxMsgSize );	// should never happens
					MemCopy( OUT hdr_ptr, &header, Sizeof(header) );

					encoded += SizeOf<_MsgHeader> + header.Size();
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
			switch_end
		}


		SendData( true );
		CHECK( isDisconnected or pending == 0 );

		_toSend.encoded	= encoded;
		lastSendMsg		= Default;
	}

/*
=================================================
	_ReceiveMessages
=================================================
*/
	void  TcpChannel::_ReceiveMessages (TcpSocket &socket, const FrameUID frameId, const EClientLocalID clientId, INOUT bool &isDisconnected) __NE___
	{
		auto&	storage		= _received.storage;
		Bytes	received	= _received.received;	// offset in 'storage' to the end of received data
		Bytes	decoded;							// offset in 'storage' to the begin of data which can be decoded, if enough size
		auto&	allocator	= _msgFactory->GetAllocator( frameId );
		bool	retry		= true;

		//                |<---------->|- available for Receive()
		//   __________________________|
		//  |______|______|____________| - storage
		//         ^      ^
		//  decoded       received

		for (uint i = 0; retry and (i < _maxAttemptsToReceive); ++i)
		{
					retry		= false;
			auto	[err, recv]	= socket.Receive( OUT storage.Ptr( received ), storage.Size() - received );

			switch_enum( err )
			{
				case_likely SocketReceiveError::Received :
				{
					ASSERT( recv > 0 );
					received += recv;

					DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(decoded), received ));

					for (; (received - decoded) >= sizeof(_MsgHeader);)
					{
						ASSERT( received >= decoded );

						// check message header
						_MsgHeader	header;	MemCopy( OUT &header, storage.Ptr( decoded ), Sizeof(header) );

						if_unlikely( (header.Size() + sizeof(header)) > (received - decoded) )
						{
							ASSERT( (received - decoded) < Sizeof(header)	or
									header.Size() <= _maxMsgSize			or
									header.Magic() == _magicByte );

							retry = true;
							break;  // not enough data
						}

						DataDecoder		des{ storage.Ptr( decoded + sizeof(header) ), header.Size(), allocator };

						ASSERT( header.Size() <= _maxMsgSize );
						ASSERT( header.Magic() == _magicByte );

						decoded += sizeof(header) + header.Size();

						if_unlikely( header.Magic() != _magicByte )
							continue;  // invalid header - skip


						// cache optimization:
						// allocate chunk before allocating the message.
						auto&	dst = _received.queue( CSMessage::UnpackGroupID( header.Id() ));

						if_unlikely( dst.last.empty() )
						{
							dst.last = dst.first.AddChunk( allocator, NetConfig::MsgPerChunk );
							if_unlikely( dst.last.empty() )
								break;	// out of memory
						}

						if_unlikely( dst.last->IsFull() )
						{
							dst.last = dst.last.AddChunk( allocator, NetConfig::MsgPerChunk );
							if_unlikely( dst.last.empty() )
								break;	// out of memory
						}


						// create & decode message
						CSMessagePtr	msg;
						if_likely( _msgFactory->DeserializeMsg( frameId, header.Id(), clientId, OUT msg, des ))
						{
							ASSERT( des.IsComplete() );
							dst.last->emplace_back( msg );
						}
						else
						{
							// TODO:
							//  out of memory - try again in next frame
							//	data corruption - skip

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
							MemMove( OUT storage.Ptr(), storage.Ptr( decoded ), received - decoded );	// TODO: optimize
							DEBUG_ONLY( _ValidateMsgStream( storage.Ptr(), received - decoded ));
						}
						received	-= decoded;
						decoded		= 0_b;
						retry		= true;
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
				case SocketReceiveError::ConnectionAborted :
				case SocketReceiveError::NotConnected :
				case SocketReceiveError::NoSocket :
				case SocketReceiveError::UnknownError :
				default :
					isDisconnected = true;
					break;
			}
			switch_end
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
		// possible errors:
		//	- encoding limited to '_maxMsgSize'
		//	-
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
		if_unlikely( _toSend.pendingFirst.empty() )
			_toSend.pendingFirst = msgList;

		_toSend.pendingLast.Append( msgList );
		_toSend.pendingLast.MoveToLast();
	}

/*
=================================================
	_ValidateMsgStream
=================================================
*/
	void  TcpChannel::_ValidateMsgStream (const void* ptr, Bytes size) __NE___
	{
		Bytes	offset;

		for (;;)
		{
			_MsgHeader	header;

			if ( offset + Sizeof(header) > size )
				break;

			MemCopy( OUT &header, ptr + offset, Sizeof(header) );

			ASSERT( header.Magic() == _magicByte );
			ASSERT( header.Size() <= _maxMsgSize );

			if ( offset + Sizeof(header) + header.Size() > size )
				break;

			offset += Sizeof(header) + header.Size();
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ProcessMessages
=================================================
*/
	void  TcpClientChannel::ProcessMessages (const FrameUID frameId, INOUT MsgQueueStatistic &stat) __NE___
	{
		if ( _lastFrameId != frameId )
		{
			_toSend.FlushPendingQueue();
			_lastFrameId = frameId;

			if_unlikely( _lastSentMsg != Default and _reliable )
			{
				AE_LOG_DBG( "Reliability is broken: some messages are not sent and will be discarded, client will be disconnected" );
				_Reconnect();
			}
			_lastSentMsg = _toSend.queue.begin();
		}

		_received.ResetQueue();

		_ProcessMessages( frameId, INOUT stat );
	}

/*
=================================================
	_ProcessMessages
=================================================
*/
	void  TcpClientChannel::_ProcessMessages (const FrameUID frameId, INOUT MsgQueueStatistic &stat) __NE___
	{
		switch_enum( _status )
		{
			// TODO: custom timeout
			case EStatus::Connecting :
			{
				switch ( _socket.ConnectionStatus() )
				{
					case TcpSocket::EStatus::Connected :
						AE_LOG_DBG( "Connected client to TCP server: "s << _serverAddress.ToString() );
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
				if ( _status != EStatus::Connected )
					break;
			}

			case_likely EStatus::Connected :
			{
				bool	disconnected = false;

				_ReceiveMessages( _socket, frameId, Default, INOUT disconnected );
				_SendMessages( _socket, INOUT _lastSentMsg, Default, INOUT disconnected );

				stat.incompleteOutput += uint(_lastSentMsg != Default);

				if_unlikely( disconnected )
				{
					AE_LOG_DBG( "Client disconnected, try to reconnect or try another server..." );
					_Reconnect();
				}
				break;
			}

			case EStatus::Failed :
				++_serverIndex;

			case EStatus::Disconnected :
			default :
				AE_LOG_DBG( "Client disconnected, try to reconnect or try another server..." );
				_Reconnect();
				break;
		}
		switch_end
	}

/*
=================================================
	_Reconnect
=================================================
*/
	void  TcpClientChannel::_Reconnect () __NE___
	{
		_socket.FastClose();

		_status				= EStatus::Disconnected;
		_toSend.encoded		= 0_b;
		_received.received	= 0_b;

		_serverProvider->GetAddress( c_TcpChannelType, _serverIndex, True{"TCP"}, OUT _serverAddress );

		if ( _socket.AsyncConnect( _serverAddress ))
		{
			AE_LOG_DBG( "Try connecting client to TCP server: "s << _serverAddress.ToString() );
			_status = EStatus::Connecting;
		}
		else
		{
			AE_LOG_DBG( "Invalid server address: "s << _serverAddress.ToString() );
			++_serverIndex;
		}
	}

/*
=================================================
	ClientAPI::Create
=================================================
*/
	RC<IChannel>  TcpClientChannel::ClientAPI::Create (RC<MessageFactory> mf, RC<IAllocator> alloc, RC<IServerProvider> serverProvider,
													   Bool reliable, StringView dbgName) __NE___
	{
		CHECK_ERR( mf );
		CHECK_ERR( alloc );
		CHECK_ERR( serverProvider );

		RC<TcpClientChannel>	result{new TcpClientChannel{ RVRef(mf), RVRef(serverProvider), RVRef(alloc), reliable }};

		CHECK_ERR( result->_IsValid() );

		DEBUG_ONLY(
			if ( dbgName.empty() ) dbgName = "TCP client";
		result->_socket.SetDebugName( String{dbgName} );
		)
		Unused( dbgName );

		result->_Reconnect();

		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	TcpClientChannel::TcpClientChannel (RC<MessageFactory>	mf,
										RC<IServerProvider>	serverProvider,
										RC<IAllocator>		alloc,
										Bool				reliable) __NE___ :
		TcpChannel{ RVRef(mf), RVRef(alloc) },
		_reliable{ reliable },
		_serverProvider{ RVRef(serverProvider) }
	{
		_allocator->Reserve( NetConfig::ChannelStorageSize*2 );

		_toSend  .storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
		_received.storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
	}

/*
=================================================
	destructor
=================================================
*/
	TcpClientChannel::~TcpClientChannel () __NE___
	{
		_toSend  .storage.Dealloc( _allocator.get() );
		_received.storage.Dealloc( _allocator.get() );
	}

/*
=================================================
	_IsValid
=================================================
*/
	bool  TcpClientChannel::_IsValid () C_NE___
	{
		return	TcpChannel::_IsValid()	and
				_toSend.storage			and
				_received.storage		and
				_serverProvider;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ProcessMessages
=================================================
*/
	void  TcpServerChannel::ProcessMessages (const FrameUID frameId, INOUT MsgQueueStatistic &stat) __NE___
	{
		if ( _lastFrameId != frameId )
		{
			_toSend.FlushPendingQueue();
			_lastFrameId = frameId;

			for (uint idx : BitIndexIterate( _poolBits ))
			{
				auto&	client = _clientPool[idx];

				if_likely( client.lastSentMsg == Default )
					client.lastSentMsg = _toSend.queue.begin();
				else
				if ( _reliable )
				{
					AE_LOG_DBG( "Reliability is broken: some messages are no sent and will be discarded, client ("s <<
							    ToString<16>(uint(client.id)) << ") will be disconnected" );

					_Disconnect( idx );
				}
			}
		}

		_received.ResetQueue();

		if_likely( _socket.IsOpen() )
		{
			_CheckNewConnections();
			_UpdateClients( frameId, INOUT stat );
		}
	}

/*
=================================================
	_CheckNewConnections
=================================================
*/
	void  TcpServerChannel::_CheckNewConnections () __NE___
	{
		for (uint i = 0; i < _maxClients; ++i)
		{
			IpAddress	addr;
			TcpSocket	client;

			if_likely( not client.Accept( _socket, OUT addr ))
				break;

			const int	idx = BitScanForward( ~_poolBits.to_ullong() );

			if_unlikely( idx < 0 or idx >= int(_maxClients) )
				break;	// client pool overflow

			if ( auto client_id = _listener->OnClientConnected( c_TcpChannelType, addr );  client_id != Default )
			{
				// save client
				_poolBits.set( idx );

				auto&	dst		= _clientPool[idx];
				dst.id			= client_id;
				dst.lastSentMsg = _toSend.queue.begin();

				DEBUG_ONLY( client.SetDebugName( String{_socket.GetDebugName()} << " client " << ToString<16>(uint(client_id)) );)

				Reconstruct( OUT dst.socket, RVRef(client) );
				dst.socket.KeepAlive();

				CHECK( _clientAddrMap.insert_or_assign( addr, ClientIdx_t(idx) ).first );

				ASSERT( _uniqueClientId.insert( client_id ).second );
				ASSERT( _uniqueClientId.size() == _clientAddrMap.size() );

				AE_LOG_DBG( "client ("s << ToString<16>(uint(client_id)) << ") connected, addr: " << addr.ToString() );
			}
		}
	}

/*
=================================================
	_UpdateClients
=================================================
*/
	void  TcpServerChannel::_UpdateClients (const FrameUID frameId, INOUT MsgQueueStatistic &stat) __NE___
	{
		for (uint idx : BitIndexIterate( _poolBits ))
		{
			auto&	client			= _clientPool[idx];
			void*	storage			= _tempStorage.Ptr( idx * _maxMsgSize );
			bool	disconnected	= false;

			// receive message from client
			{
				// restore data which is not yet decoded
				if_unlikely( client.received > 0 )
				{
					MemCopy( OUT _received.storage.Ptr(), storage, client.received );
					_received.received	= client.received;
					client.received		= 0_b;
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

					client.received		= _received.received;
					_received.received	= 0_b;
				}
			}

			// send messages to client
			if_likely( not disconnected )
			{
				_toSend.encoded = client.encoded;
				_toSend.storage	= RVRef(client.encodedStorage);

				_SendMessages( client.socket, INOUT client.lastSentMsg, client.id, INOUT disconnected );

				stat.incompleteOutput += uint(client.lastSentMsg != Default);

				client.encoded			= _toSend.encoded;
				client.encodedStorage	= RVRef(_toSend.storage);
			}

			if_unlikely( disconnected )
				_Disconnect( idx );
		}
	}

/*
=================================================
	ServerAPI::Create
=================================================
*/
	RC<IChannel>  TcpServerChannel::ServerAPI::Create (RC<MessageFactory> mf, RC<IAllocator> alloc,
													   RC<IClientListener> listener,
													   ushort port, uint maxConnections, Bool reliable, StringView dbgName) __NE___
	{
		CHECK_ERR( mf );
		CHECK_ERR( alloc );
		CHECK_ERR( listener );

		RC<TcpServerChannel>	result	{new TcpServerChannel{ RVRef(mf), RVRef(listener), RVRef(alloc), reliable }};
		TcpSocket::Config		cfg;	cfg.maxConnections = maxConnections;

		CHECK_ERR( result->_IsValid() );

		DEBUG_ONLY(
			if ( dbgName.empty() ) dbgName = "TCP server";
			result->_socket.SetDebugName( String{dbgName} );
		)
		Unused( dbgName );

		CHECK_ERR( result->_socket.Listen( IpAddress::FromLocalPortTCP(port), cfg ));

		AE_LOG_DBG( "Started TCP server on port: "s << ToString(port) );
		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	TcpServerChannel::TcpServerChannel (RC<MessageFactory>	mf,
										RC<IClientListener>	listener,
										RC<IAllocator>		alloc,
										Bool				reliable) __NE___ :
		TcpChannel{ RVRef(mf), RVRef(alloc) },
		_reliable{ reliable },
		_listener{ RVRef(listener) }
	{
		_allocator->Reserve( NetConfig::ChannelStorageSize * (1 + _maxClients) + _maxMsgSize * _maxClients );

		for (usize i = 0; i < _clientPool.size(); ++i)
		{
			_clientPool[i].encodedStorage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
		}

		_tempStorage.Alloc( _maxMsgSize * _maxClients, DefaultAllocatorAlign, _allocator.get() );

		_received.storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
	}

/*
=================================================
	destructor
=================================================
*/
	TcpServerChannel::~TcpServerChannel () __NE___
	{
		for (uint idx : BitIndexIterate( _poolBits ))
		{
			_listener->OnClientDisconnected( c_TcpChannelType, _clientPool[idx].id );
		}

		for (usize i = 0; i < _clientPool.size(); ++i)
		{
			_clientPool[i].encodedStorage.Dealloc( _allocator.get() );
		}
		_received.storage.Dealloc( _allocator.get() );

		_tempStorage.Dealloc( _allocator.get() );
	}

/*
=================================================
	_IsValid
=================================================
*/
	bool  TcpServerChannel::_IsValid () C_NE___
	{
		return	TcpChannel::_IsValid()	and
				_tempStorage			and
				_received.storage		and
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

/*
=================================================
	DisconnectClientsWithIncompleteMsgQueue
=================================================
*/
	void  TcpServerChannel::DisconnectClientsWithIncompleteMsgQueue () __NE___
	{
		for (uint idx : BitIndexIterate( _poolBits ))
		{
			if_unlikely( _clientPool[idx].lastSentMsg != Default )
			{
				_Disconnect( idx );
			}
		}
	}

/*
=================================================
	DisconnectClient
=================================================
*/
	void  TcpServerChannel::_Disconnect (const uint idx) __NE___
	{
		auto&		client	= _clientPool[idx];
		const auto	id		= client.id;

		client.id		= Default;
		client.received	= 0_b;
		client.socket.FastClose();

		_poolBits.reset( idx );

		for (auto it = _clientAddrMap.begin(); it != _clientAddrMap.end(); ++it)
		{
			if ( it->second == idx )
			{
				_clientAddrMap.EraseByIter( it );
				break;
			}
		}

		ASSERT( _uniqueClientId.erase( id ));
		_listener->OnClientDisconnected( c_TcpChannelType, id );

		AE_LOG_DBG( "client ("s << ToString<16>(uint(id)) << ") disconnected" );
	}


} // AE::Networking
