// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	--- Task Dependencies ---

	AsyncTask			- reference counted pointer to any task

	ArrayView<AsyncTask>
	WeakDep
	StrongDep
	WeakDepArray, ArrayView<WeakDep>
	StrongDepArray, ArrayView<StrongDep>

	<custom>			- use Scheduler().RegisterDependency< custom >(...) to register dependency type,
						  then use 'co_await <custom>' to wait for completion.

	Weak dependency		- if task in weak dependency is canceled then dependent task will be executed.
						  Call chain:  'task' depends on 'weak',  weak.Cancel(),  weak.OnCancel(),  task.Run().

	Strong dependency	- if task in strong dependency is canceled then dependent task will be canceled.
						  Call chain:  'task' depends on 'strong',  strong.Cancel(),  strong.OnCancel(),  task.OnCancel().


	--- Coroutines ---

	AsyncCoro			- coroutine must be added to task scheduler, will be executed when all dependencies are complete.
						  If at least one of strong dependencies is cancelled then coroutine will never resume (fast cancellation),
						  only 'OnCancel()' method will be called to cleanup.

	InlineCoro<>		- same as 'AsyncCoro' but coroutine will start immediately until 'co_await' will add task to scheduler for delayed execution.
						  Should be used only as return type, to hold pointer use 'AsyncCoro' or 'AsyncTask' type.

	ScheduledCoro<>		- same as 'AsyncCoro', implicitly added to Scheduler.

	UncancellableCoro	- coroutine will always resume, even if strong dependency is cancelled, even if explicitly cancelled by 'Scheduler().Cancel()'.
						  Use 'Coro_IsCanceled' to handle canceled state. Only 'Coro_Error()' and 'co_return' can stop coroutine resuming.
						  Should be used in very rare cases, prefer to use 'Coro_WaitResult()' which return completion status.

	Promise<>			- same as 'AsyncCoro' but can return payload.

	InlinePromise<>		- same as 'InlineCoro' but can return payload.
						  Should be used only as return type, to hold pointer use 'Promise<>' type.

	ScheduledPromise<>	- same as 'Promise', implicitly added to Scheduler.

	DeferResult<>		- it is not a coroutine, it holds result and returns when input dependencies are complete and result can be accessed.
						  Should be used to sync access to memory range.

	TODO:
	DeferPromise<>		- lazy promise, will start execution immediately when used with 'co_await'. Will be added to queue when use 'co_await' inside.

	bool	Coro_IsCanceled
	UserApi	Coro_Get()
	void	Coro_Error ()				- stop execution and set 'Error' state.
	void	Coro_NextQueue (ETaskQueue)	- set queue type for next resume, has no effect if not suspended.

		-- with fast cancellation --

	void	Coro_Continue ()			- stop execution and add to queue without any dependency.
	void	Coro_Continue (deps)		- stop execution and add to queue with dependencies,
										  will continue execution only when all strong dependencies are complete and
										  all weak dependencies complete or cancelled, otherwise coroutine will be cancelled.
										  Pointer to the dependency will be reset to null.
	void	Coro_ContinueInQueue (ETaskQueue) - stop execution and add coroutine to another queue.

	auto	Coro_WaitResultOrCancel (deps) - same as 'Coro_Continue', return promise result.

	void	co_await task				- same as 'Coro_Continue(task)' but pointers are not changed.
	auto	co_await promise			- same as 'Coro_WaitResultOrCancel(promise)' but pointers are not changed.

		-- with successfully complete bit --

	auto	Coro_WaitResult (deps)		- stop execution and add to queue with dependencies,
										  will continue execution only when all dependencies are complete or cancelled.
										  For task will return bitfield with successfully completed bits,
										  for promise will return copy of their result and bitfield.
										  Pointer to the dependency will be reset to null.
										  Array types are not supported.
	auto	Coro_WaitResultRef (deps)	- same as 'Coro_WaitResult()' but use reference to promise result, dependencies will not reset to null.
	auto	Coro_WaitUniqueResult (dep)	- same as 'Coro_WaitResult()' but result will be moved.
										  May have data race if promise used in multiple coroutines.

	--- C++ docs ---

	https://en.cppreference.com/w/cpp/language/coroutines.html
	https://lewissbaker.github.io/2022/08/27/understanding-the-compiler-transform
*/

#pragma once

#include "threading/Primitives/SpinLock.h"
#include "threading/TaskSystem/TaskProfiler.h"
#include "threading/TaskSystem/EThread.h"

#ifdef AE_CFG_RELEASE
#	define AE_ENABLE_TASK_NAME		0
#else
#	define AE_ENABLE_TASK_NAME		1
#endif

namespace AE::Threading
{
	enum class ETaskStatus : uint
	{
		Initial,
		Pending,		// task has been added to the queue and is waiting until input dependencies complete
		InProgress,		// task was acquired by thread
		Cancellation,	// task required to be canceled, may be still executing
		Continue,		// task will be returned to scheduler

		_Finished,
		Completed,		// successfully completed

		_Interrupted,
		Canceled,		// task was externally canceled
		Error,			// task has internal error
	};
}

namespace AE::_Coro_
{
	using namespace AE::Threading;

	static constexpr ETaskQueue  c_DefaultTaskQueue = ETaskQueue::PerFrame;


	template <typename T>				class BaseCoro;
	template <typename T>				struct Promise_CoroTraits;
	template <typename T, ETaskQueue>	class ScheduledInlinePromise;
	template <ETaskQueue>				class ScheduledInlineCoro;

	template <typename T>				class CoroAwaiter_WaitResultCopyOrCancel_Array;
	template <typename ...Deps>			class CoroAwaiter_WaitResultCopyOrCancel;

	class CoroAwaiterImpl;
	class AsyncTaskCoro_IsCanceled;
	class AsyncTaskCoro_Get;
	class AsyncTaskCoro_NextQueue;
	class CoroAwaiter_Continue;

	class AsyncTaskCoro_Error {};
	class AsyncTaskCoro_Success {};
	enum class MakeDeferResult {};

	struct AsyncTaskCoro_ChangeQueue
	{
		ETaskQueue	value;
	};



	//
	// Async Task
	//
	class AsyncTaskImpl : public EnableRC< AsyncTaskImpl >
	{
	// types
	public:
		enum class EFlags : ubyte
		{
			Unknown				= 0,
			RunCancelled		= 1 << 0,	// disable fast cancellation,
											// if not set and '_willBeCanceled == true' and task is not executing
											// then task will cancel immediately when processed.
											// if set then task will always start/resume, user must check 'Coro_IsCanceled' status.

			DoNotRun			= 1 << 1,	// coroutine will never start.
											// if all input dependency are complete then task marked as completed.
			_BITOPS_
		};

		// Extra: 0 or 1 - weak or strong dependency
		using TaskDependency = PackedRC< AsyncTaskImpl, 2 >;

	protected:
		using EStatus = ETaskStatus;

	private:
		struct alignas(16) OutputChunk
		{
		// types
			static constexpr uint	_chunkSize	= 7;
			static constexpr usize	_mask		= CT_ToBitMask< usize, CeilIntLog2( _chunkSize )>;

			using TaskArray	= StaticArray< TaskDependency, _chunkSize >;

		// variables
			usize			_packed		= 0;	// {OutputChunk*, uint:4}
			TaskArray		tasks		{};

		// methods
			OutputChunk ()									__NE___ {}

				void			Init ()						__NE___;

			ND_ uint			Count ()					C_NE___	{ return _packed & _mask; }
			ND_ OutputChunk*	Next ()						C_NE___	{ return std::launder( BitCast<OutputChunk *>( _packed & ~_mask )); }

				void			SetCount (uint value)		__NE___	{ _packed = (_packed & ~_mask) | (value & _mask);				ASSERT( value == Count() ); }
				void			SetNext (OutputChunk* ptr)	__NE___	{ _packed = (_packed & _mask) | (BitCast<usize>(ptr) & ~_mask);	ASSERT( ptr == Next() ); }
		};

		StaticAssert64( sizeof(OutputChunk) == 64 );

		using CoroHandle_t	= std::coroutine_handle< AsyncTaskImpl >;
		using DbgString_t	= std::unique_ptr<char[]>;

	public:
		class TaskSchedulerApi;
		class LfTaskQueueApi;
		class CoroutineApi;
		struct UserApi;
		struct CanceledTask;
		struct TaskDependencyManagerApi;

		enum class NullResult : char {};
		static const NullResult		_nullResult {};


	// variables
	private:
		Atomic< int >					_waitCount			{0};					// 0 - all complete, >0 - has uncomplete dependencies
		Atomic< EStatus >				_status				{EStatus::Initial};
		Atomic< bool >					_willBeCanceled		{false};				// 'true' - allow to fast cancel.
		const EFlags					_flags				{Default};
		ETaskQueue						_queueType			{ETaskQueue::PerFrame};

		#if AE_ENABLE_TASK_NAME
		  protected:
			Atomic<bool>				_isRunning			{false};
			DbgString_t					_dbgName;
		  private:
			uint						_resumeCount		{0};
		#endif

		PtrWithSpinLock< OutputChunk >	_output				{null};

		PROFILE_ONLY(
			RC<ITaskProfiler>			_profiler;
		)


	// methods
	public:
		~AsyncTaskImpl ()											__NE_OV;

		ND_ ETaskQueue	QueueType ()								C_NE___	{ return _queueType; }

		ND_ EStatus		Status ()									C_NE___	{ return _status.load(); }

		ND_ bool		IsInQueue ()								C_NE___	{ return Status() <  EStatus::_Finished; }
		ND_ bool		IsFinished ()								C_NE___	{ return Status() >  EStatus::_Finished; }		// status: Completed / Error / Canceled
		ND_ bool		IsInterrupted ()							C_NE___	{ return Status() >  EStatus::_Interrupted; }	// status: Error / Canceled
		ND_ bool		IsCompleted ()								C_NE___	{ return Status() == EStatus::Completed; }		// status: Completed
		ND_ bool		IsDummy ()									C_NE___;

	  #if AE_ENABLE_TASK_NAME
		ND_ StringView	DbgName ()									C_NE___	{ return _dbgName ? StringView{_dbgName.get()} : StringView{}; }
		ND_ bool		DbgIsRunning ()								C_NE___	{ return _isRunning.load(); }
	  #else
		ND_ StringView	DbgName ()									C_NE___	{ return Default; }
	  #endif

		// Coroutine //
		ND_ auto		final_suspend ()							C_NE___	{ return std::suspend_always{}; }				// must not be 'suspend_never'

		ND_ auto		yield_value (AsyncTaskCoro_Error)			__NE___	{ _Error();  return std::suspend_always{}; }
		ND_ auto		yield_value (AsyncTaskCoro_ChangeQueue)		__NE___;

