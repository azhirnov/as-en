// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	AsyncRDataSource / AsyncWDataSource and AsyncRStream / AsyncWStream must be created only as RC<> object,
	because Request may internally keep reference to it data source object.

	compatible with:
		Result	co_await request
		Result	Coro_WaitResult (request1, request2, ...)
		Result	Coro_WaitResultOrCancel (request1, ...)

	Synchronous DataSource defined in 'base' module:
	  [DataSource](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/base/DataSource/DataSource.h)

	Network DataSource defined in 'VFS' module:
	  [NetworkStorageClient](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/vfs/Network/NetworkStorageClient.h)
	  [NetworkStorageServer](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/vfs/Network/NetworkStorageServer.h)
*/

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{
	class AsyncDSRequestDependencyManager;
}

namespace AE::_Coro_
{

	//
	// Async Data Source Request interface
	//
	class IAsyncDataSourceRequest : public EnableRC<IAsyncDataSourceRequest>
	{
	// types
	public:
		enum class EStatus : uint
		{
			Destroyed,
			Pending,		// waiting to allocate memory
			InProgress,		// request in progress by OS

			_Finished,
			Completed,		// successfully completed
			Canceled,		// cancelled or has error
		};

		struct Result
		{
		// variables
			Bytes			pos;					// 'pos' argument from 'ReadBlock()' or 'WriteBlock()'.
			Bytes			dataSize;				// actually readn / written.
			void const*		data		= null;		// non-null pointer if read request is successfully completed,
													// always null for write request.
			RC<>			rc;						// to keep alive mem object for read request,
													// same as 'RC<> mem' which passed to 'ReadBlock()' or 'ReadSeq()',
													// always null for write request.
			EStatus			status		= EStatus::Canceled;

		// methods
			template <typename T>
			ND_ ArrayView<T>	AsArray ()	C_NE___	{ return ArrayView<T>{ Cast<T>(data), usize(dataSize)/sizeof(T) }; }
			ND_ FastRStream		AsStream ()	C_NE___	{ NonNull( data );  return FastRStream{ data, data + dataSize }; }

			ND_ explicit operator bool ()	C_NE___	{ return status == EStatus::Completed; }
		};
		
		struct CanceledRequest;

	protected:
		using TaskDependency	= AsyncTaskImpl::TaskDependencyManagerApi::TaskDependency;
		using Dependencies_t	= FixedArray< TaskDependency, 4 >;		// TODO: use 'AsyncTask::OutputChunk'


	// variables
	protected:
		Atomic<EStatus>			_status			{EStatus::Destroyed};
		AtomicBytes<Bytes32u>	_actualSize		{0_b};	// read / written

		SpinLock				_depsGuard;
		Dependencies_t			_deps;


	// interface
	public:
		// Returns 'true' if cancelled, 'false' if already completed/cancelled or on error.
			virtual bool		Cancel ()				__NE___	{ return false; }

		ND_ virtual Result		GetResult ()			C_NE___ { return {}; }

		ND_ EStatus				Status ()				C_NE___ { return _status.load(); }

		ND_ bool				IsCompleted ()			C_NE___	{ return Status() == EStatus::Completed; }
		ND_ bool				IsCancelled ()			C_NE___	{ return Status() == EStatus::Canceled; }
		ND_ bool				IsFinished ()			C_NE___	{ return Status() >  EStatus::_Finished; }

	protected:
		IAsyncDataSourceRequest ()						__NE___ = default;

		friend class Base::StaticRC<IAsyncDataSourceRequest>;
		__Cx__ explicit IAsyncDataSourceRequest (EStatus status) __NE___ : EnableRC{_ConstInitStaticRC(0)}, _status{status} {}

		friend class Threading::AsyncDSRequestDependencyManager;
		ND_ bool  _AddOnCompleteDependency (AsyncTaskImpl &task, Bool isStrong)	__NE___;
			void  _SetDependencyCompleteStatus (bool complete)					__NE___;
	};

	
	struct IAsyncDataSourceRequest::CanceledRequest
	{
		static constinit StaticRC<IAsyncDataSourceRequest>	s_canceled;
	};


	//
	// Async DataSource Request Awaiter
	//
	struct IAsyncDataSourceRequest_Awaiter
	{
	private:
		IAsyncDataSourceRequest &	_req;

	public:
		IAsyncDataSourceRequest_Awaiter (IAsyncDataSourceRequest &req)	__NE___	: _req{req} {}

		ND_ bool	await_ready ()										C_NE___	{ return _req.IsFinished(); }	// call 'await_suspend()' to get coroutine handle
		ND_ auto	await_resume ()										__NE___	{ return _req.GetResult(); }	// return result of 'co_await'

