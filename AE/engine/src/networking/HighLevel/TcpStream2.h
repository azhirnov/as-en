// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe: must be externally synchronized
	Use single coroutine with send/receive loop to avoid sync issues.

	Designed to stream large messages over TCP connection.
	Non-blocking version.

	Async operations can be cancelled by 'Scheduler().Cancel(...)' call.

	'co_await TcpStream2::Send(...)' will cancel current coroutine,
	use Coro_WaitResult instead to check for errors without canceling current coroutine.
*/

#pragma once

#include "networking/LowLevel/TcpSocket.h"
#include "networking/LowLevel/SocketDependency.h"
#include "pch/Serializing.h"

namespace AE::Networking
{

	//
	// TCP Stream v2
	//

	class TcpStream2
	{
	// types
	public:
		template <typename Msg> struct ReceiveAndEncodeAwaiter;

		template <typename T>
		using InlPromise	= Threading::InlinePromise< T, Threading::ETaskQueue::Background >;
		using InlCoro		= Threading::InlineCoro< Threading::ETaskQueue::Background >;
		using TimePoint_t	= HighResClock::time_point;


	// variables
	protected:
		TcpSocket						_server;
		TcpSocket						_client;

		DynUntypedStorage				_sentBuffer;
		DynUntypedStorage				_recvBuffer;
		Bytes							_received;
		Ptr<Serializing::ObjectFactory>	_factory;

		bool							_recursion			= false;
		bool							_connectionLost		= false;
		Atomic<bool>					_isClosed			{false};
		Atomic<bool>					_syncSendLooping	{true};

		LinearAllocator<>				_allocator;


	// methods
	public:
			void  Close ()															__NE___;

		// Thread-safe:  yes
		ND_ bool  IsClosed ()														C_NE___	{ return _isClosed.load(); }

		ND_ bool  IsConnectionLost ()												C_NE___	{ return _connectionLost; }

		// connect single client
		ND_ auto  WaitForClient (TimePoint_t endTime,
								 const SourceLoc loc = SourceLoc::current())		__NE___ -> InlCoro;
		ND_ bool  IsConnected ()													C_NE___	{ return _client.IsOpen(); }

		// send without buffering
		ND_	auto  Send (const void* data, Bytes dataSize)							__NE___ -> InlCoro;		// TODO: unsafe
		ND_ bool  SyncSend (const void* data, Bytes dataSize)						__NE___;
		ND_ bool  SyncSend (const void* data, Bytes dataSize,
							TimePoint_t endTime)									__NE___;

		// Will cancel all 'SyncSend()' calls, until 'EnableSyncSend()' is called.
		// Thread-safe:  yes
			void  CancelSyncSend ()													__NE___	{ _syncSendLooping.store( false ); }
			void  EnableSyncSend ()													__NE___	{ _syncSendLooping.store( true ); }

		ND_ auto  ReadReceived (OUT void* data, Bytes minSize, Bytes maxSize,
								TimePoint_t endTime)								__NE___ -> InlPromise<Bytes>;	// TODO: unsafe

		// read without pause
		ND_ Bytes  ReadReceived (OUT void* data, Bytes size)						__NE___;

		ND_ TcpSocket const&	ClientSocket ()										C_NE___	{ return _client; }


	protected:
		template <typename MsgType>
		ND_ auto  _ReceiveAndEncode ()												__NE___;

		template <typename MsgType>
		ND_ auto  _ReceiveAndEncode (TimePoint_t endTime)							__NE___;

		template <typename MsgType>
		ND_ auto  _ReceiveAndEncodeOrCancel (TimePoint_t endTime)					__NE___;

		template <typename MsgType>
		ND_ auto  _SyncReceiveAndEncode ()											__NE___ -> RC<MsgType>;

		// serialize to temporary buffer and call 'Send()'
		template <typename MsgType>
		ND_ auto  _Send (const MsgType &)											__NE___ -> Threading::AsyncTask;

		template <typename MsgType>
		ND_ bool  _SyncSend (const MsgType &)										__NE___;

		ND_ bool  _InitServer (ushort port,
							   Bytes bufferSize,
							   Ptr<Serializing::ObjectFactory> factory)				__NE___;

		ND_ auto  _InitClient (IpAddress addr,
							   Bytes bufferSize,
							   Ptr<Serializing::ObjectFactory> factory,
							   TimePoint_t endTime,
							   const SourceLoc loc = SourceLoc::current())		__NE___ -> InlCoro;

	private:
		ND_ bool  _Init (Bytes bufferSize,
						 Ptr<Serializing::ObjectFactory> factory)					__NE___;

