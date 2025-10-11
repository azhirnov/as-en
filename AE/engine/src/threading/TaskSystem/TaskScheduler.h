// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Async task states:
		TaskScheduler::Run() {
			set 'pending' state
			if one of dependencies are cancelled
				set 'cancellation' state
			if failed to enqueue
				OnCancel()
				return
		}
		TaskScheduler::ProcessTask() {
			if 'cancellation' state or one of input dependencies has been canceled {
				OnCancel()
				set 'canceled' state.
			}
			if 'pending' state and all input dependencies are complete {
				set 'in_progress' state
				Run() {
					user may check for 'cancellation' state.

					if something goes wrong
						call OnFailure() and return.
					if need to restart task
						call Continue() and return.
				}
				if successfully completed
					set 'completed' state and return.
				if 'cancellation' state
					OnCancel()
					set 'canceled' state and return.
				if 'canceled' or 'failed'
					return.
				if throw exception
					OnCancel()
					set 'canceled' state and return.
				if 'continue' state
					task added to queue.
			}
		}

	Order guaranties:
		- AsyncTask::Run() will be called after all input dependencies Run() or OnCancel() methods have completed.
		- Run() and OnCancel() methods of the same task executed sequentially.

	Error handling:
		- If can't create task or add to queue						-> return default cancelled task
		- If null task used as dependency							-> ignore
		- If null coroutine used as dependency (co_await noop_coro)	-> cancel coroutine

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/TaskScheduler-ru.md)
*/

#pragma once

#include "threading/TaskSystem/AsyncTask.h"
#include "threading/Containers/LfIndexedPool.h"
#include "threading/Memory/GlobalLinearAllocator.h"

namespace AE::Threading { class TaskScheduler; }
namespace AE
{
	Threading::TaskScheduler&  Scheduler () __NE___;

#ifndef AE_CFG_RELEASE
	static constexpr Base::minutes		DefaultTimeout	{60};	// 60 min - for debugging
#else
	static constexpr Base::milliseconds	DefaultTimeout	{500};	// 0.5 sec
#endif
}

namespace AE::Threading
{
	enum class EThreadSeed : usize {};
	enum class ECpuCoreId  : ubyte  { Unknown = 0xFF };		// logical core for SMT/hyperthreding


	//
	// Thread interface
	//
	class IThread : public EnableRC< IThread >
	{
	// types
	public:
		struct ProfilingInfo
		{
			StringView		threadName;
			StringView		coreName;
			usize			threadId	= 0;
			ECpuCoreId		coreId		= Default;
			uint			curFreq		= 0;	// MHz
			uint			minFreq		= 0;	// MHz
			uint			maxFreq		= 0;	// MHz
		};


	// interface
	public:
			virtual bool  Attach (uint uid, ECpuCoreId coreId)	__NE___ = 0;
			virtual void  Detach ()								__NE___ = 0;

		ND_ virtual usize			DbgID ()					C_NE___ = 0;
		ND_ virtual ProfilingInfo	GetProfilingInfo ()			C_NE___ = 0;
	};
//-----------------------------------------------------------------------------



	//
	// Task Dependency Manager interface
	//
	class ITaskDependencyManager : public EnableRC< ITaskDependencyManager >
	{
	// types
	public:
		using Task				= AE::_Coro_::AsyncTaskImpl;
		using TaskApi			= Task::TaskDependencyManagerApi;
		using TaskDependency	= Task::TaskDependency;
		using CheckDepFn_t		= Function< void (StringView, AsyncTask) >;


	// interface
	public:
		// returns 'true' if added dependency to task.
		// returns 'false' if dependency is cancelled or on error.
		ND_ virtual bool  Resolve (AnyTypeCRef dep, Task &, Bool defaultIsStrongDep) __NE___ = 0;

		// only for debugging
		DEBUG_ONLY(
			virtual void  DbgDetectDeadlock (const CheckDepFn_t &) __NE___ {};)
	};
//-----------------------------------------------------------------------------



