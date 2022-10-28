// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Async task states:
		TaskScheduler::Run() {
			set pending state
		}
		TaskScheduler::ProcessTask() {
			if cancellation or one of input dependencies has been canceled {
				OnCancel()
				set canceled state
			}
			if pending state and all input dependencies are complete {
				set in_progress state
				Run() {
					if cancellation
						return
					if something goes wrong
						set failed state and return
				}
				if seccessfully completed
					set completed state and return
				if cancellation
					OnCancel()
					set canceled state and return
				if canceled or failed
					return
			}
		}

	Order guaranties:
		AsyncTask::Run() will be called after all input dependencies Run() or OnCancel() methods have completed
*/

#pragma once

#include "base/Algorithms/ArrayUtils.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedArray.h"
#include "base/Containers/NtStringView.h"
#include "base/Containers/AnyTypeRef.h"
#include "base/Algorithms/Cast.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/RefCounter.h"
#include "base/Utils/EnumBitSet.h"
#include "base/CompileTime/TypeList.h"
#include "base/CompileTime/TemplateUtils.h"
#include "base/Math/POTValue.h"

#include "threading/Primitives/SpinLock.h"
#include "threading/TaskSystem/TaskProfiler.h"
#include "threading/Primitives/CoroutineHandle.h"

#ifdef AE_ENABLE_VTUNE_API
#	include <ittnotify.h>
#	define AE_VTUNE( /* code */... )	__VA_ARGS__
#else
#	define AE_VTUNE( /* code */... )
#endif

#ifdef AE_DBG_OR_DEV
#	define AE_SCHEDULER_PROFILING( /* code */... )	__VA_ARGS__
#else
#	define AE_SCHEDULER_PROFILING( /* code */... )
#endif

namespace AE::Threading
{
	using TaskDependencyManagerPtr	= RC< class ITaskDependencyManager >;

	
	namespace _hidden_
	{
		template <bool IsStrongDep>
		struct _TaskDependency
		{
			AsyncTask	_task;

			explicit _TaskDependency (AsyncTask &&task) : _task{RVRef(task)} {}
			explicit _TaskDependency (const AsyncTask &task) : _task{task} {}
			_TaskDependency (_TaskDependency &&) = default;
			_TaskDependency (const _TaskDependency &) = default;
		};
	
		template <bool IsStrongDep>
		struct _TaskDependencyArray : ArrayView< AsyncTask >
		{
			_TaskDependencyArray (AsyncTask const* ptr, usize count) : ArrayView{ptr, count} {}

			_TaskDependencyArray (std::initializer_list<AsyncTask> list) : ArrayView{list} {}

			template <typename AllocT>
			_TaskDependencyArray (const Array<AsyncTask,AllocT> &vec) : ArrayView{vec} {}

			template <usize S>
			_TaskDependencyArray (const StaticArray<AsyncTask,S> &arr) : ArrayView{arr} {}
			
			template <usize S>
			_TaskDependencyArray (const FixedArray<AsyncTask,S> &arr) : ArrayView{arr} {}

			template <usize S>
			_TaskDependencyArray (const AsyncTask (&arr)[S]) : ArrayView{arr} {}
		};

	} // _hidden_

	using WeakDep   = Threading::_hidden_::_TaskDependency<false>;
	using StrongDep = Threading::_hidden_::_TaskDependency<true>;
	
	using WeakDepArray   = Threading::_hidden_::_TaskDependencyArray<false>;
	using StrongDepArray = Threading::_hidden_::_TaskDependencyArray<true>;
//-----------------------------------------------------------------------------


	enum class EThread : uint
	{
		Main,		// thread with window message loop
		Worker,
		Renderer,	// single thread for opengl, multiple for vulkan (can be mixed with 'Worker')	// TODO: RendererHi, RendererLow
		FileIO,
		Network,
		_Count
	};


	//
	// Async Task interface
	//
	
	class alignas(AE_CACHE_LINE) IAsyncTask : public EnableRC< IAsyncTask >
	{
		friend class ITaskDependencyManager;	// can change '_waitBits' and '_canceledDepsCount'
		friend class LfTaskQueue;				// can change '_status'
		friend class TaskScheduler;				// can change '_status'
		friend class IThread;					// can change '_status'
		
