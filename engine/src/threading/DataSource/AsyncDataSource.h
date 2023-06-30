// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/DataSource.h"
#include "base/Memory/SharedMem.h"

#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/TaskSystem/Promise.h"

namespace AE::Threading
{
namespace _hidden_
{
    class alignas(AE_CACHE_LINE) IAsyncDataSourceRequest : public EnableRC<IAsyncDataSourceRequest>
    {
    // types
    public:
        enum class EStatus : uint
        {
            Destroyed,
            InProgress,
            _Finished,
            Completed,
            Cancelled,
        };

        struct Result
        {
            Bytes           offset;                 // 'offset' argument from 'ReadBlock()' or 'WriteBlock()'
            Bytes           dataSize;               // actually readn / written
            void const*     data        = null;     // read-only non-null memory if successfully completed, null otherwise.

            template <typename T>
            ND_ ArrayView<T>    AsArray ()  C_NE___ { return ArrayView<T>{ Cast<T>(data), usize(dataSize)/sizeof(T) }; }
        };

        struct ResultWithRC : Result
        {
            RC<IAsyncDataSourceRequest>     request;
        };

        using Promise_t = Promise< ResultWithRC >;


    // variables
    protected:
        Atomic<EStatus>     _status         {EStatus::Destroyed};
        Atomic<uint>        _actualSize     {0};    // readn / written


    // interface
    public:
        // returns 'true' if cancelled, 'false' if already complete/cancelled or on other error.
            virtual bool        Cancel ()                                       __NE___ = 0;

        ND_ EStatus             Status ()                                       C_NE___ { return _status.load(); }

        ND_ bool                IsCompleted ()                                  C_NE___ { return Status() == EStatus::Completed; }
        ND_ bool                IsCancelled ()                                  C_NE___ { return Status() == EStatus::Cancelled; }
        ND_ bool                IsFinished ()                                   C_NE___ { return Status() >  EStatus::_Finished; }

        ND_ virtual Result      GetResult ()                                    C_NE___ = 0;

        ND_ virtual Promise_t   AsPromise (ETaskQueue q = ETaskQueue::PerFrame) __NE___ = 0;
    };

} // _hidden_

    using AsyncDSRequestResult      = Threading::_hidden_::IAsyncDataSourceRequest::ResultWithRC;
    using AsyncDSRequest            = RC< Threading::_hidden_::IAsyncDataSourceRequest >;
    using WeakAsyncDSRequest        = Threading::_hidden_::_TaskDependency< AsyncDSRequest, false >;




    //
    // Async Read-only Data Source
    //

    class AsyncRDataSource : public IDataSource
    {
    // types
    public:
        using ReadRequestPtr    = AsyncDSRequest;
        using Result_t          = AsyncDSRequest::Value_t::ResultWithRC;


    // methods
    public:
        AsyncRDataSource ()                         __NE___ {}

            ESourceType     GetSourceType ()        C_NE_OV { return ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async | ESourceType::ThreadSafe; }

        ND_ virtual Bytes   Size ()                 C_NE___ = 0;

        // returns null on error
        ND_ virtual auto    ReadBlock (Bytes offset, Bytes size, RC<SharedMem> dstBlock)__NE___ -> ReadRequestPtr = 0;
        ND_ virtual auto    ReadBlock (Bytes offset, Bytes size)                        __NE___ -> ReadRequestPtr = 0;

        ND_ ReadRequestPtr  ReadRemaining (Bytes offset = 0_b)                          __NE___ { return ReadBlock( offset, Size() - offset ); }

        // returns 'true' if cancelled, 'false' if all requests already complete/cancelled or on other error.
            virtual bool    CancelAllRequests ()    __NE___ = 0;
    };



    //
    // Async Write-only Data Source
    //

    class AsyncWDataSource : public IDataSource
    {
    // types
    public:
        using WriteRequestPtr   = AsyncDSRequest;
        using Result_t          = AsyncDSRequest::Value_t::ResultWithRC;


    // methods
    public:
        AsyncWDataSource ()                     __NE___ {}

            ESourceType     GetSourceType ()    C_NE_OV { return ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::Async | ESourceType::ThreadSafe; }

        //ND_ virtual Bytes Size ()             C_NE___ = 0;

        // returns null on error
        ND_ virtual auto    WriteBlock (Bytes offset, Bytes size, RC<SharedMem> srcBlock) __NE___ -> WriteRequestPtr = 0;

        ND_ virtual auto    Alloc (Bytes size)  __NE___ -> RC<SharedMem> = 0;

        // returns 'true' if cancelled, 'false' if all requests already complete/cancelled or on other error.
            virtual bool    CancelAllRequests ()__NE___ = 0;
    };



    //
    // Async Read-only Stream
    //

    class AsyncRStream : public IDataSource
    {
    // types
    public:
        struct ReadResult
        {
            Bytes   offset;
            Bytes   readn;
        };


    // interface
    public:
        AsyncRStream () {}

            ESourceType     GetSourceType ()        C_NE_OV { return ESourceType::SequentialAccess | ESourceType::ReadAccess | ESourceType::Async | ESourceType::ThreadSafe; }

        ND_ virtual Promise<ReadResult>  ReadSeq () __NE___ = 0;
    };



    //
    // Async Write-only Stream
    //

    class AsyncWStream : public IDataSource
    {
    // types
    public:
        struct WriteResult
        {
            Bytes   offset;
            Bytes   written;
        };


    // interface
    public:
        AsyncWStream () {}

            ESourceType     GetSourceType ()            C_NE_OV { return ESourceType::SequentialAccess | ESourceType::WriteAccess | ESourceType::Async | ESourceType::ThreadSafe; }

        ND_ virtual Promise<WriteResult>  WriteSeq ()   __NE___ = 0;
    };


} // AE::Threading