	enum class EIOServiceType : uint
	{
		File,	// async file IO
	};


	//
	// Input/Output Service
	//
	class IOService : public EnableRC< IOService >
	{
	// interface
	public:
		// returns number of processed events.
		//		Thread-safe: yes
		ND_ virtual usize			ProcessEvents ()	__NE___ = 0;

		ND_ virtual EIOServiceType	GetIOServiceType () C_NE___ = 0;
	};
//-----------------------------------------------------------------------------



	//
	// Task Scheduler
	//
	class TaskScheduler final : public Noncopyable
	{
		friend struct InPlace<TaskScheduler>;

	// types
	public:
		struct Config
		{
			ubyte		maxPerFrameQueues	= 2;
			ubyte		maxBackgroundQueues	= 2;
			ubyte		maxRenderQueues		= 2;
			ubyte		maxIOAccessThreads	= 0;
			ECpuCoreId	mainThreadCoreId	= Default;
		};

		class InstanceCtor {
		public:
			static void  Create ()	__NE___;
			static void  Destroy ()	__NE___;
		};

		using LoopingFlag_t	= Atomic<uint>;
		using TimePoint_t	= HighResClock::time_point;

	private:
		using Task				= _Coro_::AsyncTaskImpl;
		using TaskApi			= Task::TaskSchedulerApi;
		using OutputChunk_t		= TaskApi::OutputChunk;

		struct PerQueue
		{
			Unique<class LfTaskQueue>	ptr;

			DEBUG_ONLY( ulong			totalProcessed	= 0;)
		};

		using TaskQueues_t		= StaticArray< PerQueue, uint(ETaskQueue::_Count) >;
		using TaskDepsMngr_t	= FlatHashMap< TypeId, RC<ITaskDependencyManager> >;
		using OutputChunkPool_t	= LfIndexedPool< OutputChunk_t, uint, 64*64, 64, GlobalLinearAllocatorRef >;


	// variables
	private:
		TaskQueues_t		_queues;

		OutputChunkPool_t	_chunkPool;

		SharedMutex			_taskDepsMngrsGuard;	// TODO: init on start, remove lock
		TaskDepsMngr_t		_taskDepsMngrs;

		Mutex				_threadGuard;
		Array<RC<IThread>>	_threads;
		RC<IThread>			_mainThread;

		RC<IOService>		_fileIOService;

		PROFILE_ONLY(
			AtomicRC<ITaskProfiler>	_profiler;
		)
		DEBUG_ONLY( struct{
			StructAtomic<TimePoint_t>	lastUpdate;
			Atomic<ulong>				numChecks	{0};
			Atomic<ulong>				numLocks	{0};
			const secondsf				interval	{10.f};
			const double				minRate		{0.01};
		}							_deadlockCheck;)


	// methods
	public:
		ND_ bool  Setup (const Config &cfg)											__NE___;
			void  SetProfiler (RC<ITaskProfiler> profiler)							__NE___;
			void  Release ()														__NE___;

			template <typename T>
			bool  RegisterDependency (RC<ITaskDependencyManager> mngr)				__NE___;

			template <typename T>
			bool  UnregisterDependency ()											__NE___;


	// thread api //
			bool  AddThread (RC<IThread> thread, ECpuCoreId coreId = Default)		__NE___;

			bool  ProcessTask (ETaskQueue type, EThreadSeed seed)					__NE___;
			bool  ProcessTasks (const EThreadArray &threads, EThreadSeed seed)		__NE___;
			bool  ProcessTasks (const EThreadArray &threads, EThreadSeed seed,
								uint maxTasks)										__NE___;
			bool  ProcessFileIO ()													__NE___;

			void  SuspendThread (uint iteration)									__NE___;

		ND_ static EThreadSeed	GetDefaultSeed ()									__NE___;


	// task api //
		template <typename CoroType, typename ...Deps>
			CoroType	Run (CoroType				task,
							 const Tuple<Deps...> &	deps	= Default,
							 const SourceLoc &		loc		= SourceLoc::current())	__NE___;

