// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/UdpUnreliable.h"
#include "networking/HighLevel/Client.h"
#include "networking/HighLevel/Server.h"

namespace AE::Networking
{
namespace {
	static constexpr auto	c_UdpChannelType = EChannel::Unreliable;
}

/*
=================================================
	_PacketHeader
=================================================
*/
	struct UdpUnreliable::_PacketHeader
	{
		ushort			magic		= 0;
		ushort			packetId	= 0;
	};

/*
=================================================
	_MsgHeader
=================================================
*/
	struct UdpUnreliable::_MsgHeader
	{
		ushort			magic	= 0;
		Bytes16u		size;
		CSMessageUID	msgId	= CSMessageUID(0);
	};

/*
=================================================
	constructor
=================================================
*/
	UdpUnreliable::UdpUnreliable (RC<MessageFactory> mf, RC<IAllocator> alloc) __NE___ :
		_msgFactory{ RVRef(mf) },
		_allocator{ RVRef(alloc) }
	{
		//_input .storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
		//_output.storage.Alloc( NetConfig::ChannelStorageSize, DefaultAllocatorAlign, _allocator.get() );
	}

/*
=================================================
	destructor
=================================================
*/
	UdpUnreliable::~UdpUnreliable () __NE___
	{
		//_input .storage.Dealloc( _allocator.get() );
		//_output.storage.Dealloc( _allocator.get() );
	}

/*
=================================================
	_SendMessages
=================================================
*/
	template <typename Address>
	void  UdpUnreliable::_SendMessages (const Address &addr, QueueAndStorage &qs, INOUT bool &isDisconnected) C_NE___
	{
		if_unlikely( qs.queue.empty() )
			return;

		Bytes		encoded;		// offset in 'qs.storage' to the end of encoded messages
		Bytes		pending;		// offset in 'qs.storage' to begin of data which can be sent
		auto		last_failed_it	= qs.queue.end();

		const auto	SendData		= [this, &addr, &qs, &encoded, &pending, &isDisconnected] ()
		{{
			for (bool retry = true; retry and (encoded - pending > 0);)
			{
				ASSERT( encoded > pending );

						retry		= false;
				auto	[err, sent]	= _socket.Send( addr, qs.storage.Ptr( pending ), encoded - pending );

				switch ( err )
				{
					case_likely SocketSendError::Sent :
					{
						AE_LOGI( "Sent "s << ToString(sent) );
						ASSERT( sent > 0 );
						pending += sent;
						//retry	= pending < encoded;	// TODO ?
						break;
					}

					case SocketSendError::NotSent :
					case SocketSendError::ResourceTemporarilyUnavailable :
						break; // skip

					case SocketSendError::NoSocket :
					case SocketSendError::NotConnected :
					case SocketSendError::ConnectionResetByPeer :
					case SocketSendError::UnknownError :
						isDisconnected = true;
						break;
				}
			}
			encoded -= pending;
			pending  = 0_b;
		}};

		for (auto it = qs.queue.begin(); (it != qs.queue.end()) and (not isDisconnected);)
		{
			if_unlikely( encoded + sizeof(_MsgHeader) >= qs.storage.Size() )
				SendData();

			auto*			hdr_ptr		= qs.storage.Ptr( encoded );
			const Bytes		msg_off		= encoded + sizeof(_MsgHeader);
			DataEncoder		enc			{ qs.storage.Ptr( msg_off ), qs.storage.Size() - msg_off };
			auto			err			= (*it)->Serialize( enc );

			switch_enum( err )
			{
				case_likely CSMessage::EncodeError::OK :
				{
					encoded = qs.storage.Size() - enc.RemainingSize();

					_MsgHeader	header;
					header.magic = _magicByte;
					header.msgId = (*it)->UniqueId();
					header.size	 = encoded - msg_off;

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
					Reconstruct( INOUT enc, qs.storage.Ptr(), qs.storage.Size() );

					last_failed_it = it;
					break;
				}
			}
			switch_end
		}

		SendData();
		ASSERT( isDisconnected or encoded == pending );
	}

/*
=================================================
	_ReceiveMessages
=================================================
*
	template <typename Address>
	void  UdpUnreliable::_ReceiveMessages (const Address &addr, QueueAndStorage &qs, const FrameUID frameId, INOUT bool &isDisconnected) C_NE___
	{
	}

/*
=================================================
	_OnEncodingError / _OnDecodingError
=================================================
*/
	inline void  UdpUnreliable::_OnEncodingError (CSMessagePtr) C_NE___
	{
		// TODO ?
	}

