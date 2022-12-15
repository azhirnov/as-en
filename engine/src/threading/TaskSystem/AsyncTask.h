// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
		Task Dependencies

	AsyncTask
	ArrayView<AsyncTask>
	WeakDep
	StrongDep
	WeakDepArray
	StrongDepArray
	<custom>				- use Scheduler().RegisterDependency< custom >(...)

		Coroutines

	bool		co_await Coro_IsCanceled
	EStatus		co_await Coro_Status
	ETaskQueue	co_await Coro_Queue
*/

#pragma once

#include "base/Algorithms/ArrayUtils.h"
#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedArray.h"
#include "base/Containers/NtStringView.h"
#include "base/Algorithms/Cast.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/RefCounter.h"

#include "threading/Primitives/Atomic.h"
#include "threading/Primitives/SpinLock.h"
#include "threading/TaskSystem/TaskProfiler.h"
#include "threading/TaskSystem/EThread.h"
#include "threading/Primitives/CoroutineHandle.h"

namespace AE::Threading
{
	namespace _hidden_
	{
		template <typename TaskType, bool IsStrongDep>
		struct _TaskDependency
		{
			TaskType	_task;

			explicit _TaskDependency (TaskType &&task)					__NE___	: _task{ RVRef(task) } {}
			explicit _TaskDependency (const TaskType &task)				__NE___	: _task{ task } {}
			_TaskDependency (_TaskDependency &&)						__NE___	= default;
			_TaskDependency (const _TaskDependency &)					__NE___	= default;
		};
	
		template <typename TaskType, bool IsStrongDep>
		struct _TaskDependencyArray : ArrayView< TaskType >
		{
			_TaskDependencyArray (_TaskDependencyArray &&)				__NE___	= default;
			_TaskDependencyArray (const _TaskDependencyArray &)			__NE___	= default;

			_TaskDependencyArray (TaskType const* ptr, usize count)		__NE___	: ArrayView<TaskType>{ptr, count} {}

			_TaskDependencyArray (std::initializer_list<TaskType> list)	__NE___	: ArrayView<TaskType>{list} {}

			template <typename AllocT>
			_TaskDependencyArray (const Array<TaskType,AllocT> &vec)	__NE___	: ArrayView<TaskType>{vec} {}

			template <usize S>
			_TaskDependencyArray (const StaticArray<TaskType,S> &arr)	__NE___	: ArrayView<TaskType>{arr} {}
			
			template <usize S>
			_TaskDependencyArray (const FixedArray<TaskType,S> &arr)	__NE___	: ArrayView<TaskType>{arr} {}

			template <usize S>
			_TaskDependencyArray (const TaskType (&arr)[S])				__NE___	: ArrayView<TaskType>{arr} {}
		};

	} // _hidden_

	using WeakDep			= Threading::_hidden_::_TaskDependency< AsyncTask, false >;
	using StrongDep			= Threading::_hidden_::_TaskDependency< AsyncTask, true >;
	
	using WeakDepArray		= Threading::_hidden_::_TaskDependencyArray< AsyncTask, false >;
	using StrongDepArray	= Threading::_hidden_::_TaskDependencyArray< AsyncTask, true >;
//-----------------------------------------------------------------------------



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

		struct TaskDependency
		{
			ubyte	bitIndex : 7;	// to reset bit in '_waitBits'
			ubyte	isStrong : 1;	// to increment '_canceledDepsCount'
		};

	private:
		static constexpr uint	ElemInChunk	= 12;

		struct OutputChunk
		{
		// variables
			OutputChunk *								next		= null;
			uint										selfIndex	= UMax;
			uint										count		= 0;
			StaticArray< AsyncTask, ElemInChunk >		tasks		{};
			StaticArray< TaskDependency, ElemInChunk >	deps		{};
			
		// methods
			OutputChunk ()			__NE___ {}

			void  Init (uint idx)	__NE___;
		};
		STATIC_ASSERT( sizeof(OutputChunk) == 128 );

		using WaitBits_t = ulong;


	// variables
	private:
		ETaskQueue					_queueType			= ETaskQueue::Worker;	// packed with atomic counter in 'EnableRC<>'
		Atomic< EStatus >			_status				{EStatus::Initial};
		Atomic< uint >				_canceledDepsCount	{0};					// > 0 if canceled		// TODO: pack with '_status'
		Atomic< WaitBits_t >		_waitBits			{~WaitBits_t{0}};		// 0 - all complete, otherwise - has uncomplete dependencies
		