		template <typename CoroType, typename ...Deps>
			CoroType    Run (ETaskQueue				queueType,
							 CoroType				task,
							 const Tuple<Deps...> &	deps	= Default,
							 StringView				dbgName	= Default,
							 const SourceLoc &		loc		= SourceLoc::current())	__NE___;
			
		template <typename TaskType, typename ...Deps>
		ND_	bool  EnqueueNew (ETaskQueue			queueType,
							  TaskType				task,
							  const Tuple<Deps...>&	deps	= Default,
							  StringView			dbgName	= Default,
							  Bool					defaultIsStrongDep = True{},
							  const SourceLoc &		loc		= SourceLoc::current())	__NE___;

			bool  Cancel (const AsyncTask &task, Bool fastCancel = False{})			__NE___;
			bool  Enqueue (AsyncTask task)											__NE___;
			bool  Enqueue (AsyncTask task, ETaskQueue queue)						__NE___;


	// synchronizations //
		template <typename ...Deps>
		ND_ AsyncTask	WaitAsync (ETaskQueue queue, const Tuple<Deps...> &deps)	__NE___;

		ND_ bool		Wait (ArrayView<AsyncTask>	tasks,
							  const EThreadArray &	threads,
							  nanoseconds			timeout)						__NE___;
		ND_ bool		Wait (ArrayView<AsyncTask>	tasks,
							  nanoseconds			timeout)						__NE___;
		ND_ bool		Wait (ArrayView<AsyncTask>	tasks,
							  TimePoint_t			endTime)						__NE___;

		ND_ bool		Wait (ArrayView<AsyncTask>	tasks,
							  const EThreadArray &	threads,
							  TimePoint_t			endTime,
							  uint					maxTasksPerTick)				__NE___;


	// other //
		Nd__IF Ptr<IOService>	GetFileIOService ()									C_NE___ { return _fileIOService.get(); }

		Nd__IF static auto		GetCanceledTask ()									__NE___	{ return _Coro_::AsyncTaskImpl::CanceledTask::s_canceled.GetRC(); }
		Nd__IF static auto		GetCanceledDSRequest ()								__NE___;

		friend TaskScheduler&	AE::Scheduler ()									__NE___;

		PROFILE_ONLY(
			ND_ RC<ITaskProfiler>	GetProfiler ()									__NE___	{ return _profiler.load(); }
		)


	// debugging //
			void  DbgDetectDeadlock ()												__NE___;


		class AsyncTaskApi
		{
			friend class _Coro_::AsyncTaskImpl;

			ND_ static OutputChunkPool_t&  GetChunkPool ()							__NE___;

			template <typename ...Args>
			NdCx__ static bool  AddDependencies (Task &task, const Tuple<Args...> &args,
												 Bool defaultIsStrongDep)			__NE___;
		};

	private:
		TaskScheduler ()															__NE___;
		~TaskScheduler ()															__NE___;

		ND_ static TaskScheduler&  _Instance ()										__NE___;

		ND_ bool  _InitIOServices (const Config &cfg)								__NE___;

		ND_ bool  _InsertTask (AsyncTask task)										__NE___;

		ND_ static bool	 _IsAllComplete (ArrayView<AsyncTask> tasks)				__NE___;

		template <usize I, typename ...Args>
		NdCx__ bool  _AddDependencies (Task &task, const Tuple<Args...> &args,
										Bool defaultIsStrongDep)					__NE___;

		template <typename T>
		ND_ bool  _AddCustomDependency (Task &task, const T &dep, Bool defaultIsStrongDep)	__NE___;

