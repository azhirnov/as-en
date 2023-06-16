// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Async task states:
		TaskScheduler::Run() {
			set 'pending' state
			if one of dependencies are cancelled
				set 'cancellation' state
			if failed to enqueue
				Oncancel() ???
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
				if seccessfully completed
					set 'completed' state and return.
				if 'cancellation' state
					OnCancel()
					set 'canceled' state and return.
				if 'canceled' or 'failed'
					return
				if throw exception
					OnCancel()
					set 'canceled' state and return.
				if 'continue' state
					task added to queue
			}
		}

	Order guaranties:
		- AsyncTask::Run() will be called after all input dependencies Run() or OnCancel() methods have completed.
		- Run() and OnCancel() methods of the same task executed sequentially.

	Error handling:
		- If can't create task or add to queue						-> return default cancelled task
		- If null task used as dependency							-> ignore
		- If null coroutine used as dependency (co_await noop_coro)	-> cancel coroutine
*/

#pragma once

#include "base/Containers/AnyTypeRef.h"
#include "base/Utils/EnumBitSet.h"
#include "base/CompileTime/TypeList.h"
#include "base/Math/POTValue.h"

#include "threading/TaskSystem/AsyncTask.h"
#include "threading/TaskSystem/Coroutine.h"
#include "threading/Containers/LfIndexedPool2.h"
#include "threading/Memory/GlobalLinearAllocator.h"

#ifdef AE_DEBUG
#	define AE_SCHEDULER_PROFILING( /* code */... )	__VA_ARGS__
#else
#	define AE_SCHEDULER_PROFILING( /* code */... )
#endif

namespace AE::Threading { class TaskScheduler; }
namespace AE { Threading::TaskScheduler&  Scheduler () __NE___; }

namespace AE::Threading
{
	using TaskDependencyManagerPtr	= RC< class ITaskDependencyManager >;


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
			uint			coreId		= 0;
			uint			curFreq		= 0;	// MHz
			uint			minFreq		= 0;	// MHz
			uint			maxFreq		= 0;	// MHz
		};


	// interface
	public:
			virtual bool  Attach (uint uid, uint coreId)	__NE___ = 0;
			virtual void  Detach ()							__NE___ = 0;

		ND_ virtual usize			DbgID ()				C_NE___ = 0;
		ND_ virtual ProfilingInfo	GetProfilingInfo ()		C_NE___ = 0;
	};
//-----------------------------------------------------------------------------



	//
	// Task Dependency Manager interface
	//
	class ITaskDependencyManager : public EnableRC< ITaskDependencyManager >
	{
	// types
	public:
		using CheckDepFn_t	= Function< void (StringView, AsyncTask, IAsyncTask::TaskDependency) >;


	// interface
	public:
		// returns 'true' if added dependency to task.
		// returns 'false' if dependency is cancelled or on error.
		ND_ virtual bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)	__NE___ = 0;
		
		// only for debugging
		AE_SCHEDULER_PROFILING(
			virtual void  DbgDetectDeadlock (const CheckDepFn_t &)							__NE___ {};)

		// helper functions
		static void  _SetDependencyCompletionStatus (const AsyncTask &task, uint depIndex, bool cancel = false) __NE___;
	};
//-----------------------------------------------------------------------------



	enum class EIOServiceType : uint
	{
		File,
	};


	//
	// Input/Output Service
	//
	class IOService : public ITaskDependencyManager
	{
	// interface
	public:
		// returns number of processed events.
		//		thread-safe: yes
		ND_ virtual usize			ProcessEvents ()	__NE___ = 0;

		ND_ virtual EIOServiceType	GetIOServiceType () C_NE___ = 0;
	};