		template <typename P>
		ND_ bool	await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, Tuple{_req.GetRC()} );
		}
	};
	
	template <>
	struct CoroTraits< RC< _Coro_::IAsyncDataSourceRequest >>
	{
		using type		= RC< _Coro_::IAsyncDataSourceRequest >;
		using Result	= IAsyncDataSourceRequest::Result;

	//	Nd__IF static Result*		GetResultPtr (const type &p)	__NE___	// not supported
		Nd__IF static Result		GetResultCopy (const type &p)	__NE___ { return p->GetResult(); }
		Nd__IF static Result		MoveResult (const type &p)		__NE___ { return p->GetResult(); }
	};

} // AE::_Coro_


namespace AE::Threading
{
	using EAsyncDSRequestStatus	= _Coro_::IAsyncDataSourceRequest::EStatus;
	using AsyncDSRequest		= RC< _Coro_::IAsyncDataSourceRequest >;
	using AsyncDSRequestResult	= _Coro_::IAsyncDataSourceRequest::Result;
	using WeakAsyncDSRequest	= _Coro_::_TaskDependency< AsyncDSRequest, False{"weak"} >;



	//
	// Async Read-only Data Source interface
	//
	class AsyncRDataSource : public IDataSource
	{
	// types
	public:
		using ReadRequestPtr	= AsyncDSRequest;


	// interface
	public:
		ND_	ESourceType  GetSourceType ()	C_NE_OV	{ return ESourceType::RandomAccess | ESourceType::ReadAccess | ESourceType::Async | ESourceType::ThreadSafe; }


		// Returns file size.
		// If 'GetSourceType()' doesn't returns 'FixedSize'
		// size may be unknown and 'UMax' will be returned.
		//
		ND_ virtual Bytes	Size ()																	C_NE___ = 0;


		// Read file from 'pos' to 'pos + dataSize'.
		//	'pos'		- position in the file where data will be readn.
		//	'data'		- pointer to memory where to put data.
		//	'dataSize'	- size of the 'data'.
		//	'mem'		- holds 'data' memory until it in use.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		// Actually readn data size may be less than 'size'.
		//
		ND_ virtual ReadRequestPtr  ReadBlock (Bytes pos, OUT void* data, Bytes dataSize, RC<> mem)	__NE___ = 0;


		// Read file from 'pos' to 'pos + size'.
		//	'pos'	- position in the file where data will be readn.
		//	'size'	- size of the data.
		//	'mem'	- container for memory.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		// Actually readn data size may be less than 'size'.
		//
		ND_ ReadRequestPtr  ReadBlock (Bytes pos, Bytes size, RC<SharedMem> mem)					__NE___
		{
			ASSERT( mem and size <= mem->Size() );
			void*	data = mem ? mem->Data() : null;
			return ReadBlock( pos, OUT data, size, RVRef(mem) );
		}


		// Read file from 'pos' to 'pos + size'.
		// Memory will be allocated by internal allocator.
		//	'pos'	- position in the file where data will be readn.
		//	'size'	- size of the data.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		// Actually readn data size may be less than 'size'.
		//
		ND_ virtual ReadRequestPtr  ReadBlock (Bytes pos, Bytes size)								__NE___ = 0;


		// Read file from 'pos' to end of file.
		// Memory will be allocated by internal allocator.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ ReadRequestPtr	ReadRemaining (Bytes pos)												__NE___	{ return ReadBlock( pos, Size() - pos ); }


		// Cancel all pending IO requests.
		// Returns:
		//	'true'  if cancelled
		//	'false' if all requests already completed/cancelled or on other error.
		//
			virtual bool	CancelAllRequests ()													__NE___ = 0;
	};



	//
	// Async Write-only Data Source interface
	//
	class AsyncWDataSource : public IDataSource
	{
	// types
	public:
		using WriteRequestPtr	= AsyncDSRequest;


	// interface
	public:
		ND_	ESourceType  GetSourceType ()	C_NE_OV	{ return ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::Async | ESourceType::ThreadSafe; }


		// Allocate memory block using internal allocator.
		// Returns 'null' on error.
		//
		ND_ virtual RC<SharedMem>	Alloc (SizeAndAlign)									__NE___ = 0;
		ND_ RC<SharedMem>			Alloc (Bytes size)										__NE___ { return Alloc( SizeAndAlign{ size, DefaultAllocatorAlign }); }


		// Write data to the file.
		//	'pos'		- position in the file where data will be written.
		//	'data'		- pointer to memory with the data.
		//	'dataSize'	- size of data.
		//	'mem'		- holds 'data' memory until it in use.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ virtual WriteRequestPtr  WriteBlock (Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___ = 0;


