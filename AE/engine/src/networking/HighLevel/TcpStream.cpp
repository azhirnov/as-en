// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/HighLevel/TcpStream.h"

namespace AE::Networking
{
/*
=================================================
	_Init
=================================================
*/
	bool  TcpStream::_Init (Bytes bufferSize, Ptr<Serializing::ObjectFactory> factory) __NE___
	{
		CHECK_ERR( factory );
		_factory = factory;

		CHECK_ERR( _sentBuffer.Alloc( bufferSize, DefaultAllocatorAlign, null ));
		CHECK_ERR( _recvBuffer.Alloc( bufferSize, DefaultAllocatorAlign, null ));
		return true;
	}

/*
=================================================
	_InitServer
=================================================
*/
	bool  TcpStream::_InitServer (ushort port, Bytes bufferSize, Ptr<Serializing::ObjectFactory> factory) __NE___
	{
		CHECK_ERR( not _server.IsOpen() );

		TcpSocket::Config	cfg;
		cfg.nonBlocking			= true;
		cfg.noDelay				= true;
		cfg.reuseAddress		= false;
		cfg.receiveBufferSize	= bufferSize;
		cfg.maxConnections		= 1;

		CHECK_ERR( _server.Listen( IpAddress::FromLocalPortTCP( port ), cfg ));
		return _Init( bufferSize, factory );
	}

/*
=================================================
	WaitForClient
=================================================
*/
	bool  TcpStream::WaitForClient () __NE___
	{
		CHECK_ERR( _server.IsOpen() );
		CHECK_ERR( not _client.IsOpen() );

		AE_LOGI( "Waiting for client..." );

		for (uint i = 0; i < 10; ++i)
		{
			IpAddress	addr;
			if ( _client.Accept( _server, OUT addr ))
			{
				AE_LOGI( "connected "s << addr.ToString() );
				return true;
			}
			ThreadUtils::MilliSleep( seconds{1} );
		}
		return false;
	}

/*
=================================================
	_WaitForClient
=================================================
*/
	bool  TcpStream::_WaitForClient (const TcpStream &mainServer) __NE___
	{
		CHECK_ERR( not _server.IsOpen() );
		CHECK_ERR( not _client.IsOpen() );
		CHECK_ERR( mainServer._server.IsOpen() );

		AE_LOGI( "Waiting for client..." );

		for (uint i = 0; i < 10; ++i)
		{
			IpAddress	addr;
			if ( _client.Accept( mainServer._server, OUT addr ))
			{
				return _Init( mainServer._sentBuffer.Size(), mainServer._factory );
			}
			ThreadUtils::MilliSleep( seconds{1} );
		}
		return false;
	}

/*
=================================================
	_InitClient
=================================================
*/
	bool  TcpStream::_InitClient (IpAddress addr, Bytes bufferSize, Ptr<Serializing::ObjectFactory> factory) __NE___
	{
		CHECK_ERR( not _client.IsOpen() );

		TcpSocket::Config	cfg;
		cfg.noDelay				= true;
		cfg.nonBlocking			= true;
		cfg.receiveBufferSize	= bufferSize;

		AE_LOGI( "Waiting for server..." );
		for (;;)
		{
			if ( _client.Connect( addr ))
			{
				AE_LOGI( "connected to server "s << addr.ToString() );
				break;
			}
			ThreadUtils::MilliSleep( seconds{1} );
		}
		return _Init( bufferSize, factory );
	}

/*
=================================================
	Close
=================================================
*/
	void  TcpStream::Close () __NE___
	{
		_client.FastClose();
		_server.FastClose();

		_sentBuffer.Dealloc( null );
		_recvBuffer.Dealloc( null );
	}

/*
=================================================
	Send
=================================================
*/
	bool  TcpStream::Send (const void* data, const Bytes dataSize) __NE___
	{
		for (Bytes offset; offset < dataSize;)
		{
			auto [err, sent] = _client.Send( data + offset, dataSize - offset );
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
	Receive
=================================================
*/
	bool  TcpStream::Receive () __Th___
	{
		ASSERT( not _recvBuffer.Empty() );

		if ( _received >= _recvBuffer.Size() )
			return false;

		auto [err, recv] = _client.Receive( _recvBuffer.Ptr( _received ), _recvBuffer.Size() - _received );
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
	ReadReceived
=================================================
*/
	Bytes  TcpStream::ReadReceived (OUT void* data, Bytes size) __NE___
	{
		size = Min( size, _received );
		MemCopy( OUT data, _recvBuffer.Ptr(), size );

		_received -= size;
		MemMove( OUT _recvBuffer.Ptr(), _recvBuffer.Ptr( size ), _received );	// TODO: optimize

		return size;
	}

/*
=================================================
	ReadReceived
=================================================
*/
	Bytes  TcpStream::ReadReceived (OUT void* data, const Bytes minSize, const Bytes maxSize, Atomic<bool> &looping) __Th___
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

			if ( not Receive() )  // throw
				ThreadUtils::Sleep_1us();
		}
		return readn;
	}

} // AE::Networking
