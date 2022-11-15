// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

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
#include "threading/Primitives/CoroutineHandle.h"

namespace AE::Threading
{
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
		// variables
			OutputChunk *								next		= null;
			uint										selfIndex	= UMax;
			uint										count		= 0;
			StaticArray< AsyncTask, ElemInChunk >		tasks		{};
			StaticArray< TaskDependency, ElemInChunk >	deps		{};
			
		// methods
			OutputChunk ()					__NE___ {}

			void			Init (uint idx)	__NE___;
			static auto&	_GetPool ()		__NE___;
		};
		STATIC_ASSERT( sizeof(OutputChunk) == 128_b );

		using WaitBits_t = ulong;


	// variables
	private:
		const EThread				_threadType;		// packed with atomic counter in 'EnableRC<>'
		Atomic< EStatus >			_status				{EStatus::Initial};
		Atomic< uint >				_canceledDepsCount	{0};		// TODO: pack with '_status'
		Atomic< WaitBits_t >		_waitBits			{~WaitBits_t{0}};	// 0 - all complete, otherwise - has uncomplete dependencies
		
		PtrSpinLock< OutputChunk >	_output				{null};

		PROFILE_ONLY(
			RC<ITaskProfiler>		_profiler;
		)
		DEBUG_ONLY(
			Atomic<bool>			_isRunning			{false};
		)


	// methods
	public:
		virtual ~IAsyncTask ()				__NE___;

		ND_ EThread	ThreadType ()			C_NE___	{ return _threadType; }

		ND_ EStatus	Status ()				C_NE___	{ return _status.load(); }

		ND_ bool	IsInQueue ()			C_NE___	{ return Status() < EStatus::_Finished; }
		ND_ bool	IsFinished ()			C_NE___	{ return Status() > EStatus::_Finished; }
		ND_ bool	IsInterropted ()		C_NE___	{ return Status() > EStatus::_Interropted; }

		ND_ virtual StringView	DbgName ()	C_NE___ = 0;

	protected:
		explicit IAsyncTask (EThread type)	__NE___;

			// can throw exception
			virtual void  Run ()			__TH___ = 0;
			
			// can throw only fatal exception
			virtual void  OnCancel ()		__NE___	{ ASSERT( not _isRunning.load() ); }
		
		DEBUG_ONLY( ND_ bool  _IsRunning ()	C_NE___	{ return _isRunning.load(); })

			// call this only inside 'Run()' method.
		ND_ bool  IsCanceled () const		__NE___	{ ASSERT( _isRunning.load() );  return Status() == EStatus::Cancellation; }

			// call this only inside 'Run()' method
			void  OnFailure ()				__NE___;

			// call this only inside 'Run()' method.
			// doesn't restart if have been canceled in another thread.
			// throw exception if failed to add dependencies.
			template <typename ...Deps>
			void  Continue (const Tuple<Deps...> &deps) __NE___;
			void  Continue ()				__NE___		{ return Continue( Tuple{} ); }

			// call this before reusing task
		ND_	bool  _ResetState ()			__NE___;
			
			// Only for debugging!
			void  _DbgSet (EStatus status)	__NE___;

			// Only in constructor!
			void  _MakeCompleted ()			__NE___;

			bool  _SetCancellationState ()	__NE___;

	private:
		// call this methods only after 'Run()' method
		void  _OnFinish (OUT bool& rerun)	__NE___;
		void  _Cancel ()					__NE___;
		void  _FreeOutputChunks (bool isCanceled) __NE___;

		DEBUG_ONLY( ND_ static slong  _AsyncTaskTotalCount () __NE___; )
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
					
				void				unhandled_exception ()	C_TH___	{ throw; }				// rethrow exceptions

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
		explicit CoroutineRunnerTask (Handle_t handle, EThread type = EThread::Worker) __NE___ :
			IAsyncTask{ type }, _coroutine{ RVRef(handle) }
		{
			ASSERT( _coroutine.IsValid() );
			_coroutine.Promise().Init( this );
		}

		~CoroutineRunnerTask ()					__NE_OV	{ ASSERT( _coroutine.Done() ); }
		
		DEBUG_ONLY( ND_ bool  DbgIsRunning ()	C_NE___	{ return _IsRunning(); })


		// must be inside coroutine!
		template <typename ...Deps>
		static void  ContinueTask (CoroutineRunnerTask &task, const Tuple<Deps...> &deps) __NE___
		{
			return task.Continue( deps );
		}

		static void  FailTask (CoroutineRunnerTask &task) __NE___
		{
			return task.OnFailure();
		}

	private:
		void  Run () __TH_OV
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
					
				return false;	// resume coroutine
			}
		};

	public:
		explicit CoroutineRunnerError ()	__NE___ {}

		ND_ auto  operator co_await ()		__NE___	{ return Awaiter{}; }
	};

  } // _hidden_

  
/*
=================================================
	MakeCoroutineTask
----
	warning: for lamba don't use capture!
=================================================
*/
	using CoroutineTask = _hidden_::AsyncTaskCoroutine::Handle_t;
	
	ND_ inline AsyncTask  MakeCoroutineTask (CoroutineTask handle, EThread type = EThread::Worker) __TH___
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
	ND_ inline auto  operator co_await (const AsyncTask &dep) __NE___
	{
		return _hidden_::CoroutineRunnerAwaiter< AsyncTask >{ dep };
	}

	template <typename ...Deps>
	ND_ auto  operator co_await (const Tuple<Deps...> &deps) __NE___
	{
		return _hidden_::CoroutineRunnerAwaiter< Tuple<Deps...> >{ deps };
	}

# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------
	
} // AE::Threading