			void		unhandled_exception ()						__NE___	{ _Error(); }


		// Execute task.
		// Can be executed multiple times if used 'co_await'.
		//
			void		Run (OUT bool& rerun)						__NE___;

	protected:
		explicit AsyncTaskImpl (EFlags flags = Default)				__NE___;
		AsyncTaskImpl (ETaskQueue queue, EFlags flags)				__NE___ : AsyncTaskImpl{flags} { _queueType = queue; }

		// Executed in the same thread as 'Run()' was/could be executed.
		// Status before: 'Cancellation', status after: 'Canceled'.
		// You can free some resources here, otherwise they will be released later,
		// when all reference to this task will be nulled.
		//
			virtual void  OnCancel ()								__NE___	{ ASSERT( not _isRunning.load() );  ASSERT( Status() == EStatus::Cancellation ); }

		// Only for debugging!
			void  _DbgSet (EStatus status)							__NE___;

		// Only during initialization or while executing
			void  _SetQueueType (ETaskQueue type)					__NE___;

		#if AE_ENABLE_TASK_NAME
			void  _SetDebugName (StringView dbgName)				__NE___;
			void  _SetDebugName (const SourceLoc &loc)				__NE___;
		#endif

		// Only in constructor!
			void  _MakeCompletedUnsafe ()							__NE___;

		// Allowed anywhere, before enqueue.
			void  _MakeCompletedSafe ()								__NE___;

			bool  _ResetCancellationState ()						__NE___;

		// For inline coroutines
			void  _Inline_OnStart ()								__NE___;
			void  _Inline_MakeCompletedOnReturn ()					__NE___;

	private:
		friend class Base::StaticRC<AsyncTaskImpl>;
		__Cx__ explicit AsyncTaskImpl (EStatus status)				__NE___ : EnableRC{_ConstInitStaticRC(0)}, _waitCount{0}, _status{status}, _queueType{ETaskQueue::Background} {}

			void  _ReleaseObject ()									__NE_OF;
			bool  _SetCancellationState ()							__NE___;

		// Call this methods only inside 'Run()' method.
			void  _Error ()											__NE___;

			template <typename ...Deps>
		ND_ bool  _Continue (const Tuple<Deps...> &,
							 Bool defaultIsStrongDep = True{})		__NE___;

		// Call this methods only after 'Run()' method.
			void  _OnFinish (OUT bool& rerun)						__NE___;
			void  _Cancel ()										__NE___;
			void  _FreeOutputChunks (Bool isCanceled)				__NE___;

	//	Nd__IF Atomic<int>&  _WaitCounter ()						__NE___	{ return std::launder< Atomic<int> &>(_unused); }	// TODO
	};
//-----------------------------------------------------------------------------



	struct AsyncTaskImpl::CanceledTask
	{
		static constinit StaticRC<AsyncTaskImpl>	s_canceled;
	};


	struct AsyncTaskImpl::TaskDependencyManagerApi
	{
		using TaskDependency	= AsyncTaskImpl::TaskDependency;

		____IF static auto		IncWaitCounter (AsyncTaskImpl &self)								__NE___	{ return self._waitCount.fetch_add( 1 ); }

			   static void		SetDependencyCompletionStatus (AsyncTaskImpl &task, Bool isCanceled = False{}) __NE___;
	};


	class AsyncTaskImpl::TaskSchedulerApi
	{
		friend class Threading::TaskScheduler;

		using OutputChunk		= AsyncTaskImpl::OutputChunk;
		using TaskDependency	= AsyncTaskImpl::TaskDependency;

		DEBUG_ONLY( ND_ static slong  AsyncTaskTotalCount ()										__NE___;)

		____IF static auto			IncWaitCounter (AsyncTaskImpl &self)							__NE___	{ return self._waitCount.fetch_add( 1 ); }
		Nd__IF static auto			DecWaitCounter (AsyncTaskImpl &self)							__NE___	{ return self._waitCount.fetch_sub( 1 ); }
		____IF static void			SetWaitCounter (AsyncTaskImpl &self, int value)					__NE___	{ self._waitCount.store( value ); }
		Nd__IF static auto			GetWaitCounter (const AsyncTaskImpl &self)						__NE___	{ return self._waitCount.load(); }

		Nd__IF static bool			SetCancellationState (AsyncTaskImpl &self)						__NE___	{ return self._SetCancellationState(); }
		____IF static void			CancelAsDependency (AsyncTaskImpl &self)						__NE___	{ self._willBeCanceled.store( true ); }
		Nd__IF static bool			SetPending (AsyncTaskImpl &self, INOUT EStatus &expected)		__NE___	{ return self._status.CAS( INOUT expected, EStatus::Pending ); }
		PROFILE_ONLY( static void	SetProfiler (AsyncTaskImpl &self, RC<ITaskProfiler> prof)		__NE___	{ if ( prof ) { self._profiler = prof;  prof->Enqueue( self ); }})
		Nd__IF static auto&			OutputChunkRef (AsyncTaskImpl &self)							__NE___	{ return self._output; }
		Nd__IF static auto			LoadWaitCount (const AsyncTaskImpl &self)						__NE___	{ return self._waitCount.load(); }
		Nd__IF static bool			IsFastCancellation (const AsyncTaskImpl &self)					__NE___ { return self._willBeCanceled.load() and NoBits( self._flags, EFlags::RunCancelled ); }
		____IF static void			SetQueueType (AsyncTaskImpl &self, ETaskQueue type)				__NE___	{ self._SetQueueType( type ); }

		#if AE_ENABLE_TASK_NAME
			   static void			Init (AsyncTaskImpl&, ETaskQueue, StringView, const SourceLoc&)	__NE___;
		#else
			   static void			Init (AsyncTaskImpl &self, ETaskQueue queue)					__NE___	{ if ( queue != Default ) self._SetQueueType( queue ); }
		#endif
	};


	class AsyncTaskImpl::LfTaskQueueApi
	{
		friend class Threading::LfTaskQueue;

		using EFlags	= AsyncTaskImpl::EFlags;

		Nd__IF static bool			WillBeCanceled (const AsyncTaskImpl &self)						__NE___ { return self._willBeCanceled.load(); }
		Nd__IF static EFlags		GetFlags (const AsyncTaskImpl &self)							__NE___ { return self._flags; }
		Nd__IF static bool			InputDepsAreNotFinished (AsyncTaskImpl &self)					__NE___	{ return self._waitCount.load() > 0; }
		____IF static void			Cancel (AsyncTaskImpl &self)									__NE___	{ return self._Cancel(); }
		Nd__IF static bool			SetInProgress (AsyncTaskImpl &, bool cancel, INOUT EStatus &)	__NE___;
			   static void			MakeComplete (AsyncTaskImpl &self)								__NE___;
	};


	class AsyncTaskImpl::CoroutineApi
	{
		template <typename T> friend class BaseCoro;
		template <typename T> friend class CoroAwaiter_WaitResultCopyOrCancel_Array;
		template <typename ...Deps> friend class CoroAwaiter_WaitResultCopyOrCancel;
		friend class CoroAwaiterImpl;
		friend class AsyncTaskCoro_IsCanceled;
		friend class AsyncTaskCoro_NextQueue;
		friend class CoroAwaiter_Continue;

		template <typename ...Deps>
		Nd__IF static bool			Continue (AsyncTaskImpl &self, const Tuple<Deps...> &deps,
											  Bool defaultIsStrongDep)								__NE___	{ return self._Continue( deps, defaultIsStrongDep ); }
		Nd__IF static bool			IsCanceled (AsyncTaskImpl &self)								__NE___	{ ASSERT( self._isRunning.load() );  return self.Status() == EStatus::Cancellation; }
		____IF static void			SetCancellationState (AsyncTaskImpl &self)						__NE___	{ self._SetCancellationState(); }
		____IF static void			SetQueueType (AsyncTaskImpl &self, ETaskQueue type)				__NE___	{ self._SetQueueType( type ); }
		Nd__IF static bool			IsRunCancelled (AsyncTaskImpl &self)							__NE___	{ return AllBits( self._flags, EFlags::RunCancelled ); }
	};


	struct AsyncTaskImpl::UserApi
	{
	private:
		AsyncTaskImpl const&	_self;

	public:
		____IF explicit UserApi (AsyncTaskImpl const* ptr)	__NE___ : _self{*ptr} {}

		Nd__IF ETaskQueue	QueueType ()					C_NE___	{ return _self.QueueType(); }
		Nd__IF EStatus		Status ()						C_NE___	{ return _self.Status(); }

		Nd__IF bool			IsInQueue ()					C_NE___	{ return _self.IsInQueue(); }
		Nd__IF bool			IsFinished ()					C_NE___	{ return _self.IsFinished(); }
		Nd__IF bool			IsInterrupted ()				C_NE___	{ return _self.IsInterrupted(); }
		Nd__IF bool			IsCompleted ()					C_NE___	{ return _self.IsCompleted(); }

		DEBUG_ONLY(
			ND_ StringView	DbgName ()						C_NE___	{ return _self.DbgName(); }
		)
	};



	forceinline bool  AsyncTaskImpl::IsDummy () C_NE___
	{
		return this == CanceledTask::s_canceled.operator->();
	}


	forceinline bool  AsyncTaskImpl::LfTaskQueueApi::SetInProgress (AsyncTaskImpl &self, bool cancel, INOUT EStatus &expected) __NE___
	{
		const EStatus	new_status = cancel ? EStatus::Cancellation : EStatus::InProgress;
		for (;;)
		{
			if ( expected != EStatus::Pending )
				break;

			if_likely( self._status.CAS( INOUT expected, new_status ))
				return true;
		}
		return false;
	}


	forceinline void  AsyncTaskImpl::LfTaskQueueApi::MakeComplete (AsyncTaskImpl &self) __NE___
	{
		ASSERT( self._status.load() == EStatus::InProgress );

		bool	rerun;
		self._OnFinish( OUT rerun );

		ASSERT( not rerun );
		ASSERT( self._status.load() == EStatus::Completed );
	}
//-----------------------------------------------------------------------------



	//
	// Base Coroutine
	//
	template <typename PromiseType>
	class BaseCoro
	{
		template <typename T>
		friend class BaseCoro;

	// types
	public:
		using promise_type	= PromiseType;
		using CoroHandle_t	= std::coroutine_handle< AsyncTaskImpl >;
		using Self			= BaseCoro< PromiseType >;


	// variables
	private:
		AsyncTask	_coro;


	// methods
	public:
		BaseCoro ()										__NE___ : _coro{ AsyncTaskImpl::CanceledTask::s_canceled.GetRC() } {}
		BaseCoro (Default_t)							__NE___ : BaseCoro{} {}
		BaseCoro (std::nullptr_t)						__NE___ {}