		ND_ bool   _Receive ()														__NE___;

		template <typename MsgType>
		ND_ RC<MsgType>  _Encode ()													__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	ReceiveAndEncodeAwaiter
=================================================
*/
	template <typename MsgType>
	struct TcpStream2::ReceiveAndEncodeAwaiter
	{
	private:
		TcpStream2 &	_self;
		RC<MsgType>		_msg;
		TimePoint_t		_endTime;
		bool			_result		= false;
		bool			_isStrong	= false;

	public:
		ReceiveAndEncodeAwaiter (TcpStream2 &s)										__NE___ : _self{s}, _endTime{HighResClock::now() + seconds{1}}, _isStrong{false} {}
		ReceiveAndEncodeAwaiter (TcpStream2 &s, TimePoint_t endTime, Bool isStrong)	__NE___ : _self{s}, _endTime{endTime}, _isStrong{isStrong} {}

		NdCx__ bool  await_ready ()													C_NE___	{ return false; }
		NdCx__ auto  await_resume ()												C_NE___	{ return _msg; }		// return result of 'co_await'

		template <typename P>
		Nd____ bool  await_suspend (std::coroutine_handle<P> curCoro,
									const SourceLoc	&loc = SourceLoc::current())	__NE___;
	};

	template <typename MsgType>
	auto  TcpStream2::_ReceiveAndEncode ()											__NE___	{ return ReceiveAndEncodeAwaiter<MsgType>{ *this }; }

	template <typename MsgType>
	auto  TcpStream2::_ReceiveAndEncode (TimePoint_t endTime)						__NE___	{ return ReceiveAndEncodeAwaiter<MsgType>{ *this, endTime, False{"weak"} }; }

	template <typename MsgType>
	auto  TcpStream2::_ReceiveAndEncodeOrCancel (TimePoint_t endTime)				__NE___	{ return ReceiveAndEncodeAwaiter<MsgType>{ *this, endTime, True{"strong"} }; }

/*
=================================================
	ReceiveAndEncodeAwaiter::await_suspend
=================================================
*/
	template <typename MsgType>
	template <typename P>
	bool  TcpStream2::ReceiveAndEncodeAwaiter<MsgType>::await_suspend (std::coroutine_handle<P> curCoro, const SourceLoc &loc) __NE___
	{
		_msg = _self._Encode<MsgType>();
		if ( _msg )
			return false;  // resume

		if ( _self._Receive() )
		{
			_msg = _self._Encode<MsgType>();
			if ( _msg )
				return false;  // resume
		}

		if_unlikely( _self._connectionLost )
		{
			Scheduler().Cancel( curCoro.promise(), True{} );
			return false;  // resume
		}

		return _Coro_::CoroAwaiterImpl::AwaitSuspendImpl2( curCoro.promise(), Tuple{SocketDependency{ _self._client, _endTime, loc }}, Bool{_isStrong} );  // suspend
	}

/*
=================================================
	_SyncReceiveAndEncode
----
	receive and encode without suspending,
	can be used outside of coroutine
=================================================
*/
	template <typename MsgType>
	auto  TcpStream2::_SyncReceiveAndEncode () __NE___ -> RC<MsgType>
	{
		Unused( _Receive() );
		return _Encode<MsgType>();
	}

/*
=================================================
	_Send
=================================================
*/
	template <typename MsgType>
	auto  TcpStream2::_Send (const MsgType &msg) __NE___ -> Threading::AsyncTask
	{
		using namespace AE::ImportCoroutines;

		//AE_LOG_DBG( "Send "s << TypeIdOf(msg).Name() );

		CHECK_ERR( not _recursion );
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
	_SyncSend
=================================================
*/
	template <typename MsgType>
	bool  TcpStream2::_SyncSend (const MsgType &msg) __NE___
	{
		//AE_LOG_DBG( "Send "s << TypeIdOf(msg).Name() );

		CHECK_ERR( not _recursion );
		SCOPED_SET( _recursion, true, false );

		Bytes	size;
		{
			Serializing::Serializer  enc {FastWStream{ _sentBuffer.Data(), _sentBuffer.End() }};
			enc.factory = _factory;

			CHECK_ERR( enc( &msg ));

			size = _sentBuffer.Size() - enc.stream.RemainingSize();
		}

		return SyncSend( _sentBuffer.Data(), size );
	}

/*
=================================================
	_Encode
=================================================
*/
	template <typename MsgType>
	RC<MsgType>  TcpStream2::_Encode () __NE___
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