//-----------------------------------------------------------------------------
	


	//
	// Lock-free Task Queue
	//
	class LfTaskQueue
	{
	// types
	private:
		static constexpr uint	TasksPerChunk		= (1u << 7) - 2;	// 2 pointers are reserved
		static constexpr uint	MaxChunks			= 2;
		static constexpr uint	MaxDepth			= 128*4;
		static constexpr uint	SpinlockWaitCount	= 3;

		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;
		using TaskArr_t		= StaticArray< AsyncTask, TasksPerChunk >;
		using EStatus		= IAsyncTask::EStatus;

		union PackedBits
		{
			struct {
				usize	locked	: 1;	// 1 - spin-lock is locked
				usize	pos		: 8;	// last iterator position in 'Chunk::array'
				usize	count	: 8;	// elements count in 'Chunk::array'
			}		pack;
			usize	bits;

			PackedBits ()									__NE___ : bits{0} {}

			ND_ bool		IsLocked ()						C_NE___	{ return pack.locked == 1; }
			ND_ PackedBits	Lock ()							C_NE___	{ PackedBits tmp{*this};  tmp.pack.locked = 1;  return tmp; }
			ND_ bool		operator == (PackedBits rhs)	C_NE___	{ return bits == rhs.bits; }
		};

		struct alignas(AE_CACHE_LINE) Chunk
		{
			BitAtomic< PackedBits >		packed	{PackedBits{}};
			Atomic< Chunk *>			next	{null};
			TaskArr_t					array	{};

			Chunk () __NE___ {}
		};

		STATIC_ASSERT_64( sizeof(Chunk) == 1_Kb );
		STATIC_ASSERT( TasksPerChunk * MaxChunks * MaxDepth < 1'000'000 );

		using ChunkArray_t	= StaticArray< Chunk *, MaxChunks >;


	// variables
	private:
		ChunkArray_t	_chunks		{};

		//POTValue		_seedMask;

		AE_SCHEDULER_PROFILING(
			String			_name;
			Atomic<ulong>	_searchTime		{0};	// Nanoseconds	// task search time
			Atomic<ulong>	_workTime		{0};	// Nanoseconds
			Atomic<ulong>	_insertionTime	{0};	// Nanoseconds
			Atomic<ulong>	_maxTasks		{0};
			Atomic<slong>	_taskCount		{0};
			Atomic<ulong>	_totalProcessed	{0};
		)


	// methods
	public:
		LfTaskQueue (POTValue seedMask, StringView name)				__Th___;
		~LfTaskQueue ()													__NE___	{ Release(); }

		ND_ AsyncTask	Pull (usize seed)								__NE___;
			bool		Process (usize seed)							__NE___;
			void		Add (AsyncTask task, usize seed)				__NE___;

			void		WriteProfilerStat ()							__NE___;

		ND_ Bytes		MaxAllocationSize ()							C_NE___;
		ND_ Bytes		AllocatedSize ()								C_NE___;

			void		Release ()										__NE___;
			

	  // debugging //
	  #ifdef AE_DEBUG
			void	DbgDetectDeadlock (const Function<void (AsyncTask)> &fn)__NE___;
		ND_ ulong	GetTotalProcessedTasks ()								C_NE___	{ return _totalProcessed.load(); }
	  #endif

			AE_GLOBALLY_ALLOC

	private:
		ND_ static bool  _RemoveTask (TaskArr_t& arr, INOUT usize& pos, INOUT usize& count, OUT AsyncTask& task) __NE___;
	};
