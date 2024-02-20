// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    docs:
    https://learn.microsoft.com/en-us/windows/win32/fileio/synchronous-and-asynchronous-i-o
    https://learn.microsoft.com/en-us/troubleshoot/windows/win32/asynchronous-disk-io-synchronous
    https://learn.microsoft.com/en-us/windows/win32/fileio/i-o-completion-ports
*/

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "threading/DataSource/WinAsyncDataSource.h"

namespace AE::Threading
{
#   include "base/DataSource/WindowsFileHelper.cpp.h"

namespace
{
    using RWReqPromise_t = AsyncDSRequest::Value_t::Promise_t;

    static constexpr uint   c_CompletionKey = 0xAE0024;

/*
=================================================
    InitIOCompletionPort
----
    https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-setfilecompletionnotificationmodes
=================================================
*/
    ND_ static bool  InitIOCompletionPort (HANDLE file) __NE___
    {
        if_unlikely( file == INVALID_HANDLE_VALUE )
            return false;   // error

        auto    io_service = Scheduler().GetFileIOService();
        if_unlikely( not io_service )
        {
            AE_LOGI( "FileIOService is not initialized" );
            return false;   // error
        }

        HANDLE  dst_io_port = Cast<WindowsIOService>(io_service)->GetIOCompletionPort().Ref<HANDLE>();

        // attach file to the IO thread completion port
        HANDLE  port = ::CreateIoCompletionPort( file, dst_io_port, c_CompletionKey, 0 );   // winxp
        if_unlikely( port == null )
        {
            WIN_CHECK_DEV( "CreateIoCompletionPort() for file failed: " );
            return false;   // error
        }

        // configure IO port to enqueue events if:
        //  - operation starts asynchronously
        if_unlikely( ::SetFileCompletionNotificationModes( file, FILE_SKIP_COMPLETION_PORT_ON_SUCCESS | FILE_SKIP_SET_EVENT_ON_HANDLE ) == FALSE )  // winvista
        {
            WIN_CHECK_DEV( "SetFileCompletionNotificationModes() failed: " );
            return false;   // error
        }

        return true;
    }

} // namespace
//-----------------------------------------------------------------------------



    class WindowsIOService::ReadRequestApi
    {
        friend class ReadRequest;
        static void  Recycle (ReadRequest*) __NE___;
    };

    class WindowsIOService::AsyncRDataSourceApi
    {
        friend class WinAsyncRDataSource;
        ND_ static bool  CreateResult (OUT AsyncDSRequest &, RC<WinAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___;
    };

    class WindowsIOService::WriteRequestApi
    {
        friend class WriteRequest;
        static void  Recycle (WriteRequest*) __NE___;
    };

