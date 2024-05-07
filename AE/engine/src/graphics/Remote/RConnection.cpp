// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Remote/RConnection.h"

namespace AE::RemoteGraphics
{
	using namespace AE::Networking;

	static constexpr Bytes	c_BufferSize	= 4_Mb;

/*
=================================================
	_Init
=================================================
*/
	bool  RConnection::_Init ()
	{
		CHECK_ERR( _sentBuffer.Alloc( c_BufferSize, DefaultAllocatorAlign, null ));
		CHECK_ERR( _recvBuffer.Alloc( c_BufferSize, DefaultAllocatorAlign, null ));
		return true;
	}

/*
=================================================
	InitServer
=================================================
*/
	bool  RConnection::InitServer (ushort port, Ptr<Serializing::ObjectFactory> factory)
	{
		TcpSocket::Config	cfg;
		cfg.noDelay				= true;
		cfg.nonBlocking			= true;
		cfg.reuseAddress		= false;
		cfg.receiveBufferSize	= c_BufferSize;
		cfg.maxConnections		= 1;

		_factory = factory;
		CHECK_ERR( _server.Listen( IpAddress::FromLocalPortTCP( port ), cfg ));
		CHECK_ERR( _Init() );
		return true;
	}

/*
=================================================
	WaitForClient
=================================================
*/
	bool  RConnection::WaitForClient ()
	{
		CHECK_ERR( not _socket.IsOpen() );

		for (uint i = 0; i < 10; ++i)
		{
			IpAddress	addr;
			if ( _socket.Accept( _server, OUT addr ))
			{
				//AE_LOG_DBG( "RmG server: connected client "s << addr.ToString() );
				_socket.KeepAlive();
				return true;
			}
			ThreadUtils::Sleep_15ms();
		}
		return false;
	}

/*
=================================================
	InitClient
=================================================
*/
	bool  RConnection::InitClient (IpAddress addr, Ptr<Serializing::ObjectFactory> factory)
	{
		TcpSocket::Config	cfg;
		cfg.noDelay				= true;
		cfg.nonBlocking			= true;
		cfg.receiveBufferSize	= c_BufferSize;

		_factory = factory;
		CHECK_ERR( _socket.Connect( addr ));
		return _Init();
	}

/*
=================================================
	Close
=================================================
*/
	void  RConnection::Close ()
	{
		_socket.FastClose();
		_server.FastClose();

		_sentBuffer.Dealloc( null );
		_recvBuffer.Dealloc( null );
	}

/*
=================================================
	Send
=================================================
*/
	bool  RConnection::Send (const void *data, const Bytes dataSize)
	{
		for (Bytes offset; offset < dataSize;)
		{
			auto [err, sent] = _socket.Send( data + offset, dataSize - offset );
			switch ( err )
			{
				case_likely SocketSendError::Sent :
					offset += sent;				break;

				case SocketSendError::NotSent :
				case SocketSendError::ResourceTemporarilyUnavailable :
					ThreadUtils::Sleep_1us();	break;

				case SocketSendError::_Error :
				case SocketSendError::UDP_MessageTooLong :
					// unused

				case SocketSendError::NoSocket :
				case SocketSendError::NotConnected :
				case SocketSendError::ConnectionResetByPeer :
				case SocketSendError::UnknownError :
				case SocketSendError::PermissionDenied :
				default :
					return false;
			}
		}

		return true;
	}

/*
=================================================
	Send
=================================================
*/
	bool  RConnection::Send (const Msg::BaseMsg &msg)
	{
		//AE_LOG_DBG( "Send "s << TypeIdOf(msg).Name() );

		if_unlikely( _recursion )
			return false;

		SCOPED_SET( _recursion, true, false );

		Bytes	size;
		{
			Serializing::Serializer  enc {FastWStream{ _sentBuffer.Data(), _sentBuffer.End() }};
			enc.factory = _factory;

			CHECK_ERR( enc( &msg ) and enc.Flush() );

			size = _sentBuffer.Size() - enc.stream.RemainingSize();
		}

		bool	ok = Send( _sentBuffer.Data(), size );

		if_unlikely( msg.GetTypeId() == TypeIdOf<Msg::UploadData>() )
			ok = ok and _SendUploadData( RefCast<Msg::UploadData>( msg ));

		if_unlikely( msg.GetTypeId() == TypeIdOf<Msg::UploadDataAndCopy>() )
			ok = ok and _SendUploadData( RefCast<Msg::UploadDataAndCopy>( msg ));

		return ok;
	}

/*
=================================================
	_SendUploadData
=================================================
*/
	bool  RConnection::_SendUploadData (const Msg::UploadData &msg)
	{
		ASSERT( msg.size > 0 );

		if ( msg.stream )
		{
			for (Bytes total; total < msg.size;)
			{
				Bytes	readn = msg.stream->ReadSeq( _sentBuffer.Data(), Min( msg.size - total, _sentBuffer.Size() ));
				if_unlikely( readn == 0 )
					break;

				total += readn;
				CHECK_ERR( Send( _sentBuffer.Data(), readn ));
				ASSERT( total <= msg.size );
			}
			return true;
		}

		if ( msg.data != null )
		{
			return Send( msg.data, msg.size );
		}

		DBG_WARNING( "data not uploaded" );
		return false;
	}