		// Write data from 'mem' to the file.
		//	'pos'	- position in the file where data will be written.
		//	'size'	- size of data.
		//	'mem'	- container for memory.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ WriteRequestPtr  WriteBlock (Bytes pos, Bytes size, RC<SharedMem> mem)			__NE___
		{
			ASSERT( mem and size <= mem->Size() );
			const void*	data = mem ? mem->Data() : null;
			return WriteBlock( pos, data, size, RVRef(mem) );
		}


		// Cancel all pending IO requests.
		// Returns:
		//	'true'  if cancelled
		//	'false' if all requests already completed/cancelled or on other error.
		//
			virtual bool	CancelAllRequests ()											__NE___ = 0;
	};



	//
	// Async Read-only Stream interface
	//
	class AsyncRStream : public IDataSource
	{
	// types
	public:
		using ReadRequestPtr	= AsyncDSRequest;
		using PosAndSize		= RStream::PosAndSize;


	// interface
	public:
		ND_	ESourceType  GetSourceType ()	C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::ReadAccess | ESourceType::Async | ESourceType::ThreadSafe; }

		// Returns:
		//	- current position in stream, this value must be valid.
		//	- file size if it is known, 'UMax' otherwise.
		//
		ND_ virtual PosAndSize	PositionAndSize ()											C_NE___ = 0;


		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ virtual ReadRequestPtr	ReadSeq (OUT void* data, Bytes dataSize, RC<> mem)		__NE___ = 0;


		// Memory will be allocated by internal allocator.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ virtual ReadRequestPtr	ReadSeq (Bytes size)									__NE___ = 0;


		// Cancel all pending IO requests.
		// Returns:
		//	'true'  if cancelled
		//	'false' if all requests already completed/cancelled or on other error.
		//
			virtual bool	CancelAllRequests ()											__NE___ = 0;
	};



	//
	// Async Write-only Stream interface
	//
	class AsyncWStream : public IDataSource
	{
	// types
	public:
		using WriteRequestPtr	= AsyncDSRequest;


	// interface
	public:
		ND_	ESourceType  GetSourceType ()	C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::WriteAccess | ESourceType::Async | ESourceType::ThreadSafe; }


		// Returns current position in stream.
		// This value should be equal to the file size.
		//
		ND_ virtual Bytes	Position ()														C_NE___ = 0;


		// Allocate memory block using internal allocator.
		// Returns 'null' on error.
		//
		ND_ virtual RC<SharedMem>	Alloc (SizeAndAlign)									__NE___ = 0;
		ND_ RC<SharedMem>			Alloc (Bytes size)										__NE___ { return Alloc( SizeAndAlign{ size, DefaultAllocatorAlign }); }


		// Write data to the file.
		//	'data'		- pointer to memory with the data.
		//	'dataSize'	- size of data.
		//	'mem'		- holds 'data' memory until it in use.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ virtual WriteRequestPtr  WriteSeq (const void* data, Bytes dataSize, RC<> mem)	__NE___ = 0;


		// Write data from 'mem' to the file.
		//	'size'	- size of data.
		//	'mem'	- container for memory.
		// Returns non-null pointer, request in pending state on success, request in canceled state on error.
		//
		ND_ WriteRequestPtr  WriteSeq (Bytes size, RC<SharedMem> mem)						__NE___
		{
			ASSERT( mem and size <= mem->Size() );
			const void*	data = mem ? mem->Data() : null;
			return WriteSeq( data, size, RVRef(mem) );
		}


		// Cancel all pending IO requests.
		// Returns:
		//	'true'  if cancelled
		//	'false' if all requests already completed/cancelled or on other error.
		//
			virtual bool	CancelAllRequests ()											__NE___ = 0;
	};



	//
	// Async Data Source Request Dependency Manager
	//
	class AsyncDSRequestDependencyManager final : public ITaskDependencyManager
	{
	// methods
	public:

		// ITaskDependencyManager //
		bool  Resolve (AnyTypeCRef dep, Task &task, Bool defaultIsStrongDep) __NE_OV;

	private:
		friend class TaskScheduler;
		AsyncDSRequestDependencyManager () __NE___ = default;
	};



/*
=================================================
	operator co_await (AsyncDSRequest)
=================================================
*/
	inline auto  operator co_await (const AsyncDSRequest &req) __NE___
	{
		return _Coro_::IAsyncDataSourceRequest_Awaiter{ *req };
	}

	inline auto  operator co_await (const WeakAsyncDSRequest &req) __NE___
	{
		return _Coro_::IAsyncDataSourceRequest_Awaiter{ *req };
	}
	
	forceinline auto  TaskScheduler::GetCanceledDSRequest () __NE___
	{
		return _Coro_::IAsyncDataSourceRequest::CanceledRequest::s_canceled.GetRC();
	}

} // AE::Threading

namespace AE::ImportCoroutines
{
	using AE::Threading::operator co_await;
}