//-----------------------------------------------------------------------------



	//
	// Task Scheduler
	//
	class TaskScheduler final : public Noncopyable
	{
		friend class IAsyncTask;	// calls '_AddDependencies()', '_GetChunkPool()'
		friend struct InPlace<TaskScheduler>;

	// types
	public:
		struct Config
		{
			ubyte	maxPerFrameQueues	= 2;
			ubyte	maxBackgroundQueues	= 2;
			ubyte	maxRenderQueues		= 2;
			ubyte	maxIOThreads		= 0;
			ubyte	mainThreadCoreId	= UMax;
		};

	private:
		using TimePoint_t		= std::chrono::high_resolution_clock::time_point;
		using EStatus			= IAsyncTask::EStatus;
		using OutputChunk_t		= IAsyncTask::OutputChunk;
		using WaitBits_t		= IAsyncTask::WaitBits_t;

		struct PerQueue
		{
			Unique<LfTaskQueue>		ptr;
			
			AE_SCHEDULER_PROFILING(
				ulong				totalProcessed	= 0;
			)
		};

		using TaskQueues_t		= StaticArray< PerQueue, uint(ETaskQueue::_Count) >;
		using ThreadPtr			= RC< IThread >;
		
		using TaskDepsMngr_t	= FlatHashMap< std::type_index, TaskDependencyManagerPtr >;

		using OutputChunkPool_t	= LfIndexedPool2< IAsyncTask::OutputChunk, uint, 64*64, 64 >;

		static constexpr uint			_WaitAll_SpinCount	= 20'000;
		static constexpr microseconds	_WaitAll_SleepTime	{100};

		class CanceledTask final : public IAsyncTask
		{
		public:
			CanceledTask () : IAsyncTask{ETaskQueue::PerFrame} { _DbgSet( EStatus::Canceled ); }
			
			void		Run ()		__Th_OV {}
			StringView  DbgName ()	C_NE_OV	{ return "canceled"; }
		};

		class WaitAsyncTask final : public IAsyncTask
		{
		public:
			explicit WaitAsyncTask (ETaskQueue type) : IAsyncTask{type} {}

			void		Run ()		__Th_OV {}
			StringView  DbgName ()	C_NE_OV	{ return "WaitAsync"; }
		};


	// variables
	private:
		TaskQueues_t		_queues;

		AsyncTask			_canceledTask;			// readonly
		
		OutputChunkPool_t	_chunkPool;

		SharedMutex			_taskDepsMngrsGuard;	// TODO: init on start, remove lock
		TaskDepsMngr_t		_taskDepsMngrs;

		Mutex				_threadGuard;
		Array<ThreadPtr>	_threads;
		ThreadPtr			_mainThread;

		RC<IOService>		_fileIOService;
		Atomic<uint>		_coreId				{0};

		PROFILE_ONLY(
			AtomicRC<ITaskProfiler>	_profiler;
		)
		AE_SCHEDULER_PROFILING( struct{
			BitAtomic<TimePoint_t>		lastUpdate;
			Atomic<ulong>				numChecks	{0};
			Atomic<ulong>				numLocks	{0};
			const secondsf				interval	{10.f};
			const double				minRate		{0.01};
		}							_deadlockCheck;)


	// methods
	public:
			static void  CreateInstance ();
			static void  DestroyInstance ();

		ND_ bool  Setup (const Config &cfg)											__NE___;
			void  SetProfiler (RC<ITaskProfiler> profiler)							__NE___;
			void  Release ()														__NE___;

			template <typename T>
			bool  RegisterDependency (TaskDependencyManagerPtr mngr)				__NE___;

			template <typename T>
			bool  UnregisterDependency ()											__NE___;


	// thread api //
			bool  AddThread (ThreadPtr thread)										__NE___;
			bool  AddThread (ThreadPtr thread, uint coreId)							__NE___;

			bool  ProcessTask (ETaskQueue type, uint seed)							__NE___;
			bool  ProcessTasks (const EThreadArray &threads, uint seed)				__NE___;

		ND_ AsyncTask  PullTask (ETaskQueue type, uint seed)						__NE___;


	// task api //
		template <typename TaskType,
				  typename ...Ctor,
				  typename ...Deps
				 >
			AsyncTask     Run (Tuple<Ctor...>		&&	ctor = Default,
							   const Tuple<Deps...> &	deps = Default)				__NE___;
		
		template <typename ...Deps>
			bool		  Run (AsyncTask				task,
							   const Tuple<Deps...> &	deps = Default)				__NE___;


	  #ifdef AE_HAS_COROUTINE
		template <typename ...Deps>
			AsyncTask     Run (ETaskQueue				queueType,
							   CoroTask					coro,
							   const Tuple<Deps...> &	deps	= Default,
							   StringView				dbgName	= Default)			__NE___;

		template <typename ...Deps>
			AsyncTask     Run (CoroTask					coro,
							   const Tuple<Deps...> &	deps	= Default)			__NE___;
		
		template <typename T,
				  typename ...Deps
				 >
		ND_ Coroutine<T>  Run (ETaskQueue				queueType,
							   Coroutine<T>				coro,
							   const Tuple<Deps...> &	deps	= Default,
							   StringView				dbgName	= Default)			__NE___;

		template <typename T,
				  typename ...Deps
				 >
		ND_ Coroutine<T>  Run (Coroutine<T>				coro,
							   const Tuple<Deps...>	&	deps	= Default)			__NE___;
	  #endif // AE_HAS_COROUTINE


			bool  Cancel (const AsyncTask &task)									__NE___;

			bool  Enqueue (AsyncTask task)											__NE___;


	// synchronizations //
		template <typename ...Deps>
		ND_ AsyncTask	WaitAsync (ETaskQueue queue, const Tuple<Deps...> &deps)	__NE___;

		ND_ bool		Wait (ArrayView<AsyncTask>	tasks,
							  const EThreadArray &	threads = Default,
							  nanoseconds			timeout = GetDefaultTimeout())	__NE___;


	// other //
		ND_ Ptr<IOService>		GetFileIOService ()									C_NE___ { return _fileIOService.get(); }

		ND_ AsyncTask			GetCanceledTask ()									C_NE___	{ return _canceledTask; }

		ND_ IThread const*		GetMainThread ()									C_NE___	{ return _mainThread.get(); }
		
		friend TaskScheduler&	AE::Scheduler ()									__NE___;

		ND_ static constexpr nanoseconds  GetDefaultTimeout ()						__NE___;
		
		PROFILE_ONLY(
			ND_ RC<ITaskProfiler>	GetProfiler ()									__NE___	{ return _profiler.load(); }
		)


	// debugging //
			void  DbgDetectDeadlock ()												__NE___;


	private:
		TaskScheduler ();
		~TaskScheduler ()															__NE___;
		
		ND_ static TaskScheduler&  _Instance ()										__NE___;

		ND_ bool  _InitIOServices (const Config &cfg)								__NE___;

		ND_ bool  _InsertTask (AsyncTask task, uint bitIndex)						__NE___;

		ND_ OutputChunkPool_t&  _GetChunkPool ()									__NE___	{ return _chunkPool; }

		template <usize I, typename ...Args>
		ND_ constexpr bool  _AddDependencies (const AsyncTask &task, const Tuple<Args...> &args, INOUT uint &bitIndex)		__NE___;

		template <typename T>
		ND_ bool  _AddCustomDependency (const AsyncTask &task, const T &dep, INOUT uint &bitIndex)							__NE___;

		ND_ bool  _AddTaskDependencies (const AsyncTask &task, const AsyncTask &deps, Bool isStrong, INOUT uint &bitIndex)	__NE___;
	};