		PtrSpinLock< OutputChunk >	_output				{null};

		PROFILE_ONLY(
			RC<ITaskProfiler>		_profiler;
		)
		DEBUG_ONLY(
			Atomic<bool>			_isRunning			{false};
		)


	// methods
	public:
		virtual ~IAsyncTask ()						__NE___;

		ND_ ETaskQueue	QueueType ()				C_NE___	{ return _queueType; }

		ND_ EStatus		Status ()					C_NE___	{ return _status.load(); }

		ND_ bool		IsInQueue ()				C_NE___	{ return Status() <  EStatus::_Finished; }
		ND_ bool		IsFinished ()				C_NE___	{ return Status() >  EStatus::_Finished; }
		ND_ bool		IsInterropted ()			C_NE___	{ return Status() >  EStatus::_Interropted; }
		ND_ bool		IsCompleted ()				C_NE___	{ return Status() == EStatus::Completed; }

		ND_ virtual StringView	DbgName ()			C_NE___ = 0;
		
		DEBUG_ONLY( ND_ bool  DbgIsRunning ()		C_NE___	{ return _isRunning.load(); })


	protected:
		explicit IAsyncTask (ETaskQueue type)		__NE___;

			// can throw exception
			virtual void  Run ()					__Th___ = 0;
			
			virtual void  OnCancel ()				__NE___	{ ASSERT( not _isRunning.load() ); }

			// call this only inside 'Run()' method.
		ND_ bool  IsCanceled () const				__NE___	{ ASSERT( _isRunning.load() );  return Status() == EStatus::Cancellation; }

			// call this only inside 'Run()' method
			void  OnFailure ()						__NE___;

			// call this only inside 'Run()' method.
			// doesn't restart if have been canceled in another thread.
			// throw exception if failed to add dependencies.
			template <typename ...Deps>
			void  Continue (const Tuple<Deps...> &) __NE___;
			void  Continue ()						__NE___	{ return Continue( Tuple{} ); }

			// call this before reusing task
		ND_	bool  _ResetState ()					__NE___;
			
			// Only for debugging!
			void  _DbgSet (EStatus status)			__NE___;

			// Only during initialization
			void  _SetQueueType (ETaskQueue type)	__NE___;

			// Only in constructor!
			void  _MakeCompleted ()					__NE___;

			bool  _SetCancellationState ()			__NE___;

	private:
		// call this methods only after 'Run()' method
		void  _OnFinish (OUT bool& rerun)			__NE___;
		void  _Cancel ()							__NE___;
		void  _FreeOutputChunks (bool isCanceled)	__NE___;

		DEBUG_ONLY( ND_ static slong  _AsyncTaskTotalCount () __NE___;)
	};
//-----------------------------------------------------------------------------
	
	
	
