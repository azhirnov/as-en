// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "networking/HighLevel/TcpStream2.h"

namespace AE::Networking
{
	using namespace AE::Threading;

/*
=================================================
	Send
=================================================
*/
	auto  TcpStream2::Send (const void* data, const Bytes dataSize) __NE___ -> InlCoro
	{
		for (Bytes offset; offset < dataSize;)
		{
			auto [err, sent] = _client.Send( data + offset, dataSize - offset );
			switch ( err )
			{
				case_likely SocketSendError::Sent :
					offset += sent;
					break;  // try again

				case SocketSendError::NotSent :
				case SocketSendError::ResourceTemporarilyUnavailable :
					Coro_Delay( milliseconds{20} );  // pause
					break;

				case SocketSendError::_Error :
				case SocketSendError::UDP_MessageTooLong :
					// unused

				case SocketSendError::NoSocket :
				case SocketSendError::NotConnected :
				case SocketSendError::ConnectionResetByPeer :
				case SocketSendError::UnknownError :
				case SocketSendError::PermissionDenied :
				default :
					_connectionLost = true;
					Coro_Error();
			}
		}
		co_return;
	}

/*
=================================================
	SyncSend
=================================================
*/
	bool  TcpStream2::SyncSend (const void* data, Bytes dataSize) __NE___
	{
		return SyncSend( data, dataSize, HighResClock::now() + SocketDependency::c_Timeout );
	}

	bool  TcpStream2::SyncSend (const void* data, const Bytes dataSize, const TimePoint_t endTime) __NE___
	{
		Bytes offset;
		for (; offset < dataSize and _syncSendLooping.load();)
		{
			auto [err, sent] = _client.Send( data + offset, dataSize - offset );
			if_likely( err == SocketSendError::Sent )
			{
				offset += sent;
				continue;
			}

			switch ( err )
			{
				case SocketSendError::Sent :	break;

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
					_connectionLost = true;
					return false;
			}

			if ( HighResClock::now() > endTime )
				return false;
		}

		return offset == dataSize;
	}

/*
=================================================
	ReadReceived
=================================================
*/
	Bytes  TcpStream2::ReadReceived (OUT void* data, Bytes size) __NE___
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
	auto  TcpStream2::ReadReceived (OUT void* data, const Bytes minSize, const Bytes maxSize, const TimePoint_t endTime) __NE___ -> InlPromise<Bytes>
	{
		CHECK_CE( minSize <= maxSize );

		Bytes readn;
		for (;;)
		{
			CHECK_CE( not _connectionLost );

			if ( _received > 0 )
			{
				readn += ReadReceived( data + readn, maxSize - readn );

				if_unlikely( readn >= minSize )
					break;
			}

			if ( not _Receive() )
				co_await Tuple{SocketDependency{ _client, endTime }};
		}
		co_return readn;
	}

/*
=================================================
	WaitForClient
=================================================
*/
	auto  TcpStream2::WaitForClient (const TimePoint_t endTime, const SourceLoc loc) __NE___ -> InlCoro
	{
		CHECK_CE( _server.IsOpen() );
		CHECK_CE( not _client.IsOpen() );

		AE_PRIVATE_LOG_I( "Waiting for client...", loc );

		for (;;)
		{
			IpAddress	addr;
			if ( _client.Accept( _server, OUT addr ))
			{
				AE_PRIVATE_LOG_I( "client connected "s << addr.ToString(), loc );
				co_return;
			}

			co_await Tuple{SocketDependency{ _server, endTime }};
		}
	}

/*
=================================================
	_Receive
=================================================
*/
	bool  TcpStream2::_Receive () __NE___
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
				_connectionLost = true;
				return false;
		}
	}

/*
=================================================
	Close
=================================================
*/
	void  TcpStream2::Close () __NE___
	{
		_client.FastClose();
		_server.FastClose();

		_sentBuffer.Dealloc( null );
		_recvBuffer.Dealloc( null );

		_isClosed.store( true );
	}

/*
=================================================
	_Init
=================================================
*/
	bool  TcpStream2::_Init (Bytes bufferSize, Ptr<Serializing::ObjectFactory> factory) __NE___
	{
		CHECK_ERR( factory );
		_factory = factory;

		CHECK_ERR( _sentBuffer.Alloc( bufferSize, DefaultAllocatorAlign, null ));
		CHECK_ERR( _recvBuffer.Alloc( bufferSize, DefaultAllocatorAlign, null ));

		_connectionLost = false;
		_isClosed.store( false );
		_syncSendLooping.store( true );
		return true;
	}

/*
=================================================
	_InitServer
=================================================
*/
	bool  TcpStream2::_InitServer (ushort port, Bytes bufferSize, Ptr<Serializing::ObjectFactory> factory) __NE___
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
	_InitClient
=================================================
*/
	auto  TcpStream2::_InitClient (IpAddress addr, Bytes bufferSize, Ptr<Serializing::ObjectFactory> factory,
								   const TimePoint_t endTime, const SourceLoc loc) __NE___ -> InlCoro
	{
		CHECK_CE( not _client.IsOpen() );

		TcpSocket::Config	cfg;
		cfg.noDelay				= true;
		cfg.nonBlocking			= true;
		cfg.receiveBufferSize	= bufferSize;

		AE_PRIVATE_LOG_I( "Waiting for server...", loc );

		for (;;)
		{
			if ( _client.Connect( addr ))
			{
				AE_PRIVATE_LOG_I( "connected to server "s << addr.ToString(), loc );
				break;
			}

			if ( HighResClock::now() > endTime )
			{
				AE_PRIVATE_LOG_W( "Failed to connect to server "s << addr.ToString() << ", time is out", loc );
				Coro_Error();
			}

			Coro_Delay( milliseconds{100} );
		}

		CHECK_CE( _Init( bufferSize, factory ));
	}

} // AE::Networking