		ND_ bool  _AddTaskDependencies (Task &task, const AsyncTask &deps, Bool isStrong)	__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	RegisterDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::RegisterDependency (RC<ITaskDependencyManager> mngr) __NE___
	{
		CHECK_ERR( mngr );
		EXLOCK( _taskDepsMngrsGuard );
		return _taskDepsMngrs.insert_or_assign( TypeIdOf<T>(), RVRef(mngr) ).second;	// should not throw
	}

/*
=================================================
	UnregisterDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::UnregisterDependency () __NE___
	{
		EXLOCK( _taskDepsMngrsGuard );
		return _taskDepsMngrs.erase( TypeIdOf<T>() ) > 0;
	}

/*
=================================================
	Run
----
	always return non-null task
=================================================
*/
	template <typename CoroType, typename ...Deps>
	forceinline CoroType  TaskScheduler::Run (CoroType task, const Tuple<Deps...> &deps, const SourceLoc &loc) __NE___
	{
		return Run<CoroType>( Default, RVRef(task), deps, Default, loc );
	}

	template <typename CoroType, typename ...Deps>
	forceinline CoroType  TaskScheduler::Run (ETaskQueue queueType, CoroType task, const Tuple<Deps...> &deps,
											  StringView dbgName, const SourceLoc &loc) __NE___
	{
		if_likely( EnqueueNew( queueType, task, deps, dbgName, True{"strong deps"}, loc ));
		else
			task = CoroType{_Coro_::AsyncTaskImpl::CanceledTask::s_canceled.operator->()};
		return RVRef(task);
	}
	
/*
=================================================
	EnqueueNew
=================================================
*/
	template <typename TaskType, typename ...Deps>
	bool  TaskScheduler::EnqueueNew (ETaskQueue queueType, TaskType task, const Tuple<Deps...> &deps,
									 StringView dbgName, Bool defaultIsStrongDep, const SourceLoc &loc) __NE___
	{
		StaticAssert( IsBaseOf< Task, RemoveAllQualifiers<decltype(*task)> >);
		if constexpr( IsCoroutine< TaskType >){
			StaticAssert( not IsInlineCoroutine< TaskType >);  // inline coroutine is not allowed here, use 'Enqueue()'
		}

		CHECK_ERR( task );
		CHECK_ERR( task->Status() == ETaskStatus::Initial );
		
		#if AE_ENABLE_TASK_NAME
			TaskApi::Init( *task, queueType, dbgName, loc );
		#else
			TaskApi::Init( *task, queueType );
			Unused( dbgName, loc );
		#endif
		
		// in 'AddDependencies()' current task has been added to the input dependencies
		// and they may decrease counter '_waitCount' at any time, so set +1 to forbid state changing
		TaskApi::SetWaitCounter( *task, 1 );

		if_unlikely( not _AddDependencies<0>( *task, deps, defaultIsStrongDep ))
		{
			// add task to queue only to call 'OnCancel()'
			Unused( TaskApi::SetCancellationState( *task ));
		}
		
		// remove +1 and check
		auto	count = TaskApi::DecWaitCounter( *task );
		ASSERT_Gt( count, 0 );
		Unused( count );

		return Enqueue( AsyncTask{RVRef(task)} );
	}

/*
=================================================
	_AddDependencies
=================================================
*/
	template <usize I, typename ...Args>
	__CxIF bool  TaskScheduler::_AddDependencies (Task &task, const Tuple<Args...> &args, Bool defaultIsStrongDep) __NE___
	{
		if constexpr( I < CountOf<Args...>() )
		{
			using T = typename TypeList< Args... >::template Get<I>;

			// current task will start anyway, regardless of whether dependent tasks are canceled
			if constexpr( IsSame< T, WeakDep >) {
				if_unlikely( not _AddTaskDependencies( task, args.template Get<I>()._task, False{"weak"} )) return false;
			}else
			if constexpr( IsSame< T, WeakDepArray >) {
				for (auto& dep : args.template Get<I>())
					if_unlikely( not _AddTaskDependencies( task, dep, False{"weak"} )) return false;
			}else
			if constexpr( IsSame< T, ArrayView<WeakDep> >) {
				for (auto& dep : args.template Get<I>())
					if_unlikely( not _AddTaskDependencies( task, dep._task, False{"weak"} )) return false;
			}else

			// current task will be canceled if one of dependent task are canceled
			if constexpr( IsSame< T, StrongDep >) {
				if_unlikely( not _AddTaskDependencies( task, args.template Get<I>()._task, True{"strong"} )) return false;
			}else
			if constexpr( IsSame< T, StrongDepArray >) {
				for (auto& dep : args.template Get<I>())
					if_unlikely( not _AddTaskDependencies( task, dep, True{"strong"} )) return false;
			}else
			if constexpr( IsSame< T, ArrayView<StrongDep> >) {
				for (auto& dep : args.template Get<I>())
					if_unlikely( not _AddTaskDependencies( task, dep._task, True{"strong"} )) return false;
			}else

			// default weak/strong
			if constexpr( IsSpecializationOf< T, ArrayView >				and
						  requires{ AsyncTask{ typename T::value_type{} }; })
			{
				for (auto& dep : args.template Get<I>())
					if_unlikely( not _AddTaskDependencies( task, AsyncTask{dep}, defaultIsStrongDep )) return false;
			}else
			if constexpr( IsConstructible< AsyncTask, T >) {
				if_unlikely( not _AddTaskDependencies( task, AsyncTask{args.template Get<I>()}, defaultIsStrongDep )) return false;
			}else

			// custom
			{
				if_unlikely( not _AddCustomDependency( task, args.template Get<I>(), defaultIsStrongDep )) return false;
			}

			return _AddDependencies<I+1>( task, args, defaultIsStrongDep );
		}
		else
		{
			Unused( task, args, defaultIsStrongDep );
			return true;
		}
	}
	