    class WindowsIOService::AsyncWDataSourceApi
    {
        friend class WinAsyncWDataSource;
        ND_ static bool  CreateResult (OUT AsyncDSRequest &, RC<WinAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    WindowsIOService::_RequestBase::_RequestBase () __NE___
    {
        StaticAssert( sizeof(_overlapped) == sizeof(OVERLAPPED) );
    }

/*
=================================================
    _Cancel
=================================================
*/
    bool  WindowsIOService::_RequestBase::_Cancel (const File_t &file) __NE___
    {
        if ( HANDLE hfile = file.Ref<HANDLE>();  hfile != INVALID_HANDLE_VALUE )
        {
            auto&   ov = _overlapped.Ref<OVERLAPPED>();

            if ( ::CancelIoEx( hfile, INOUT &ov ) != FALSE )    // winvista
                return true;

            // ERROR_NOT_FOUND          - function cannot find a request to cancel  - already completed.
            // ERROR_OPERATION_ABORTED  - operation was canceled.

            long    err = ::GetLastError();
            if ( not AnyEqual( err, ERROR_NOT_FOUND, ERROR_OPERATION_ABORTED ))
                WIN_CHECK_DEV2( err, "CancelIoEx() failed: " );
        }
        return false;
    }

/*
=================================================
    _Init
=================================================
*/
    void  WindowsIOService::_RequestBase::_Init (Bytes pos, RC<> mem) __NE___
    {
        auto&   ov = _overlapped.Ref<OVERLAPPED>();
        ZeroMem( OUT ov );

        SetOverlappedOffset( INOUT ov, pos );

        _memRC = RVRef(mem);
        _actualSize.store( 0_b );

        CHECK( _status.exchange( EStatus::InProgress ) == EStatus::Destroyed );

        // async request will be added to IO queue, so increase ref counter
        int cnt = RefCounterUtils::IncRef( *this );
        ASSERT( cnt == 1 );
    }

/*
=================================================
    _Cleanup
=================================================
*/
    void  WindowsIOService::_RequestBase::_Cleanup () __NE___
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
    void  WindowsIOService::_RequestBase::_Complete (const Bytes size, const long err) __NE___
    {
        _actualSize.store( size );

        const auto*     ov          = _overlapped.Ptr<OVERLAPPED>();
        const bool      complete    = HasOverlappedIoCompleted( ov ) and AnyEqual( err, ERROR_SUCCESS, ERROR_HANDLE_EOF );
        const EStatus   stat        = _status.exchange( complete ? EStatus::Completed : EStatus::Cancelled );

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
            DBG_CHECK_MSG( not complete or cnt > 1, "request complete but result is never used" );
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _Create
=================================================
*/
    bool  WindowsIOService::ReadRequest::_Create (RC<WinAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
    {
        ASSERT_LE( dataSize, MaxValue<DWORD>() );

        // initialize
        auto&   ov      = _overlapped.Ref<OVERLAPPED>();
        HANDLE  hfile   = file->Handle().Ref<HANDLE>();
                _data   = data;

        _Init( pos, RVRef(mem) );
        _dataSource = RVRef(file);

        // read data
        DWORD   readn = 0;
        if_likely( ::ReadFile( hfile, OUT _data, DWORD(dataSize), OUT &readn, INOUT &ov ) != FALSE )
        {
            // completed synchronously
            _Complete( Bytes{readn}, ERROR_SUCCESS );
            return true;
        }

        auto    err = ::GetLastError();
        if_unlikely( err != ERROR_IO_PENDING )
        {
            WIN_CHECK_DEV2( err, "ReadFile() failed: " );

            _dataSource = null;
            _memRC      = null;
            _data       = null;
            return false;
        }

        // goes async
        return true;
    }

/*
=================================================
    Cancel
=================================================
*/
    bool  WindowsIOService::ReadRequest::Cancel () __NE___
    {
        return _dataSource and _Cancel( _dataSource->Handle() );
    }

/*
=================================================
    GetResult
=================================================
*/
    WindowsIOService::ReadRequest::Result  WindowsIOService::ReadRequest::GetResult () C_NE___
    {
        ASSERT( IsFinished() );

        Result  res;
        res.pos         = GetOverlappedOffset( _overlapped.Ref<OVERLAPPED>() );
        res.dataSize    = _actualSize.load();
        res.data        = IsCompleted() ? _data : null;
        return res;
    }

/*
=================================================
    _GetResult
=================================================
*/
    WindowsIOService::ReadRequest::ResultWithRC  WindowsIOService::ReadRequest::_GetResult () __NE___
    {
        ASSERT( IsFinished() );

        ResultWithRC    res;
        res.pos         = GetOverlappedOffset( _overlapped.Ref<OVERLAPPED>() );
        res.dataSize    = _actualSize.load();
        res.data        = IsCompleted() ? _data : null;
        res.rc          = _memRC;
        return res;
    }

/*
=================================================
    AsPromise
=================================================
*/
    RWReqPromise_t  WindowsIOService::ReadRequest::AsPromise (ETaskQueue queueType) __NE___
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
    void  WindowsIOService::ReadRequest::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );
        {
            _Cleanup();
            _data       = null;
            _dataSource = null;
        }
        MemoryBarrier( EMemoryOrder::Release );

        WindowsIOService::ReadRequestApi::Recycle( this );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _Create
=================================================
*/
    bool  WindowsIOService::WriteRequest::_Create (RC<WinAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
    {
        ASSERT_LE( dataSize, MaxValue<DWORD>() );

        // initialize
        auto&   ov      = _overlapped.Ref<OVERLAPPED>();
        HANDLE  hfile   = file->Handle().Ref<HANDLE>();

        _Init( pos, RVRef(mem) );
        _dataSource = RVRef(file);

        // write data
        DWORD   written = 0;
        if_likely( ::WriteFile( hfile, data, DWORD(dataSize), OUT &written, INOUT &ov ) != FALSE )
        {
            // completed synchronously
            _Complete( Bytes{written}, ERROR_SUCCESS );
            return true;
        }

        auto    err = ::GetLastError();
        if_unlikely( err != ERROR_IO_PENDING )
        {
            WIN_CHECK_DEV2( err, "WriteFile() failed: " );

            _dataSource = null;
            _memRC      = null;
            return false;
        }

        // goes async
        return true;
    }

/*
=================================================
    Cancel
=================================================
*/
    bool  WindowsIOService::WriteRequest::Cancel () __NE___
    {
        return _dataSource and _Cancel( _dataSource->Handle() );
    }

/*
=================================================
    GetResult
=================================================
*/
    WindowsIOService::WriteRequest::Result  WindowsIOService::WriteRequest::GetResult () C_NE___
    {
        ASSERT( IsFinished() );

        Result  res;
        res.pos         = GetOverlappedOffset( _overlapped.Ref<OVERLAPPED>() );
        res.dataSize    = _actualSize.load();
        res.data        = null;
        return res;
    }

/*
=================================================
    _GetResult
=================================================
*/
    WindowsIOService::WriteRequest::ResultWithRC  WindowsIOService::WriteRequest::_GetResult () __NE___
    {
        ASSERT( IsFinished() );

        ResultWithRC    res;
        res.pos         = GetOverlappedOffset( _overlapped.Ref<OVERLAPPED>() );
        res.dataSize    = _actualSize.load();
        return res;
    }

/*
=================================================
    AsPromise
=================================================
*/
    RWReqPromise_t  WindowsIOService::WriteRequest::AsPromise (ETaskQueue queueType) __NE___
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
    void  WindowsIOService::WriteRequest::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );
        {
            _Cleanup();
            _dataSource = null;
        }
        MemoryBarrier( EMemoryOrder::Release );

        WindowsIOService::WriteRequestApi::Recycle( this );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    WinAsyncRDataSource::WinAsyncRDataSource (const File_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file.Ref<HANDLE>() },
        _fileSize{ GetFileSize( _file.Ref<HANDLE>() )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {
        if ( not IsOpen()   or
             not InitIOCompletionPort( _file.Ref<HANDLE>() ))
        {
            ::CloseHandle( _file.Ref<HANDLE>() );
            _file.Ref<HANDLE>() = INVALID_HANDLE_VALUE;
        }
    }

    WinAsyncRDataSource::WinAsyncRDataSource (NtStringView filename, EFlags flags)  __NE___ : WinAsyncRDataSource{ filename.c_str(), flags } {}
    WinAsyncRDataSource::WinAsyncRDataSource (const String &filename, EFlags flags) __NE___ : WinAsyncRDataSource{ filename.c_str(), flags } {}
    WinAsyncRDataSource::WinAsyncRDataSource (const char* filename, EFlags flags)   __NE___ :
        WinAsyncRDataSource{ File_t{OpenFileForRead( filename, flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    WinAsyncRDataSource::WinAsyncRDataSource (NtWStringView filename, EFlags flags) __NE___ : WinAsyncRDataSource{ filename.c_str(), flags } {}
    WinAsyncRDataSource::WinAsyncRDataSource (const WString &filename, EFlags flags)__NE___ : WinAsyncRDataSource{ filename.c_str(), flags } {}
    WinAsyncRDataSource::WinAsyncRDataSource (const wchar_t* filename, EFlags flags)__NE___ :
        WinAsyncRDataSource{ File_t{OpenFileForRead( filename, flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
    }

    WinAsyncRDataSource::WinAsyncRDataSource (const Path &path, EFlags flags)       __NE___ : WinAsyncRDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    WinAsyncRDataSource::~WinAsyncRDataSource () __NE___
    {
        if ( _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE )
            ::CloseHandle( _file.Ref<HANDLE>() );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  WinAsyncRDataSource::GetSourceType () C_NE___
    {
        return  (AllBits( _flags, EFlags::SequentialScan )  ? ESourceType::SequentialAccess : ESourceType::Unknown) |
                (AllBits( _flags, EFlags::RandomAccess )    ? ESourceType::RandomAccess     : ESourceType::Unknown) |
                ESourceType::Async      | ESourceType::FixedSize |
                ESourceType::ThreadSafe | ESourceType::ReadAccess;
    }

/*
=================================================
    IsOpen
=================================================
*/
    bool  WinAsyncRDataSource::IsOpen () C_NE___
    {
        return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
    }

/*
=================================================
    ReadBlock
=================================================
*/
    AsyncDSRequest  WinAsyncRDataSource::ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
    {
        AsyncDSRequest  req;
        if_likely( WindowsIOService::AsyncRDataSourceApi::CreateResult( OUT req, GetRC<WinAsyncRDataSource>(), pos, data, dataSize, RVRef(mem) ));
        else
            req = AsyncDSRequest{Scheduler().GetCanceledDSRequest()};
        return req;
    }

    AsyncDSRequest  WinAsyncRDataSource::ReadBlock (Bytes pos, Bytes size) __NE___
    {
        RC<SharedMem>   mem     = SharedMem::Create( AE::GetDefaultAllocator(), size ); // TODO: optimize
        void*           data    = mem ? mem->Data() : null;
        return ReadBlock( pos, data, size, RVRef(mem) );
    }

/*
=================================================
    CancelAllRequests
=================================================
*/
    bool  WinAsyncRDataSource::CancelAllRequests () __NE___
    {
        ASSERT( IsOpen() );

        if ( ::CancelIoEx( _file.Ref<HANDLE>(), null ) != FALSE )   // winvista
            return true;

        // ERROR_NOT_FOUND          - function cannot find a request to cancel  - already completed.
        // ERROR_OPERATION_ABORTED  - operation was canceled.

        long    err = ::GetLastError();
        if ( not AnyEqual( err, ERROR_NOT_FOUND, ERROR_OPERATION_ABORTED ))
            WIN_CHECK_DEV2( err, "CancelIoEx() failed: " );

        return false;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    WinAsyncWDataSource::WinAsyncWDataSource (const File_t &file, EFlags DEBUG_ONLY(, Path filename))   __NE___ :
        _file{ file.Ref<HANDLE>() }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {
        if ( not IsOpen()   or
             not InitIOCompletionPort( _file.Ref<HANDLE>() ))
        {
            ::CloseHandle( _file.Ref<HANDLE>() );
            _file.Ref<HANDLE>() = INVALID_HANDLE_VALUE;
        }
    }

    WinAsyncWDataSource::WinAsyncWDataSource (NtStringView filename, EFlags flags)  __NE___ : WinAsyncWDataSource{ filename.c_str(), flags } {}
    WinAsyncWDataSource::WinAsyncWDataSource (const String &filename, EFlags flags) __NE___ : WinAsyncWDataSource{ filename.c_str(), flags } {}
    WinAsyncWDataSource::WinAsyncWDataSource (const char* filename, EFlags flags)   __NE___ :
        WinAsyncWDataSource{ File_t{OpenFileForWrite( filename, INOUT flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    WinAsyncWDataSource::WinAsyncWDataSource (NtWStringView filename, EFlags flags) __NE___ : WinAsyncWDataSource{ filename.c_str(), flags } {}
    WinAsyncWDataSource::WinAsyncWDataSource (const WString &filename, EFlags flags)__NE___ : WinAsyncWDataSource{ filename.c_str(), flags } {}
    WinAsyncWDataSource::WinAsyncWDataSource (const wchar_t* filename, EFlags flags)__NE___ :
        WinAsyncWDataSource{ File_t{OpenFileForWrite( filename, INOUT flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
    }

    WinAsyncWDataSource::WinAsyncWDataSource (const Path &path, EFlags flags)       __NE___ : WinAsyncWDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    WinAsyncWDataSource::~WinAsyncWDataSource () __NE___
    {
        if ( _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE )
            ::CloseHandle( _file.Ref<HANDLE>() );
    }

/*
=================================================
    IsOpen
=================================================
*/
    bool  WinAsyncWDataSource::IsOpen () C_NE___
    {
        return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  WinAsyncWDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::ThreadSafe | ESourceType::Async;
    }

/*
=================================================
    WriteBlock
=================================================
*/
    AsyncDSRequest  WinAsyncWDataSource::WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
    {
        AsyncDSRequest  req;
        if_likely( WindowsIOService::AsyncWDataSourceApi::CreateResult( OUT req, GetRC<WinAsyncWDataSource>(), pos, data, dataSize, RVRef(mem) ));
        else
            req = AsyncDSRequest{Scheduler().GetCanceledDSRequest()};
        return req;
    }

/*
=================================================
    Alloc
=================================================
*/
    RC<SharedMem>  WinAsyncWDataSource::Alloc (const SizeAndAlign value) __NE___
    {
        // TODO: use LfFixedBlockAllocator and MemChunkList

        return SharedMem::Create( AE::GetDefaultAllocator(), value );   // TODO: optimize
    }

/*
=================================================
    CancelAllRequests
=================================================
*/
    bool  WinAsyncWDataSource::CancelAllRequests () __NE___
    {
        ASSERT( IsOpen() );

        if ( ::CancelIoEx( _file.Ref<HANDLE>(), null ) != FALSE )   // winvista
            return true;

        // ERROR_NOT_FOUND          - function cannot find a request to cancel  - already completed.
        // ERROR_OPERATION_ABORTED  - operation was canceled.

        long    err = ::GetLastError();
        if ( not AnyEqual( err, ERROR_NOT_FOUND, ERROR_OPERATION_ABORTED ))
            WIN_CHECK_DEV2( err, "CancelIoEx() failed: " );

        return false;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ReadRequestApi::Recycle
=================================================
*/
    inline void  WindowsIOService::ReadRequestApi::Recycle (ReadRequest* ptr) __NE___
    {
        auto    self = Cast<WindowsIOService>(Scheduler().GetFileIOService());

        CHECK( self->_readResultPool.Unassign( ptr ));
    }

/*
=================================================
    AsyncRDataSourceApi::CreateResult
=================================================
*/
    bool  WindowsIOService::AsyncRDataSourceApi::CreateResult (OUT AsyncDSRequest &req, RC<WinAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
    {
        CHECK_ERR( file and file->IsOpen() );

        auto    self = Cast<WindowsIOService>(Scheduler().GetFileIOService());
        Index_t index;

        CHECK_ERR( self->_readResultPool.Assign( OUT index ));

        // request may complete immediately
        RC<ReadRequest> res { &self->_readResultPool[ index ]};

        if_likely( res->_Create( RVRef(file), pos, data, dataSize, RVRef(mem) ))
        {
            req = RVRef(res);
            return true;
        }

        res->_Complete( 0_b, ERROR_OPERATION_ABORTED );
        res = null;
        ASSERT( not self->_readResultPool.IsAssigned( index ));

        return false;
    }

/*
=================================================
    WriteRequestApi::Recycle
=================================================
*/
    inline void  WindowsIOService::WriteRequestApi::Recycle (WriteRequest* ptr) __NE___
    {
        auto    self = Cast<WindowsIOService>(Scheduler().GetFileIOService());

        CHECK( self->_writeResultPool.Unassign( ptr ));
    }

/*
=================================================
    AsyncWDataSourceApi::CreateResult
=================================================
*/
    bool  WindowsIOService::AsyncWDataSourceApi::CreateResult (OUT AsyncDSRequest &req, RC<WinAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
    {
        CHECK_ERR( file and file->IsOpen() );

        auto    self = Cast<WindowsIOService>(Scheduler().GetFileIOService());
        Index_t index;

        CHECK_ERR( self->_writeResultPool.Assign( OUT index ));

        // request may complete immediately
        RC<WriteRequest>    res { &self->_writeResultPool[ index ]};

        if_likely( res->_Create( RVRef(file), pos, data, dataSize, RVRef(mem) ))
        {
            req = RVRef(res);
            return true;
        }

        res->_Complete( 0_b, ERROR_OPERATION_ABORTED );
        res = null;
        ASSERT( not self->_readResultPool.IsAssigned( index ));

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
    WindowsIOService::WindowsIOService (uint maxAccessThreads) __NE___ :
        _ioCompletionPort{ ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, null, c_CompletionKey, maxAccessThreads )}
    {
        if ( _ioCompletionPort.Ref<HANDLE>() == null )
            WIN_CHECK_DEV( "CreateIoCompletionPort() for global port failed: " );
    }

/*
=================================================
    destructor
=================================================
*/
    WindowsIOService::~WindowsIOService () __NE___
    {
        if ( _ioCompletionPort.Ref<HANDLE>() != null )
            ::CloseHandle( _ioCompletionPort.Ref<HANDLE>() );
    }

/*
=================================================
    IsInitialized
=================================================
*/
    bool  WindowsIOService::IsInitialized () C_NE___
    {
        return _ioCompletionPort.Ref<HANDLE>() != null;
    }

/*
=================================================
    ProcessEvents
=================================================
*/
    usize  WindowsIOService::ProcessEvents () __NE___
    {
        if_unlikely( not IsInitialized() )
            return 0;

        ASSERT( Scheduler().GetFileIOService() == this );

        usize   num_events = 0;
        for (;;)
        {
            const DWORD     timeout             = 0;    // or INFINITE
            DWORD           byte_transferred    = 0;
            ULONG_PTR       completion_key      = 0;
            LPOVERLAPPED    overlapped          = null;

            bool    ok  = ::GetQueuedCompletionStatus( _ioCompletionPort.Ref<HANDLE>(), OUT &byte_transferred,
                                                       OUT &completion_key, OUT &overlapped, timeout ) != FALSE;    // winxp
            long    err = ok ? ERROR_SUCCESS : ::GetLastError();

            // ERROR_HANDLE_EOF         - reached end of file
            // ERROR_OPERATION_ABORTED  - request was cancelled by 'CancelIoEX()'

            if_likely( AnyEqual( err, ERROR_SUCCESS, ERROR_HANDLE_EOF, ERROR_OPERATION_ABORTED ) and (overlapped != null) )
            {
                ASSERT( completion_key == c_CompletionKey );

                _RequestBase*   res = Cast<_RequestBase>( overlapped - Bytes{c_OverlappedOffset});
                ASSERT( res->_overlapped.Ptr<OVERLAPPED>() == overlapped );

                res->_Complete( Bytes{byte_transferred}, err );

                ++num_events;
            }
            else
            {
                if ( not AnyEqual( err, ERROR_SUCCESS, WAIT_TIMEOUT ))
                    WIN_CHECK_DEV2( err, "GetQueuedCompletionStatus() failed: " );

                break;
            }
        }

        return num_events;
    }


} // AE::Threading

#endif // AE_PLATFORM_WINDOWS