		// explicit cast
		explicit BaseCoro (AsyncTaskImpl &p)			__NE___ : _coro{ p.GetRC<AsyncTaskImpl>() } {}
		explicit BaseCoro (CoroHandle_t handle)			__NE___ : _coro{ handle.promise().GetRC<AsyncTaskImpl>() } {}
		explicit BaseCoro (AsyncTaskImpl* ptr)			__NE___ : _coro{ ptr } {}

		template <typename T>
		  requires( T::template _AllowExplicitCast<T> )
		explicit BaseCoro (const BaseCoro<T> &other)	__NE___ : _coro{other._coro} {}

		template <typename T>
		  requires( T::template _AllowExplicitCast<T> )
		explicit BaseCoro (BaseCoro<T> &&other)			__NE___ : _coro{RVRef(other._coro)} {}


		// implicit cast
		template <typename T>
		  requires( T::template _AllowImplicitCast<T> )
		BaseCoro (const BaseCoro<T> &other)				__NE___ : _coro{other._coro} {}

		template <typename T>
		  requires( T::template _AllowImplicitCast<T> )
		BaseCoro (BaseCoro<T> &&other)					__NE___ : _coro{RVRef(other._coro)} {}


		BaseCoro (Self &&)								__NE___ = default;
		BaseCoro (const Self &)							__NE___ = default;

		Self&  operator = (std::nullptr_t)				__NE___ { _coro = null;  return *this; }
		Self&  operator = (Default_t)					__NE___ { _coro = AsyncTaskImpl::CanceledTask::s_canceled.GetRC();  return *this; }
		Self&  operator = (Self &&)						__NE___ = default;
		Self&  operator = (const Self &)				__NE___ = default;

		ND_ explicit operator bool ()					C_NE___	{ return bool{_coro}; }


		// implicit cast to AsyncTask
		template <typename T = PromiseType>
			requires( T::_allowImplicitCastToAsyncTask )
		ND_ operator AsyncTask ()						CrNE___	{ return _coro; }

		template <typename T = PromiseType>
			requires( T::_allowImplicitCastToAsyncTask )
		ND_ operator AsyncTask ()						rvNE___	{ return RVRef(_coro); }


		// explicit cast to AsyncTask
		template <typename T = PromiseType>
			requires( not T::_allowImplicitCastToAsyncTask )
		ND_ explicit operator AsyncTask ()				CrNE___	{ return _coro; }

		template <typename T = PromiseType>
			requires( not T::_allowImplicitCastToAsyncTask )
		ND_ explicit operator AsyncTask ()				rvNE___	{ return RVRef(_coro); }


		ND_ auto&  operator * ()						C_NE___	{ return *_coro; }
		ND_ auto*  operator -> ()						C_NE___	{ return _coro.operator->(); }

		template <typename T>
		ND_ bool  operator == (const T &rhs)			C_NE___ { return _coro == rhs; }

		ND_ PromiseType*	SafeCast ()					C_NE___	{ return _coro and not _coro->IsDummy() ? Cast<PromiseType>( _coro.get() ) : null; }
		ND_ PromiseType*	UnsafeCast ()				C_NE___	{ ASSERT( _coro and not _coro->IsDummy() );  return Cast<PromiseType>( _coro.get() ); }
		ND_ RC<PromiseType>	CastRC ()					CrNE___	{ return UnsafeCast().GetRC(); }
		ND_ RC<PromiseType>	CastRC ()					rvNE___	{ ASSERT( _coro and not _coro->IsDummy() );  return RC<PromiseType>( RVRef(_coro) ); }
	};
//-----------------------------------------------------------------------------



	//
	// Async Coroutine
	//
	class AsyncCoroImpl : public AsyncTaskImpl
	{
	// types
	public:
		using Self			= AsyncCoroImpl;
		using Coroutine_t	= BaseCoro< Self >;

		static constexpr bool	_allowImplicitCastToAsyncTask = true;

		template <typename T>
		static constexpr bool	_AllowImplicitCast = IsSame< T, AsyncTaskImpl >;


	// methods
	public:
		ND_ auto			initial_suspend ()							C_NE___	{ return std::suspend_always{}; }
			void			return_void ()								__NE___	{}

		ND_	auto			get_return_object ()						__NE___	{ return Coroutine_t{*this}; }

		// If this function defined, compiler will use nothrow new and on error calls 'get_return_object_on_allocation_failure()',
		// otherwise 'std::bad_alloc' would be thrown.
		// Function returns cancelled task.
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___ { return Coroutine_t{}; }

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }	// fixed crash on MSVC
	};
//-----------------------------------------------------------------------------



	//
	// Uncancellable Async Coroutine
	//
	class UncancellableCoroImpl final : public AsyncTaskImpl
	{
	// types
	public:
		using Self			= UncancellableCoroImpl;
		using Coroutine_t	= BaseCoro< Self >;

		static constexpr bool	_allowImplicitCastToAsyncTask = true;

		template <typename T>
		static constexpr bool	_AllowImplicitCast = IsBaseOf< Self, T >;


	// methods
	public:
		UncancellableCoroImpl ()										__NE___ : AsyncTaskImpl{ EFlags::RunCancelled } {}

		ND_ auto			initial_suspend ()							C_NE___	{ return std::suspend_always{}; }
			void			return_void ()								__NE___	{}

		ND_ auto			yield_value (AsyncTaskCoro_Success)			__NE___	{ _ResetCancellationState();  return std::suspend_never{}; }
		using AsyncTaskImpl::yield_value;

		ND_	auto			get_return_object ()						__NE___	{ return Coroutine_t{*this}; }
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___ { return Coroutine_t{}; }

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }	// fixed crash on MSVC
	};
//-----------------------------------------------------------------------------



	//
	// Inline Coroutine
	//
	template <ETaskQueue Queue>
	class InlineCoroImpl final : public AsyncCoroImpl
	{
	// types
	public:
		using Self			= InlineCoroImpl< Queue >;
		using Coroutine_t	= ScheduledInlineCoro< Queue >;

		template <typename T>
		static constexpr bool	_AllowImplicitCast = IsBaseOf< Self, T >;


	// methods
	public:
		InlineCoroImpl ()												__NE___ { this->_Inline_OnStart(); }

		ND_ auto			initial_suspend ()							C_NE___	{ return std::suspend_never{}; }
			void			return_void ()								__NE___	{ this->_Inline_MakeCompletedOnReturn(); }

		ND_	auto			get_return_object (const SourceLoc &loc = SourceLoc::current()) __NE___;
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___;

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }	// fixed crash on MSVC
	};


	//
	// Scheduled Inline Coroutine
	//
	template <ETaskQueue Queue>
	class ScheduledInlineCoro : Base::Noncopyable
	{
	// types
	public:
		using promise_type	= InlineCoroImpl< Queue >;

	// variables
	private:
		AsyncTaskImpl*	_coro	= null;

	// methods
	public:
		explicit ScheduledInlineCoro (AsyncTaskImpl* ptr)				__NE___ : _coro{ ptr } {}
		~ScheduledInlineCoro ()											__NE___	{ _AddToScheduler(); }

		operator BaseCoro<AsyncCoroImpl> ()								rvNE___	{ auto res = BaseCoro<AsyncCoroImpl>{_coro};  _AddToScheduler();  return res; }
		operator AsyncTask ()											rvNE___	{ auto res = AsyncTask{_coro};  _AddToScheduler();  return res; }

	private:
		void  _AddToScheduler ()										__NE___;
	};


	template <ETaskQueue Q>
	forceinline auto  InlineCoroImpl<Q>::get_return_object (const SourceLoc &loc)	__NE___
	{
		#if AE_ENABLE_TASK_NAME
			this->_SetDebugName( loc );
		#endif
		Unused( loc );
		return Coroutine_t{ this };
	}

	template <ETaskQueue Q>
	forceinline auto  InlineCoroImpl<Q>::get_return_object_on_allocation_failure ()	__NE___
	{
		return Coroutine_t{ AsyncTaskImpl::CanceledTask::s_canceled.operator->() };
	}
//-----------------------------------------------------------------------------



	//
	// Scheduled Async Coroutine
	//
	template <ETaskQueue Queue>
	class ScheduledCoroImpl final : public AsyncTaskImpl
	{
	// types
	public:
		using Self			= ScheduledCoroImpl< Queue >;
		using Coroutine_t	= BaseCoro< Self >;

		static constexpr bool	_allowImplicitCastToAsyncTask = true;

		template <typename T>
		static constexpr bool	_AllowImplicitCast = IsBaseOf< Self, T >;


	// methods
	public:
		// Coroutine //
		ND_ auto			initial_suspend ()							C_NE___	{ return std::suspend_always{}; }		// delayed start
			void			return_void ()								__NE___	{}

		ND_	auto			get_return_object (const SourceLoc &loc = SourceLoc::current()) __NE___;
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___ { return Coroutine_t{}; }

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }	// fixed crash on MSVC
	};
//-----------------------------------------------------------------------------



	//
	// Async Promise
	//
	template <typename ResultType>
	class AsyncPromiseImpl : public AsyncTaskImpl
	{
		friend struct Promise_CoroTraits< BaseCoro< AsyncPromiseImpl< ResultType >>>;

	// types
	public:
		using Coroutine_t	= BaseCoro< AsyncPromiseImpl< ResultType >>;
		using Result_t		= ResultType;

		static constexpr bool	_allowImplicitCastToAsyncTask = false;


	// variables
	protected:
		ResultType		_value;


	// methods
	protected:
		AsyncPromiseImpl (ETaskQueue queue, EFlags flags)				__NE___ : AsyncTaskImpl{ queue, flags } {}

	public:
		AsyncPromiseImpl ()												__NE___ {}

		template <typename ...Args>
		explicit AsyncPromiseImpl (MakeDeferResult, Args&& ...args)	__NE___ : AsyncTaskImpl{ EFlags::DoNotRun }, _value{ FwdArg<Args>(args)... }
		{
			StaticAssert( CountOf<Args...>() > 0 );
			CheckNothrow( IsNoExcept( ResultType{ FwdArg<Args>(args)... }));
		}

		// Coroutine //
		ND_ auto			get_return_object ()						__NE___	{ return Coroutine_t{ *this }; }
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___	{ return Coroutine_t{}; }

		ND_ auto			initial_suspend ()							C_NE___ { return std::suspend_always{}; }
		ND_ auto			final_suspend ()							C_NE___	{ return std::suspend_always{}; }							// must not be 'suspend_never'

			void			return_value (ResultType value)				__NE___	{ _value = RVRef(value); }									// set value by 'co_return'
	//	ND_ auto			yield_value (ResultType value)				__NE___	{ _value = RVRef(value);  return std::suspend_always{}; }	// set value by 'co_yield'

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }		// fixed crash on MSVC
	};

	template <>	class AsyncPromiseImpl <void> final {};	// not supported