	// types
	public:
		enum class EStatus : uint
		{
			Initial,
			Pending,		// task has been added to the queue and is waiting until input dependencies complete
			InProgress,		// task was acquired by thread
			Cancellation,	// task required to be canceled
			Continue,		// task will be returned to scheduler

			_Finished,
			Completed,		// successfully completed

			_Interropted,
			Canceled,		// task was externally canceled
			Failed,			// task has internal error and has been failed
		};

	private:
		struct TaskDependency
		{
			ubyte	bitIndex : 7;	// to reset bit in '_waitBits'
			ubyte	isStrong : 1;	// to increment '_canceledDepsCount'
		};

		static constexpr uint	ElemInChunk	= 12;

		struct OutputChunk
		{
			OutputChunk *								next		= null;
			uint										selfIndex	= UMax;
			uint										count		= 0;
			StaticArray< AsyncTask, ElemInChunk >		tasks		{};
			StaticArray< TaskDependency, ElemInChunk >	deps		{};

			void Init (uint idx);

			static auto&  _GetPool ();
		};
		STATIC_ASSERT( sizeof(OutputChunk) == 128_b );

		using WaitBits_t = ulong;


	// variables
	private:
		const EThread				_threadType;		// packed with atomic counter in 'EnableRC<>'
		Atomic< EStatus >			_status				{EStatus::Initial};
		Atomic< uint >				_canceledDepsCount	{0};		// TODO: pack with '_status'
		Atomic< WaitBits_t >		_waitBits			{~WaitBits_t{0}};
		
		PtrSpinLock< OutputChunk >	_output				{null};

		PROFILE_ONLY(
			RC<ITaskProfiler>		_profiler;
		)
		DEBUG_ONLY(
			Atomic<bool>			_isRunning			{false};
		)


	// methods
	public:
		virtual ~IAsyncTask ();

		ND_ EThread	ThreadType ()	 const	{ return _threadType; }

		ND_ EStatus	Status ()		 const	{ return _status.load(); }

		ND_ bool	IsInQueue ()	 const	{ return Status() < EStatus::_Finished; }
		ND_ bool	IsFinished ()	 const	{ return Status() > EStatus::_Finished; }
		ND_ bool	IsInterropted () const	{ return Status() > EStatus::_Interropted; }

		ND_ virtual StringView	DbgName ()	const = 0;

	protected:
		explicit IAsyncTask (EThread type);

			virtual void  Run () = 0;
			virtual void  OnCancel ()		{ ASSERT( not _isRunning.load() ); }
		
		DEBUG_ONLY( ND_ bool  _IsRunning () const	{ return _isRunning.load(); })

			// call this only inside 'Run()' method.
		ND_ bool  IsCanceled () const		{ ASSERT( _isRunning.load() );  return Status() == EStatus::Cancellation; }

			// call this only inside 'Run()' method
			void  OnFailure ();

			// call this only inside 'Run()' method.
			// returns 'false' if failed to restart task
		ND_	bool  Continue ();

			template <typename ...Deps>
		ND_	bool  Continue (const Tuple<Deps...> &deps);

			// call this before reusing task
		ND_	bool  _ResetState ();
			
			// Only for debugging!
			void  _DbgSet (EStatus status);

			// Only in constructor!
			void  _MakeCompleted ();

	private:
		// call this methods only after 'Run()' method
		void  _OnFinish (OUT bool& rerun);
		void  _Cancel ();

		bool  _SetCancellationState ();
		void  _FreeOutputChunks (bool isCanceled);
	};
//-----------------------------------------------------------------------------




	//
	// Thread interface
	//

	class IThread : public EnableRC< IThread >
	{
	// interface
	public:
		virtual bool  Attach (uint uid) = 0;
		virtual void  Detach () = 0;

		ND_ virtual StringView  DbgName ()	const	{ return "thread"; }
		ND_ virtual usize		DbgID ()	const = 0;

	// helper functions (try to not use it)
	protected:
		static void  _RunTask (AsyncTask);
		static void  _OnTaskFinish (const AsyncTask &);
	};
//-----------------------------------------------------------------------------



	//
	// Task Dependency Manager interface
	//

	class ITaskDependencyManager : public EnableRC< ITaskDependencyManager >
	{
	// interface
	public:
		virtual ~ITaskDependencyManager () {}

