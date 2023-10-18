// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/TaskSystem/Promise.h"

namespace AE::Threading
{
namespace _hidden_
{

    //
    // Async Data Source Request interface
    //
    class alignas(AE_CACHE_LINE) IAsyncDataSourceRequest : public EnableRC<IAsyncDataSourceRequest>
    {
    // types
    public:
        enum class EStatus : uint
        {
            Destroyed,
            Pending,        // waiting to allocate memory
            InProgress,     // request in progress by OS
            _Finished,
            Completed,      // successfully completed
            Cancelled,      // cancelled or has error
        };

        struct Result
        {
            Bytes           pos;                    // 'pos' argument from 'ReadBlock()' or 'WriteBlock()'
            Bytes           dataSize;               // actually readn / written
            void const*     data        = null;     // if successfully completed non-null memory for read request.

            template <typename T>
            ND_ ArrayView<T>    AsArray ()  C_NE___ { return ArrayView<T>{ Cast<T>(data), usize(dataSize)/sizeof(T) }; }
        };

        struct ResultWithRC : Result
        {
            RC<>            rc;                     // to keep alive mem object for read request
        };

        using Promise_t = Promise< ResultWithRC >;


    // variables
    protected:
        Atomic<EStatus>     _status         {EStatus::Destroyed};
        Atomic<uint>        _actualSize     {0};    // readn / written


    // interface
    public:
        // returns 'true' if cancelled, 'false' if already completed/cancelled or on other error.
            virtual bool        Cancel ()                                           __NE___ = 0;

        ND_ EStatus             Status ()                                           C_NE___ { return _status.load(); }

        ND_ bool                IsCompleted ()                                      C_NE___ { return Status() == EStatus::Completed; }
        ND_ bool                IsCancelled ()                                      C_NE___ { return Status() == EStatus::Cancelled; }
        ND_ bool                IsFinished ()                                       C_NE___ { return Status() >  EStatus::_Finished; }

        ND_ virtual Result      GetResult ()                                        C_NE___ = 0;

        // Use 'promise.Then()' to process result of async IO.
        // Or use 'result = co_await request->AsPromise()'.
        ND_ virtual Promise_t   AsPromise (ETaskQueue q = ETaskQueue::Background)   __NE___ = 0;
    };

} // _hidden_

    using AsyncDSRequestResult      = Threading::_hidden_::IAsyncDataSourceRequest::ResultWithRC;
    using AsyncDSRequest            = RC< Threading::_hidden_::IAsyncDataSourceRequest >;
    using WeakAsyncDSRequest        = Threading::_hidden_::_TaskDependency< AsyncDSRequest, false >;




    //
    // Async Read-only Data Source interface
    //
    class AsyncRDataSource : public IDataSource
    {
    // types
    public:
        using ReadRequestPtr    = AsyncDSRequest;
        using Result_t          = AsyncDSRequest::Value_t::ResultWithRC;


    // interface
    public:
        AsyncRDataSource ()                                                                 __NE___ {}

        ND_ ESourceType  GetSourceType ()                                                   C_NE_OV { return ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async | ESourceType::ThreadSafe; }


        // Returns file size.
        ND_ virtual Bytes   Size ()                                                         C_NE___ = 0;


        // Read file from 'pos' to 'pos + dataSize'.
        // 'pos'        - position in the file where data will be readn.
        // 'data'       - pointer to memory where to put data.
        // 'dataSize'   - size of the 'data'.
        // 'mem'        - holds 'data' memory until it in use.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ virtual ReadRequestPtr  ReadBlock (Bytes pos, void* data,
                                               Bytes dataSize, RC<> mem)                    __NE___ = 0;


        // Read file from 'pos' to 'pos + size'.
        // 'pos'    - position in the file where data will be readn.
        // 'size'   - size of the data.
        // 'mem'    - container for memory.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ ReadRequestPtr  ReadBlock (Bytes pos, Bytes size, RC<SharedMem> mem)            __NE___
        {
            ASSERT( size <= mem->Size() );
            void*   data = mem->Data();
            return ReadBlock( pos, data, size, RVRef(mem) );
        }


        // Read file from 'pos' to 'pos + size'.
        // Memory will be allocated by internal allocator.
        // 'pos'    - position in the file where data will be readn.
        // 'size'   - size of the data.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ virtual ReadRequestPtr  ReadBlock (Bytes pos, Bytes size)                       __NE___ = 0;


        // Read file from 'pos' to end of file.
        // Memory will be allocated by internal allocator.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ ReadRequestPtr  ReadRemaining (Bytes pos)                                       __NE___ { return ReadBlock( pos, Size() - pos ); }