//-----------------------------------------------------------------------------



	//
	// Inline Promise
	//
	template <typename ResultType, ETaskQueue Queue>
	class InlinePromiseImpl final : public AsyncPromiseImpl<ResultType>
	{
		friend struct Promise_CoroTraits< BaseCoro< InlinePromiseImpl< ResultType, Queue >>>;

	// types
	public:
		using Self			= InlinePromiseImpl< ResultType, Queue >;
		using Coroutine_t	= ScheduledInlinePromise< ResultType, Queue >;

		template <typename T>
		static constexpr bool	_AllowImplicitCast = IsBaseOf< Self, T >;	// TODO: check


	// methods
	public:
		InlinePromiseImpl ()											__NE___ { this->_Inline_OnStart(); }

		// Coroutine //
		ND_ auto			get_return_object (const SourceLoc &loc = SourceLoc::current()) __NE___;
		ND_ static auto		get_return_object_on_allocation_failure ()	__NE___;

		ND_ auto			initial_suspend ()							C_NE___ { return std::suspend_never{}; }

			void			return_value (ResultType value)				__NE___	{ this->_value = RVRef(value);  this->_Inline_MakeCompletedOnReturn(); }	// set value by 'co_return'

		ND_ static void*	operator new (usize size)					__NE___	{ return NothrowAllocatable::operator new( size ); }			// fixed crash on MSVC
	};



	//
	// Scheduled Inline Promise
	//
	template <typename ResultType, ETaskQueue Queue>
	class ScheduledInlinePromise : Base::Noncopyable
	{
	// types
	public:
		using promise_type	= InlinePromiseImpl< ResultType, Queue >;
		using BasePromise_t	= BaseCoro< AsyncPromiseImpl< ResultType >>;


	// variables
	private:
		AsyncTaskImpl*	_coro	= null;


	// methods
	public:
		explicit ScheduledInlinePromise (AsyncTaskImpl* ptr)		__NE___ : _coro{ ptr } {}
		~ScheduledInlinePromise ()									__NE___	{ _AddToScheduler(); }

		operator BasePromise_t ()									rvNE___	{ auto res = BasePromise_t{_coro};  _AddToScheduler();  return res; }

	private:
		void  _AddToScheduler ()									__NE___;
	};


	template <typename R, ETaskQueue Q>
	forceinline auto  InlinePromiseImpl<R,Q>::get_return_object (const SourceLoc &loc) __NE___
	{
		#if AE_ENABLE_TASK_NAME
			this->_SetDebugName( loc );
		#endif
		Unused( loc );
		return Coroutine_t{ this };
	}

	template <typename R, ETaskQueue Q>
	forceinline auto  InlinePromiseImpl<R,Q>::get_return_object_on_allocation_failure () __NE___
	{
		return Coroutine_t{ AsyncTaskImpl::CanceledTask::s_canceled.operator->() };
	}
//-----------------------------------------------------------------------------



	//
	// Scheduled Async Promise
	//
	template <typename ResultType, ETaskQueue Queue>
	class ScheduledPromiseImpl final : public AsyncPromiseImpl<ResultType>
	{
		friend struct Promise_CoroTraits< BaseCoro< ScheduledPromiseImpl< ResultType, Queue >>>;

	// types
	public:
		using Self			= ScheduledPromiseImpl< ResultType, Queue >;
		using Coroutine_t	= BaseCoro< Self >;

		template <typename T>
		static constexpr bool	_AllowImplicitCast = IsBaseOf< Self, T >;


	// methods
	public:
		// Coroutine //
		ND_ auto			get_return_object (const SourceLoc &loc = SourceLoc::current())	__NE___;
		ND_ static auto		get_return_object_on_allocation_failure ()						__NE___	{ return Coroutine_t{}; }

		ND_ static void*	operator new (usize size)										__NE___	{ return NothrowAllocatable::operator new( size ); }		// fixed crash on MSVC
	};
//-----------------------------------------------------------------------------



	//
	// Task Dependency Wrapper
	//
	template <typename TaskType, bool IsStrongDep>
	struct _TaskDependency
	{
		TaskType	_task;

		template <typename T> explicit _TaskDependency (T &&task)		__NE___	: _task{ FwdArg<T>(task) } {}
		template <typename T> explicit _TaskDependency (const T &task)	__NE___	: _task{ task } {}

		_TaskDependency (_TaskDependency &&)							__NE___	= default;
		_TaskDependency (const _TaskDependency &)						__NE___	= default;

		ND_ auto*	operator -> ()										C_NE___	{ return _task.operator->(); }
		ND_ auto&	operator * ()										C_NE___	{ return *_task; }

		ND_ explicit operator bool ()									C_NE___	{ return bool{_task}; }
	};

	template <typename TaskType, bool IsStrongDep>
	struct _TaskDependencyArray : ArrayView< TaskType >
	{
		_TaskDependencyArray (_TaskDependencyArray &&)				__NE___	= default;
		_TaskDependencyArray (const _TaskDependencyArray &)			__NE___	= default;

		_TaskDependencyArray (TaskType const* ptr, usize count)		__NE___	: ArrayView<TaskType>{ptr, count} {}

		_TaskDependencyArray (std::initializer_list<TaskType> list)	__NE___	: ArrayView<TaskType>{list.begin(), list.end()} {}

		template <typename AllocT>
		_TaskDependencyArray (const Array<TaskType,AllocT> &vec)	__NE___	: ArrayView<TaskType>{vec} {}

		template <usize S>
		_TaskDependencyArray (const StaticArray<TaskType,S> &arr)	__NE___	: ArrayView<TaskType>{arr} {}

		template <usize S>
		_TaskDependencyArray (const FixedArray<TaskType,S> &arr)	__NE___	: ArrayView<TaskType>{arr} {}

		template <usize S>
		_TaskDependencyArray (const TaskType (&arr)[S])				__NE___	: ArrayView<TaskType>{arr} {}
	};
//-----------------------------------------------------------------------------



	template <typename T>
	struct CoroTraits;


	template <typename T>
	struct AsyncTask_CoroTraits
	{
		using Result	= AsyncTaskImpl::NullResult;
		using type		= T;

		Nd__IF static Result const*	GetResultPtr (const type &)		__NE___ { return &AsyncTaskImpl::_nullResult; }
		Nd__IF static Result		GetResultCopy (const type &)	__NE___ { return AsyncTaskImpl::_nullResult; }
		Nd__IF static Result		MoveResult (const type &)		__NE___ { return AsyncTaskImpl::_nullResult; }
	};

	template <>
	struct CoroTraits< AsyncTaskImpl >		: AsyncTask_CoroTraits< AsyncTaskImpl > {};

	template <>
	struct CoroTraits< AsyncTask >			: AsyncTask_CoroTraits< AsyncTask > {};

	template <ETaskQueue Queue>
	struct CoroTraits< BaseCoro<ScheduledCoroImpl<Queue> >>	: AsyncTask_CoroTraits< BaseCoro<ScheduledCoroImpl<Queue> >> {};

	template <>
	struct CoroTraits< BaseCoro< AsyncCoroImpl >> : AsyncTask_CoroTraits< BaseCoro< AsyncCoroImpl >> {};

	template <ETaskQueue Queue>
	struct CoroTraits< BaseCoro< InlineCoroImpl<Queue> >>  : AsyncTask_CoroTraits< BaseCoro< InlineCoroImpl<Queue> >> {};


	template <typename T>
	struct Promise_CoroTraits
	{
		using type		= T;
		using _Impl_t	= typename T::promise_type;
		using Result	= typename _Impl_t::Result_t;

		Nd__IF static Result*	GetResultPtr (const type &p)	__NE___ { return p->IsCompleted() ? &RefCast<_Impl_t>(*p)._value : null; }
		Nd__IF static Result	GetResultCopy (const type &p)	__NE___ { return p->IsCompleted() ?  RefCast<_Impl_t>(*p)._value : Result{}; }
		Nd__IF static Result	MoveResult (const type &p)		__NE___ { return p->IsCompleted() ? RVRef( RefCast<_Impl_t>(*p)._value ) : Result{}; }
	};

	template <typename ResultType>
	struct CoroTraits< BaseCoro< AsyncPromiseImpl< ResultType >>> :
		Promise_CoroTraits< BaseCoro< AsyncPromiseImpl< ResultType >>> {};

	template <typename ResultType, ETaskQueue Queue>
	struct CoroTraits< BaseCoro< InlinePromiseImpl< ResultType, Queue >>> :
		Promise_CoroTraits< BaseCoro< InlinePromiseImpl< ResultType, Queue >>> {};

	template <typename ResultType, ETaskQueue Queue>
	struct CoroTraits< BaseCoro< ScheduledPromiseImpl< ResultType, Queue >>> :
		Promise_CoroTraits< BaseCoro< ScheduledPromiseImpl< ResultType, Queue >>> {};