	template <typename ...Args>
	__CxIF bool  TaskScheduler::AsyncTaskApi::AddDependencies (Task &task, const Tuple<Args...> &args, Bool defaultIsStrongDep) __NE___
	{
		return Scheduler()._AddDependencies<0>( task, args, defaultIsStrongDep );
	}

/*
=================================================
	_AddCustomDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::_AddCustomDependency (Task &task, const T &dep, Bool defaultIsStrongDep) __NE___
	{
		StaticAssert( not IsConst<T> );
		SHAREDLOCK( _taskDepsMngrsGuard );

		auto	iter = _taskDepsMngrs.find( TypeIdOf<T>() );
		CHECK_ERR_MSG( iter != _taskDepsMngrs.end(),
			"Can't find dependency manager for type: "s << TypeNameOf<T>() );

		return iter->second->Resolve( AnyTypeCRef{dep}, task, defaultIsStrongDep );
	}
	
/*
=================================================
	WaitAsync
=================================================
*/
	template <typename ...Deps>
	AsyncTask  TaskScheduler::WaitAsync (ETaskQueue queue, const Tuple<Deps...> &deps) __NE___
	{
		// TODO: remove?
		return Run( queue, AsyncTask{DeferResult<bool>( false )}, deps, "WaitAsync" );
	}

} // AE::Threading
//-----------------------------------------------------------------------------


namespace AE
{
/*
=================================================
	Scheduler
=================================================
*/
	Nd__IF Threading::TaskScheduler&  Scheduler () __NE___
	{
		return Threading::TaskScheduler::_Instance();
	}

} // AE

namespace AE::_Coro_
{
/*
=================================================
	ScheduledCoroImpl::get_return_object
=================================================
*/
	template <ETaskQueue Queue>
	forceinline auto  ScheduledCoroImpl<Queue>::get_return_object (const SourceLoc &loc) __NE___
	{
		return Scheduler().Run( Queue, Coroutine_t{ *this }, Tuple{}, Default, loc );
	}
	