//-----------------------------------------------------------------------------

	
	
/*
=================================================
	GetDefaultTimeout
=================================================
*/
	constexpr nanoseconds  TaskScheduler::GetDefaultTimeout () __NE___
	{
		#ifdef AE_DEBUG
			return minutes{60};		// 60 min - for debugging
		#else
			return seconds{30};		// 30 sec
		#endif
	}

/*
=================================================
	RegisterDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::RegisterDependency (TaskDependencyManagerPtr mngr) __NE___
	{
		CHECK_ERR( mngr );
		EXLOCK( _taskDepsMngrsGuard );
		return _taskDepsMngrs.insert_or_assign( typeid(T), mngr ).second;	// should not throw
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
		return _taskDepsMngrs.erase( typeid(T) ) > 0;
	}

/*
=================================================
	Run
----
	always return non-null task
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  TaskScheduler::Run (Tuple<Ctor...>&& ctorArgs, const Tuple<Deps...> &deps) __NE___
	{
		STATIC_ASSERT( IsBaseOf< IAsyncTask, TaskType > );

		AsyncTask	task;
		uint		bit_index	= 0;
		try{
			task = ctorArgs.Apply([] (auto&& ...args) { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)... ); });	// throw
		}
		catch(...) {
			return GetCanceledTask();
		}
		
		if_unlikely( not _AddDependencies<0>( task, deps, INOUT bit_index ))
		{
			// add task to queue only to call 'OnCancel()'
			task->_SetCancellationState();
			bit_index = 0;	// no dependencies
		}

		CHECK_ERR( _InsertTask( task, bit_index ), GetCanceledTask() );
		return task;
	}
	
/*
=================================================
	Run
----
	returns 'true' if task added to queue
=================================================
*/
	template <typename ...Deps>
	bool  TaskScheduler::Run (AsyncTask task, const Tuple<Deps...> &deps) __NE___
	{
		CHECK_ERR( task );

		uint	bit_index = 0;
		
		if_unlikely( not _AddDependencies<0>( task, deps, INOUT bit_index ))
		{
			// add task to queue only to call 'OnCancel()'
			task->_SetCancellationState();
			bit_index = 0;	// no dependencies
		}

		return _InsertTask( RVRef(task), bit_index );
	}
	
