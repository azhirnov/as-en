// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	POISX AIO (only for BSD-like OS)

	Details:
		- Some Linux implementations has internal thread pool (https://man7.org/linux/man-pages/man3/aio_init.3.html)

	Multithreading:
	  ???

	[docs](https://man7.org/linux/man-pages/man7/aio.7.html)
	[AIO + kqueue](https://habr.com/ru/articles/600123/#asynchronous-file-io-with-kqueue) - for FreeBSD, not supported on MacOS
*/

#include <aio.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#include "threading/DataSource/UnixAsyncDataSource.h"

#ifndef AE_PLATFORM_BSD
# error Only for BSD-like OS, Linux and MacOS are not supported
#endif

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	UnixIOService::_RequestBase::_RequestBase () __NE___
	{
		StaticAssert( sizeof(aiocb) == sizeof(_aioCb) );
		StaticAssert( alignof(aiocb) == alignof(decltype(_aioCb)) );
	}

/*
=================================================
	_Cancel
=================================================
*/
	bool  UnixIOService::_RequestBase::_Cancel () __NE___
	{
		auto&	cb = _aioCb.Ref< aiocb >();
		if ( cb.aio_fildes >= 0 )
		{
			int ret = ::aio_cancel( cb.aio_fildes, &cb );
			switch ( ret )
			{
				case_likely AIO_CANCELED :	return true;
				case_likely AIO_ALLDONE :	return false;
				case AIO_NOTCANCELED :		break;	// TODO: aio_error
				case -1 :					UNIX_CHECK_DEV( "aio_cancel() failed: " );	break;
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
		auto&	cb = _aioCb.Ref< aiocb >();
		ZeroMem( OUT cb );

		cb.aio_fildes	= _dataSource->Handle();
		cb.aio_offset	= slong{pos};	// mutable
		cb.aio_buf		= data;
		cb.aio_nbytes	= ulong{dataSize};

		auto		kq	= Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetKQueue();
		ASSERT( queue >= 0 );

		cb.aio_sigevent.sigev_notify_kqueue			= kq;
		cb.aio_sigevent.sigev_notify				= SIGEV_KEVENT;
		cb.aio_sigevent.sigev_notify_kevent_flags	= EV_CLEAR;
		cb.aio_sigevent.sigev_value.sigval_ptr		= &obj;

		int ret = ::aio_read( &cb );
		if_unlikely( ret != 0 )
		{
			UNIX_CHECK_DEV( "aio_read() failed: " );
			return false;
		}

		// may immediately complete
		ret = ::aio_error( &cb );

		if_likely( ret == EINPROGRESS )
			return true;	// goes async

		ssize_t	size = ::aio_return( &cb );
		bool	ok   = (ret == 0);

		_Complete( Bytes{usize(Max( size, 0u ))}, ok );
		return ok;
	}

/*
=================================================
	GetResult
=================================================
*/
	UnixIOService::ReadRequest::Result  UnixIOService::ReadRequest::GetResult () C_NE___
	{
		ASSERT( IsFinished() );

		auto&	cb		= _aioCb.Ref< aiocb >();

		Result	res;
		res.dataSize	= _actualSize.load();
		res.pos			= _offset;
		res.data		= IsCompleted() ? const_cast<void*>(cb.aio_buf) : null;

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

		auto&	cb		= _aioCb.Ref< aiocb >();

		ResultWithRC	res;
		res.dataSize	= _actualSize.load();
		res.rc			= _memRC;
		res.pos			= _offset;
		res.data		= IsCompleted() ? const_cast<void*>(cb.aio_buf) : null;

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
		auto&	cb = _aioCb.Ref< aiocb >();
		ZeroMem( OUT cb );

		cb.aio_fildes	= _dataSource->Handle();
		cb.aio_offset	= slong{pos};
		cb.aio_buf		= const_cast<void*>(data);
		cb.aio_nbytes	= ulong{dataSize};

		auto		kq	= Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetKQueue();
		ASSERT( kq >= 0 );

		cb.aio_sigevent.sigev_notify_kqueue			= kq;
		cb.aio_sigevent.sigev_notify				= SIGEV_KEVENT;
		cb.aio_sigevent.sigev_notify_kevent_flags	= EV_CLEAR;
		cb.aio_sigevent.sigev_value.sigval_ptr		= &obj;

		int ret = ::aio_write( &cb );
		if_unlikely( ret != 0 )
		{
			UNIX_CHECK_DEV( "aio_write() failed: " );
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
		if ( IsOpen() )
		{
			int ret = ::aio_cancel( Handle(), null );
			switch ( ret )
			{
				case_likely AIO_CANCELED :	return true;
				case_likely AIO_ALLDONE :	return false;
				case -1 :					UNIX_CHECK_DEV( "aio_cancel() failed: " );	break;
			}
		}
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
		if ( IsOpen() )
		{
			int ret = ::aio_cancel( Handle(), null );
			switch ( ret )
			{
				case_likely AIO_CANCELED :	return true;
				case_likely AIO_ALLDONE :	return false;
				case -1 :					UNIX_CHECK_DEV( "aio_cancel() failed: " );	break;
			}
		}
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
	UnixIOService::UnixIOService (uint) __NE___
	{
		_kQueue = ::kqueue();

		if_unlikely( _kQueue < 0 )
			UNIX_CHECK_DEV( "kqueue() failed: " );

		CHECK( IsInitialized() );
	}

/*
=================================================
	destructor
=================================================
*/
	UnixIOService::~UnixIOService () __NE___
	{
		::close( _kQueue );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  UnixIOService::IsInitialized () C_NE___
	{
		return _kQueue >= 0;
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

		struct kevent	events [64];
		struct timespec	timeout {};

		const int	cnt = ::kevent( _kQueue, null, 0, events, CountOf(events), &timeout );
		if ( cnt == 0 )
			return 0;

		if_unlikely( cnt < 0 )
		{
			UNIX_CHECK_DEV( "kevent() failed: " );
			return 0;
		}

		for_likely (int i = 0; i < cnt; ++i)
		{
			if ( events[i].filter != EVFILT_AIO )
				continue;

			auto*		req		= Cast<_RequestBase>( events[i].udata );
			aiocb *		cb		= req->_aioCb.Ptr< aiocb >();
			int			err		= ::aio_error( cb );
			ssize_t		size	= ::aio_return( cb );

			ASSERT( err != EINPROGRESS );

			if_unlikely( err > 0 )
				UNIX_CHECK_DEV2( err, "aio_read/aio_write failed: " );

			req->_Complete( Bytes{usize(Max( size, 0u ))}, (err == 0) );
		}
		return cnt;
	}


} // AE::Threading