	template <typename ResultType, ETaskQueue Queue>
	forceinline auto  ScheduledPromiseImpl<ResultType,Queue>::get_return_object (const SourceLoc &loc) __NE___
	{
		return Scheduler().Run( Queue, Coroutine_t{ *this }, Tuple{}, Default, loc );
	}

/*
=================================================
	ScheduledInlineCoro::_AddToScheduler
=================================================
*/
	template <ETaskQueue Queue>
	forceinline void  ScheduledInlineCoro<Queue>::_AddToScheduler () __NE___
	{
		// inline coro without 'co_await' must finish at this point
		if ( _coro != null							and
			 _coro->Status() == ETaskStatus::Continue )
		{
			Scheduler().Enqueue( AsyncTask{_coro}, Queue );
		}
		_coro = null;
	}
	
/*
=================================================
	ScheduledInlineCoro::_AddToScheduler
=================================================
*/
	template <typename ResultType, ETaskQueue Queue>
	forceinline void  ScheduledInlinePromise<ResultType, Queue>::_AddToScheduler () __NE___
	{
		// inline coro without 'co_await' must finish at this point
		if ( _coro != null							and
			 _coro->Status() == ETaskStatus::Continue )
		{
			Scheduler().Enqueue( AsyncTask{_coro}, Queue );
		}
		_coro = null;
	}

/*
=================================================
	AsyncTaskImpl::yield_value
=================================================
*/
	forceinline auto  AsyncTaskImpl::yield_value (AsyncTaskCoro_ChangeQueue newQueue) __NE___
	{
		_queueType = newQueue.value;

		Unused( _Continue( Tuple{} ));	// TODO ?
		return std::suspend_always{};
	}

/*
=================================================
	AsyncTaskImpl::_Continue
----
	if 'deps' is empty then always return 'true' to suspend.
	if 'deps' is not empty and is complete or cancelled then return 'false' to resume.
	if 'deps' is not complete or cancelled then return 'true' to suspend.
=================================================
*/
	template <typename ...Deps>
	forceinline bool  AsyncTaskImpl::_Continue (const Tuple<Deps...> &deps, Bool defaultIsStrongDep) __NE___
	{
		ASSERT( _isRunning.load() );
		ASSERT( _waitCount.load() == 0 or AllBits( _flags, EFlags::RunCancelled ));	// all input dependencies must complete

		if constexpr( CountOf<Deps...>() > 0 )
		{
			// in 'AddDependencies()' current task has been added to the input dependencies
			// and they may decrease counter '_waitCount' at any time, so set +1 to forbid state changing
			_waitCount.store( 1 );

			if_unlikely( not TaskScheduler::AsyncTaskApi::AddDependencies( *this, deps, defaultIsStrongDep ))
			{
				// cancel task
				Unused( _SetCancellationState() );
				bool suspend = defaultIsStrongDep and NoBits( _flags, EFlags::RunCancelled );
				return suspend;
			}

			// remove +1 and check
			auto	count = _waitCount.fetch_sub( 1 );
			ASSERT_Gt( count, 0 );

			if_unlikely( count == 1 )
			{
				const EStatus	stat = _status.load();
				ASSERT( AnyEqual( stat, EStatus::InProgress, EStatus::Cancellation ));

				if ( stat == EStatus::Cancellation			and
					 NoBits( _flags, EFlags::RunCancelled ))
					return true;  // suspend

				return false;  // resume
			}
		}
		else
		{
			Unused( deps );

			const EStatus	stat = _status.load();
			ASSERT( AnyEqual( stat, EStatus::InProgress, EStatus::Cancellation ));

			if ( stat == EStatus::Cancellation			and
				 AllBits( _flags, EFlags::RunCancelled ))
				return false;  // resume
		}


		EStatus expected = EStatus::InProgress;
		for (; not _status.CAS( INOUT expected, EStatus::Continue );)
		{
			// status has been changed in another thread
			if_unlikely( (expected == EStatus::Cancellation) or (expected > EStatus::_Finished) )
				break;

			// 'CAS' can return 'false' even if expected value is the same as current value in atomic
			ASSERT( expected == EStatus::InProgress );
			ThreadUtils::Pause();
		}

		ASSERT( expected != EStatus::Completed );	// no way to this may happens
		DEBUG_ONLY( _isRunning.store( false );)

		return true; // suspend
	}

} // AE::_Coro_