/*
=================================================
	Run (coroutine)
----
	always return non-null task
=================================================
*/
#ifdef AE_HAS_COROUTINE

	template <typename ...Deps>
	AsyncTask  TaskScheduler::Run (ETaskQueue queueType, CoroTask coro, const Tuple<Deps...> &deps, StringView dbgName) __NE___
	{
		CHECK_ERR( coro );
		coro._InitCoro( queueType, dbgName );

		AsyncTask	task = AsyncTask{coro};

		CHECK_ERR( Run( task, deps ), GetCanceledTask() );
		return task;
	}
	
	template <typename ...Deps>
	AsyncTask  TaskScheduler::Run (CoroTask coro, const Tuple<Deps...> &deps) __NE___
	{
		return Run( ETaskQueue::PerFrame, RVRef(coro), deps );
	}
	
	template <typename T, typename ...Deps>
	Coroutine<T>  TaskScheduler::Run (ETaskQueue queueType, Coroutine<T> coro, const Tuple<Deps...> &deps, StringView dbgName) __NE___
	{
		CHECK_ERR( coro );
		coro._InitCoro( queueType, dbgName );

		AsyncTask	task		= AsyncTask{coro};
		uint		bit_index	= 0;
		
		ASSERT( task->Status() == EStatus::Initial );

		if_unlikely( not _AddDependencies<0>( task, deps, INOUT bit_index ))
		{
			// add task to queue only to call 'OnCancel()'
			task->_SetCancellationState();
			bit_index = 0;	// no dependencies
		}

		// TODO: return canceled coroutine
		CHECK_ERR( _InsertTask( RVRef(task), bit_index ));

		return coro;
	}
	
	template <typename T, typename ...Deps>
	Coroutine<T>  TaskScheduler::Run (Coroutine<T> coro, const Tuple<Deps...> &deps) __NE___
	{
		return Run( ETaskQueue::PerFrame, RVRef(coro), deps );
	}

#endif // AE_HAS_COROUTINE