		ND_ virtual bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) = 0;


		// helper functions
		static void  _SetDependencyCompletionStatus (const AsyncTask &task, uint depIndex, bool cancel = false);
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

			PackedBits () : bits{0} {}

			ND_ bool		IsLocked ()					const	{ return pack.locked == 1; }
			ND_ PackedBits	Lock ()						const	{ PackedBits tmp{*this};  tmp.pack.locked = 1;  return tmp; }
			ND_ bool		operator == (PackedBits rhs)const	{ return bits == rhs.bits; }
		};

		struct alignas(AE_CACHE_LINE) Chunk
		{
			BitAtomic< PackedBits >		packed	{PackedBits{}};
			Atomic< Chunk *>			next	{null};
			TaskArr_t					array	{};

			Chunk () {}
		};
		STATIC_ASSERT( sizeof(Chunk) == 1_Kb );
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
		)


	// methods
	public:
		LfTaskQueue ()		{}
		~LfTaskQueue ()		{ Release(); }

		ND_ AsyncTask	Pull (usize seed);
			bool		Process (usize seed);
			void		Add (AsyncTask task, usize seed);

			void		Setup (POTValue seedMask, StringView name);
			void		WriteProfilerStat ();

		ND_ Bytes		MaxAllocationSize ()	const;
		ND_ Bytes		AllocatedSize ()		const;

			void		Release ();

	private:
		ND_ static bool  _RemoveTask (TaskArr_t& arr, INOUT usize& pos, INOUT usize& count, OUT AsyncTask& task);
	};
//-----------------------------------------------------------------------------
	
	
# ifdef AE_HAS_COROUTINE
  namespace _hidden_
  {
	//
	// Async Task Coroutine
	//

	class AsyncTaskCoroutine
	{
	// types
	public:
		struct promise_type;
		using Handle_t = CoroutineHandle< promise_type >;

		struct promise_type
		{
		// variables
		private:
			AtomicRC<IAsyncTask>	_task;		// TODO: optimize

		// methods
		public:
			ND_ Handle_t			get_return_object ()			{ return Handle_t::FromPromise( *this ); }

			ND_ std::suspend_always	initial_suspend () noexcept		{ return {}; }			// delayed start
			ND_ std::suspend_always	final_suspend () noexcept		{ return {}; }			// must not be 'suspend_never'

				void				return_void ()					{}
					
				void				unhandled_exception ()			{}						// ignore exceptions

			ND_ AsyncTask			GetTask ()						{ return _task.get(); }

		private:
			friend class CoroutineRunnerTask;

				void				Init (IAsyncTask *task)			{ ASSERT( task != null );  _task.reset( task ); }
				void				Reset ()						{ _task.reset( null ); }
		};
	};



	//
	// Coroutine runner as Async Task
	//

	class CoroutineRunnerTask final : public IAsyncTask
	{
	// types
	private:
		using Promise_t	= typename AsyncTaskCoroutine::promise_type;
		using Handle_t	= typename AsyncTaskCoroutine::Handle_t;


	// variables
	private:
		Handle_t	_coroutine;


	// methods
	public:
		explicit CoroutineRunnerTask (Handle_t handle, EThread type = EThread::Worker) :
			IAsyncTask{ type }, _coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~CoroutineRunnerTask () override
		{
			ASSERT( _coroutine.Done() );
		}

		// must be inside coroutine!
		template <typename ...Deps>
		ND_ static bool  ContinueTask (IAsyncTask &task, const Tuple<Deps...> &deps)
		{
			ASSERT( dynamic_cast<CoroutineRunnerTask *>(&task) != null );

			auto&	t = static_cast<CoroutineRunnerTask &>(task);
			ASSERT( t._IsRunning() );
			return t.Continue( deps );
		}

	private:
		void  Run () override
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Resume();

			if_likely( _coroutine.Done() )
				_coroutine.Promise().Reset();
			else
				ASSERT( Status() == EStatus::Continue );
		}
			
		void  OnCancel () override
		{
			IAsyncTask::OnCancel();
			_coroutine.Promise().Reset();
		}

		StringView  DbgName () const override { return "coroutine"; }
	};
	


	//
	// Coroutine Runner Awaiter
	//
	
	template <typename T>
	class CoroutineRunnerAwaiter;
	
	template <>
	class CoroutineRunnerAwaiter< AsyncTask >
	{
	private:
		AsyncTask const&	_dep;

	public:
		explicit CoroutineRunnerAwaiter (const AsyncTask &dep) : _dep{dep} {}

		// pause coroutine execution if dependency is not complete
		bool	await_ready () const	{ return _dep ? _dep->IsFinished() : true; }

		void	await_resume ()			{}
		
		// return task to scheduler with new dependencies
		void	await_suspend (std::coroutine_handle< AsyncTaskCoroutine::promise_type > h)
		{
			auto	task = h.promise().GetTask();
			CHECK_ERRV( task );
			Unused( CoroutineRunnerTask::ContinueTask( *task, Tuple{_dep} ));
		}
	};

	template <typename ...Deps>
	class CoroutineRunnerAwaiter< Tuple<Deps...> >
	{
	private:
		Tuple<Deps...> const&	_deps;

	public:
		explicit CoroutineRunnerAwaiter (const Tuple<Deps...> &deps) : _deps{deps} {}

		// pause coroutine execution if dependencies are not complete
		bool	await_ready () const	{ return false; }

		void	await_resume ()			{}
		
		// return task to scheduler with new dependencies
		void	await_suspend (std::coroutine_handle< AsyncTaskCoroutine::promise_type > h)
		{
			auto	task = h.promise().GetTask();
			CHECK_ERRV( task );
			Unused( CoroutineRunnerTask::ContinueTask( *task, _deps ));
		}
	};

  } // _hidden_

  
