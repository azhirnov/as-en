// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    file system details:
    https://en.wikipedia.org/wiki/Comparison_of_file_systems
    https://source.android.com/docs/core/architecture/android-kernel-file-system-support
    https://en.wikipedia.org/wiki/Apple_File_System

*/

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <signal.h>

# ifdef AE_ASYNCIO_USE_LINUX_AIO
#   include "threading/DataSource/UnixAsyncDataSource_LinuxAIO.h"
# endif
# ifdef AE_ASYNCIO_USE_IO_URING
#   include "threading/DataSource/UnixAsyncDataSource_IOuring.h"
# endif
# ifdef AE_ASYNCIO_USE_POSIX_AIO
#   include "threading/DataSource/UnixAsyncDataSource_PosixAIO.h"
# endif
# ifdef AE_ASYNCIO_USE_BSD_AIO
#   include "threading/DataSource/UnixAsyncDataSource_BSDPosixAIO.h"
# endif
# ifdef AE_ASYNCIO_USE_SYNCIO
#   include "threading/DataSource/UnixAsyncDataSource_SyncIO.h"
# endif

# include "threading/DataSource/UnixAsyncDataSource.h"

namespace AE::Threading
{
#   include "base/DataSource/UnixFileHelper.cpp.h"

    using RWReqPromise_t = AsyncDSRequest::Value_t::Promise_t;

    class UnixIOService::ReadRequestApi
    {
        friend class ReadRequest;
        static void  Recycle (ReadRequest*) __NE___;
    };

    class UnixIOService::AsyncRDataSourceApi
    {
        friend class UnixAsyncRDataSource;
        ND_ static bool  CreateResult (OUT AsyncDSRequest &, RC<UnixAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___;
    };

    class UnixIOService::WriteRequestApi
    {
        friend class WriteRequest;
        static void  Recycle (WriteRequest*) __NE___;
    };