/*
=================================================
	_AddDependencies
=================================================
*/
	template <usize I, typename ...Args>
	constexpr bool  TaskScheduler::_AddDependencies (const AsyncTask &task, const Tuple<Args...> &args, INOUT uint &bitIndex) __NE___
	{
		if constexpr( I < CountOf<Args...>() )
		{
			using T = typename TypeList< Args... >::template Get<I>;

			// current task will start anyway, regardless of whether dependent tasks are canceled
			if constexpr( IsSameTypes< T, WeakDep >) {
				if_unlikely( not _AddTaskDependencies( task, args.template Get<I>()._task, False{"weak"}, INOUT bitIndex )) return false;
			}else
			if constexpr( IsSameTypes< T, WeakDepArray >)
				for (auto& dep : args.template Get<I>()) {
					if_unlikely( not _AddTaskDependencies( task, dep, False{"weak"}, INOUT bitIndex )) return false;
				}
			else
			// current task will be canceled if one of dependent task are canceled
			if constexpr( IsSameTypes< T, StrongDep >) {
				if_unlikely( not _AddTaskDependencies( task, args.template Get<I>()._task, True{"strong"}, INOUT bitIndex )) return false;
			}else
			if constexpr( IsSameTypes< T, StrongDepArray > or IsSameTypes< T, ArrayView<AsyncTask> >)
				for (auto& dep : args.template Get<I>()) {
					if_unlikely( not _AddTaskDependencies( task, dep, False{"strong"}, INOUT bitIndex )) return false;
				}
			else
			// implicitlly it is strong dependency
			if constexpr( IsSpecializationOf< T, RC > and IsBaseOf< IAsyncTask, RemoveRC<T> >) {
				if_unlikely( not _AddTaskDependencies( task, args.template Get<I>(), True{"strong"}, INOUT bitIndex )) return false;
			//}else
			//if constexpr( std::is_convertible_v< T, AsyncTask >) {
			//	if_unlikely( not _AddTaskDependencies( task, AsyncTask{args.template Get<I>()}, True{"strong"}, INOUT bitIndex )) return false;
			}else{
				if_unlikely( not _AddCustomDependency( task, args.template Get<I>(), INOUT bitIndex )) return false;
			}

			return _AddDependencies<I+1>( task, args, INOUT bitIndex );
		}
		else
		{
			Unused( task, args, bitIndex );
			return true;
		}
	}
	
/*
=================================================
	_AddCustomDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::_AddCustomDependency (const AsyncTask &task, const T &dep, INOUT uint &bitIndex) __NE___
	{
		STATIC_ASSERT( not IsConst<T> );
		SHAREDLOCK( _taskDepsMngrsGuard );

		auto	iter = _taskDepsMngrs.find( typeid(T) );
		CHECK_ERR( iter != _taskDepsMngrs.end() );

		return iter->second->Resolve( AnyTypeCRef{dep}, task, INOUT bitIndex );
	}
	
/*
=================================================
	WaitAsync
=================================================
*/
	template <typename ...Deps>
	AsyncTask  TaskScheduler::WaitAsync (ETaskQueue queue, const Tuple<Deps...> &deps) __NE___
	{
		return Run<WaitAsyncTask>( Tuple{queue}, deps );
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	Continue
=================================================
*/
	template <typename ...Deps>
	void  IAsyncTask::Continue (const Tuple<Deps...> &deps) __NE___
	{
		ASSERT( _isRunning.load() );
		ASSERT( _waitBits.load() == 0 );	// all input dependencies must complete
		
		if constexpr( sizeof...(Deps) > 0 )
		{
			// in '_AddDependencies()' current task has been added to the input dependencies
			// and they may remove bits from '_waitBits' at any time
			_waitBits.store( UMax );

			uint	bit_index = 0;
			if_unlikely( not Scheduler()._AddDependencies<0>( GetRC(), deps, INOUT bit_index ))
			{
				// cancel task
				Unused( _SetCancellationState() );
				return;
			}
		
			// some dependencies may already be completed, so merge bit mask with current
			_waitBits.fetch_and( ToBitMask<WaitBits_t>( bit_index ));
		}
		Unused( deps );

		for (EStatus expected = EStatus::InProgress;
			 not _status.CAS( INOUT expected, EStatus::Continue );)
		{
			// status has been changed in another thread
			if_unlikely( (expected == EStatus::Cancellation) | (expected > EStatus::_Finished) )
				return;
			
			// 'CAS' can return 'false' even if expected value is the same as current value in atomic
			ASSERT( expected == EStatus::InProgress );
			ThreadUtils::Pause();
		}
	}

} // AE::Threading


namespace AE
{
/*
=================================================
	Scheduler
=================================================
*/
	ND_ forceinline Threading::TaskScheduler&  Scheduler () __NE___
	{
		return Threading::TaskScheduler::_Instance();
	}

} // AE