//-----------------------------------------------------------------------------



	//
	// Successfully Completed Dependencies
	//
	template <usize Count>
	struct CompletedDeps
	{
		friend class CoroAwaiterImpl;

	// variables
	protected:
		std::bitset<Count>				_bits;
		DEBUG_ONLY(
			mutable std::bitset<Count>	_isChecked;
		)

	// methods
	private:
		CompletedDeps ()	= default;

	public:
		ND_ bool	AllComplete ()				C_NE___	{ DEBUG_ONLY(_isChecked = std::bitset<Count>{}.flip());  return _bits.all(); }

		template <usize Index = 0>
		ND_ bool	IsComplete ()				C_NE___	{ StaticAssert( Index < Count );  DEBUG_ONLY(_isChecked.set(Index));  return _bits.test( Index ); }

		ND_ bool	IsComplete (usize index)	C_NE___	{ ASSERT( index < Count );  DEBUG_ONLY(_isChecked.set(index));  return _bits.test( index ); }

		ND_ explicit operator bool ()			C_NE___	{ return AllComplete(); }
	};


	template <typename T>
	concept _IsTaskWithStatus = requires(T task)
	{
		bool{task};
		{ task->IsCompleted() } -> SameAs<bool>;
	};


	//
	// Coroutine Awaiter
	//
	class CoroAwaiterImpl
	{
	// types
	private:

		template <usize Count, bool IsRequired>
		struct _IsCompleteVisitor
		{
			CompletedDeps<Count>	complete;

			template <usize I, typename T>
			void  operator () (const T &dep) __NE___
			{
				if constexpr( _IsTaskWithStatus<T> )
					complete._bits.set( I, dep and dep->IsCompleted() );
				else
					StaticAssert( not IsRequired );
			}
		};

	// methods
	public:
		Nd__IF static CompletedDeps<1>  AwaitResultImpl (const AsyncTask &dep) __NE___
		{
			CompletedDeps<1>	result;
			result._bits.set( 0, dep and dep->IsCompleted() );
			return result;
		}

		template <typename P>
		Nd__IF static bool  AwaitSuspendImpl (std::coroutine_handle<P> curCoro, const AsyncTask &dep, Bool defaultIsStrongDep = True{}) __NE___
		{
			// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
			StaticAssert( IsBaseOf< AsyncTaskImpl, P >);

			if ( dep and dep->IsCompleted() )
				return false; // resume

			// add new dependencies to the task, but doesn't add to queue
			return AsyncTaskImpl::CoroutineApi::Continue( curCoro.promise(), Tuple{ dep }, defaultIsStrongDep );
		}


		template <typename ...Deps>
		Nd__IF static CompletedDeps<CountOf<Deps...>()>  AwaitResultImpl2 (const Tuple<Deps...> &deps) __NE___
		{
			_IsCompleteVisitor< CountOf<Deps...>(), true >  tmp;
			deps.Visit( INOUT tmp );
			return tmp.complete;
		}

		template <typename P, typename ...Deps>
		Nd__IF static bool  AwaitSuspendImpl2 (std::coroutine_handle<P> curCoro, const Tuple<Deps...> &deps, Bool defaultIsStrongDep = True{}) __NE___
		{
			// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
			StaticAssert( IsBaseOf< AsyncTaskImpl, P >);

			_IsCompleteVisitor< CountOf<Deps...>(), false >  tmp;
			deps.Visit( INOUT tmp );

			if ( tmp.complete._bits.all() )
				return false; // resume

			// add new dependencies to the task, but doesn't add to queue
			return AsyncTaskImpl::CoroutineApi::Continue( curCoro.promise(), deps, defaultIsStrongDep );
		}


		template <typename T>
		Nd__IF static exact_t  GetResultCopy (const T &task) __NE___
		{
			return CoroTraits<T>::GetResultCopy( task );
		}

		template <typename T>
		Nd__IF static exact_t  GetResultPtr (const T &task) __NE___
		{
			return CoroTraits<T>::GetResultPtr( task );
		}

		template <typename T>
		Nd__IF static exact_t  MoveResult (const T &task) __NE___
		{
			return CoroTraits<T>::MoveResult( task );
		}
	};
//-----------------------------------------------------------------------------



	//
	// Wrapper
	//
	template <typename T>
	struct ConstPtrWrap
	{
		static constexpr bool	_allowImplicitCast = false;

		T const*	value;

		explicit ConstPtrWrap (const T *val)	__NE___ : value{val} {}

		ND_ T const&	Ref ()					C_NE___	{ return *value; }
		ND_ bool		Exists ()				C_NE___	{ return value != null; }
	};

	template <typename T>
	struct CopyWrap
	{
		static constexpr bool	_allowImplicitCast = false;

		T			value;

		explicit CopyWrap (T val)				__NE___ : value{RVRef(val)} {}

		ND_ T &			Ref ()					__NE___	{ return value; }
		ND_ T const&	Ref ()					C_NE___	{ return value; }
		ND_ bool		Exists ()				C_NE___	{ return true; }
	};

	template <typename T>
	struct CopyWrapImplicit
	{
		static constexpr bool	_allowImplicitCast = true;

		T			value;

		explicit CopyWrapImplicit (T val)		__NE___ : value{RVRef(val)} {}

		ND_ T &			Ref ()					__NE___	{ return value; }
		ND_ T const&	Ref ()					C_NE___	{ return value; }
		ND_ bool		Exists ()				C_NE___	{ return true; }
	};



	//
	// Successfully Completed Dependencies with Result
	//
	template <template<class> typename WrapT, typename ...Types>
	struct CompletedDepsResult : CompletedDeps< CountOf<Types...>() >
	{
		template <typename ...Deps> friend class CoroAwaiter_WithResultRef;
		template <typename ...Deps> friend class CoroAwaiter_WithResultCopy;
		template <typename ...Deps> friend class CoroAwaiter_WithUniqueResult;
		template <typename ...Deps> friend class CoroAwaiter_WaitResultCopyOrCancel;
		using Base_t = CompletedDeps< CountOf<Types...>() >;

	// variables
	protected:
		Tuple< WrapT<Types>...>		_results;

		static constexpr usize	Count = CountOf<Types...>();

	// methods
	protected:
		template <typename ...Args>
		CompletedDepsResult (Base_t bits, Bool mustCheck, Args&& ...args) __NE___ : Base_t{RVRef(bits)}, _results{ FwdArg<Args>(args)... }
		{
			StaticAssert( CountOf<Args...>() == Count );
			DEBUG_ONLY(
				auto	fn = [bits, i = 0u] (auto& res) mutable { if (bits.IsComplete(i++)) CHECK( res.Exists() ); };
				_results.ForEach( fn );

				if ( not mustCheck ) this->_isChecked.flip(); // set all 'true'
			)
			Unused( mustCheck );
		}

		DEBUG_ONLY(
			template <usize I>
			void  _IsCompleteAndChecked () const
			{
				StaticAssert( I < Count );
				CHECK_MSG( this->_isChecked.test(I), "call IsComplete with same index before accessing Promise result" );
				CHECK_MSG( this->_bits.test(I), "Promise result is not ready" );
			}
		)

	public:
		template <usize I>
		Nd__IF exact_t		get ()		C_NE___	{ StaticAssert( I < Count );  DEBUG_ONLY(_IsCompleteAndChecked<I>());  return _results.template Get<I>().Ref(); }

		template <usize I>
		Nd__IF exact_t		get ()		__NE___	{ StaticAssert( I < Count );  DEBUG_ONLY(_IsCompleteAndChecked<I>());  return _results.template Get<I>().Ref(); }
	};


	template <template<class> typename WrapT, typename Type>
	struct CompletedDepsResult<WrapT, Type> : CompletedDeps<1>
	{
		template <typename ...Deps> friend class CoroAwaiter_WithResultRef;
		template <typename ...Deps> friend class CoroAwaiter_WithResultCopy;
		template <typename ...Deps> friend class CoroAwaiter_WithUniqueResult;
		template <typename ...Deps> friend class CoroAwaiter_WaitResultCopyOrCancel;
		using Base_t = CompletedDeps<1>;

	// variables
	protected:
		WrapT<Type>		_result;

	// methods
	protected:
		template <typename Arg>
		CompletedDepsResult (Base_t bits, Bool mustCheck, Arg&& arg) __NE___ : Base_t{bits}, _result{ FwdArg<Arg>(arg) }
		{
			DEBUG_ONLY(
				if (this->_bits.all()) CHECK( _result.Exists() );

				if ( not mustCheck ) this->_isChecked.flip();  // set all 'true'
			)
			Unused( mustCheck );
		}

		DEBUG_ONLY(
			void  _IsCompleteAndChecked () const
			{
				CHECK_MSG( this->_isChecked.test(0), "call IsComplete with same index before accessing Promise result" );
				CHECK_MSG( this->_bits.test(0), "Promise result is not ready" );
			}
		)

	public:
		template <usize I = 0>
		Nd__IF exact_t		get ()			C_NE___	{ StaticAssert( I == 0 );  DEBUG_ONLY(_IsCompleteAndChecked());  return _result.Ref(); }

		template <usize I = 0>
		Nd__IF exact_t		get ()			__NE___	{ StaticAssert( I == 0 );  DEBUG_ONLY(_IsCompleteAndChecked());  return _result.Ref(); }

		Nd__IF exact_t		operator -> ()	__NE___	{ return &get(); }
		Nd__IF exact_t		operator * ()	__NE___	{ return get(); }

		template <typename T = WrapT<Type>>	requires(T::_allowImplicitCast)
		Nd__IF operator Type const& ()		CrNE___	{ return get(); }

		template <typename T = WrapT<Type>>	requires(T::_allowImplicitCast)
		Nd__IF operator Type & ()			r_NE___	{ return get(); }

		template <typename T = WrapT<Type>>	requires(T::_allowImplicitCast)
		Nd__IF operator Type && ()			rvNE___	{ ASSERT( this->_bits.test(0) );  DEBUG_ONLY( this->_bits.reset(0) );  return RVRef( _result.Ref() ); }
	};


	template <template<class> typename WrapT, typename ...Types>
	struct CompletedDepsResult< WrapT, TypeList<Types...> > : CompletedDepsResult< WrapT, Types... >
	{
		template <typename ...Deps> friend class CoroAwaiter_WithResultRef;
		template <typename ...Deps> friend class CoroAwaiter_WithResultCopy;
		template <typename ...Deps> friend class CoroAwaiter_WithUniqueResult;
		template <typename ...Deps> friend class CoroAwaiter_WaitResultCopyOrCancel;
		using Base_t = CompletedDeps< CountOf<Types...>() >;

	// methods
	private:
		template <typename ...Args>
		CompletedDepsResult (Base_t bits, Bool mustCheck, Args&& ...args) __NE___ :
			CompletedDepsResult< WrapT, Types... >{ RVRef(bits), mustCheck, FwdArg<Args>(args)... }
		{}
	};

} // AE::_Coro_

template <template<class> typename WrapT, typename ...Types>
struct std::tuple_size< AE::_Coro_::CompletedDepsResult< WrapT, Types ...> >
{
	static constexpr AE::usize	value = sizeof...(Types);
};

template <template<class> typename WrapT, typename ...Types>
struct std::tuple_size< AE::_Coro_::CompletedDepsResult< WrapT, AE::Base::TypeList<Types ...>> >
{
	static constexpr AE::usize	value = sizeof...(Types);
};

template <AE::usize I, template<class> typename WrapT, typename ...Types>
struct std::tuple_element< I, AE::_Coro_::CompletedDepsResult< WrapT, Types ...> >
{
	using type = typename AE::Base::TypeList< Types... >::template Get<I>;
};

template <AE::usize I, template<class> typename WrapT, typename ...Types>
struct std::tuple_element< I, AE::_Coro_::CompletedDepsResult< WrapT, AE::Base::TypeList<Types ...>> >
{
	using type = typename AE::Base::TypeList< Types... >::template Get<I>;
};

namespace AE::_Coro_
{
	//
	// Coroutine Is Canceled
	//
	class AsyncTaskCoro_IsCanceled final
	{
	private:
		struct Awaiter
		{
		private:
			bool	_isCanceled = false;

		public:
			NdCx__ bool  await_ready ()									C_NE___	{ return false; }		// call 'await_suspend()' to get coroutine handle
			Nd__IF bool  await_resume ()								__NE___	{ return _isCanceled; }	// return result of 'co_await'

