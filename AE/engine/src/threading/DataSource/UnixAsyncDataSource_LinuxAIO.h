// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Multithreading:
	  using Linux AIO context between threads may decrease performance, use context per thread.

	[docs](https://man7.org/linux/man-pages/man2/io_setup.2.html)

	problems:
	- [asynchronous appending write may silently be synchronous](https://www.scylladb.com/2016/02/09/qualifying-filesystems/)
	- [asyncIO may block in many cases](https://lwn.net/Articles/724198/)
*/

#include <linux/unistd.h>
#include <linux/aio_abi.h>

namespace
{
	ND_ inline int  io_setup (unsigned nr, OUT aio_context_t *ctxp) {
		return syscall( __NR_io_setup, nr, ctxp );
	}

	ND_ inline int  io_destroy (aio_context_t ctx) {
		return syscall( __NR_io_destroy, ctx );
	}

	ND_ inline int  io_cancel (aio_context_t ctx, struct iocb *iocb, OUT struct io_event *result) {
		return syscall( __NR_io_cancel, ctx, iocb, OUT result );
	}

	ND_ inline int  io_submit (aio_context_t ctx, long nr, struct iocb **iocbpp) {
		return syscall( __NR_io_submit, ctx, nr, iocbpp);
	}

	ND_ inline int  io_getevents (aio_context_t ctx, long min_nr, long max_nr, struct io_event *events, struct timespec *timeout) {
		return syscall( __NR_io_getevents, ctx, min_nr, max_nr, events, timeout );
	}

	struct aio_ring
	{
		unsigned	id;				// kernel internal index number
		unsigned	nr;				// number of io_events
		unsigned	head;
		unsigned	tail;

		unsigned	magic;
		unsigned	compat_features;
		unsigned	incompat_features;
		unsigned	header_length;	// size of aio_ring

		io_event	events [0];
	};
	static constexpr unsigned	AIO_RING_MAGIC = 0xa10a10a1;

/*
=================================================
	io_getevents_nonblock
=================================================
*/
	ND_ inline int  io_getevents_nonblock (aio_context_t ctx, const long max_nr, OUT struct io_event *events)
	{
		using namespace AE::Base;

		aio_ring*	ring = BitCast< aio_ring *>(ctx);
		if_unlikely( ring == null or ring->magic != AIO_RING_MAGIC )
		{
			struct timespec	timeout = {};
			return io_getevents( ctx, 0, max_nr, OUT events, &timeout );
		}

		// user space
		int i = 0;
		for (; i < max_nr;)
		{
			unsigned	head = ring->head;

			if ( head == ring->tail )
				// There are no more completions
				break;

			// There is another completion to reap
			events[i] = ring->events[head];
			MemoryBarrier( EMemoryOrder::Acquire );

			ring->head = (head + 1) % ring->nr;
			++i;
		}
		return i;
	}

/*
=================================================
	IntToPtr
=================================================
*/
	template <typename R, typename T>
	ND_ forceinline R  IntToPtr (T u) __NE___
	{
		using namespace AE::Base;
		StaticAssert( IsInteger< T >);
		StaticAssert( IsPointer< R >);
		StaticAssert( sizeof(R) <= sizeof(T) );

		if constexpr( sizeof(T) == sizeof(R) )
			return BitCast<R>( u );
		else
			return UnsafeBitCast<R>( u );
	}

/*
=================================================
	PtrToInt
=================================================
*/
	template <typename R, typename T>
	ND_ forceinline R  PtrToInt (T* ptr) __NE___
	{
		using namespace AE::Base;
		StaticAssert( IsInteger< R > );
		StaticAssert( sizeof(T*) <= sizeof(R) );

		if constexpr( sizeof(T*) == sizeof(R) )
			return BitCast<R>( ptr );
		else
			return UnsafeBitCast<R>( ptr );
	}

} // namespace


#include "threading/DataSource/UnixAsyncDataSource.h"

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	UnixIOService::_RequestBase::_RequestBase () __NE___
	{
		StaticAssert( sizeof(_aioCb) == sizeof(iocb) );
		StaticAssert( sizeof(LinuxAIO_iocb) == sizeof(iocb) );
		StaticAssert( sizeof(LinuxAIO_aio_context_t) == sizeof(aio_context_t) );
	}

/*
=================================================
	_Cancel
=================================================
*/
	bool  UnixIOService::_RequestBase::_Cancel () __NE___
	{
		auto*	ctx_per_thread = Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetLinuxAIOContext( _queueIndex );

		if ( ctx_per_thread != null )
		{
			EXLOCK( ctx_per_thread->guard );

			auto		ctx		= BitCast< aio_context_t >( ctx_per_thread->ctx );
			io_event	event	= {};

			if ( ::io_cancel( ctx, _aioCb.Ptr< iocb >(), OUT &event ) == 0 )
			{
				_Complete( 0_b, false );
				return true;
			}
		}
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Create
=================================================
*/
	bool  UnixIOService::ReadRequest::_Create (RC<UnixAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
	{
		// initialize
		_Init( RVRef(mem) );
		_dataSource	= RVRef(file);
		_offset		= pos;

		// read data
		auto&		cb		= _aioCb.Ref< iocb >();
		ZeroMem( OUT cb );

		cb.aio_fildes		= _dataSource->Handle();
		cb.aio_lio_opcode	= IOCB_CMD_PREAD;
		cb.aio_buf			= PtrToInt<__u64>( data );
		cb.aio_offset		= slong{pos};		// TODO: may be mutable?
		cb.aio_nbytes		= ulong{dataSize};

		auto&	ctx_per_thread	= Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetLinuxAIOContext();
		auto	ctx				= BitCast< aio_context_t >( ctx_per_thread.ctx );

		iocb*	cbarr [1]	= {&cb};
		int		ret			= io_submit( ctx, 1, cbarr );

		ctx_per_thread.guard.unlock();

		if_unlikely( ret != 1 )
		{
			UNIX_CHECK_DEV( "io_submit() failed: " );
			return false;
		}
		return true;
	}

/*
=================================================
	GetResult
=================================================
*/
	UnixIOService::ReadRequest::Result  UnixIOService::ReadRequest::GetResult () C_NE___
	{
		ASSERT( IsFinished() );

		auto&	cb		= _aioCb.Ref< iocb >();

		Result	res;
		res.dataSize	= _actualSize.load();
		res.pos			= _offset;
		res.data		= IsCompleted() ? IntToPtr<void*>(cb.aio_buf) : null;

		return res;
	}

/*
=================================================
	_GetResult
=================================================
*/
	UnixIOService::ReadRequest::ResultWithRC  UnixIOService::ReadRequest::_GetResult () __NE___
	{
		ASSERT( IsFinished() );

		auto&	cb		= _aioCb.Ref< iocb >();

		ResultWithRC	res;
		res.dataSize	= _actualSize.load();
		res.rc			= _memRC;
		res.pos			= _offset;
		res.data		= IsCompleted() ? IntToPtr<void*>(cb.aio_buf) : null;

		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Create
=================================================
*/
	bool  UnixIOService::WriteRequest::_Create (RC<UnixAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
	{
		// initialize
		_Init( RVRef(mem) );
		_dataSource	= RVRef(file);
		_offset		= pos;

		// write data
		auto&		cb		= _aioCb.Ref< iocb >();
		ZeroMem( OUT cb );

		cb.aio_fildes		= _dataSource->Handle();
		cb.aio_lio_opcode	= IOCB_CMD_PWRITE;
		cb.aio_buf			= PtrToInt<__u64>(data);
		cb.aio_offset		= slong{pos};		// TODO: may be mutable?
		cb.aio_nbytes		= ulong{dataSize};

		auto&	ctx_per_thread	= Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetLinuxAIOContext();
		auto	ctx				= BitCast< aio_context_t >( ctx_per_thread.ctx );

		iocb*	cbarr [1]	= {&cb};
		int		ret			= io_submit( ctx, 1, cbarr );

		ctx_per_thread.guard.unlock();

		if_unlikely( ret != 1 )
		{
			UNIX_CHECK_DEV( "io_submit() failed: " );
			return false;
		}
		return true;
	}

/*
=================================================
	GetResult
=================================================
*/
	UnixIOService::WriteRequest::Result  UnixIOService::WriteRequest::GetResult () C_NE___
	{
		ASSERT( IsFinished() );

		Result	res;
		res.dataSize	= _actualSize.load();
		res.data		= null;
		res.pos			= _offset;

		return res;
	}

/*
=================================================
	_GetResult
=================================================
*/
	UnixIOService::WriteRequest::ResultWithRC  UnixIOService::WriteRequest::_GetResult () __NE___
	{
		ASSERT( IsFinished() );

		ResultWithRC	res;
		res.dataSize	= _actualSize.load();
		res.pos			= _offset;

		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  UnixAsyncRDataSource::CancelAllRequests () __NE___
	{
		ASSERT( IsOpen() );
		DBG_WARNING( "not supported" );
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  UnixAsyncWDataSource::CancelAllRequests () __NE___
	{
		ASSERT( IsOpen() );
		DBG_WARNING( "not supported" );
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
----
	warning: Scheduler().GetFileIOService() is not valid here
=================================================
*/
	UnixIOService::UnixIOService (uint maxAccessThreads) __NE___
	{
		maxAccessThreads = Min( maxAccessThreads, _aioContextArr.size() );

		usize	max_requests = (ReadRequestPool_t::capacity() + WriteRequestPool_t::capacity()) / 2;
		max_requests = Min( max_requests, 32u<<10 );

		for (uint i = 0; i < maxAccessThreads; ++i)
		{
			for (; max_requests > _LinuxAIO_MinRequests;)
			{
				if ( ::io_setup( max_requests, OUT &RefCast<aio_context_t>(_aioContextArr[i].ctx) ) != 0 )
				{
					int	err = UnixUtils::GetErrorCode();
					if ( err == EAGAIN )
					{
						max_requests /= 2;
						continue;
					}

					UNIX_CHECK_DEV2( err, "io_setup() failed: " );
				}
				break;
			}

			if ( _aioContextArr[i].ctx == 0 )
				break;
		}
	}

/*
=================================================
	destructor
=================================================
*/
	UnixIOService::~UnixIOService () __NE___
	{
		for (auto& per_thread : _aioContextArr)
		{
			if ( per_thread.ctx != 0 )
				Unused( ::io_destroy( RefCast<aio_context_t>( per_thread.ctx )));
		}
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  UnixIOService::IsInitialized () C_NE___
	{
		return _aioContextArr[0].ctx != 0;
	}

/*
=================================================
	ProcessEvents
=================================================
*/
	usize  UnixIOService::ProcessEvents () __NE___
	{
		if_unlikely( not IsInitialized() )
			return 0;

		ASSERT( Scheduler().GetFileIOService() == this );

		io_event	events [64];
		usize		num_events = 0;

		for (auto& item : _aioContextArr)
		{
			if_unlikely( item.ctx == 0 )
				continue;

			DeferExLock  lock {item.guard};
			if ( not lock.try_lock() )
				continue;

			const int	cnt = io_getevents_nonblock( item.ctx, CountOf(events), OUT events );
			if ( cnt == 0 )
				continue;

			if_unlikely( cnt < 0 )
			{
				UNIX_CHECK_DEV( "io_getevents() failed: " );
				continue;
			}

			for_likely (int i = 0; i < cnt; ++i)
			{
				auto*			cb	= IntToPtr< iocb *>( events[i].obj );
				_RequestBase*	res = Cast<_RequestBase>( cb - Bytes{_LinuxAIO_CBOffset});

				res->_Complete( Bytes{ulong(Max( events[i].res, 0 ))}, (events[i].res >= 0) );
			}
			num_events += cnt;
		}
		return num_events;
	}

/*
=================================================
	_GetLinuxAIOContext
=================================================
*/
	UnixIOService::LinuxAIO_CtxPerThread&  UnixIOService::_GetLinuxAIOContext () __NE___
	{
		for (;;)
		{
			for (auto& item : _aioContextArr)
			{
				if_unlikely( item.ctx == 0 )
					continue;

				if ( not item.guard.try_lock() )
					continue;

				return item;
			}
		}
	}


} // AE::Threading