/*
=================================================
	MakeCoroutineTask
=================================================
*/
	using CoroutineTask = _hidden_::AsyncTaskCoroutine::Handle_t;
	
	ND_ inline AsyncTask  MakeCoroutineTask (CoroutineTask handle, EThread type = EThread::Worker)
	{
		return AsyncTask{ new _hidden_::CoroutineRunnerTask{ RVRef(handle), type }};
	}
	
/*
=================================================
	operator co_await (task dependencies)
----
	it is safe to use reference because CoroutineRunnerAwaiter object
	destroyed before Tuple destruction.
=================================================
*/
	template <typename ...Deps>
	ND_ _hidden_::CoroutineRunnerAwaiter<AsyncTask>  operator co_await (const AsyncTask &dep)
	{
		return _hidden_::CoroutineRunnerAwaiter<AsyncTask>{ dep };
	}

	template <typename ...Deps>
	ND_ _hidden_::CoroutineRunnerAwaiter< Tuple<Deps...> >  operator co_await (const Tuple<Deps...> &deps)
	{
		return _hidden_::CoroutineRunnerAwaiter< Tuple<Deps...> >{ deps };
	}

# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------



	//
	// Task Scheduler
	//

	class TaskScheduler final : public Noncopyable
	{
		friend class IAsyncTask;

	// types
	private:
		using TimePoint_t		= std::chrono::high_resolution_clock::time_point;
		using EStatus			= IAsyncTask::EStatus;
		using EThreadMask		= EnumBitSet< EThread >;
		using OutputChunk_t		= IAsyncTask::OutputChunk;
		using WaitBits_t		= IAsyncTask::WaitBits_t;

		using TaskQueues_t		= StaticArray< LfTaskQueue, uint(EThread::_Count) >;
		using ThreadPtr			= RC< IThread >;
		
		using TaskDepsMngr_t	= FlatHashMap< std::type_index, TaskDependencyManagerPtr >;

		static constexpr uint			_WaitAll_SpinCount	= 20'000;
		static constexpr microseconds	_WaitAll_SleepTime	{100};
		

	// variables
	private:
		TaskQueues_t		_queues;

		SharedMutex			_taskDepsMngrsGuard;	// TODO: init on start, remove lock
		TaskDepsMngr_t		_taskDepsMngrs;

		Mutex				_threadGuard;
		Array<ThreadPtr>	_threads;
		
		PROFILE_ONLY(
			AtomicRC<ITaskProfiler>	_profiler;
		)
		AE_VTUNE(
			__itt_domain*	_vtuneDomain	= null;
		)


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();

		bool  Setup (usize maxWorkerThreads);
		void  SetProfiler (RC<ITaskProfiler> profiler);
		void  Release ();
			
		AE_VTUNE(
		 ND_ __itt_domain*	GetVTuneDomain () const	{ return _vtuneDomain; }
		)

		template <typename T>
		bool  RegisterDependency (TaskDependencyManagerPtr mngr);

		template <typename T>
		bool  UnregisterDependency ();


	// thread api
		bool  AddThread (const ThreadPtr &thread);

		bool  ProcessTask (EThread type, uint seed);

		ND_ AsyncTask  PullTask (EThread type, uint seed);


	// task api
		template <typename TaskType, typename ...Ctor, typename ...Deps>
		AsyncTask  Run (Tuple<Ctor...>&& ctor = Default, const Tuple<Deps...> &deps = Default);

		#ifdef AE_HAS_COROUTINE
		template <typename ...Deps>
		AsyncTask  Run (CoroutineTask handle, const Tuple<Deps...> &deps = Default);
		#endif
		
		template <typename ...Deps>
		bool  Run (AsyncTask task, const Tuple<Deps...> &deps = Default);

		ND_ bool  Wait (ArrayView<AsyncTask> tasks, EThreadMask mask = Default, nanoseconds timeout = GetDefaultTimeout());

		bool  Cancel (const AsyncTask &task);

		bool  Enqueue (AsyncTask task);


		ND_ static constexpr nanoseconds  GetDefaultTimeout ();
		
		friend TaskScheduler&  Scheduler ();


	private:
		TaskScheduler ();
		~TaskScheduler ();
		
		ND_ static TaskScheduler*  _Instance ();

		AsyncTask  _InsertTask (AsyncTask task, uint bitIndex);

		template <usize I, typename ...Args>
		constexpr bool  _AddDependencies (const AsyncTask &task, const Tuple<Args...> &args, INOUT uint &bitIndex);

		template <typename T>
		bool  _AddCustomDependency (const AsyncTask &task, T &dep, INOUT uint &bitIndex);

		bool  _AddTaskDependencies (const AsyncTask &task, const AsyncTask &deps, Bool isStrong, INOUT uint &bitIndex);
	};