			template <typename P>
			Nd__IF bool  await_suspend (std::coroutine_handle<P> curCoro)__NE___
			{
				// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
				StaticAssert( IsBaseOf< AsyncTaskImpl, P >);

				_isCanceled = AsyncTaskImpl::CoroutineApi::IsCanceled( curCoro.promise() );
				return false;	// always resume coroutine
			}
		};

	public:
		__Cx__ AsyncTaskCoro_IsCanceled ()								__NE___ {}
		Nd__IF auto  operator co_await ()								C_NE___	{ return Awaiter{}; }
	};


	//
	// Get Async Task
	//
	class AsyncTaskCoro_Get final
	{
	private:
		struct Awaiter
		{
		private:
			AsyncTaskImpl const*	_task = null;

		public:
			NdCx__ bool  await_ready ()									C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
			Nd__IF auto  await_resume ()								__NE___	{ return AsyncTaskImpl::UserApi{_task}; }	// return result of 'co_await'

			template <typename P>
			Nd__IF bool  await_suspend (std::coroutine_handle<P> curCoro)__NE___
			{
				// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
				StaticAssert( IsBaseOf< AsyncTaskImpl, P >);

				_task = &curCoro.promise();
				return false;	// always resume coroutine
			}
		};

	public:
		__Cx__ AsyncTaskCoro_Get ()										__NE___ {}
		Nd__IA auto  operator co_await ()								C_NE___	{ return Awaiter{}; }
	};


	//
	// Set Coroutine Queue Type
	//
	class AsyncTaskCoro_NextQueue final
	{
	private:
		struct Awaiter
		{
		private:
			const ETaskQueue	_queue;

		public:
			__Cx__ explicit Awaiter (ETaskQueue q)						__NE___	: _queue{q} {}
			NdCx__ bool  await_ready ()									C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
			__CxIF void  await_resume ()								__NE___	{}					// return result of 'co_await'

			template <typename P>
			Nd__IF bool  await_suspend (std::coroutine_handle<P> curCoro)__NE___
			{
				// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
				StaticAssert( IsBaseOf< AsyncTaskImpl, P >);

				AsyncTaskImpl::CoroutineApi::SetQueueType( curCoro.promise(), _queue );
				return false;	// always resume coroutine
			}
		};

		const ETaskQueue	_queue;

	public:
		__Cx__ AsyncTaskCoro_NextQueue (ETaskQueue q)					__NE___ : _queue{q} {}
		Nd__IA auto  operator co_await ()								C_NE___	{ return Awaiter{ _queue }; }
	};
//-----------------------------------------------------------------------------



/*
=================================================
	CoroAwaiter_FastCancel
----
	input: const reference
	output: none
	fast cancel: yes
=================================================
*/
	template <typename T>
	class CoroAwaiter_FastCancel;


	template <>
	class CoroAwaiter_FastCancel< AsyncTask >
	{
	// variables
	private:
		AsyncTask const&	_dep;

	// methods
	public:
		explicit CoroAwaiter_FastCancel (const AsyncTask &dep)			__NE___ : _dep{dep} {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }
		__Cx__ void  await_resume ()									C_NE___	{}					// return result of 'co_await'

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl( curCoro, _dep );
		}
	};


	template <typename ...Deps>
	class CoroAwaiter_FastCancel< Tuple<Deps...> >
	{
	// variables
	private:
		Tuple<Deps...> const&	_deps;


	// methods
	public:
		explicit CoroAwaiter_FastCancel (const Tuple<Deps...> &deps)	__NE___ : _deps{deps} {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }
		__Cx__ void  await_resume ()									C_NE___	{}					// return result of 'co_await'

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, _deps );
		}
	};


	class CoroAwaiter_Continue
	{
	// methods
	public:
		explicit CoroAwaiter_Continue ()								__NE___ {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }
		__Cx__ void  await_resume ()									C_NE___	{}					// return result of 'co_await'

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			// compatible with all 'promise_type' which is inherited from 'AsyncTaskImpl'
			StaticAssert( IsBaseOf< AsyncTaskImpl, P >);

			// add new dependencies to the task, but doesn't add to queue
			return AsyncTaskImpl::CoroutineApi::Continue( curCoro.promise(), Tuple{}, True{"strong dependency"} );
		}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	CoroAwaiter_FastCancelAndReset
----
	input: reference, will reset to null
	output: none
	fast cancel: yes
=================================================
*/
	template <typename T>
	class CoroAwaiter_FastCancelAndReset;


	template <>
	class CoroAwaiter_FastCancelAndReset< AsyncTask >
	{
	// variables
	private:
		AsyncTask	_dep;

	// methods
	public:
		explicit CoroAwaiter_FastCancelAndReset (AsyncTask &dep)		__NE___ : _dep{ RVRef(dep) } {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }
		__Cx__ void  await_resume ()									C_NE___	{}					// return result of 'co_await'

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl( curCoro, _dep );
		}
	};


	template <typename ...Deps>
	class CoroAwaiter_FastCancelAndReset< Tuple<Deps...> >
	{
	// variables
	private:
		Tuple<Deps...>		_deps;


	// methods
	public:
		explicit CoroAwaiter_FastCancelAndReset (Deps& ...deps)			__NE___ : _deps{ RVRef(deps)... } {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }
		__Cx__ void  await_resume ()									C_NE___	{}					// return result of 'co_await'

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, _deps );
		}
	};


	template <typename ...Deps>
	Nd__IF auto  Wait_FastCancelAndReset (Deps& ...deps) __NE___
	{
		return CoroAwaiter_FastCancelAndReset< Deps... >{ deps... };
	}

	Nd__IF auto  Wait_FastCancelAndReset () __NE___
	{
		return CoroAwaiter_Continue{};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CoroAwaiter_WaitResultCopyOrCancel
----
	input: reference, will reset to null
	output: copy of promise result
	fast cancel: yes
=================================================
*/
	template <typename ...Deps>
	class CoroAwaiter_WaitResultCopyOrCancel
	{
	// types
	private:
		using ResultTypeList_t	= TypeList< typename CoroTraits< Deps >::Result ... >;
		using DepsResult		= CompletedDepsResult< CopyWrapImplicit, ResultTypeList_t >;
		StaticAssert( CountOf<Deps...>() > 0 );

	// variables
	private:
		Tuple< Deps ... >	_deps;

	// methods
	public:
		explicit CoroAwaiter_WaitResultCopyOrCancel (Deps& ...deps)		__NE___ : _deps{ RVRef(deps)... } {}

		CoroAwaiter_WaitResultCopyOrCancel (Bool, Deps const& ...deps)	__NE___ : _deps{ deps... } {}	// copy

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
		Nd__IA auto  await_resume ()									C_NE___						// return result of 'co_await'
		{
			auto	bits = CoroAwaiterImpl::AwaitResultImpl2( _deps );
			return	_deps.Apply( [bits] (auto&& ...args) __NE___ {
									return DepsResult{ bits, False{"don't check result"}, CoroAwaiterImpl::GetResultCopy( args ) ... };
								});
		}

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			ASSERT_MSG( not AsyncTaskImpl::CoroutineApi::IsRunCancelled( curCoro.promise() ),
				"Unsafe use of 'co_await' or 'Coro_WaitResultOrCancel' inside coroutine which is uncancellable" );

			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, _deps, True{"strong dependency"} );
		}
	};

/*
=================================================
	CoroAwaiter_WaitResultCopyOrCancel_Array
----
	input: const reference
	output: copy of promise result
	fast cancel: yes
=================================================
*/
	template <typename T>
	class CoroAwaiter_WaitResultCopyOrCancel_Array
	{
	// types
		using ResultArray_t = Array< typename T::promise_type::Result_t >;

	// variables
	private:
		Array<T> const&		_deps;

	// methods
	public:
		explicit CoroAwaiter_WaitResultCopyOrCancel_Array (Array<T> const& deps) __NE___ : _deps{deps} {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
		Nd__IA auto  await_resume ()									C_NE___	-> ResultArray_t	// return result of 'co_await'
		{
			ResultArray_t	result;
			result.reserve( _deps.size() );

			for (auto& p : _deps) {
				result.emplace_back( CoroAwaiterImpl::GetResultCopy( p ));
			}
			return RVRef(result);
		}

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			ASSERT_MSG( not AsyncTaskImpl::CoroutineApi::IsRunCancelled( curCoro.promise() ),
				"Unsafe use of 'co_await' or 'Coro_WaitResultOrCancel' inside coroutine which is uncancellable" );

			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, Tuple{ ArrayView<T>{ _deps }}, True{"strong dependency"} );
		}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	CoroAwaiter_WithResultCopy
----
	input: reference, will reset to null
	output: copy of promise result
	fast cancel: no
=================================================
*/
	template <typename ...Deps>
	class CoroAwaiter_WithResultCopy
	{
	// types
	private:
		using ResultTypeList_t	= TypeList< typename CoroTraits< Deps >::Result ... >;
		using DepsResult		= CompletedDepsResult< CopyWrap, ResultTypeList_t >;
		StaticAssert( CountOf<Deps...>() > 0 );

	// variables
	private:
		Tuple< Deps ... >	_deps;

	// methods
	public:
		explicit CoroAwaiter_WithResultCopy (Deps& ...deps)				__NE___ : _deps{ RVRef(deps)... } {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
		Nd__IA auto  await_resume ()									C_NE___						// return result of 'co_await'
		{
			auto	bits = CoroAwaiterImpl::AwaitResultImpl2( _deps );
			return	_deps.Apply( [bits] (auto&& ...args) __NE___ {
									return DepsResult{ bits, True{"must check result"}, CoroAwaiterImpl::GetResultCopy( args ) ... };
								});
		}

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, _deps, False{"weak dependency"} );
		}
	};

/*
=================================================
	CoroAwaiter_WithResultRef
----
	input: const reference
	output: reference to promise result
	fast cancel: no
=================================================
*/
	template <typename ...Deps>
	class CoroAwaiter_WithResultRef
	{
	// types
	private:
		using ResultTypeList_t	= TypeList< typename CoroTraits< Deps >::Result ... >;
		using DepsResult		= CompletedDepsResult< ConstPtrWrap, ResultTypeList_t >;
		StaticAssert( CountOf<Deps...>() > 0 );

	// variables
	private:
		Tuple< Deps ... >	_deps;

	// methods
	public:
		template <typename ...Args>
		explicit CoroAwaiter_WithResultRef (Args&& ...deps)				__NE___ : _deps{ FwdArg<Args>(deps)... } {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
		Nd__IA auto  await_resume ()									C_NE___						// return result of 'co_await'
		{
			auto	bits = CoroAwaiterImpl::AwaitResultImpl2( _deps );
			return	_deps.Apply( [bits] (auto&& ...args) __NE___ {
									return DepsResult{ bits, True{"must check result"}, CoroAwaiterImpl::GetResultPtr( args ) ... };
								});
		}

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, _deps, False{"weak dependency"} );
		}
	};

	template <typename ...Deps>
	Nd__IF auto  WaitResultRef (Deps&& ...deps) __NE___
	{
		return CoroAwaiter_WithResultRef< RemoveReference<Deps> ... >{ FwdArg<Deps>(deps)... };
	}

