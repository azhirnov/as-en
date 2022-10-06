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
#include "base/Utils/Noncopyable.h"
#include "base/Utils/RefCounter.h"
#include "base/Utils/EnumBitSet.h"
#include "base/CompileTime/TypeList.h"
#include "base/CompileTime/TemplateUtils.h"
#include "base/Math/POTValue.h"

#include "threading/Primitives/SpinLock.h"

#ifdef AE_ENABLE_VTUNE_API
#	include <ittnotify.h>
#	define AE_VTUNE( /* code */... )	__VA_ARGS__
#else
#	define AE_VTUNE( /* code */... )
#endif

#if defined(AE_DEBUG) or defined(AE_PROFILE)
#	define AE_SCHEDULER_PROFILING( /* code */... )	__VA_ARGS__
#else
#	define AE_SCHEDULER_PROFILING( /* code */... )
#endif

namespace AE::Threading
{
	using AsyncTask					= RC< class IAsyncTask >;
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

	}	// _hidden_

	using WeakDep   = Threading::_hidden_::_TaskDependency<false>;
	using StrongDep = Threading::_hidden_::_TaskDependency<true>;
	
	using WeakDepArray   = Threading::_hidden_::_TaskDependencyArray<false>;
	using StrongDepArray = Threading::_hidden_::_TaskDependencyArray<true>;
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

		enum class EThread : uint
		{
			Main,		// thread with window message loop
			Worker,
			Renderer,	// single thread for opengl, multiple for vulkan (can be mixed with 'Worker')	// TODO: RendererHi, RendererLow
			FileIO,
			Network,
			_Count
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

		DEBUG_ONLY(
			Atomic<bool>			_isRunning			{false};
		)

		// TODO: profiling:
		// - added to queue time
		// - start execution time


	// methods
	public:
		virtual ~IAsyncTask ();

		ND_ EThread	Type ()			 const	{ return _threadType; }

		ND_ EStatus	Status ()		 const	{ return _status.load(); }

		ND_ bool	IsInQueue ()	 const	{ return Status() < EStatus::_Finished; }
		ND_ bool	IsFinished ()	 const	{ return Status() > EStatus::_Finished; }
		ND_ bool	IsInterropted () const	{ return Status() > EStatus::_Interropted; }

	protected:
		explicit IAsyncTask (EThread type);

			virtual void			Run () = 0;
			virtual void			OnCancel ()			{ ASSERT( not _isRunning.load() ); }
		ND_ virtual NtStringView	DbgName ()	const	{ return "unknown"; }
		
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
	// types
	public:
		using EThread = IAsyncTask::EThread;

	// interface
	public:
		virtual bool  Attach (uint uid) = 0;
		virtual void  Detach () = 0;

		ND_ virtual NtStringView  DbgName () const	{ return "thread"; }

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
		using EThread			= IAsyncTask::EThread;
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
		
		AE_VTUNE(
			__itt_domain*	_vtuneDomain	= null;
		)


	// methods
	public:
		static void  CreateInstance ();
		static void  DestroyInstance ();

		bool  Setup (usize maxWorkerThreads);
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
		
		template <typename ...Deps>
		bool  Run (AsyncTask task, const Tuple<Deps...> &deps = Default);

		bool  Enqueue (AsyncTask task);

		ND_ bool  Wait (ArrayView<AsyncTask> tasks, EThreadMask mask = Default, nanoseconds timeout = GetDefaultTimeout());

		bool  Cancel (const AsyncTask &task);


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
	
/*
=================================================
	Scheduler
=================================================
*/
	ND_ inline TaskScheduler&  Scheduler ()
	{
		return *TaskScheduler::_Instance();
	}

}	// AE::Threading