	inline void  UdpUnreliable::_OnDecodingError (CSMessageUID) C_NE___
	{
		// TODO ?
	}

/*
=================================================
	Send
=================================================
*/
	void  UdpUnreliable::Send (MsgList_t msgList) __NE___
	{
		//_output.queue.insert( _output.queue.end(), msgs.begin(), msgs.end() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ProcessMessages
=================================================
*/
	void  UdpUnreliableClientChannel::ProcessMessages (const FrameUID frameId, INOUT MsgQueueStatistic &) __NE___
	{
		if_unlikely( not _socket.IsOpen() )
			return;

	//	_input.queue.clear();

		bool	disconnected = false;
	//	_ReceiveMessages( _serverAddress, _input, frameId, INOUT disconnected );
		_SendMessages( _serverAddress, _output, INOUT disconnected );

		_output.queue.clear();

		if ( disconnected )
			_Reconnect();
	}

/*
=================================================
	_Reconnect
=================================================
*/
	void  UdpUnreliableClientChannel::_Reconnect () __NE___
	{
		_serverProvider->GetAddress( c_UdpChannelType, _serverIndex, False{"UDP"}, OUT _serverAddress );

		++_serverIndex;
	}

/*
=================================================
	ClientAPI::Create
=================================================
*/
	RC<IChannel>  UdpUnreliableClientChannel::ClientAPI::Create (RC<MessageFactory> mf, RC<IAllocator> alloc,
																 RC<IServerProvider> serverProvider, ushort port) __NE___
	{
		CHECK_ERR( mf );
		CHECK_ERR( alloc );
		CHECK_ERR( serverProvider );

		RC<UdpUnreliableClientChannel>	result {new UdpUnreliableClientChannel{ RVRef(mf), RVRef(serverProvider), RVRef(alloc) }};

		DEBUG_ONLY( result->_socket.SetDebugName( "UDP client" );)
		CHECK_ERR( result->_socket.Open( IpAddress::FromLocalPortUDP(port) ));

		AE_LOGI( "Started UDP client on port: "s << ToString(port) );
		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	UdpUnreliableClientChannel::UdpUnreliableClientChannel (RC<MessageFactory>	mf,
															RC<IServerProvider>	serverProvider,
															RC<IAllocator>		alloc) __NE___ :
		UdpUnreliable{ RVRef(mf), RVRef(alloc) },
		_serverProvider{ RVRef(serverProvider) }
	{}

/*
=================================================
	destructor
=================================================
*/
	UdpUnreliableClientChannel::~UdpUnreliableClientChannel ()__NE___
	{}
//-----------------------------------------------------------------------------



/*
=================================================
	ProcessMessages
=================================================
*/
	void  UdpUnreliableServerChannel::ProcessMessages (const FrameUID frameId, INOUT MsgQueueStatistic &) __NE___
	{
		if_unlikely( not _socket.IsOpen() )
			return;

	//	_input.queue.clear();

	//	_ReceiveMessages( _input, frameId, INOUT disconnected );

	//	for (auto it = _clients.begin(); it != _clients.end();)
	//	{
	//		bool	disconnected = false;
	//		_SendMessages( it->first, _output, INOUT disconnected );
	//
	//		if_likely( not disconnected ){
	//			++it;
	//		}else{
	//			_listener->OnClientDisconnected( _channelId, it->first );
	//			it = _clients.EraseByIter( it );
	//		}
	//	}

		_output.queue.clear();
	}

/*
=================================================
	ServerAPI::Create
=================================================
*/
	RC<IChannel>  UdpUnreliableServerChannel::ServerAPI::Create (RC<MessageFactory> mf, RC<IAllocator> alloc,
																 RC<IClientListener> listener, ushort port) __NE___
	{
		CHECK_ERR( mf );
		CHECK_ERR( alloc );
		CHECK_ERR( listener );

		RC<UdpUnreliableServerChannel>	result{ new UdpUnreliableServerChannel{ RVRef(mf), RVRef(listener), RVRef(alloc) }};

		DEBUG_ONLY( result->_socket.SetDebugName( "UDP server" );)
		CHECK_ERR( result->_socket.Open( IpAddress::FromLocalPortUDP(port) ));

		AE_LOGI( "Started UDP server on port: "s << ToString(port) );
		return result;
	}

/*
=================================================
	constructor
=================================================
*/
	UdpUnreliableServerChannel::UdpUnreliableServerChannel (RC<MessageFactory>	mf,
															RC<IClientListener>	listener,
															RC<IAllocator>		alloc) __NE___ :
		UdpUnreliable{ RVRef(mf), RVRef(alloc) },
		_listener{ RVRef(listener) }
	{}

/*
=================================================
	destructor
=================================================
*/
	UdpUnreliableServerChannel::~UdpUnreliableServerChannel () __NE___
	{}


} // AE::Networking