# ifdef AE_HAS_COROUTINE
  namespace _hidden_
  {
	class CoroutineRunnerTask;


	//
	// Async Task Coroutine
	//
	class AsyncTaskCoroutine
	{
	public:
		struct promise_type;
		using Handle_t = CoroutineHandle< promise_type >;

		struct promise_type final
		{
		// types
		public:
			using Task_t = CoroutineRunnerTask;


		// variables
		private:
			Atomic< Task_t *>		_task	{null};

		// methods
		public:
									~promise_type ()		__NE___	{ ASSERT( _task.load() == null ); }

			ND_ Handle_t			get_return_object ()	__NE___	{ return Handle_t::FromPromise( *this ); }

			ND_ std::suspend_always	initial_suspend ()		C_NE___	{ return {}; }			// delayed start
			ND_ std::suspend_always	final_suspend ()		C_NE___	{ return {}; }			// must not be 'suspend_never'	// TODO: don't suspend

				void				return_void ()			C_NE___	{}
					
				void				unhandled_exception ()	C_Th___	{ throw; }				// rethrow exceptions

			ND_ Task_t*				GetTask ()				C_NE___;

		private:
			friend class CoroutineRunnerTask;
				void				Init (Task_t *task)		__NE___;
				void				Reset (Task_t *task)	__NE___;
		};
	};



	//
	// Coroutine runner as Async Task
	//
	class CoroutineRunnerTask final : public IAsyncTask
	{
	// types
	public:
		using Promise_t	= typename AsyncTaskCoroutine::promise_type;
		using Handle_t	= typename AsyncTaskCoroutine::Handle_t;


	// variables
	private:
		Handle_t	_coroutine;


	// methods
	public:
		explicit CoroutineRunnerTask (Handle_t handle, ETaskQueue type = ETaskQueue::Worker)	__NE___ :
			IAsyncTask{ type }, _coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~CoroutineRunnerTask ()																	__NE_OV { ASSERT( IsCompleted() ? _coroutine.Done() : true ); }

		// must be inside coroutine!
		template <typename ...Deps>
			static void  ContinueTask (CoroutineRunnerTask &task, const Tuple<Deps...> &deps)	__NE___ { return task.Continue( deps ); }
			static void  FailTask (CoroutineRunnerTask &task)									__NE___	{ return task.OnFailure(); }
		ND_ static bool  IsCanceledTask (CoroutineRunnerTask &task)								__NE___	{ return task.IsCanceled(); }
		ND_ static auto  GetTaskStatus (CoroutineRunnerTask &task)								__NE___	{ return task.Status(); }
		ND_ static auto  GetTaskQueue (CoroutineRunnerTask &task)								__NE___	{ return task.QueueType(); }


	private:
		void  Run () __Th_OV
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Resume();	// throw

			if_likely( _coroutine.Done() )
				_coroutine.Promise().Reset( this );
			else
				ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
		}
			
		void  OnCancel () __NE_OV
		{
			IAsyncTask::OnCancel();
			_coroutine.Promise().Reset( this );
		}

		StringView  DbgName () C_NE_OV	{ return "coroutine"; }
	};

	
	inline CoroutineRunnerTask*  AsyncTaskCoroutine::promise_type::GetTask () C_NE___
	{
		auto* t = _task.load();
		ASSERT( t != null );
		ASSERT( AsyncTask{t}.use_count() > 1 );
		ASSERT( t->DbgIsRunning() );
		return t;
	}

	inline void  AsyncTaskCoroutine::promise_type::Init (CoroutineRunnerTask *task) __NE___
	{
		ASSERT( task != null );
		_task.store( task );
	}

	inline void  AsyncTaskCoroutine::promise_type::Reset (CoroutineRunnerTask *task) __NE___
	{
	#ifdef AE_DEBUG
		ASSERT( _task.exchange( null ) == task );
	#else
		Unused( task );
		_task.store( null );
	#endif
	}


	//
	// Coroutine Runner Awaiter
	//
	template <typename DepsType>
	class CoroutineRunnerAwaiter;
	
	template <>
	class CoroutineRunnerAwaiter< AsyncTask >
	{
	// variables
	private:
		AsyncTask const&	_dep;
		

	// methods
	public:
		explicit CoroutineRunnerAwaiter (const AsyncTask &dep) __NE___ : _dep{dep} {}

		// pause coroutine execution if dependency is not complete
		ND_ bool	await_ready ()		C_NE___	{ return _dep ? _dep->IsFinished() : true; }

			void	await_resume ()		__NE___	{}
		
		// return task to scheduler with new dependencies
		template <typename P>
		void  await_suspend (std::coroutine_handle<P> curCoro) __NE___
		{
			auto*	task = curCoro.promise().GetTask();
			CHECK_ERRV( task != null );
			P::Task_t::ContinueTask( *task, Tuple{_dep} );
		}
	};


	template <typename ...Deps>
	class CoroutineRunnerAwaiter< Tuple<Deps...> >
	{
	// variables
	private:
		Tuple<Deps...> const&	_deps;
		

	// methods
	public:
		explicit CoroutineRunnerAwaiter (const Tuple<Deps...> &deps) __NE___ : _deps{deps} {}

		// pause coroutine execution if dependencies are not complete
		ND_ bool	await_ready ()		C_NE___	{ return false; }

			void	await_resume ()		__NE___	{}
		
		// return task to scheduler with new dependencies
		template <typename P>
		void  await_suspend (std::coroutine_handle<P> curCoro) __NE___
		{
			auto*	task = curCoro.promise().GetTask();
			CHECK_ERRV( task != null );
			P::Task_t::ContinueTask( *task, _deps );
		}
	};


	//
	// Coroutine Runner Error (for CHECK_CE)
	//
	class CoroutineRunnerError
	{
	private:
		struct Awaiter
		{
			ND_ bool	await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
				void	await_resume ()		__NE___	{}
				
			template <typename P>
			ND_ bool	await_suspend (std::coroutine_handle<P> curCoro) __NE___
			{
				auto*	task = curCoro.promise().GetTask();
				if_likely( task != null )
					P::Task_t::FailTask( *task );
					
				return false;	// always resume coroutine
			}
		};

	public:
		explicit CoroutineRunnerError ()	__NE___ {}

		ND_ auto  operator co_await ()		C_NE___	{ return Awaiter{}; }
	};
	

	//
	// Coroutine Is Canceled
	//
	class Coroutine_IsCanceled
	{
	private:
		struct Awaiter
		{
		private:
			bool	_isCanceled = false;

		public:
			ND_ bool	await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
			ND_ bool	await_resume ()		__NE___	{ return _isCanceled; }
				
			template <typename P>
			ND_ bool	await_suspend (std::coroutine_handle<P> curCoro) __NE___
			{
				auto*	task = curCoro.promise().GetTask();
				if_likely( task != null )
					_isCanceled = P::Task_t::IsCanceledTask( *task );
					
				return false;	// always resume coroutine
			}
		};

	public:
		constexpr Coroutine_IsCanceled ()	__NE___ {}

		ND_ auto  operator co_await ()		C_NE___	{ return Awaiter{}; }
	};


	//
	// Coroutine Status
	//
	class Coroutine_Status
	{
	private:
		struct Awaiter
		{
		private:
			IAsyncTask::EStatus		_status = IAsyncTask::EStatus::Initial;

		public:
			ND_ bool	await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
			ND_ auto	await_resume ()		__NE___	{ return _status; }
				
			template <typename P>
			ND_ bool	await_suspend (std::coroutine_handle<P> curCoro) __NE___
			{
				auto*	task = curCoro.promise().GetTask();
				if_likely( task != null )
					_status = P::Task_t::GetTaskStatus( *task );
					
				return false;	// always resume coroutine
			}
		};

	public:
		constexpr Coroutine_Status ()		__NE___ {}

		ND_ auto  operator co_await ()		C_NE___	{ return Awaiter{}; }
	};


	//
	// Coroutine Queue
	//
	class Coroutine_Queue
	{
	private:
		struct Awaiter
		{
		private:
			ETaskQueue		_queue	= ETaskQueue::_Count;

		public:
			ND_ bool	await_ready ()		C_NE___	{ return false; }	// call 'await_suspend()' to get coroutine handle
			ND_ auto	await_resume ()		__NE___	{ return _queue; }
				
			template <typename P>
			ND_ bool	await_suspend (std::coroutine_handle<P> curCoro) __NE___
			{
				auto*	task = curCoro.promise().GetTask();
				if_likely( task != null )
					_queue = P::Task_t::GetTaskQueue( *task );
					
				return false;	// always resume coroutine
			}
		};

	public:
		constexpr Coroutine_Queue ()		__NE___ {}

		ND_ auto  operator co_await ()		C_NE___	{ return Awaiter{}; }
	};

  } // _hidden_