//-----------------------------------------------------------------------------

	
	
/*
=================================================
	GetDefaultTimeout
=================================================
*/
	constexpr nanoseconds  TaskScheduler::GetDefaultTimeout ()
	{
		#ifdef AE_DBG_OR_DEV
			return nanoseconds{60*60'000'000'000ll};	// 60 min - for debugging
		#else
			return nanoseconds{30'000'000'000ll};		// 30 sec
		#endif
	}

/*
=================================================
	RegisterDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::RegisterDependency (TaskDependencyManagerPtr mngr)
	{
		CHECK_ERR( mngr );
		EXLOCK( _taskDepsMngrsGuard );
		return _taskDepsMngrs.insert_or_assign( typeid(T), mngr ).second;
	}
	
/*
=================================================
	UnregisterDependency
=================================================
*/
	template <typename T>
	bool  TaskScheduler::UnregisterDependency ()
	{
		EXLOCK( _taskDepsMngrsGuard );
		return _taskDepsMngrs.erase( typeid(T) ) > 0;
	}

/*
=================================================
	Run
=================================================
*/
	template <typename TaskType, typename ...Ctor, typename ...Deps>
	AsyncTask  TaskScheduler::Run (Tuple<Ctor...>&& ctorArgs, const Tuple<Deps...> &deps)
	{
		STATIC_ASSERT( IsBaseOf< IAsyncTask, TaskType > );

		AsyncTask	task		= ctorArgs.Apply([] (auto&& ...args) { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)... ); });
		uint		bit_index	= 0;
		
		CHECK_ERR( _AddDependencies<0>( task, deps, INOUT bit_index ));

		return _InsertTask( RVRef(task), bit_index );
	}
	
/*
=================================================
	Run
=================================================
*/
	template <typename ...Deps>
	bool  TaskScheduler::Run (AsyncTask task, const Tuple<Deps...> &deps)
	{
		CHECK_ERR( task );

		uint	bit_index = 0;
		CHECK_ERR( _AddDependencies<0>( task, deps, INOUT bit_index ));

		return _InsertTask( RVRef(task), bit_index ) != null;
	}
	