	bool  RConnection::_SendUploadData (const Msg::UploadDataAndCopy &msg)
	{
		ASSERT( msg.size > 0 );
		ASSERT( msg.data != null );

		return Send( msg.data, msg.size );
	}

/*
=================================================
	Receive
=================================================
*/
	bool  RConnection::Receive () __Th___
	{
		if ( _received >= _recvBuffer.Size() )
			return false;

		auto [err, recv] = _socket.Receive( _recvBuffer.Ptr( _received ), _recvBuffer.Size() - _received );
		switch ( err )
		{
			case_likely SocketReceiveError::Received :
				_received += recv;
				return true;

			case SocketReceiveError::NotReceived :
			case SocketReceiveError::ResourceTemporarilyUnavailable :
				return false;  // skip

			case SocketReceiveError::_Error :
				// unused

			case SocketReceiveError::ConnectionResetByPeer :
			case SocketReceiveError::ConnectionRefused :
			case SocketReceiveError::NotConnected :
			case SocketReceiveError::NoSocket :
			case SocketReceiveError::UnknownError :
			default :
				throw ConnectionLost(0);
		}
	}

/*
=================================================
	Encode
=================================================
*/
	auto  RConnection::Encode () -> RC<Msg::BaseMsg>
	{
		if ( _received == 0 )
			return Default;

		_allocator.Discard();

		Msg::Deserializer	dec {FastRStream{ _recvBuffer.Ptr(), _recvBuffer.Ptr( _received )}};
		dec.factory		= _factory;
		dec.allocator	= &_allocator;

		RC<Msg::BaseMsg>	msg;

		if ( not dec( OUT msg ))
			return Default;

		const Bytes	size = _received - dec.stream.RemainingSize();

		_received -= size;
		MemMove( OUT _recvBuffer.Ptr(), _recvBuffer.Ptr( size ), _received );

		//AE_LOG_DBG( "Encode "s << TypeIdOf(*msg).Name() );

		return msg;
	}

/*
=================================================
	ReadReceived
=================================================
*/
	Bytes  RConnection::ReadReceived (OUT void* data, Bytes size)
	{
		size = Min( size, _received );
		MemCopy( OUT data, _recvBuffer.Ptr(), size );

		_received -= size;
		MemMove( OUT _recvBuffer.Ptr(), _recvBuffer.Ptr( size ), _received );

		return size;
	}

/*
=================================================
	ReadReceived
=================================================
*/
	Bytes  RConnection::ReadReceived (OUT void* data, const Bytes minSize, const Bytes maxSize, Atomic<bool> &looping) __Th___
	{
		CHECK_ERR( minSize <= maxSize );

		Bytes readn;
		for (; looping.load();)
		{
			if ( _received > 0 )
			{
				readn += ReadReceived( data + readn, maxSize - readn );

				if_unlikely( readn >= minSize )
					return readn;
			}

			if ( not Receive() )
				ThreadUtils::Sleep_1us();
		}
		return readn;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Initialize
=================================================
*/
	bool  RConnectionArray::Initialize (IpAddress addr)
	{
		if ( _initialized )
			return true;

		_initialized = true;
		_bits.Store( Bitfield<uint>{ ToBitMask<uint>( RmNetConfig::socketCount )});

		ushort	port = RmNetConfig::serverPort;

		for (auto& item : _arr)
		{
			addr.SetPort( port++ );
			CHECK_ERR( item.InitClient( addr, &_objFactory ));
		}
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	bool  RConnectionArray::Deinitialize ()
	{
		for (auto& item : _arr) {
			item.Close();
		}
		_initialized = false;
		return true;
	}

/*
=================================================
	Lock
=================================================
*/
	uint  RConnectionArray::Lock ()
	{
		for (uint i = 0;; ++i)
		{
			if ( uint bit = TryLock();  bit != 0 )
				return bit;

			ThreadUtils::ProgressiveSleep( i );
		}
	}


} // AE::RemoteGraphics
