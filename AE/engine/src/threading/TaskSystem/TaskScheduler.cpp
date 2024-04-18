// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/Memory/GlobalLinearAllocator.h"
#include "threading/TaskSystem/ThreadManager.h"
#include "threading/TaskSystem/LfTaskQueue.h"

#include "threading/DataSource/WinAsyncDataSource.h"
#include "threading/DataSource/UnixAsyncDataSource.h"

namespace AE::Threading
{
/*
=================================================
	constants
=================================================
*/
namespace {
	static constexpr uint	c_AvgTaskTime_us	{128};		// ~0.1ms
	static constexpr uint	c_WaitTimeStep_us	{2 << 10};	// ~2ms


	ND_ static uint  CalcMaxTasksPerTick (nanoseconds timeout) __NE___
	{
		uint	max_tasks = (Min( uint((timeout.count() + 512) >> 10), c_WaitTimeStep_us ) + c_AvgTaskTime_us/2) / c_AvgTaskTime_us;
		return Clamp( max_tasks, 2u, 32u );
	}
}

/*
=================================================
	SeedFromThreadID
----
	return 8 bit seed
=================================================
*/
namespace {
	ND_ inline EThreadSeed  SeedFromThreadID () __NE___
	{
		usize	seed = ThreadUtils::GetIntID();
	#if AE_PLATFORM_BITS >= 64
		seed = (seed >> 32) ^ (seed & 0xFFFFFFFF);
	#endif
		seed = (seed >> 16) ^ (seed & 0xFFFF);
		seed = (seed >> 8) ^ (seed & 0xFF);
		return EThreadSeed(seed);
	}
}
//-----------------------------------------------------------------------------


/*
=================================================
	OutputChunk::Init
=================================================
*/
	void  IAsyncTask::OutputChunk::Init () __NE___
	{
		next	= null;
		count	= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_AsyncTaskTotalCount
=================================================
*/
DEBUG_ONLY(
	INTERNAL_LINKAGE( Atomic<slong>  asyncTaskCounter {0} );

	slong  IAsyncTask::_AsyncTaskTotalCount () __NE___		{ return asyncTaskCounter.load(); }
)
/*
=================================================
	constructor
=================================================
*/
	IAsyncTask::IAsyncTask (ETaskQueue type) __NE___ :
		_queueType{ type }
	{
		ASSERT( _queueType < ETaskQueue::_Count );
		DEBUG_ONLY( ++asyncTaskCounter );
	}

/*
=================================================
	_SetQueueType
=================================================
*/
	void  IAsyncTask::_SetQueueType (ETaskQueue type) __NE___
	{
		ASSERT( Status() == EStatus::Initial );
		ASSERT( _queueType < ETaskQueue::_Count );

		_queueType = type;
	}

/*
=================================================
	destructor
=================================================
*/
	IAsyncTask::~IAsyncTask () __NE___
	{
		ASSERT( _output.get() == null );
		DEBUG_ONLY(
			auto	stat = Status();
			ASSERT( stat == EStatus::Initial or stat > EStatus::_Finished );
		)
		DEBUG_ONLY( --asyncTaskCounter );
	}

/*
=================================================
	_SetCancellationState
----
	set 'Cancellation' state, 'Canceled' state will be set later
=================================================
*/
	bool  IAsyncTask::_SetCancellationState () __NE___
	{
		for (EStatus expected = EStatus::Pending;
			 not _status.CAS( INOUT expected, EStatus::Cancellation );)
		{
			// status has been changed in another thread
			if_unlikely( (expected == EStatus::Cancellation) or (expected > EStatus::_Finished) )
				return (expected == EStatus::Cancellation);

			// 'CAS' can return 'false' even if expected value is the same as current value in atomic.
			// any other state is valid.

			ThreadUtils::Pause();
		}
		return true;
	}

/*
=================================================
	OnFailure
=================================================
*/
	void  IAsyncTask::OnFailure () __NE___
	{
		ASSERT( _isRunning.load() );

		for (EStatus expected = EStatus::InProgress;
			 not _status.CAS( INOUT expected, EStatus::Failed );)
		{
			// status has been changed in another thread
			if_unlikely( expected > EStatus::_Finished )
				return;

			ASSERT( expected != EStatus::Cancellation );	// TODO: Failed or Canceled ?
			ThreadUtils::Pause();
		}

		EXLOCK( _output );
		_FreeOutputChunks( true );
	}

/*
=================================================
	_OnFinish
=================================================
*/
	void  IAsyncTask::_OnFinish (OUT bool& rerun) __NE___
	{
		ASSERT( not _isRunning.load() );

		// Flush cache before processing 'output'.
		// Spinlock '_output' issue Acquire -> Process -> Release fences,
		// but next task can be executed before 'Release' fence.
		MemoryBarrier( EMemoryOrder::Release );

		EXLOCK( _output );

		// try to set completed state
		EStatus	expected = EStatus::InProgress;
		if ( _status.CAS_Loop( INOUT expected, EStatus::Completed ) or expected == EStatus::Failed )
		{
			ASSERT( _waitBits.load() == 0 );	// all input dependencies must complete

			_FreeOutputChunks( False{"NOT canceled"} );
			return;
		}

		// continue task execution
		if ( expected == EStatus::Continue )
		{
			rerun = true;
			return;
		}

		// cancel
		if ( expected == EStatus::Cancellation )
		{
			OnCancel();

			_status.store( EStatus::Canceled );

			_FreeOutputChunks( True{"canceled"} );
			return;
		}

		DBG_WARNING( "unknown state" );
	}

/*
=================================================
	_Cancel
=================================================
*/
	void  IAsyncTask::_Cancel () __NE___
	{
		ASSERT( not _isRunning.load() );

		EXLOCK( _output );	// TODO: move inside branch ?

		// Pending/InProgress -> Cancellation
		if ( _SetCancellationState() )
		{
			// cache is invalidated in 'EXLOCK( _output )'
			OnCancel();

			// set canceled state
			CHECK( _status.exchange( EStatus::Canceled ) == EStatus::Cancellation );

			_FreeOutputChunks( True{"canceled"} );
		}
	}

/*
=================================================
	_FreeOutputChunks
=================================================
*/
	void  IAsyncTask::_FreeOutputChunks (bool isCanceled) __NE___
	{
		ASSERT( _output.is_locked() );

		auto&	chunk_pool = Scheduler()._GetChunkPool();

		for (OutputChunk* chunk = _output.get();  chunk != null; )
		{
			for (uint i = 0; i < chunk->count; ++i)
			{
				auto&		dep			= chunk->tasks[i];
				const uint	idx			= chunk->deps[i].bitIndex;
				const uint	is_strong	= chunk->deps[i].isStrong;
				const auto	mask		= WaitBits_t{1} << idx;

				if_unlikely( isCanceled and is_strong )
					dep->_canceledDepsCount.fetch_add( 1 );

				WaitBits_t	old_bits = dep->_waitBits.fetch_and( ~mask ); // 1 -> 0
				Unused( old_bits );
				ASSERT( AllBits( old_bits, mask ));

				//if ( bits == 0 )
				//{
					// TODO: add ready to run tasks to list
				//}

				dep = null;
			}
			chunk->count = 0;

			OutputChunk*	old_chunk = chunk;

			chunk			= old_chunk->next;
			old_chunk->next = null;
			CHECK( chunk_pool.Unassign( old_chunk ));
		}

		_output.set( null );
	}

/*
=================================================
	_ResetState
=================================================
*/
	bool  IAsyncTask::_ResetState () __NE___
	{
		ASSERT( not _isRunning.load() );
		EXLOCK( _output );

		for (EStatus expected = EStatus::Completed;
			 not _status.CAS( INOUT expected, EStatus::Initial );)
		{
			if ( expected == EStatus::Completed	or
				 expected == EStatus::Failed	or
				 expected == EStatus::Canceled )
			{
				ThreadUtils::Pause();
				continue;
			}

			RETURN_ERR( "can't reset task which is not finished" );
		}

		_waitBits.store( UMax );
		_canceledDepsCount.store( 0 );
		ASSERT( _output.get() == null );

		return true;
	}

/*
=================================================
	_DbgSet
-----
	Only for debugging!
=================================================
*/
	void  IAsyncTask::_DbgSet (EStatus status) __NE___
	{
		_status.store( status );
		_waitBits.store( 0 );
	}

/*
=================================================
	_MakeCompleted
=================================================
*/
	void  IAsyncTask::_MakeCompleted () __NE___
	{
		ASSERT( _status.load() == EStatus::Initial );

		_status.store( EStatus::Completed );
		_waitBits.store( 0 );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_SetDependencyCompletionStatus
=================================================
*/
	void  ITaskDependencyManager::_SetDependencyCompletionStatus (const AsyncTask &task, uint depIndex, bool cancel) __NE___
	{
		ASSERT( task );

		if_unlikely( cancel )
			task->_canceledDepsCount.fetch_add( 1 );

		const auto	mask		= IAsyncTask::WaitBits_t{1} << depIndex;
		const auto	old_bits	= task->_waitBits.fetch_and( ~mask ); // 1 -> 0

		Unused( old_bits );
		ASSERT( AllBits( old_bits, mask ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Wakeup
----
	Windows: 5..20us to wakeup thread
=================================================
*/
	void  TaskScheduler::ThreadWakeup::Wakeup (EThreadBits bits) __NE___
	{
		{
			std::unique_lock	lock {_mutex};
			_activeThreads = bits;
		}
		_cv.notify_all();
	}

	void  TaskScheduler::ThreadWakeup::Wakeup (ETaskQueueBits bits) __NE___
	{
		EThreadBits		threads;
		for (ETaskQueue q : bits) {
			threads.insert( EThread(q) );
		}
		return Wakeup( threads );
	}

/*
=================================================
	WakeupAndDetach
=================================================
*/
	void  TaskScheduler::ThreadWakeup::WakeupAndDetach (LoopingFlag_t &looping) __NE___
	{
		{
			std::unique_lock	lock {_mutex};	// TODO: not needed?
			looping.store( 0 );
		}
		_cv.notify_all();
	}

/*
=================================================
	Suspend
=================================================
*/
	void  TaskScheduler::ThreadWakeup::Suspend (EThreadBits waitThreads, LoopingFlag_t &looping) __NE___
	{
		std::unique_lock	lock {_mutex};

		for (;;)
		{
			if ( (waitThreads & _activeThreads).Any() )
			{
				_activeThreads &= waitThreads;
				return;
			}

			// wakeup if thread will be terminated (joined)
			if ( looping.load() == 0 )
				return;

			_cv.wait( lock );
		}
	}

	void  TaskScheduler::ThreadWakeup::Suspend (ETaskQueueBits waitQueues, LoopingFlag_t &looping) __NE___
	{
		EThreadBits		wait_threads;
		for (ETaskQueue q : waitQueues) {
			wait_threads.insert( EThread(q) );
		}
		return Suspend( wait_threads, looping );
	}

	void  TaskScheduler::ThreadWakeup::Suspend (const EThreadArray &waitThreads, LoopingFlag_t &looping) __NE___
	{
		return Suspend( waitThreads.ToThreadMask(), looping );
	}
//-----------------------------------------------------------------------------



	//
	// Canceled Task
	//
	class TaskScheduler::_CanceledTask final : public IAsyncTask
	{
	public:
		_CanceledTask ()		__NE___	: IAsyncTask{ETaskQueue::PerFrame} { _DbgSet( EStatus::Canceled ); }

		void		Run ()		__Th_OV {}
		StringView  DbgName ()	C_NE_OV	{ return "canceled"; }
	};


	//
	// Dummy Request
	//
	class TaskScheduler::_DummyRequest final : public Threading::_hidden_::IAsyncDataSourceRequest
	{
	// methods
	public:
		_DummyRequest ()					__NE___	{ _status.store( EStatus::Cancelled ); }

		// IAsyncDataSourceRequest //
		Result		GetResult ()			C_NE_OV	{ return Default; }
		bool		Cancel ()				__NE_OV	{ return false; }
		Promise_t	AsPromise (ETaskQueue)	__NE_OV	{ return Default; }
	};
//-----------------------------------------------------------------------------


/*
=================================================
	_Instance
=================================================
*/
	INTERNAL_LINKAGE( InPlace<TaskScheduler>  s_TaskScheduler );

	TaskScheduler&  TaskScheduler::_Instance () __NE___
	{
		return s_TaskScheduler.AsRef();
	}

/*
=================================================
	InstanceCtor
=================================================
*/
	void  TaskScheduler::InstanceCtor::Create () __NE___
	{
		MemoryManagerImpl::InstanceCtor::Create();

		s_TaskScheduler.Create();
		MemoryBarrier( EMemoryOrder::Release );
	}

	void  TaskScheduler::InstanceCtor::Destroy () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );

		s_TaskScheduler.Destroy();

		MemoryBarrier( EMemoryOrder::Release );

		MemoryManagerImpl::InstanceCtor::Destroy();
	}

/*
=================================================
	constructor
=================================================
*/
	TaskScheduler::TaskScheduler () __NE___ :
		_canceledTask{ MakeRC<_CanceledTask>() },
		_cancelledRequest{ MakeRC<_DummyRequest>() }
	{
		DEBUG_ONLY(
			_deadlockCheck.lastUpdate.store( TimePoint_t::clock::now() );
		)
	}

/*
=================================================
	destructor
=================================================
*/
	TaskScheduler::~TaskScheduler () __NE___
	{
		// you must call 'Release' before destroy
		EXLOCK( _threadGuard );
		CHECK( _threads.empty() );
	}

/*
=================================================
	Setup
=================================================
*/
	bool  TaskScheduler::Setup (const Config &cfg) __NE___
	{
		CHECK_ERR( uint(cfg.mainThreadCoreId) < ThreadUtils::MaxThreadCount() );

		// add main thread
		{
			EXLOCK( _threadGuard );
			CHECK_ERR( _threads.empty() );

			_mainThread = ThreadMngr::_CreateMainThread();
			CHECK_ERR( _mainThread->Attach( 0, cfg.mainThreadCoreId ));
		}

		// all queues must be null
		for (auto& q : _queues) {
			CHECK_ERR( not q.ptr );
		}

		// setup queues
		TRY{
			_queues[uint( ETaskQueue::Main		)].ptr.reset( new LfTaskQueue{ POTValue{ 2 },						"main"		});
			_queues[uint( ETaskQueue::PerFrame	)].ptr.reset( new LfTaskQueue{ POTValue{ cfg.maxPerFrameQueues },	"perFrame"	});
			_queues[uint( ETaskQueue::Renderer	)].ptr.reset( new LfTaskQueue{ POTValue{ cfg.maxRenderQueues },		"renderer"	});
			_queues[uint( ETaskQueue::Background)].ptr.reset( new LfTaskQueue{ POTValue{ cfg.maxBackgroundQueues },	"background"});
			StaticAssert( uint(ETaskQueue::_Count) == 4 );
		}
		CATCH_ALL(
			RETURN_ERR( "failed to initialize task queues" );
		)

		// all queues must not be null
		for (auto& q : _queues) {
			CHECK_ERR( q.ptr );
		}

		CHECK_ERR( _InitIOServices( cfg ));

		MemoryBarrier( EMemoryOrder::Release );
		return true;
	}

/*
=================================================
	_InitIOServices
=================================================
*/
	bool  TaskScheduler::_InitIOServices (const Config &cfg) __NE___
	{
		auto	io_dep_mngr = RC<AsyncDSRequestDependencyManager>{ new AsyncDSRequestDependencyManager{} };
		CHECK_ERR( RegisterDependency< AsyncDSRequest >( io_dep_mngr ));
		CHECK_ERR( RegisterDependency< WeakAsyncDSRequest >( io_dep_mngr ));
		//CHECK_ERR( RegisterDependency< StrongAsyncDSRequest >( io_dep_mngr ));

		#ifdef AE_PLATFORM_WINDOWS
		if ( cfg.maxIOAccessThreads > 0 )
			_fileIOService = RC<WindowsIOService>{ new WindowsIOService{ cfg.maxIOAccessThreads }};
		#endif

		#ifdef AE_PLATFORM_UNIX_BASED
		if ( cfg.maxIOAccessThreads > 0 )
			_fileIOService = RC<UnixIOService>{ new UnixIOService{ cfg.maxIOAccessThreads }};
		#endif

		return true;
	}

/*
=================================================
	Release
=================================================
*/
	void  TaskScheduler::Release () __NE___
	{
		// detach threads
		{
			EXLOCK( _threadGuard );

			for (auto& thread : _threads) {
				thread->Detach();
			}
			_threads.clear();
			_mainThread = null;
		}

		for (auto& q : _queues)
		{
			q.ptr->WriteProfilerStat();
			q.ptr->Release();
		}

		// free dependency managers
		{
			EXLOCK( _taskDepsMngrsGuard );
			_taskDepsMngrs.clear();
		}

		_canceledTask		= null;
		_cancelledRequest	= null;
		ASSERT_Eq( IAsyncTask::_AsyncTaskTotalCount(), 0 );

		_chunkPool.Release( True{"check for assigned"} );

		PROFILE_ONLY(
			_profiler.store( null );
		)
	}

/*
=================================================
	AddThread
=================================================
*/
	bool  TaskScheduler::AddThread (RC<IThread> thread, ECpuCoreId coreId) __NE___
	{
		EXLOCK( _threadGuard );

		CHECK_ERR( thread != null );

		CHECK_ERR( thread->Attach( uint(_threads.size()), coreId ));

		ASSERT( _threads.size() <= ThreadUtils::MaxThreadCount() );

		PROFILE_ONLY(
			if ( auto prof =_profiler.load() )
				prof->AddThread( thread );
		)

		_threads.push_back( RVRef(thread) );	// should not throw
		return true;
	}

/*
=================================================
	GetDefaultSeed
=================================================
*/
	EThreadSeed  TaskScheduler::GetDefaultSeed () __NE___
	{
		return SeedFromThreadID();
	}

/*
=================================================
	ProcessTask
----
	Returns 'true' if task is processed.
	Result can be used to choose behavior:
		- on 'true' process next task with same queue type.
		- on 'false' try to process task with different queue type.
	The 'seed' is used only to distribute access from different threads to different chunks
	to avoid access to the same atomic value from multiple threads.
=================================================
*/
	bool  TaskScheduler::ProcessTask (const ETaskQueue type, const EThreadSeed seed) __NE___
	{
		CHECK_ERR( type < ETaskQueue::_Count );

		return _queues[ uint(type) ].ptr->Process( seed );
	}

/*
=================================================
	ProcessFileIO
=================================================
*/
	bool  TaskScheduler::ProcessFileIO () __NE___
	{
		if_likely( auto io_service = GetFileIOService() )
			return (io_service->ProcessEvents() > 0);

		return false;
	}

/*
=================================================
	ProcessTasks
----
	Process all tasks in specified 'threads' with the specified 'seed'.
	Returns 'true' if at least one task is processed.
	The 'seed' is used only to distribute access from different threads to different chunks
	to avoid access to the same atomic value from multiple threads.
=================================================
*/
	bool  TaskScheduler::ProcessTasks (const EThreadArray &threads, const EThreadSeed seed) __NE___
	{
		ASSERT( not threads.empty() );

		constexpr uint	max_task_per_queue = 16;

		uint	processed = 0;
		for (auto tt : threads)
		{
			if_likely( tt < EThread::_Last )
			{
				uint i = 0;
				for (; (i < max_task_per_queue) and ProcessTask( ETaskQueue(tt), seed ); ++i) {}
				processed |= i;
			}
			else
			if ( tt == EThread::FileIO )
				processed |= uint(ProcessFileIO());
			else
				DBG_WARNING( "unknown thread type" );
		}
		return processed != 0;
	}

/*
=================================================
	ProcessTasks
----
	Process all tasks in specified 'threads' with the specified 'seed'.
	Returns 'true' if at least one task is processed.
	The 'seed' is used only to distribute access from different threads to different chunks
	to avoid access to the same atomic value from multiple threads.
=================================================
*/
	bool  TaskScheduler::ProcessTasks (const EThreadArray &threads, const EThreadSeed seed, const uint maxTasks) __NE___
	{
		ASSERT( not threads.empty() );

		uint	processed = 0;
		for (auto tt = threads.begin(); (processed < maxTasks) and (tt != threads.end()); ++tt)
		{
			if_likely( *tt < EThread::_Last )
			{
				for (; (processed < maxTasks) and ProcessTask( ETaskQueue(*tt), seed ); ++processed) {}
			}
			else
			if ( *tt == EThread::FileIO )
				processed |= uint(ProcessFileIO());
			else
				DBG_WARNING( "unknown thread type" );
		}
		return processed != 0;
	}

/*
=================================================
	PullTask
----
	Extract one task from specified queue 'type'.
	Canceled tasks processed immediately.
	Cache is already invalidated when non-null task is returned.
	The 'seed' is used only to distribute access from different threads to different chunks
	to avoid access to the same atomic value from multiple threads.
=================================================
*/
	AsyncTask  TaskScheduler::PullTask (const ETaskQueue type, const EThreadSeed seed) __NE___
	{
		CHECK_ERR( type < ETaskQueue::_Count );

		return _queues[ uint(type) ].ptr->Pull( seed );
	}

/*
=================================================
	Wait
----
	Warning: deadlock may occur if 'Wait()' is called in all threads,
	use it only for debugging and testing
=================================================
*/
	inline bool  TaskScheduler::_IsAllComplete (ArrayView<AsyncTask> tasks) __NE___
	{
		// TODO: optimization: use bitfield to mark null and complete tasks
		usize	complete_count = 0;

		for (auto& task : tasks)
		{
			if_likely( not task )
				++complete_count;
			else
			if_likely( task->IsFinished() )
				++complete_count;
		}
		return complete_count >= tasks.size();
	}

	bool  TaskScheduler::Wait (ArrayView<AsyncTask> tasks, nanoseconds timeout) __NE___
	{
		if_unlikely( tasks.empty() )
			return true;

		const auto	end_time = TimePoint_t::clock::now() + timeout;

		for (;;)
		{
			if ( _IsAllComplete( tasks ))
				return true;

			const auto	tp = TimePoint_t::clock::now();
			if_unlikely( tp >= end_time )
				return false;	// time is out

			DbgDetectDeadlock();
			ThreadUtils::Sleep_500us();
		}
	}

	bool  TaskScheduler::Wait (ArrayView<AsyncTask> tasks, const EThreadArray &threads, const nanoseconds timeout) __NE___
	{
		return Wait( tasks, threads, (TimePoint_t::clock::now() + timeout), CalcMaxTasksPerTick( timeout ));
	}

	bool  TaskScheduler::Wait (ArrayView<AsyncTask> tasks, const EThreadArray &threads, const TimePoint_t endTime, const uint maxTasksPerTick) __NE___
	{
		ASSERT( not threads.empty() );
		if_unlikely( tasks.empty() )
			return true;

		const auto	seed = SeedFromThreadID();
		for (;;)
		{
			if ( _IsAllComplete( tasks ))
				return true;

			uint	processed = 0;
			for (auto tt = threads.begin(); (processed < maxTasksPerTick) and (tt != threads.end()); ++tt)
			{
				if_likely( *tt < EThread::_Last )
					for (; (processed < maxTasksPerTick) and ProcessTask( ETaskQueue(*tt), seed ); ++processed) {}
				else
				if ( *tt == EThread::FileIO )
					processed |= uint(ProcessFileIO());
				else
					DBG_WARNING( "unknown thread type" );
			}

			if_unlikely( TimePoint_t::clock::now() >= endTime )
				return false;	// time is out

			if_unlikely( processed == 0 )
			{
				DbgDetectDeadlock();
				ThreadUtils::Sleep_500us();
			}
		}
	}

/*
=================================================
	Cancel
----
	returns 'true' if task has been canceled by this call.
	returns 'false' if task is already canceled or can not be canceled, check task status for additional info.
=================================================
*/
	bool  TaskScheduler::Cancel (const AsyncTask &task) __NE___
	{
		if_unlikely( task == null )
			return false;

		return task->_SetCancellationState();
	}

/*
=================================================
	_AddTaskDependencies
-----
	returns 'false':
		- on bits overflow
		- on allocation error
=================================================
*/
	bool  TaskScheduler::_AddTaskDependencies (const AsyncTask &task, const AsyncTask &dep, Bool isStrong, INOUT uint &bitIndex) __NE___
	{
		if_unlikely( dep == null )
			return true;

		CHECK_ERR( bitIndex + 1 < CT_SizeOfInBits<IAsyncTask::WaitBits_t> );	// TODO: add dummy task with dependencies

		EXLOCK( dep->_output );	// TODO: optimize

		// if we have lock and task is not finished then we can add output dependency
		// even if status has been changed in another thread
		const EStatus	status	= dep->Status();

		// add to output
		if ( status < EStatus::_Finished )
		{
			OutputChunk_t*	root = dep->_output.get();

			for (OutputChunk_t** chunk_ref = &root;;)
			{
				if ( *chunk_ref == null )
				{
					uint	idx;
					CHECK_ERR( _chunkPool.Assign( OUT idx ));

					*chunk_ref = &_chunkPool[ idx ];
					(*chunk_ref)->Init();
				}

				if ( (*chunk_ref)->next != null )
				{
					chunk_ref = &(*chunk_ref)->next;
					continue;
				}

				if ( (*chunk_ref)->count < IAsyncTask::ElemInChunk )
				{
					const uint	i = (*chunk_ref)->count++;

					(*chunk_ref)->tasks[i]	= task;
					(*chunk_ref)->deps[i]	= IAsyncTask::TaskDependency{ bitIndex, isStrong };

					ASSERT( bitIndex == (*chunk_ref)->deps[i].bitIndex );
					++bitIndex;
					break;
				}

				chunk_ref = &(*chunk_ref)->next;
			}

			dep->_output.set( root );
		}

		// cancel current task
		if_unlikely( isStrong and status > EStatus::_Interrupted )
			return false;

		return true;
	}

/*
=================================================
	_InsertTask
=================================================
*/
	bool  TaskScheduler::_InsertTask (AsyncTask task, uint bitIndex) __NE___
	{
		ASSERT( task );

		// some dependencies may already be completed, so merge bit mask with current
		task->_waitBits.fetch_and( ToBitMask<WaitBits_t>( bitIndex ));

		return Enqueue( RVRef(task) );
	}

/*
=================================================
	Enqueue
----
	warning: 'task->_waitBits' is not changed, use 'Run()' instead

	returns 'false' on:
		- task is null
		- task was canceled or failed
=================================================
*/
	bool  TaskScheduler::Enqueue (AsyncTask task) __NE___
	{
		CHECK_ERR( task != null );
		ASSERT( task->QueueType() < ETaskQueue::_Count );

		for (EStatus expected = EStatus::Initial;
			 not task->_status.CAS( INOUT expected, EStatus::Pending );)
		{
			// status has been changed in another thread
			if_unlikely( expected > EStatus::_Finished )
				return false;

			// one of dependency is already cancelled, so current task is marked as cancelled before enqueue
			if_unlikely( expected == EStatus::Cancellation )
				break;

			// 'CAS' can return 'false' even if expected value is the same as current value in atomic
			ASSERT( expected == EStatus::Initial or expected == EStatus::Continue );
			ThreadUtils::Pause();
		}

		PROFILE_ONLY(
			if ( auto prof =_profiler.load() )
			{
				task->_profiler = prof;
				prof->Enqueue( *task );
			})

	  #if AE_USE_THREAD_WAKEUP
		_wakeup.Wakeup( task->QueueType() );
	  #endif

		const uint	tid = uint(task->QueueType());

		_queues[tid].ptr->Add( RVRef(task), SeedFromThreadID() );
		return true;
	}

/*
=================================================
	SuspendThread
=================================================
*/
	void  TaskScheduler::SuspendThread (const EThreadArray &threads, LoopingFlag_t &looping, uint iteration) __NE___
	{
	  #if AE_USE_THREAD_WAKEUP

		if_likely( iteration < 4 )
			ThreadUtils::Sleep_500us();
		else
			_wakeup.Suspend( threads, looping );

	  #else

		Unused( threads, looping );
		ThreadUtils::ProgressiveSleep( iteration );

	  #endif
	}

/*
=================================================
	SuspendThread
=================================================
*/
	void  TaskScheduler::WakeupAndDetach (LoopingFlag_t &looping) __NE___
	{
	  #if AE_USE_THREAD_WAKEUP
		_wakeup.WakeupAndDetach( looping );

	  #else
		Unused( looping );

	  #endif
	}

/*
=================================================
	SetProfiler
=================================================
*/
	void  TaskScheduler::SetProfiler (RC<ITaskProfiler> profiler) __NE___
	{
		PROFILE_ONLY(
			if ( profiler )
			{
				EXLOCK( _threadGuard );
				for (auto& t : _threads) {
					profiler->AddThread( t );
				}
				profiler->AddThread( _mainThread );
			}
			_profiler.store( RVRef(profiler) );
		)
	}

/*
=================================================
	DbgDetectDeadlock
=================================================
*/
	void  TaskScheduler::DbgDetectDeadlock () __NE___
	{
	#ifdef AE_DEBUG

		// do we need to check for deadlocks again?
		{
			_deadlockCheck.numChecks.fetch_add( 1 );

			bool	changed = false;
			for (auto& q : _queues)
			{
				ulong	cnt = q.ptr->GetTotalProcessedTasks();
				changed |= (q.totalProcessed != cnt);
				q.totalProcessed = cnt;
			}

			const TimePoint_t	cur_time = TimePoint_t::clock::now();

			if ( changed )
			{
				_deadlockCheck.lastUpdate.fetch_max( cur_time );
				return;
			}

			_deadlockCheck.numLocks.fetch_add( 1 );

			TimePoint_t	last_check = _deadlockCheck.lastUpdate.load();

			for (;;)
			{
				if ( TimeCast<secondsf>( cur_time - last_check ) < _deadlockCheck.interval )
					return;

				if ( _deadlockCheck.lastUpdate.CAS( INOUT last_check, cur_time ))
					break;

				ThreadUtils::Pause();
			}

			const double	rate = double(_deadlockCheck.numLocks.load()) / _deadlockCheck.numChecks.load();
			if ( rate < _deadlockCheck.minRate )
				return;

			_deadlockCheck.numChecks.store( 0 );
			_deadlockCheck.numLocks.store( 0 );
		}

		using TaskArr_t		= Array< Tuple< AsyncTask, IAsyncTask::TaskDependency >>;
		using TaskArr2_t	= Array< Tuple< String, IAsyncTask::TaskDependency >>;

		struct InOutDeps
		{
			AsyncTask	_self;
			TaskArr_t	in;
			TaskArr2_t	in2;
			TaskArr_t	out;
		};
		HashMap< const IAsyncTask*, InOutDeps >		map;

		const auto	CheckTask = [&map] (AsyncTask task)
		{{
			ASSERT( task );
			EXLOCK( task->_output );

			auto&	io = map[ task.get() ];
			io._self = task;
			for (const OutputChunk_t* chunk = task->_output.get();  chunk != null; )
			{
				for (uint i = 0; i < chunk->count; ++i)
				{
					io.out.emplace_back( chunk->tasks[i], chunk->deps[i] );

					AsyncTask	task2	= chunk->tasks[i];
					auto&		io2		= map[ task2.get() ];

					io2._self = task2;
					io2.in.emplace_back( task, chunk->deps[i] );
				}
				chunk = chunk->next;
			}
		}};

		for (auto& q : _queues)
		{
			q.totalProcessed = q.ptr->GetTotalProcessedTasks();
			q.ptr->DbgDetectDeadlock( CheckTask );
		}


		const auto	CheckTask2 = [&map] (StringView name, AsyncTask task, IAsyncTask::TaskDependency dep)
		{{
			auto&	io = map[ task.get() ];
			io._self = task;
			io.in2.emplace_back( String{name}, dep );
		}};

		{
			SHAREDLOCK( _taskDepsMngrsGuard );
			for (auto& mngr : _taskDepsMngrs) {
				mngr.second->DbgDetectDeadlock( CheckTask2 );
			}
		}

		String	log;

		const auto	LogDep = [&log] (const ulong idx, const InOutDeps &io)
		{{
			for (auto& dep : io.in)
			{
				if ( dep.Get<1>().bitIndex == idx )
				{
					auto&	task = dep.Get<0>();
					log << "'" << task->DbgName() << "' (" << ToString<16>(usize(task.get())) << ")";
					return;
				}
			}
			for (auto& dep : io.in2)
			{
				if ( dep.Get<1>().bitIndex == idx )
				{
					log << "'" << dep.Get<0>() << "'";
					return;
				}
			}

			// When it happens:
			//	- task is currently executed, so it removed from queue, but not yet marked as completed
			//	- error when checked custom dependency
			log << "not found";
		}};

		for (auto& [task, io] : map)
		{
			log << "\n\n'" << task->DbgName() << "' (" << ToString<16>(usize(task)) << ")"
				//<< " wait for (" << ToString(BitCount(task->_waitBits.load())) << ")"
				<< (task->_canceledDepsCount.load() > 0 ? ", canceled" : "");

			switch_enum( task->Status() )
			{
				case EStatus::Pending :			break;

				case EStatus::Initial :			log << ", status: Initial";		break;
				case EStatus::InProgress :		log << ", status: InProgress";	break;
				case EStatus::Cancellation :	log << ", status: Cancellation";break;
				case EStatus::Continue :		log << ", status: Continue";	break;
				case EStatus::Completed :		log << ", status: Completed";	break;
				case EStatus::Canceled :		log << ", status: Canceled";	break;
				case EStatus::Failed :			log << ", status: Failed";		break;

				case EStatus::_Interrupted :
				case EStatus::_Finished :
				default :						log << ", status: Unknown";		break;
			}
			switch_end

			if ( auto bits = task->_waitBits.load() )
			{
				log	<< ", in:";
				for (uint i : BitIndexIterate( bits ))
				{
					log << "\n    [" << ToString(i) << "] ";
					LogDep( i, io );
				}
			}

			if ( not io.out.empty() )
			{
				log	<< "\n  out:";

				for (auto& dep : io.out)
				{
					log << "\n    '" << dep.Get<0>()->DbgName() << "' (" << ToString<16>(usize(dep.Get<0>().get())) << ") ["
						<< ToString(dep.Get<1>().bitIndex) << "]";
				}
			}
		}

		if ( not log.empty() )
			AE_LOG_SE( log );

	#endif // AE_DEBUG
	}


} // AE::Threading
