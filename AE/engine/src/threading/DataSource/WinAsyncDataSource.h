// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/DataSource/AsyncDataSource.h"
#include "threading/Containers/LfIndexedPool3.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Threading
{
    class WinAsyncRDataSource;
    class WinAsyncWDataSource;


    //
    // Windows IO Service
    //
    class WindowsIOService final : public IOService
    {
    // types
    public:
        using IOPort_t      = UntypedStorage< sizeof(void*), alignof(void*) >;      // HANDLE
        using File_t        = UntypedStorage< sizeof(void*), alignof(void*) >;      // HANDLE
        using Overlapped_t  = UntypedStorage< sizeof(ulong)*4, alignof(ulong) >;    // OVERLAPPED
        using Index_t       = uint;

        class AsyncRDataSourceApi;
        class AsyncWDataSourceApi;

    private:
        static constexpr uint   OverlappedOffset = sizeof(void*)*3;

        using AsyncRequestPtr = RC<Threading::_hidden_::IAsyncDataSourceRequest>;


        //
        // Dummy Request
        //
        class _DummyRequest final : public Threading::_hidden_::IAsyncDataSourceRequest
        {
        // methods
        public:
            _DummyRequest ()                    __NE___ { _status.store( EStatus::Cancelled ); }

            // IAsyncDataSourceRequest //
            Result      GetResult ()            C_NE_OV { return Default; }
            bool        Cancel ()               __NE_OV { return false; }
            Promise_t   AsPromise (ETaskQueue)  __NE_OV { return Default; }
        };


        //
        // Request Base
        //
        class _RequestBase : public Threading::_hidden_::IAsyncDataSourceRequest
        {
        // types
        protected:
            using TaskDependency    = IAsyncTask::TaskDependency;
            using Dependencies_t    = FixedTupleArray< 4, AsyncTask, TaskDependency >;


        // variables
        protected:
            Overlapped_t        _overlapped;

            SpinLock            _depsGuard;
            Dependencies_t      _deps;

            const Index_t       _indexInPool;   // can be used in 'ReadRequestApi' or 'WriteRequestApi'

            // read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
            RC<>                _memRC;         // keep memory alive


        // methods
        protected:
            explicit _RequestBase (Index_t idx)                                                     __NE___;

                void  _Init (Bytes pos, RC<> mem)                                                   __NE___;
                void  _Cleanup ()                                                                   __NE___;
            ND_ bool  _Cancel (const File_t &file)                                                  __NE___;

        private:
            friend class WindowsIOService;
                void  _Complete (Bytes size, long err, const void* ov)                              __NE___;

            ND_ bool  _AddOnCompleteDependency (AsyncTask task, INOUT uint &index, Bool isStrong)   __NE___;
        };


        //
        // Read Request
        //
        class ReadRequestApi;
        class ReadRequest final : public _RequestBase
        {
        // variables
        private:
            // read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
            void*                       _data       = null;
            RC<WinAsyncRDataSource>     _dataSource;    // keep alive


        // methods
        public:
            explicit ReadRequest (Index_t idx)  __NE___ : _RequestBase{idx} {}

            // IAsyncDataSourceRequest //
            Result      GetResult ()            C_NE_OV;
            bool        Cancel ()               __NE_OV;
            Promise_t   AsPromise (ETaskQueue)  __NE_OV;

        private:
            friend class AsyncRDataSourceApi;
            ND_ bool  _Create (RC<WinAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<SharedMem> mem) __NE___;

            ND_ ResultWithRC  _GetResult ()     __NE___;

                void  _ReleaseObject ()         __NE_OV;
        };


        //
        // Write Request
        //
        class WriteRequestApi;
        class WriteRequest final : public _RequestBase
        {
        // variables
        private:
            // read-only data: accessed only in '_Init()' and '_Cleanup()' which are externally synchronized
            RC<WinAsyncWDataSource>     _dataSource;    // keep alive


        // methods
        public:
            explicit WriteRequest (Index_t idx) __NE___ : _RequestBase{idx} {}

            // IAsyncDataSourceRequest //
            Result      GetResult ()            C_NE_OV;
            bool        Cancel ()               __NE_OV;
            Promise_t   AsPromise (ETaskQueue)  __NE_OV;

        private:
            friend class AsyncWDataSourceApi;
            ND_ bool  _Create (RC<WinAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___;

            ND_ ResultWithRC  _GetResult ()     __NE___;

                void  _ReleaseObject ()         __NE_OV;
        };


    private:
        template <typename T, usize ChunkSize, usize MaxChunks>
        using PoolTmpl              = LfIndexedPool2< T, Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

        using ReadRequestPool_t     = PoolTmpl< ReadRequest,  1u<<10, 8 >;
        using WriteRequestPool_t    = PoolTmpl< WriteRequest, 1u<<10, 8 >;


    // variables
    private:
        IOPort_t            _ioCompletionPort;

        AsyncDSRequest      _cancelledRequest;

        ReadRequestPool_t   _readResultPool;
        WriteRequestPool_t  _writeResultPool;


    // methods
    public:
        ~WindowsIOService ()                                                    __NE___;

        ND_ bool            IsInitialized ()                                    C_NE___;

        ND_ IOPort_t const& GetIOCompletionPort ()                              C_NE___ { return _ioCompletionPort; }
        ND_ AsyncDSRequest  GetCancelledRequest ()                              C_NE___ { return _cancelledRequest; }


        // IOService //
        usize           ProcessEvents ()                                        __NE_OV;
        EIOServiceType  GetIOServiceType ()                                     C_NE_OV { return EIOServiceType::File; }


    private:
        friend class TaskScheduler;
        explicit WindowsIOService (uint maxThreads)                             __NE___;

        bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)   __NE_OV;
    };


    class WindowsIOService::ReadRequestApi
    {
        friend class ReadRequest;
        static void  Recycle (Index_t indexInPool) __NE___;
    };

    class WindowsIOService::AsyncRDataSourceApi
    {
        friend class WinAsyncRDataSource;
        ND_ static bool  CreateResult (OUT AsyncDSRequest &, RC<WinAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___;
    };

    class WindowsIOService::WriteRequestApi
    {
        friend class WriteRequest;
        static void  Recycle (Index_t indexInPool) __NE___;
    };

    class WindowsIOService::AsyncWDataSourceApi
    {
        friend class WinAsyncWDataSource;
        ND_ static bool  CreateResult (OUT AsyncDSRequest &, RC<WinAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___;
    };
//-----------------------------------------------------------------------------



    //
    // Windows Read-only Async File
    //
    class WinAsyncRDataSource final : public AsyncRDataSource
    {
    // types
    public:
        using EFlags    = WinRFileStream::EFlags;
    private:
        using File_t    = WindowsIOService::File_t;

        static constexpr EFlags DefaultFlags    = EFlags::RandomAccess;


    // variables
    private:
        File_t          _file;
        const Bytes     _fileSize;
        const EFlags    _flags;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        WinAsyncRDataSource (const File_t &file, EFlags flags DEBUG_ONLY(, Path filename))  __NE___;

    public:
        explicit WinAsyncRDataSource (const char* filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinAsyncRDataSource (NtStringView filename, EFlags flags = DefaultFlags)   __NE___;
        explicit WinAsyncRDataSource (const String &filename, EFlags flags = DefaultFlags)  __NE___;

        explicit WinAsyncRDataSource (NtWStringView filename, EFlags flags = DefaultFlags)  __NE___;
        explicit WinAsyncRDataSource (const wchar_t* filename, EFlags flags = DefaultFlags) __NE___;
        explicit WinAsyncRDataSource (const WString &filename, EFlags flags = DefaultFlags) __NE___;

        explicit WinAsyncRDataSource (const Path &path, EFlags flags = DefaultFlags)        __NE___;

        ~WinAsyncRDataSource ()                                                             __NE_OV;

        ND_ File_t const&   Handle ()                                                       __NE___ { return _file; }


        // AsyncRDataSource //
        bool            IsOpen ()                                                           C_NE_OV;
        ESourceType     GetSourceType ()                                                    C_NE_OV;

        Bytes           Size ()                                                             C_NE_OV { return _fileSize; }

        AsyncDSRequest  ReadBlock (Bytes pos, Bytes size)                                   __NE_OV;
        AsyncDSRequest  ReadBlock (Bytes pos, void* data, Bytes dataSize, RC<> mem)         __NE_OV;

        bool            CancelAllRequests ()                                                __NE_OV;

        using AsyncRDataSource::ReadBlock;
    };
//-----------------------------------------------------------------------------



    //
    // Windows Write-only Async File
    //
    class WinAsyncWDataSource final : public AsyncWDataSource
    {
    // types
    public:
        using EFlags    = WinWFileStream::EFlags;
    private:
        using File_t    = WindowsIOService::File_t;

        static constexpr EFlags DefaultFlags    = EFlags::Unknown;


    // variables
    private:
        File_t          _file;
        const EFlags    _flags;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        WinAsyncWDataSource (const File_t &file, EFlags flags DEBUG_ONLY(, Path filename))  __NE___;

    public:
        explicit WinAsyncWDataSource (const char* filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinAsyncWDataSource (NtStringView filename, EFlags flags = DefaultFlags)   __NE___;
        explicit WinAsyncWDataSource (const String &filename, EFlags flags = DefaultFlags)  __NE___;

        explicit WinAsyncWDataSource (NtWStringView filename, EFlags flags = DefaultFlags)  __NE___;
        explicit WinAsyncWDataSource (const wchar_t* filename, EFlags flags = DefaultFlags) __NE___;
        explicit WinAsyncWDataSource (const WString &filename, EFlags flags = DefaultFlags) __NE___;

        explicit WinAsyncWDataSource (const Path &path, EFlags flags = DefaultFlags)        __NE___;

        ~WinAsyncWDataSource ()                                                             __NE_OV;

        ND_ File_t const&   Handle ()                                                       __NE___ { return _file; }


        // AsyncWDataSource //
        bool            IsOpen ()                                                           C_NE_OV;
        ESourceType     GetSourceType ()                                                    C_NE_OV;

        AsyncDSRequest  WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem)  __NE_OV;
        bool            CancelAllRequests ()                                                __NE_OV;
        RC<SharedMem>   Alloc (Bytes size)                                                  __NE_OV;

        using AsyncWDataSource::WriteBlock;
    };


} // AE::Threading

#endif // AE_PLATFORM_WINDOWS