/*
=================================================
	CoroAwaiter_WithUniqueResult
----
	input: reference, will reset to null
	output: move-copy of promise result
	fast cancel: no
=================================================
*/
	template <typename ...Deps>
	class CoroAwaiter_WithUniqueResult
	{
	// types
	private:
		using ResultTypeList_t	= TypeList< typename CoroTraits< Deps >::Result ... >;
		using DepsResult		= CompletedDepsResult< CopyWrap, ResultTypeList_t >;
		StaticAssert( CountOf<Deps...>() > 0 );

	// variables
	private:
		Tuple< Deps ... >	_deps;

	// methods
	public:
		explicit CoroAwaiter_WithUniqueResult (Deps& ...deps)			__NE___ : _deps{ RVRef(deps)... } {}

		NdCx__ bool  await_ready ()										C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
		Nd__IA auto  await_resume ()									C_NE___						// return result of 'co_await'
		{
			auto	bits = CoroAwaiterImpl::AwaitResultImpl2( _deps );
			return	_deps.Apply( [bits] (auto&& ...args) __NE___ {
									return DepsResult{ bits, True{"must check result"}, CoroAwaiterImpl::MoveResult( args ) ... };
								});
		}

		// return task to scheduler with new dependencies
		template <typename P>
		Nd__IA bool  await_suspend (std::coroutine_handle<P> curCoro)	__NE___
		{
			return CoroAwaiterImpl::AwaitSuspendImpl2( curCoro, _deps, False{"weak dependency"} );
		}
	};
//-----------------------------------------------------------------------------



	template <typename T>
	concept IsCoroutinePromise = requires()
	{
		T{}.initial_suspend();
		T{}.final_suspend();

		T{}.unhandled_exception();

		T{}.yield_value( _Coro_::AsyncTaskCoro_Error{} );
		T{}.yield_value( _Coro_::AsyncTaskCoro_ChangeQueue{ ETaskQueue::Main });

		T{}.get_return_object();
		T::get_return_object_on_allocation_failure();
	};

	template <typename T>
	concept IsCoroutine = IsCoroutinePromise< typename T::promise_type >;

	template <typename T>
	concept	IsPromise =	requires(T task)
	{
		_Coro_::BaseCoro< _Coro_::AsyncPromiseImpl< typename T::promise_type::Result_t >>{ RVRef(task) };
	};

	template <typename T>
	concept	IsInlineCoroutine = IsCoroutine<T> and requires()
	{
		{ (typename T::promise_type{}).initial_suspend() } -> SameAs< std::suspend_never >;
	};


	template <typename T>
	struct _CoroCtorResult2
	{
		using R = typename T::promise_type::Result_t;
		using type = _Coro_::BaseCoro< _Coro_::AsyncPromiseImpl< R >>;
	};

	template <typename CtorFn>
	struct _CoroCtorResult1
	{
		using R = typename FunctionInfo< CtorFn >::result;
		using type = Conditional< IsPromise<R>, _CoroCtorResult2<R>, TypeToType<AsyncTask> >::type;
	};

	template <typename ...Args>
	using CoroCtorResult = typename _CoroCtorResult1< typename TypeList<Args...>::Back::type >::type;
//-----------------------------------------------------------------------------



	enum class ESafeCoroError : uint
	{
		OK,
		LambdaWithCapture,
		NotAGlobalFunction,
		InvalidResultType,
		LambdaWithAutoRef,

		ArgIsReference,
		ArgIsPointer,
		ArgIsView,
	};

	template <int I, typename TL>
	NdCe__ Pair<ESafeCoroError, int>  SafeCoro_CheckArgs ()
	{
		using Arg = typename TL::template Get<I>;

		if constexpr( IsReference< Arg >			or
					  IsSpecializationOf< Arg, Ref >)
		{
			return { ESafeCoroError::ArgIsReference, I };
		}
		if constexpr( IsPointer< Arg >				or
					  IsSpecializationOf< Arg, Ptr >)
		{
			StaticAssert( not IsFunctionPointer< Arg >);
			StaticAssert( not IsMemberPointer< Arg >);
			return { ESafeCoroError::ArgIsPointer, I };
		}
		if constexpr( IsSpecializationOf< Arg, std::basic_string_view >	or
					  IsSpecializationOf< Arg, ArrayView >				or
					  IsSpecializationOf< Arg, MutableArrayView >		or
					  IsSpecializationOf< Arg, NtBasicStringView >		or
					  IsSpecializationOf< Arg, StructView >				or
					  IsSpecializationOf< Arg, TupleArrayView >)
		{
			return { ESafeCoroError::ArgIsView, I };
		}

		if constexpr( IsSpecializationOf< Arg, Tuple >)
		{
			using TL2 = TypeList< Arg >;
			StaticAssert( TL2::Count == Arg::Count() );

			auto [err, idx] = SafeCoro_CheckArgs< 0, TL2 >();

			if ( err != ESafeCoroError::OK )
				return { err, (idx << 8) | I };
		}

		if constexpr( I+1 < TL::Count )
		{
			return SafeCoro_CheckArgs< I+1, TL >();
		}
		else
			return { ESafeCoroError::OK, -1 };
	}


	template <typename CoroCtor, typename ArgsTL>
	NdCe__ Pair<ESafeCoroError, int>  SafeCoro_CheckAll ()
	{
		using T = RemoveCV< RemoveReference< CoroCtor >>;

		// operator()
		if constexpr( IsClass<T> )
		{
			if ( not IsEmpty<T> )
				return { ESafeCoroError::LambdaWithCapture, -1 };

			// without template/auto
			if constexpr( IsNotTemplateCallOperator<T> )
			{
				using Info = FunctionInfo< decltype( &T::operator () )>;
				StaticAssert( IsSame< typename Info::clazz, T >);

				if ( not IsCoroutine< typename Info::result >)
					return { ESafeCoroError::InvalidResultType, -1 };

				return SafeCoro_CheckArgs< 0, typename Info::args >();
			}
			else

			// all template/auto arguments
			if constexpr( IsCallOperatorSpecializationFromTypeList< T, ArgsTL >)
			{
				if constexpr( not IsTemplateArgWithoutRef< T, ArgsTL::Count >)
					return { ESafeCoroError::LambdaWithAutoRef, -1 };

				using Info = TemplateFunctionInfo< T, ArgsTL >;
				StaticAssert( IsSame< typename Info::clazz, T >);

				if ( not IsCoroutine< typename Info::result >)
					return { ESafeCoroError::InvalidResultType, -1 };

				// reference types will be converted to value type,
				// input as a reference already checked in 'IsTemplateArgWithoutRef'
				using ArgsTL2 = ArgsTL::template Apply_t< std::remove_reference >;

				return SafeCoro_CheckArgs< 0, ArgsTL2 >();
			}

			// mix of template/auto and explicit types
			else
			{
				//StaticAssert( false, "not supported" );	// TODO
				// TODO
				return { ESafeCoroError::OK, -1 };
			}
		}

		// global function
		else
		{
			if ( not IsGlobalFunction<T> )
				return { ESafeCoroError::NotAGlobalFunction, -1 };

			using Info = FunctionInfo< T >;

			if ( not IsCoroutine< typename Info::result >)
				return { ESafeCoroError::InvalidResultType, -1 };

			return SafeCoro_CheckArgs< 0, typename Info::args >();
		}
	}

	template <typename CoroCtor, typename ArgsTL>
	__Ce__ void  SafeCoro_PrintError ()
	{
		constexpr auto	err_idx = SafeCoro_CheckAll< CoroCtor, ArgsTL >();
		constexpr auto	err		= err_idx.first;
		constexpr auto	idx		= err_idx.second;

		if constexpr( err == ESafeCoroError::OK )
			return;

		StaticAssert( err != ESafeCoroError::LambdaWithCapture, "\n"
			"used lambda with capture or non-empty class, this is unsafe:\n"
			" - lambda capture lifetime does not match with coroutine lifetime\n"
			" - 'this' lifetime is not guarantied to match with coroutine lifetime, use smart pointer instead" );

		StaticAssert( err != ESafeCoroError::InvalidResultType, "\n"
			"function must return coroutine type, this validation may have false-positive for non-coroutines" );

		StaticAssert( err != ESafeCoroError::NotAGlobalFunction, "\n"
			"allowed: lambdas, methods with empty classes and static functions" );

		StaticAssert( err != ESafeCoroError::LambdaWithAutoRef, "\n"
			"Used 'auto' or template arguments with reference.\n"
			"Not allowed: 'auto&', 'auto&&', 'const auto&', same for template types." );

		#if 0 //__cpp_static_assert >= 202306L
			StaticAssert( err != ESafeCoroError::ArgIsReference,
				 std::format("\nargument {} with reference to the object is unsafe - lifetime of object may not match with coroutine lifetime", idx ));

			StaticAssert( err != ESafeCoroError::ArgIsPointer,
				std::format("\nargument {} with pointer to the object is unsafe - lifetime of object may not match with coroutine lifetime\n"
					"use smart pointers instead", idx ));

			StaticAssert( err != ESafeCoroError::ArgIsView,
				std::format("\nargument {} with view type is unsafe - lifetime of referenced memory may not match with coroutine lifetime", idx ));

		#else
			StaticAssert( idx == -1 );

			StaticAssert( err != ESafeCoroError::ArgIsReference, "\n"
				"argument with reference to the object is unsafe - lifetime of object may not match with coroutine lifetime" );

			StaticAssert( err != ESafeCoroError::ArgIsPointer, "\n"
				"argument with pointer to the object is unsafe - lifetime of object may not match with coroutine lifetime\n"
				"use smart pointers instead" );

			StaticAssert( err != ESafeCoroError::ArgIsView, "\n"
				"argument with view type is unsafe - lifetime of referenced memory may not match with coroutine lifetime" );
		#endif
	}

} // AE::_Coro_
//-----------------------------------------------------------------------------


namespace AE::Threading
{
	using AsyncCoro			= _Coro_::BaseCoro< _Coro_::AsyncCoroImpl >;
	using UncancellableCoro	= _Coro_::BaseCoro< _Coro_::UncancellableCoroImpl >;

	template <ETaskQueue Queue = _Coro_::c_DefaultTaskQueue>
	using InlineCoro		= _Coro_::ScheduledInlineCoro< Queue >;