//-----------------------------------------------------------------------------


  
	static constexpr Threading::_hidden_::Coroutine_IsCanceled	Coro_IsCanceled	{};
	static constexpr Threading::_hidden_::Coroutine_Status		Coro_Status		{};
	static constexpr Threading::_hidden_::Coroutine_Queue		Coro_Queue		{};


/*
=================================================
	MakeCoroutineTask
----
	warning: for lamba don't use capture!
=================================================
*/
	using CoroutineTask = Threading::_hidden_::AsyncTaskCoroutine::Handle_t;
	
	ND_ inline AsyncTask  MakeCoroutineTask (CoroutineTask handle, ETaskQueue type = ETaskQueue::Worker) __Th___
	{
		return AsyncTask{ new Threading::_hidden_::CoroutineRunnerTask{ RVRef(handle), type }};
	}

/*
=================================================
	operator co_await (task dependencies)
----
	it is safe to use reference because CoroutineRunnerAwaiter object
	destroyed before Tuple destruction.
=================================================
*/
	ND_ inline auto  operator co_await (const AsyncTask &dep) __NE___
	{
		return Threading::_hidden_::CoroutineRunnerAwaiter< AsyncTask >{ dep };
	}

	template <typename ...Deps>
	ND_ auto  operator co_await (const Tuple<Deps...> &deps) __NE___
	{
		return Threading::_hidden_::CoroutineRunnerAwaiter< Tuple<Deps...> >{ deps };
	}

# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------
	
} // AE::Threading
