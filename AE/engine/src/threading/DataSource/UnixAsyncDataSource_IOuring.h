// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Multithreading:
      use io_uring per thread ([ref](https://github.com/axboe/liburing/issues/571#issuecomment-1106480309)).

    [lib](https://github.com/axboe/liburing)
    [docs](https://kernel.dk/io_uring.pdf)
    [examples](https://unixism.net/loti/index.html)

    Install:
    > sudo apt install liburing-dev

    TODO: multithreading:
    problem: io_uring must be single threaded, we can create single io_uring per thread with FileIO, but can not enqueue requests from threads without FileIO.
    solution 1: from threads without FileIO use message queue instead of io_uring and process messages in FileIO thread.
    solution 2: use low-level io_uring (instead of liburing)
                https://github.com/axboe/liburing/issues/109#issuecomment-613519695
                https://github.com/axboe/liburing/issues/109#issuecomment-614930296
*/

#include <linux/io_uring.h>
#include <liburing.h>           // wrapper for io_uring

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
        StaticAssert( sizeof(IOURing_t) == sizeof(io_uring) );
    }

/*
=================================================
    _Cancel
=================================================
*/
    bool  UnixIOService::_RequestBase::_Cancel () __NE___
    {
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
        ASSERT_LE( dataSize, MaxValue<unsigned>() );

        // initialize
        _Init( RVRef(mem) );
        _dataSource = RVRef(file);
        _offset     = pos;
        _data       = data;

        // read data
        auto&   ring_thread = Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetIOURing();
        auto&   ring        = ring_thread.ring.Ref< io_uring >();

        io_uring_sqe*   sqe = ::io_uring_get_sqe( &ring );
        CHECK_ERR( sqe != null );

        ::io_uring_prep_read( sqe, _dataSource->Handle(), data, unsigned{dataSize}, ulong{pos} );
        ::io_uring_sqe_set_data( sqe, this );
        int     cnt = ::io_uring_submit( &ring );

        ring_thread.guard.unlock();

        if_unlikely( cnt != 1 )
        {
            UNIX_CHECK_DEV2( -cnt, "io_uring_submit() failed: " );
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

        Result  res;
        res.dataSize    = _actualSize.load();
        res.pos         = _offset;
        res.data        = IsCompleted() ? _data : null;

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

        ResultWithRC    res;
        res.dataSize    = _actualSize.load();
        res.rc          = _memRC;
        res.pos         = _offset;
        res.data        = IsCompleted() ? _data : null;

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
        _dataSource = RVRef(file);
        _offset     = pos;

        // write data
        auto&   ring_thread = Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetIOURing();
        auto&   ring        = ring_thread.ring.Ref< io_uring >();

        io_uring_sqe*   sqe = ::io_uring_get_sqe( &ring );
        CHECK_ERR( sqe != null );

        ::io_uring_prep_write( sqe, _dataSource->Handle(), data, unsigned{dataSize}, ulong{pos} );
        ::io_uring_sqe_set_data( sqe, this );
        int     cnt = ::io_uring_submit( &ring );

        ring_thread.guard.unlock();

        if_unlikely( cnt != 1 )
        {
            UNIX_CHECK_DEV2( -cnt, "io_uring_submit() failed: " );
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

        Result  res;
        res.dataSize    = _actualSize.load();
        res.data        = null;
        res.pos         = _offset;

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

        ResultWithRC    res;
        res.dataSize    = _actualSize.load();
        res.pos         = _offset;

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
        maxAccessThreads = Min( maxAccessThreads, _iouringArray.size() );

        usize   max_requests = (ReadRequestPool_t::capacity() + WriteRequestPool_t::capacity()) / 2;
        max_requests = Min( max_requests, 32u << 10 );

        bool    ok = true;
        for (uint i = 0; ok and (i < maxAccessThreads); ++i)
        {
            for (; max_requests > _IOuring_MinRequests;)
            {
                int err = ::io_uring_queue_init( max_requests, OUT _iouringArray[i].ring.Ptr< io_uring >(), 0 );
                if_likely( err == 0 )
                {
                    _iouringArray[i].created = true;
                    break;
                }

                err = -err;
                if ( err == EAGAIN )
                {
                    max_requests /= 2;
                    continue;
                }

                UNIX_CHECK_DEV2( err, "io_uring_queue_init() failed: " );
                ok = false;
                break;
            }
        }
    }

/*
=================================================
    destructor
=================================================
*/
    UnixIOService::~UnixIOService () __NE___
    {
        for (auto& per_thread : _iouringArray)
        {
            if ( per_thread.created )
                ::io_uring_queue_exit( per_thread.ring.Ptr< io_uring >() );
        }
    }

/*
=================================================
    IsInitialized
=================================================
*/
    bool  UnixIOService::IsInitialized () C_NE___
    {
        return _iouringArray[0].created;
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

        usize   num_events = 0;

        for (auto& item : _iouringArray)
        {
            if_unlikely( not item.created )
                continue;

            DeferExLock  lock {item.guard};
            if ( not lock.try_lock() )
                continue;

            auto*   ring = item.ring.Ptr< io_uring >();

            for (;;)
            {
                io_uring_cqe*   cqe = null;
                int             ret = ::io_uring_peek_cqe( ring, OUT &cqe );

                if_likely( ret == -EAGAIN )
                    break;

                if_likely( ret == 0 and cqe != null )
                {
                    auto*   req = Cast< _RequestBase >( ::io_uring_cqe_get_data( cqe ));
                    req->_Complete( Bytes{uint(Max( 0, cqe->res ))}, (cqe->res >= 0) );

                    ::io_uring_cqe_seen( ring, cqe );
                    ++num_events;
                    continue;
                }

                UNIX_CHECK_DEV2( -ret, "io_uring_peek_cqe() failed: " );
                break;
            }
        }
        return num_events;
    }

/*
=================================================
    _GetIOURing
=================================================
*/
    UnixIOService::IOURing_PerThread&  UnixIOService::_GetIOURing () __NE___
    {
        for (;;)
        {
            for (auto& item : _iouringArray)
            {
            if_unlikely( not item.created )
                continue;

                if ( not item.guard.try_lock() )
                    continue;

                return item;
            }
        }
    }


} // AE::Threading
