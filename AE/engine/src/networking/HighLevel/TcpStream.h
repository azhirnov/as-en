// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe: must be externally synchronized

	Designed to stream large messages over TCP connection.
*/

#pragma once

#include "networking/LowLevel/TcpSocket.h"
#include "pch/Serializing.h"

namespace AE::Networking
{

	//
	// TCP Stream
	//

	class TcpStream
	{
	// types
	public:
		enum class ConnectionLost {};


	// variables
	protected:
		Networking::TcpSocket			_server;
		Networking::TcpSocket			_client;
		DynUntypedStorage				_sentBuffer;
		DynUntypedStorage				_recvBuffer;
		Bytes							_received;
		Ptr<Serializing::ObjectFactory>	_factory;
		LinearAllocator<>				_allocator;

		bool							_recursion	= false;


	// methods
	public:
			void  Close ()												__NE___;

		// connect single client
		ND_ bool  WaitForClient ()										__NE___;
		ND_ bool  IsConnected ()										C_NE___	{ return _client.IsOpen(); }

		// send without buffering
		ND_	bool  Send (const void* data, Bytes dataSize)				__NE___;

		ND_ bool  Receive ()											__Th___;	// throw ConnectionLost

		ND_ Bytes ReadReceived (OUT void* data, Bytes size)				__NE___;
		ND_ Bytes ReadReceived (OUT void* data, Bytes minSize,
								Bytes maxSize, Atomic<bool> &looping)	__Th___;	// throw ConnectionLost

	protected:
		template <typename MsgType>
		ND_ RC<MsgType>  _Encode ()										__NE___;

		// serialize to temporary buffer and call 'Send()'
		template <typename MsgType>
		ND_ bool  _Send (const MsgType &)								__NE___;

		ND_ bool  _InitServer (ushort port,
							   Bytes bufferSize,
							   Ptr<Serializing::ObjectFactory> factory)	__NE___;

		ND_ bool  _InitClient (IpAddress addr,
							   Bytes bufferSize,
							   Ptr<Serializing::ObjectFactory> factory)	__NE___;

		// allow to connect multiple clients
		ND_ bool  _WaitForClient (const TcpStream &mainServer)			__NE___;

	private:
		ND_ bool  _Init (Bytes bufferSize,
						 Ptr<Serializing::ObjectFactory> factory)		__NE___;
	};


/*
=================================================
	_Send
=================================================
*/
	template <typename MsgType>
	bool  TcpStream::_Send (const MsgType &msg) __NE___
	{
		//AE_LOG_DBG( "Send "s << TypeIdOf(msg).Name() );

		if_unlikely( _recursion )
			return false;

		SCOPED_SET( _recursion, true, false );

		Bytes	size;
		{
			Serializing::Serializer  enc {FastWStream{ _sentBuffer.Data(), _sentBuffer.End() }};
			enc.factory = _factory;

			CHECK_ERR( enc( &msg ));

			size = _sentBuffer.Size() - enc.stream.RemainingSize();
		}

		return Send( _sentBuffer.Data(), size );
	}


/*
=================================================
	Encode
=================================================
*/
	template <typename MsgType>
	RC<MsgType>  TcpStream::_Encode () __NE___
	{
		if ( _received == 0 )
			return Default;

		_allocator.Discard();

		Serializing::Deserializer	dec {FastRStream{ _recvBuffer.Ptr(), _recvBuffer.Ptr( _received )}};
		dec.factory		= _factory;
		dec.allocator	= &_allocator;

		RC<MsgType>	msg;

		if ( not dec( OUT msg ))
			return Default;

		const Bytes	size = _received - dec.stream.RemainingSize();

		_received -= size;
		MemMove( OUT _recvBuffer.Ptr(), _recvBuffer.Ptr( size ), _received );	// TODO: optimzie

		//AE_LOG_DBG( "Encode "s << TypeIdOf(*msg).Name() );

		return msg;
	}


} // AE::Networking