/*
=================================================
	Run (coroutine)
=================================================
*/
#ifdef AE_HAS_COROUTINE
	template <typename ...Deps>
	AsyncTask  TaskScheduler::Run (CoroutineTask handle, const Tuple<Deps...> &deps)
	{
		AsyncTask	task		= MakeCoroutineTask( RVRef(handle) );
		uint		bit_index	= 0;
		
		CHECK_ERR( _AddDependencies<0>( task, deps, INOUT bit_index ));

		return _InsertTask( RVRef(task), bit_index );
	}
#endif

/*
=================================================
	_AddDependencies
=================================================
*/
	template <usize I, typename ...Args>
	constexpr bool  TaskScheduler::_AddDependencies (const AsyncTask &task, const Tuple<Args...> &args, INOUT uint &bitIndex)
	{
		if constexpr( I < CountOf<Args...>() )
		{
			using T = typename TypeList< Args... >::template Get<I>;

			// current task will start anyway, regardless of whether dependent tasks are canceled
			if constexpr( IsSameTypes< T, WeakDep >)
				CHECK_ERR( _AddTaskDependencies( task, args.template Get<I>()._task, False{"weak"}, INOUT bitIndex ))
			else
			if constexpr( IsSameTypes< T, WeakDepArray >)
				for (auto& dep : args.template Get<I>()) {
					CHECK_ERR( _AddTaskDependencies( task, dep, False{"weak"}, INOUT bitIndex ));
				}
			else
			// current task will be canceled if one of dependent task are canceled
			if constexpr( IsSameTypes< T, StrongDep >)
				CHECK_ERR( _AddTaskDependencies( task, args.template Get<I>()._task, True{"strong"}, INOUT bitIndex ))
			else
			if constexpr( IsSameTypes< T, StrongDepArray > or IsSameTypes< T, ArrayView<AsyncTask> >)
				for (auto& dep : args.template Get<I>()) {
					CHECK_ERR( _AddTaskDependencies( task, dep, False{"strong"}, INOUT bitIndex ));
				}
			else
			// implicitlly it is strong dependency
			if constexpr( IsSpecializationOf< T, RC > and IsBaseOf< IAsyncTask, RemoveRC<T> >)
				CHECK_ERR( _AddTaskDependencies( task, args.template Get<I>(), True{"strong"}, INOUT bitIndex ))
			else
				CHECK_ERR( _AddCustomDependency( task, args.template Get<I>(), INOUT bitIndex ));

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
	bool  TaskScheduler::_AddCustomDependency (const AsyncTask &task, T &dep, INOUT uint &bitIndex)
	{
		SHAREDLOCK( _taskDepsMngrsGuard );

		auto	iter = _taskDepsMngrs.find( typeid(T) );
		CHECK_ERR( iter != _taskDepsMngrs.end() );

		return iter->second->Resolve( AnyTypeCRef{dep}, task, INOUT bitIndex );
	}

/*
=================================================
	Scheduler
=================================================
*/
	ND_ inline TaskScheduler&  Scheduler ()
	{
		return *TaskScheduler::_Instance();
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	Continue
=================================================
*/
	template <typename ...Deps>
	bool  IAsyncTask::Continue (const Tuple<Deps...> &deps)
	{
		ASSERT( _isRunning.load() );
		
		// in '_AddDependencies()' current task has been added to the input dependencies
		// and they may remove bits from '_waitBits' at any time
		_waitBits.store( UMax );

		uint	bit_index = 0;
		CHECK_ERR( Scheduler()._AddDependencies<0>( GetRC(), deps, INOUT bit_index ));
		
		// some dependencies may already be completed, so merge bit mask with current
		_waitBits.fetch_and( ToBitMask<WaitBits_t>( bit_index ));
		
		for (EStatus expected = EStatus::InProgress;
			 not _status.CAS( INOUT expected, EStatus::Continue );)
		{
			// status has been changed in another thread
			if ( expected > EStatus::_Finished )
				return false;
			
			// 'CAS' can return 'false' even if expected value is the same as current value in atomic
			ASSERT( expected == EStatus::InProgress );
			ThreadUtils::Pause();
		}
		return true;
	}

} // AE::Threading
