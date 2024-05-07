// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	POISX AIO (Linux or MacOS version)

	Details:
		- Some Linux implementations has internal thread pool (https://man7.org/linux/man-pages/man3/aio_init.3.html)

	Multithreading:
	  ???

	[docs](https://man7.org/linux/man-pages/man7/aio.7.html)

	TODO:
	https://developer.apple.com/forums/thread/43439
	https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/aio_read.2.html
*/

#include <aio.h>
#include <sys/types.h>
//#include <sys/event.h>
#include <sys/time.h>

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

		int ret = ::aio_read( &cb );
		if_unlikely( ret != 0 )
		{
			UNIX_CHECK_DEV( "aio_read() failed: " );
			return false;
		}

		// may immediately complete
		ret = ::aio_error( &cb );

		if_likely( ret == EINPROGRESS )
		{
			Cast< UnixIOService >( Scheduler().GetFileIOService() )->_AsyncWaitToComplete( this );
			return true;	// goes async
		}

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
		cb.aio_offset	= slong{pos};		// mutable
		cb.aio_buf		= const_cast<void*>(data);
		cb.aio_nbytes	= ulong{dataSize};

		int ret = ::aio_write( &cb );
		if_unlikely( ret != 0 )
		{
			UNIX_CHECK_DEV( "aio_write() failed: " );
			return false;
		}

		// may immediately complete
		ret = ::aio_error( &cb );

		if_likely( ret == EINPROGRESS )
		{
			Cast< UnixIOService >( Scheduler().GetFileIOService() )->_AsyncWaitToComplete( this );
			return true;	// goes async
		}

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
		EXLOCK( _queueGuard );
		_queue.reserve( 8u << 10 );
	}

/*
=================================================
	destructor
=================================================
*/
	UnixIOService::~UnixIOService () __NE___
	{
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  UnixIOService::IsInitialized () C_NE___
	{
		return true;
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

		usize	num_events = 0;

		if ( _queueGuard.try_lock() )
		{
			usize		j	= _queueLastIdx;
			const usize	cnt = Min( _queue.size(), 64u );

			for (usize i = 0; i < cnt; ++i, ++j)
			{
				if_unlikely( j >= _queue.size() )
					j = 0;

				auto*	req		= _queue[j];
				aiocb*	cb		= req->_aioCb.Ptr< aiocb >();
				int		err		= ::aio_error( cb );

				switch ( err )
				{
					case 0 :
					case ECANCELED :
					{
						ssize_t	size = ::aio_return( cb );
						req->_Complete( Bytes{usize(Max( size, 0u ))}, (err == 0) );
						_queue.FastErase( j );
						++num_events;
						break;
					}

					case EINPROGRESS :
						break;

					default :
						req->_Complete( 0_b, false );
						_queue.FastErase( j );
						++num_events;
						break;
				}
			}

			_queueLastIdx = j;
			_queueGuard.unlock();
		}
		return num_events;
	}


} // AE::Threading