	template <ETaskQueue Queue = _Coro_::c_DefaultTaskQueue>
	using ScheduledCoro		= _Coro_::BaseCoro< _Coro_::ScheduledCoroImpl< Queue >>;


	template <typename ResultType>
	using Promise			= _Coro_::BaseCoro< _Coro_::AsyncPromiseImpl< ResultType >>;

	template <typename ResultType,
			  ETaskQueue Queue = _Coro_::c_DefaultTaskQueue>
	using ScheduledPromise	= _Coro_::BaseCoro< _Coro_::ScheduledPromiseImpl< ResultType, Queue >>;

	template <typename ResultType,
			  ETaskQueue Queue = _Coro_::c_DefaultTaskQueue>
	using InlinePromise		= _Coro_::ScheduledInlinePromise< ResultType, Queue >;


	using WeakDep			= _Coro_::_TaskDependency< AsyncTask, false >;
	using StrongDep			= _Coro_::_TaskDependency< AsyncTask, true >;

	using WeakDepArray		= _Coro_::_TaskDependencyArray< AsyncTask, false >;
	using StrongDepArray	= _Coro_::_TaskDependencyArray< AsyncTask, true >;

	#define Coro_Error()					(co_yield _Coro_::AsyncTaskCoro_Error{})
	#define Coro_IsCanceled					(co_await _Coro_::AsyncTaskCoro_IsCanceled{})
	#define Coro_Get()						(co_await _Coro_::AsyncTaskCoro_Get{})
	#define Coro_NextQueue(...)				(co_await _Coro_::AsyncTaskCoro_NextQueue{ __VA_ARGS__ })
	#define Coro_ContinueInQueue(...)		(co_yield _Coro_::AsyncTaskCoro_ChangeQueue{ __VA_ARGS__ })
	#define Coro_WaitResult(...)			(co_await _Coro_::CoroAwaiter_WithResultCopy{ __VA_ARGS__ })
	#define Coro_WaitResultRef(...)			(co_await _Coro_::WaitResultRef( __VA_ARGS__ ))
	#define Coro_WaitUniqueResult(...)		(co_await _Coro_::CoroAwaiter_WithUniqueResult{ __VA_ARGS__ })
	#define Coro_Continue(...)				(co_await _Coro_::Wait_FastCancelAndReset( __VA_ARGS__ ))
	#define Coro_WaitResultOrCancel(...)	(co_await _Coro_::CoroAwaiter_WaitResultCopyOrCancel{ __VA_ARGS__ })
	#define UncancellableCoro_Success()		(co_yield _Coro_::AsyncTaskCoro_Success{})


	using _Coro_::IsCoroutine;
	using _Coro_::IsPromise;
	using _Coro_::IsInlineCoroutine;
	using _Coro_::CoroCtorResult;


	//
	// Safe for Coroutine
	//
	template <typename T>
	struct CoSafe
	{
		T	_value;

	//	CoSafe ()						__NE___	= default;
		CoSafe (CoSafe &&)				__NE___ = default;

		template <typename B>
		CoSafe (B&& arg)				__NE___ : _value{FwdArg<B>(arg)} {}

		ND_ operator T ()				__NE___	{ return _value; }

		ND_ exact_t		operator & ()	__NE___	{ return &_value; }

		ND_ exact_t		operator -> ()	__NE___
		{
			if constexpr( requires{ _value.operator->(); })
				return _value.operator->();
			else
			if constexpr( IsPointer<T> or IsSpecializationOf< T, Ptr >){
				ASSERT( _value != null );
				return _value;
			}else{
				StaticAssert( not IsAnyPointer< T >);
				return &_value;
			}
		}
	};

	template <typename B>
	CoSafe(B&&) -> CoSafe< Conditional< IsRValueRef<B>, B, B&& >>;

/*
=================================================
	CreateAsync
----
	Helper function to validate coroutine.
	Checks:
	* lambda without capture
	* only static functions or lambdas
	* only smart pointers, no references, pointers, views
=================================================
*/
	template <typename CoroCtor, typename ...Args>
	Nd__IA exact_t  CreateAsync (CoroCtor&& ctor, Args&& ...args) __NE___
	{
		StaticAssert( CountOf<Args...>() > 0 );
		_Coro_::SafeCoro_PrintError< CoroCtor, TypeList<Args...> >();

		exact_t		result = FwdArg<CoroCtor>(ctor)( FwdArg<Args>(args) ... );

		using R = decltype(result);
		StaticAssert( IsCoroutine< R >);
		StaticAssert( not IsInlineCoroutine< R >);

		if constexpr( IsPromise< R >)
			return Promise< typename R::promise_type::Result_t >{ RVRef(result) };
		else
			return AsyncTask{ RVRef(result) };
	}

/*
=================================================
	CreateInline
=================================================
*/
	template <typename CoroCtor, typename ...Args>
	____IA exact_t  CreateInline (CoroCtor&& ctor, Args&& ...args) __NE___
	{
		StaticAssert( CountOf<Args...>() > 0 );
		_Coro_::SafeCoro_PrintError< CoroCtor, TypeList<Args...> >();

		exact_t		result = FwdArg<CoroCtor>(ctor)( FwdArg<Args>(args) ... );

		using R = decltype(result);
		StaticAssert( IsInlineCoroutine< R >);

		if constexpr( IsPromise< R >)
			return Promise< typename R::promise_type::Result_t >{ RVRef(result) };
		else
			return AsyncTask{ RVRef(result) };
	}

/*
=================================================
	CreateAsyncRev
----
	Put arguments first, then coroutine constructor.
	It is more readable for lambda:
	```
		CreateAsyncRev(
			ReadFile(), self,
			[](auto readOp, auto self) -> AsyncCoro { ... })
	```
=================================================
*/
	template <typename ...Args>
	Nd__IA exact_t  CreateAsyncRev (Args&& ...args) __NE___
	{
		StaticAssert( CountOf<Args...>() > 0 );

		exact_t		ctor = FoldExpr::Last( FwdArg<Args>(args) ... );

		using CoroCtor	= decltype(ctor);
		using ArgsTL	= typename TypeList<Args...>::PopBack::type;
		_Coro_::SafeCoro_PrintError< CoroCtor, ArgsTL >();

		exact_t		result = FoldExpr::ApplyExceptLast( FwdArg<CoroCtor>(ctor), FwdArg<Args>(args) ... );

		using R = decltype(result);
		StaticAssert( IsCoroutine< R >);
		StaticAssert( not IsInlineCoroutine< R >);

		if constexpr( IsPromise< R >)
			return Promise< typename R::promise_type::Result_t >{ RVRef(result) };
		else
			return AsyncTask{ RVRef(result) };
	}

/*
=================================================
	CreateInlineRev
=================================================
*/
	template <typename ...Args>
	____IA exact_t  CreateInlineRev (Args&& ...args) __NE___
	{
		StaticAssert( CountOf<Args...>() > 0 );

		exact_t		ctor = FoldExpr::Last( FwdArg<Args>(args) ... );

		using CoroCtor	= decltype(ctor);
		using ArgsTL	= typename TypeList<Args...>::PopBack::type;
		_Coro_::SafeCoro_PrintError< CoroCtor, ArgsTL >();

		exact_t		result = FoldExpr::ApplyExceptLast( FwdArg<CoroCtor>(ctor), FwdArg<Args>(args) ... );

		using R = decltype(result);
		StaticAssert( IsInlineCoroutine< R >);

		if constexpr( IsPromise< R >)
			return Promise< typename R::promise_type::Result_t >{ RVRef(result) };
		else
			return AsyncTask{ RVRef(result) };
	}

/*
=================================================
	WithResult
=================================================
*/
	template <typename FN, typename Result>
	____IA bool  WithResult (Promise<Result> promise, FN&& fn)
					NoExcept( IsNoExcept( FwdArg<FN>(fn)( _Coro_::CoroAwaiterImpl::GetResultCopy( promise )) ))
	{
		if ( not promise->IsCompleted() )
			return false;

		FwdArg<FN>(fn)( _Coro_::CoroAwaiterImpl::GetResultCopy( promise ));  // may throw
		return true;
	}

/*
=================================================
	DeferResult
=================================================
*/
	template <typename ResultType, typename ...Args>
	Promise<ResultType>  DeferResult (Args&& ...args) __NE___
	{
		return Promise<ResultType>{ new _Coro_::AsyncPromiseImpl< ResultType >{ _Coro_::MakeDeferResult(0), FwdArg<Args>(args) ... }};
	}

/*
=================================================
	operator co_await (task dependencies)
----
	it is safe to use reference because 'CoroAwaiter_FastCancel' object
	destroyed before 'Tuple' destruction.
=================================================
*/
	Nd__IF auto  operator co_await (const AsyncTask &dep) __NE___
	{
		return _Coro_::CoroAwaiter_FastCancel< AsyncTask >{ dep };
	}

	template <typename ...Deps>
	Nd__IF auto  operator co_await (const Tuple<Deps...> &deps) __NE___
	{
		return _Coro_::CoroAwaiter_FastCancel< Tuple<Deps...> >{ deps };
	}

/*
=================================================
	operator co_await (promise)
=================================================
*/
	template <typename Result>
	Nd__IF auto  operator co_await (const Promise<Result> &dep) __NE___
	{
		return _Coro_::CoroAwaiter_WaitResultCopyOrCancel{ True{"copy"}, dep };
	}

	template <typename Result>
	Nd__IF auto  operator co_await (const Array<Promise<Result>> &deps) __NE___
	{
		return _Coro_::CoroAwaiter_WaitResultCopyOrCancel_Array{ deps };
	}

	template <typename Result, ETaskQueue Queue>
	Nd__IF auto  operator co_await (ScheduledPromise<Result, Queue> dep) __NE___
	{
		return _Coro_::CoroAwaiter_WaitResultCopyOrCancel{ True{"copy"}, Promise<Result>{ RVRef(dep) }};
	}

} // AE::Threading

namespace AE::ImportCoroutines
{
	using AE::Threading::ETaskQueue;
	using AE::Threading::AsyncTask;
	using AE::Threading::AsyncCoro;
	using AE::Threading::UncancellableCoro;
	using AE::Threading::InlineCoro;
	using AE::Threading::ScheduledCoro;
	using AE::Threading::Promise;
	using AE::Threading::ScheduledPromise;
	using AE::Threading::InlinePromise;
	using AE::Threading::CoroCtorResult;
	using AE::Threading::CoSafe;
	using AE::Threading::CreateAsync;
	using AE::Threading::CreateAsyncRev;
	using AE::Threading::CreateInlineRev;
	using AE::Threading::DeferResult;
	using AE::Threading::WithResult;
	using AE::Threading::operator co_await;
}

