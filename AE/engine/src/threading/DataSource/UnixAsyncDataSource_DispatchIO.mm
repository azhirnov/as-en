// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    [docs](https://developer.apple.com/documentation/dispatch/1388976-dispatch_io_create)
*/

#ifdef AE_ASYNCIO_USE_DISPATCH_IO
# include "base/ObjC/NS.mm.h"
# include <sys/stat.h>
# include <dispatch/dispatch.h>

# include "threading/DataSource/UnixAsyncDataSource.h"

namespace AE::Threading
{
#   include "base/DataSource/UnixFileHelper.cpp.h"

/*
=================================================
    constructor
=================================================
*/
    UnixIOService::_RequestBase::_RequestBase () __NE___
    {
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
        ASSERT_LE( dataSize, MaxValue<size_t>() );

        // initialize
        _Init( RVRef(mem) );
        _dataSource = RVRef(file);
        _offset     = pos;
        _data       = data;

        // read data
        void*   io_queue = Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetIOQueue();

        dispatch_io_read(
            (__bridge dispatch_io_t)_dataSource->GetIOChannel(),
            off_t{pos},
            size_t{dataSize},
            (__bridge dispatch_queue_t)io_queue,
            ^ (bool done, dispatch_data_t dataObj, int error)
            {
                ASSERT( done );  // partial result is not supported

                if_likely( dataObj != null and done )
                {
                    __block Bytes readn;
                    dispatch_data_apply(
                        dataObj,
                        ^(dispatch_data_t region, size_t offset, const void* buffer, size_t size)
                        {
                            std::memcpy( OUT _data, buffer, size );
                            readn = Bytes{size};
                            return true;
                        });

                    _Complete( readn, true );
                }
                else
                {
                    // TODO: check errors
                    _Complete( 0_b, false );
                }
            });

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
        _dataSize   = dataSize;

        // write data
        void*   io_queue = Cast< UnixIOService >( Scheduler().GetFileIOService() )->_GetIOQueue();

        dispatch_data_t data_obj = dispatch_data_create(
                                        data, size_t{dataSize},
                                        (__bridge dispatch_queue_t)io_queue,
                                        ^(){} );
        CHECK_ERR( data_obj != null );

        dispatch_io_write(
            (__bridge dispatch_io_t)_dataSource->GetIOChannel(),
            off_t{pos},
            data_obj,
            (__bridge dispatch_queue_t)io_queue,
            ^ (bool done, dispatch_data_t dataObj, int error)
            {
                ASSERT( done );  // partial result is not supported

                Bytes   written;
                if ( dataObj != null )
                    written = Bytes{dispatch_data_get_size( dataObj )};
                else
                    written = _dataSize;

                _Complete( written, (done and error == 0) );
            });

        dispatch_release( data_obj );
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
    CreateIOChannel
=================================================
*/
namespace
{
    static void  CreateIOChannel (INOUT int &fd, OUT void* &ioChannel) __NE___
    {
        ASSERT( not ioChannel );

        auto    service     = Cast< UnixIOService >( Scheduler().GetFileIOService() );
        ASSERT( service);

        void*   io_queue    = service->_GetIOQueue();
        ASSERT( io_queue );

        // use DISPATCH_IO_RANDOM because with DISPATCH_IO_STREAM file offset will be ignored

        dispatch_io_t   channel = dispatch_io_create( DISPATCH_IO_RANDOM, fd, (__bridge dispatch_queue_t)io_queue, ^(int){} );
        if_unlikely( channel == null )
        {
            ::close( fd );
            fd = -1;
            return;
        }

        dispatch_io_set_low_water( channel, SIZE_MAX ); // disable partial results

        // don't modify file descriptor after channel creation!

        ioChannel = (__bridge void*)channel;
    }
}
//-----------------------------------------------------------------------------



/*
=================================================
    constructor / destructor
=================================================
*/
    UnixAsyncRDataSource::UnixAsyncRDataSource (Handle_t file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file },
        _fileSize{ GetFileSize( _file )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {
        CreateIOChannel( INOUT _file, OUT _ioChannel );
    }

    UnixAsyncRDataSource::~UnixAsyncRDataSource () __NE___
    {
        if ( _ioChannel != null )
            dispatch_io_close( (__bridge dispatch_io_t)_ioChannel, DISPATCH_IO_STOP );

        if ( IsOpen() )
            ::close( _file );
    }

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
    constructor / destructor
=================================================
*/
    UnixAsyncWDataSource::UnixAsyncWDataSource (Handle_t file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {
        CreateIOChannel( INOUT _file, OUT _ioChannel );
    }

    UnixAsyncWDataSource::~UnixAsyncWDataSource () __NE___
    {
        if ( _ioChannel != null )
            dispatch_io_close( (__bridge dispatch_io_t)_ioChannel, DISPATCH_IO_STOP );

        if ( IsOpen() )
            ::close( _file );
    }

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
    UnixIOService::UnixIOService (uint) __NE___
    {
        dispatch_queue_t    queue = null;

        queue = dispatch_queue_create_with_target(  "async io queue",
                                                    DISPATCH_QUEUE_CONCURRENT,
                                                    dispatch_get_global_queue( QOS_CLASS_UTILITY, 0 ));
        _ioQueue = (__bridge void *)queue;

        CHECK( IsInitialized() );
    }

/*
=================================================
    destructor
=================================================
*/
    UnixIOService::~UnixIOService () __NE___
    {
        if ( _ioQueue != null )
            dispatch_release( (__bridge dispatch_queue_t)_ioQueue );
    }

/*
=================================================
    IsInitialized
=================================================
*/
    bool  UnixIOService::IsInitialized () C_NE___
    {
        return _ioQueue != null;
    }

/*
=================================================
    ProcessEvents
=================================================
*/
    usize  UnixIOService::ProcessEvents () __NE___
    {
        ASSERT( not IsInitialized() or Scheduler().GetFileIOService() == this );

        return 0;
    }


} // AE::Threading

#endif // AE_ASYNCIO_USE_DISPATCH_IO