        // Cancel all pending IO requests.
        // Returns:
        //  'true'  if cancelled
        //  'false' if all requests already completed/cancelled or on other error.
        //
            virtual bool    CancelAllRequests ()                                            __NE___ = 0;
    };



    //
    // Async Write-only Data Source interface
    //
    class AsyncWDataSource : public IDataSource
    {
    // types
    public:
        using WriteRequestPtr   = AsyncDSRequest;
        using Result_t          = AsyncDSRequest::Value_t::ResultWithRC;


    // interface
    public:
        AsyncWDataSource ()                                                                 __NE___ {}

        ND_ ESourceType  GetSourceType ()                                                   C_NE_OV { return ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::Async | ESourceType::ThreadSafe; }


        // Allocate memory block using internal allocator.
        // Returns 'null' on error.
        //
        ND_ virtual RC<SharedMem>   Alloc (SizeAndAlign)                                    __NE___ = 0;
        ND_ RC<SharedMem>           Alloc (Bytes size)                                      __NE___ { return Alloc( SizeAndAlign{ size, 4_b }); }


        // Write data to the file.
        // 'pos'        - position in the file where data will be written.
        // 'data'       - pointer to memory with the data.
        // 'dataSize'   - size of data.
        // 'mem'        - holds 'data' memory until it in use.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ virtual WriteRequestPtr  WriteBlock (Bytes pos, const void* data,
                                                 Bytes dataSize, RC<> mem)                  __NE___ = 0;


        // Write data from 'mem' to the file.
        // 'pos'    - position in the file where data will be written.
        // 'size'   - size of data.
        // 'mem'    - container for memory.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ WriteRequestPtr  WriteBlock (Bytes pos, Bytes size, RC<SharedMem> mem)          __NE___
        {
            ASSERT( size <= mem->Size() );
            const void* data = mem->Data();
            return WriteBlock( pos, data, size, RVRef(mem) );
        }


        // Cancel all pending IO requests.
        // Returns:
        //  'true'  if cancelled
        //  'false' if all requests already completed/cancelled or on other error.
        //
            virtual bool    CancelAllRequests ()                                            __NE___ = 0;
    };



    //
    // Async Read-only Stream interface
    //
    class AsyncRStream : public IDataSource
    {
    // types
    public:
        using ReadRequestPtr    = AsyncDSRequest;
        using Result_t          = AsyncDSRequest::Value_t::ResultWithRC;
        using PosAndSize        = RStream::PosAndSize;


    // interface
    public:
        AsyncRStream ()                                                                     __NE___ {}

        ND_ ESourceType  GetSourceType ()                                                   C_NE_OV { return ESourceType::SequentialAccess | ESourceType::ReadAccess | ESourceType::Async | ESourceType::ThreadSafe; }

        ND_ virtual PosAndSize  PositionAndSize ()                                          C_NE___ = 0;


        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ virtual ReadRequestPtr  ReadSeq (void* data, Bytes dataSize, RC<> mem)          __NE___ = 0;


        // Memory will be allocated by internal allocator.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ virtual ReadRequestPtr  ReadSeq (Bytes size)                                    __NE___ = 0;


        // Cancel all pending IO requests.
        // Returns:
        //  'true'  if cancelled
        //  'false' if all requests already completed/cancelled or on other error.
        //
            virtual bool    CancelAllRequests ()                                            __NE___ = 0;
    };



    //
    // Async Write-only Stream interface
    //
    class AsyncWStream : public IDataSource
    {
    // types
    public:
        using WriteRequestPtr   = AsyncDSRequest;
        using Result_t          = AsyncDSRequest::Value_t::ResultWithRC;


    // interface
    public:
        AsyncWStream ()                                                                     __NE___ {}

        ND_ ESourceType  GetSourceType ()                                                   C_NE_OV { return ESourceType::SequentialAccess | ESourceType::WriteAccess | ESourceType::Async | ESourceType::ThreadSafe; }

        ND_ virtual Bytes   Position ()                                                     C_NE___ = 0;    // same as 'Size()'


        // Allocate memory block using internal allocator.
        // Returns 'null' on error.
        //
        ND_ virtual RC<SharedMem>   Alloc (SizeAndAlign)                                    __NE___ = 0;
        ND_ RC<SharedMem>           Alloc (Bytes size)                                      __NE___ { return Alloc( SizeAndAlign{ size, 4_b }); }


        // Write data to the file.
        // 'data'       - pointer to memory with the data.
        // 'dataSize'   - size of data.
        // 'mem'        - holds 'data' memory until it in use.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ virtual WriteRequestPtr  WriteSeq (const void* data, Bytes dataSize, RC<> mem)  __NE___ = 0;


        // Write data from 'mem' to the file.
        // 'size'   - size of data.
        // 'mem'    - container for memory.
        // Returns non-null pointer, request in pending state on success, request in canceled state on error.
        //
        ND_ WriteRequestPtr  WriteSeq (Bytes size, RC<SharedMem> mem)                       __NE___
        {
            ASSERT( size <= mem->Size() );
            const void* data = mem->Data();
            return WriteSeq( data, size, RVRef(mem) );
        }


        // Cancel all pending IO requests.
        // Returns:
        //  'true'  if cancelled
        //  'false' if all requests already completed/cancelled or on other error.
        //
            virtual bool    CancelAllRequests ()                                            __NE___ = 0;
    };


} // AE::Threading