    class UnixIOService::AsyncWDataSourceApi
    {
        friend class UnixAsyncWDataSource;
        ND_ static bool  CreateResult (OUT AsyncDSRequest &, RC<UnixAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    _Init
=================================================
*/
    void  UnixIOService::_RequestBase::_Init (RC<> mem) __NE___
    {
        _memRC = RVRef(mem);
        _actualSize.store( 0_b );

        CHECK( _status.exchange( EStatus::InProgress ) == EStatus::Destroyed );

        // async request will be added to IO queue, so increase ref counter
        RefCounterUtils::IncRef( *this );
    }

/*
=================================================
    _Cleanup
=================================================
*/
    void  UnixIOService::_RequestBase::_Cleanup () __NE___
    {
        _memRC = null;
        _actualSize.store( 0_b );

        ASSERT( AnyEqual( _status.load(), EStatus::Cancelled, EStatus::Completed ));
        _status.store( EStatus::Destroyed );

        DEBUG_ONLY({
            EXLOCK( _depsGuard );
            CHECK( _deps.empty() );
        })
    }

/*
=================================================
    _Complete
=================================================
*/
    void  UnixIOService::_RequestBase::_Complete (const Bytes size, const bool complete) __NE___
    {
        _actualSize.store( size );

        const EStatus   stat = _status.exchange( complete ? EStatus::Completed : EStatus::Cancelled );

        ASSERT( complete );
        ASSERT( stat == EStatus::InProgress );  Unused( stat );
        ASSERT( RefCounterUtils::UseCount( *this ) > 0 );

        _SetDependencyCompleteStatus( complete );

        // Ref count was increased in '_Init()' to keep request alive until it is complete.
        // Cancelled request may be destroyed here, successfully completed request should not be destroyed here!
        {
            auto*   ptr = this;
            auto    cnt = RefCounterUtils::DecRefAndRelease( ptr );

            Unused( cnt );
            //DBG_CHECK_MSG( not complete or cnt > 1, "request complete but result is never used" );
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Cancel
=================================================
*/
    bool  UnixIOService::ReadRequest::Cancel () __NE___
    {
        return _dataSource and _Cancel();
    }

/*
=================================================
    AsPromise
=================================================
*/
    RWReqPromise_t  UnixIOService::ReadRequest::AsPromise (ETaskQueue queueType) __NE___
    {
        auto    result = MakeDelayedPromise( [self = GetRC<ReadRequest>()] () { return self->_GetResult(); }, "AsyncReadRequest", queueType );
        if_likely( Scheduler().Run( AsyncTask{result}, Tuple{GetRC()} ))
            return result;
        else
            return Default;
    }

/*
=================================================
    _ReleaseObject
=================================================
*/
    void  UnixIOService::ReadRequest::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );
        {
            _Cleanup();
            _dataSource = null;
        }
        MemoryBarrier( EMemoryOrder::Release );

        UnixIOService::ReadRequestApi::Recycle( this );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Cancel
=================================================
*/
    bool  UnixIOService::WriteRequest::Cancel () __NE___
    {
        return _dataSource and _Cancel();
    }

/*
=================================================
    AsPromise
=================================================
*/
    RWReqPromise_t  UnixIOService::WriteRequest::AsPromise (ETaskQueue queueType) __NE___
    {
        auto    result = MakeDelayedPromise( [self = GetRC<WriteRequest>()] () { return self->_GetResult(); }, "AsyncWriteRequest", queueType );
        if_likely( Scheduler().Run( AsyncTask{result}, Tuple{GetRC()} ))
            return result;
        else
            return Default;
    }

/*
=================================================
    _ReleaseObject
=================================================
*/
    void  UnixIOService::WriteRequest::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );
        {
            _Cleanup();
            _dataSource = null;
        }
        MemoryBarrier( EMemoryOrder::Release );

        UnixIOService::WriteRequestApi::Recycle( this );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
#ifndef AE_ASYNCIO_USE_DISPATCH_IO
    UnixAsyncRDataSource::UnixAsyncRDataSource (Handle_t file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file },
        _fileSize{ GetFileSize( _file )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}
#endif

    UnixAsyncRDataSource::UnixAsyncRDataSource (const char* filename, EFlags flags) __NE___ :
        UnixAsyncRDataSource{ Handle_t{OpenFileForRead( filename, flags )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            UNIX_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    UnixAsyncRDataSource::UnixAsyncRDataSource (NtStringView filename, EFlags flags)    __NE___ : UnixAsyncRDataSource{ filename.c_str(), flags } {}
    UnixAsyncRDataSource::UnixAsyncRDataSource (const String &filename, EFlags flags)   __NE___ : UnixAsyncRDataSource{ filename.c_str(), flags } {}
    UnixAsyncRDataSource::UnixAsyncRDataSource (const Path &path, EFlags flags)         __NE___ : UnixAsyncRDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
#ifndef AE_ASYNCIO_USE_DISPATCH_IO
    UnixAsyncRDataSource::~UnixAsyncRDataSource () __NE___
    {
        if ( IsOpen() )
            ::close( _file );
    }
#endif
/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  UnixAsyncRDataSource::GetSourceType () C_NE___
    {
        return  (AllBits( _flags, EFlags::SequentialScan )  ? ESourceType::SequentialAccess : ESourceType::Unknown) |
                (AllBits( _flags, EFlags::RandomAccess )    ? ESourceType::RandomAccess     : ESourceType::Unknown) |
                ESourceType::Async      | ESourceType::FixedSize |
                ESourceType::ThreadSafe | ESourceType::ReadAccess;
    }

/*
=================================================
    ReadBlock
=================================================
*/
    AsyncDSRequest  UnixAsyncRDataSource::ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
    {
        AsyncDSRequest  req;
        if_likely( UnixIOService::AsyncRDataSourceApi::CreateResult( OUT req, GetRC<UnixAsyncRDataSource>(), pos, data, dataSize, RVRef(mem) ));
        else
            req = AsyncDSRequest{Scheduler().GetCanceledDSRequest()};
        return req;
    }

    AsyncDSRequest  UnixAsyncRDataSource::ReadBlock (Bytes pos, Bytes size) __NE___
    {
        RC<SharedMem>   mem     = SharedMem::Create( AE::GetDefaultAllocator(), size ); // TODO: optimize
        void*           data    = mem ? mem->Data() : null;
        return ReadBlock( pos, data, size, RVRef(mem) );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
#ifndef AE_ASYNCIO_USE_DISPATCH_IO
    UnixAsyncWDataSource::UnixAsyncWDataSource (Handle_t file, EFlags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}
#endif

    UnixAsyncWDataSource::UnixAsyncWDataSource (const char* filename, EFlags flags) __NE___ :
        UnixAsyncWDataSource{ Handle_t{OpenFileForWrite( filename, INOUT flags )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            UNIX_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    UnixAsyncWDataSource::UnixAsyncWDataSource (NtStringView filename, EFlags flags)    __NE___ : UnixAsyncWDataSource{ filename.c_str(), flags } {}
    UnixAsyncWDataSource::UnixAsyncWDataSource (const String &filename, EFlags flags)   __NE___ : UnixAsyncWDataSource{ filename.c_str(), flags } {}
    UnixAsyncWDataSource::UnixAsyncWDataSource (const Path &path, EFlags flags)         __NE___ : UnixAsyncWDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
#ifndef AE_ASYNCIO_USE_DISPATCH_IO
    UnixAsyncWDataSource::~UnixAsyncWDataSource () __NE___
    {
        if ( IsOpen() )
            ::close( _file );
    }
#endif
/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  UnixAsyncWDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::ThreadSafe | ESourceType::Async;
    }

/*
=================================================
    WriteBlock
=================================================
*/
    AsyncDSRequest  UnixAsyncWDataSource::WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
    {
        AsyncDSRequest  req;
        if_likely( UnixIOService::AsyncWDataSourceApi::CreateResult( OUT req, GetRC<UnixAsyncWDataSource>(), pos, data, dataSize, RVRef(mem) ));
        else
            req = AsyncDSRequest{Scheduler().GetCanceledDSRequest()};
        return req;
    }

/*
=================================================
    Alloc
=================================================
*/
    RC<SharedMem>  UnixAsyncWDataSource::Alloc (const SizeAndAlign value) __NE___
    {
        // TODO: use LfFixedBlockAllocator and MemChunkList

        return SharedMem::Create( AE::GetDefaultAllocator(), value );   // TODO: optimize
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ReadRequestApi::Recycle
=================================================
*/
    inline void  UnixIOService::ReadRequestApi::Recycle (ReadRequest* ptr) __NE___
    {
        auto    self = Cast<UnixIOService>(Scheduler().GetFileIOService());

        CHECK( self->_readResultPool.Unassign( ptr ));
    }

/*
=================================================
    AsyncRDataSourceApi::CreateResult
=================================================
*/
    bool  UnixIOService::AsyncRDataSourceApi::CreateResult (OUT AsyncDSRequest &req, RC<UnixAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
    {
        CHECK_ERR( file and file->IsOpen() );

        auto    self = Cast<UnixIOService>(Scheduler().GetFileIOService());
        Index_t index;

        CHECK_ERR( self->_readResultPool.Assign( OUT index ));

        // request may complete immediately
        RC<ReadRequest> res { &self->_readResultPool[ index ]};

        if_likely( res->_Create( RVRef(file), pos, data, dataSize, RVRef(mem) ))
        {
            req = RVRef(res);
            return true;
        }

        res->_Complete( 0_b, false );
        res = null;
        ASSERT( not self->_readResultPool.IsAssigned( index ));

        return false;
    }

/*
=================================================
    WriteRequestApi::Recycle
=================================================
*/
    inline void  UnixIOService::WriteRequestApi::Recycle (WriteRequest* ptr) __NE___
    {
        auto    self = Cast<UnixIOService>(Scheduler().GetFileIOService());

        CHECK( self->_writeResultPool.Unassign( ptr ));
    }

/*
=================================================
    AsyncWDataSourceApi::CreateResult
=================================================
*/
    bool  UnixIOService::AsyncWDataSourceApi::CreateResult (OUT AsyncDSRequest &req, RC<UnixAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
    {
        CHECK_ERR( file and file->IsOpen() );

        auto    self = Cast<UnixIOService>(Scheduler().GetFileIOService());
        Index_t index;

        CHECK_ERR( self->_writeResultPool.Assign( OUT index ));

        // request may complete immediately
        RC<WriteRequest>    res { &self->_writeResultPool[ index ]};

        if_likely( res->_Create( RVRef(file), pos, data, dataSize, RVRef(mem) ))
        {
            req = RVRef(res);
            return true;
        }

        res->_Complete( 0_b, false );
        res = null;
        ASSERT( not self->_readResultPool.IsAssigned( index ));

        return false;
    }


} // AE::Threading

#endif // AE_PLATFORM_UNIX_BASED
