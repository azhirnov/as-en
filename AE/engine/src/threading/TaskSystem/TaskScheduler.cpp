// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/TaskScheduler.h"
#include "threading/Memory/GlobalLinearAllocator.h"
#include "threading/TaskSystem/ThreadManager.h"
#include "threading/TaskSystem/LfTaskQueue.h"
#include "threading/DataSource/FileAsyncDataSource.h"

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
	Nd__In EThreadSeed  SeedFromThreadID () __NE___
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
} // AE::Threading
//-----------------------------------------------------------------------------


namespace AE::_Coro_
{
	constinit StaticRC<AsyncTaskImpl>  AsyncTaskImpl::CanceledTask::s_canceled { _ConstInitStaticRC(0), ETaskStatus::Canceled };

/*
=================================================
	OutputChunk::Init
=================================================
*/
	void  AsyncTaskImpl::OutputChunk::Init () __NE___
	{
		StaticAssert( alignof(OutputChunk) >= _mask+1 );

		_packed	= 0;
	}
//-----------------------------------------------------------------------------


/*
=================================================
	_AsyncTaskTotalCount
=================================================
*/
DEBUG_ONLY(
	INTERNAL_LINKAGE( Atomic<slong>  asyncTaskCounter {0} );

	slong  AsyncTaskImpl::TaskSchedulerApi::AsyncTaskTotalCount () __NE___ { return asyncTaskCounter.load(); }
)
/*
=================================================
	constructor
=================================================
*/
	AsyncTaskImpl::AsyncTaskImpl (EFlags flags) __NE___ :
		_flags{flags}
	{
		DEBUG_ONLY( ++asyncTaskCounter );
	}

/*
=================================================
	_SetQueueType
=================================================
*/
	void  AsyncTaskImpl::_SetQueueType (ETaskQueue type) __NE___
	{
		ASSERT( AnyEqual( Status(), EStatus::Initial, EStatus::InProgress, EStatus::Continue ));
		ASSERT( type < ETaskQueue::_Count );

		_queueType = type;
	}

/*
=================================================
	destructor
=================================================
*/
	AsyncTaskImpl::~AsyncTaskImpl () __NE___
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
	Run
=================================================
*/
	void  AsyncTaskImpl::Run (OUT bool& rerun) __NE___
	{
		DEBUG_ONLY(
			ASSERT( Status() == EStatus::InProgress );
			_isRunning.store( true );
			++_resumeCount;
		)
		PROFILE_ONLY(
			if ( _profiler )
				_profiler->Begin( *this );
		)

		// resume coroutine
		auto	coro_handle = CoroHandle_t::from_promise( *this );

		ASSERT( bool{coro_handle} and not coro_handle.done() );
		ASSERT( NoBits( _flags, EFlags::DoNotRun ));

		coro_handle.resume();	// throw

		if_unlikely( bool{coro_handle} and not coro_handle.done() )
			ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Error ));


		PROFILE_ONLY(
			if ( _profiler )
				_profiler->End( *this );
		)
		DEBUG_ONLY(
			_isRunning.store( false );
		)

		_OnFinish( OUT rerun );
	}

/*
=================================================
	_Inline_OnStart
=================================================
*/
	void  AsyncTaskImpl::_Inline_OnStart () __NE___
	{
		ASSERT( _status.load() == EStatus::Initial );
		ASSERT( _output.get() == null );

		_waitCount.store( 0 );
		_status.store( EStatus::InProgress );
		_queueType = ETaskQueue::Unknown;

		DEBUG_ONLY(
			_isRunning.store( true );
		)
	}

/*
=================================================
	_Inline_MakeCompletedOnReturn
=================================================
*/
	void  AsyncTaskImpl::_Inline_MakeCompletedOnReturn () __NE___
	{
		DEBUG_ONLY(
			auto	coro_handle = CoroHandle_t::from_promise( *this );

			CHECK( bool{coro_handle} and not coro_handle.done() );

			_isRunning.store( false );
		)

		bool	rerun;
		_OnFinish( OUT rerun );
		ASSERT( not rerun );
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  AsyncTaskImpl::_ReleaseObject () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );
		ASSERT( IsFinished() );

		if_unlikely( AllBits( _flags, EFlags::DoNotRun ))
		{
			delete this;
			return;  // coroutine is not created
		}

		auto	coro_handle = CoroHandle_t::from_promise( *this );

		// internally calls 'promise_type' dtor
		coro_handle.destroy();
	}

/*
=================================================
	_SetCancellationState
----
	set 'Cancellation' state, 'Canceled' state will be set later during '_Cancel()' call.
=================================================
*/
	bool  AsyncTaskImpl::_SetCancellationState () __NE___
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
	_Error
----
	set 'Error' state, immediately free output dependencies.
	'OnCancel()' will not be called.
=================================================
*/
	void  AsyncTaskImpl::_Error () __NE___
	{
		ASSERT( _isRunning.load() );

		for (EStatus expected = EStatus::InProgress;
			 not _status.CAS( INOUT expected, EStatus::Error );)
		{
			// status has been changed in another thread
			if_unlikely( expected > EStatus::_Finished )
				return;

			ASSERT( expected != EStatus::Cancellation );	// TODO: Error or Canceled ?
			ThreadUtils::Pause();
		}

		EXLOCK( _output );
		_FreeOutputChunks( True{"canceled"} );
	}

/*
=================================================
	_OnFinish
=================================================
*/
	void  AsyncTaskImpl::_OnFinish (OUT bool& rerun) __NE___
	{
		ASSERT( not _isRunning.load() );
		rerun = false;

		// Flush cache before processing 'output'.
		// Spinlock '_output' issue Acquire -> Process -> Release fences,
		// but next task can be executed before 'Release' fence.
		MemoryBarrier( EMemoryOrder::Release );

		EXLOCK( _output );

		// try to set completed state
		EStatus	expected = EStatus::InProgress;
		if ( _status.CAS_Loop( INOUT expected, EStatus::Completed )	or
			 AnyEqual( expected, EStatus::Completed, EStatus::Error ))
		{
			ASSERT( _waitCount.load() == 0 );	// all input dependencies must complete

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
	void  AsyncTaskImpl::_Cancel () __NE___
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
	void  AsyncTaskImpl::_FreeOutputChunks (Bool isCanceled) __NE___
	{
		ASSERT( _output.is_locked() );

		auto&	chunk_pool = TaskScheduler::AsyncTaskApi::GetChunkPool();

		for (OutputChunk* chunk = _output.get();  chunk != null; )
		{
			for (uint i = 0, cnt = chunk->Count(); i < cnt; ++i)
			{
				auto&			dep			= chunk->tasks[i];
				AsyncTaskImpl*	task		= dep.get();
				const bool		is_strong	= dep.Extra() == 1;

				if_unlikely( isCanceled and is_strong )
					task->_willBeCanceled.store( true );

				auto	count = task->_waitCount.fetch_sub( 1 );
				ASSERT_Gt( count, 0 );
				Unused( count );

				//if ( count == 1 )
				//{
					// TODO: add ready to run tasks to list
				//}

				dep = null;
			}

			OutputChunk*	old_chunk = chunk;
			chunk = old_chunk->Next();

			old_chunk->_packed = 0;

			CHECK( chunk_pool.Unassign( old_chunk ));
		}

		_output.set( null );
	}

/*
=================================================
	_DbgSet
-----
	Only for debugging!
=================================================
*/
	void  AsyncTaskImpl::_DbgSet (EStatus status) __NE___
	{
		_status.store( status );
		_waitCount.store( 0 );
	}

/*
=================================================
	_MakeCompletedUnsafe
=================================================
*/
	void  AsyncTaskImpl::_MakeCompletedUnsafe () __NE___
	{
		ASSERT( _status.load() == EStatus::Initial );
		ASSERT( _output.get() == null );	// use '_MakeCompletedSafe()' instead

		_status.store( EStatus::Completed );
		_waitCount.store( 0 );
	}

/*
=================================================
	_MakeCompletedSafe
=================================================
*/
	void  AsyncTaskImpl::_MakeCompletedSafe () __NE___
	{
		ASSERT( _status.load() == EStatus::Initial );

		// enqueue
		_waitCount.store( 0 );
		_status.store( EStatus::InProgress );

		// skip 'Run()'
		//DEBUG_ONLY( _isRunning.store( true ));
		//Run();
		//DEBUG_ONLY( _isRunning.store( false ));

		bool	rerun;
		_OnFinish( OUT rerun );

		ASSERT( not rerun );
		ASSERT( _status.load() == EStatus::Completed );
	}

/*
=================================================
	_SetDebugName
=================================================
*/
#if AE_ENABLE_TASK_NAME
	void  AsyncTaskImpl::_SetDebugName (StringView dbgName) __NE___
	{
		if ( dbgName.empty() )
			return;

		ASSERT( not _dbgName );

		_dbgName.reset( new char[dbgName.size()+1] );
		std::memcpy( OUT _dbgName.get(), dbgName.data(), dbgName.size() );
		_dbgName.get()[ dbgName.size() ] = 0;
	}
#endif
/*
=================================================
	_SetDebugName
=================================================
*/
#if AE_ENABLE_TASK_NAME
	void  AsyncTaskImpl::_SetDebugName (const SourceLoc &loc) __NE___
	{
		StringView	fn_name		= loc.FunctionName();
		StringView	file_stem	= loc.FileStem();
		usize		len			= 32;
		len += fn_name.size();
		len += file_stem.size();

		_dbgName.reset( new char[len] );

		if ( std::snprintf( OUT _dbgName.get(), len,
							"%.*s(%i): %.*s",
							int(file_stem.size()), file_stem.data(),
							loc.Line(),
							int(fn_name.size()), fn_name.data()
						  ) > 0 )
		{
			return;
		}
		_dbgName.reset();
	}
#endif
/*
=================================================
	TaskDependencyManagerApi::SetDependencyCompletionStatus
=================================================
*/
	void  AsyncTaskImpl::TaskDependencyManagerApi::SetDependencyCompletionStatus (AsyncTaskImpl &task, Bool isCanceled) __NE___
	{
		if_unlikely( isCanceled )
			task._willBeCanceled.store( true );

		const auto	count = task._waitCount.fetch_sub( 1 );
		ASSERT_Gt( count, 0 );
		Unused( count );
	}

/*
=================================================
	TaskSchedulerApi::Init
=================================================
*/
#if AE_ENABLE_TASK_NAME
	void  AsyncTaskImpl::TaskSchedulerApi::Init (AsyncTaskImpl &self, ETaskQueue queue, StringView dbgName, const SourceLoc &loc) __NE___
	{
		if ( queue != Default )
			self._SetQueueType( queue );

		if ( self._dbgName )
			return;

		if ( dbgName.empty() )
		{
		#if 1
			self._SetDebugName( loc );

		#else //defined(__cpp_lib_stacktrace) and not defined(AE_COMPILER_GCC)
			auto		stack	= std::stacktrace::current();
			auto		it		= stack.begin() + 2;
			const usize	cnt		= Min( 7u, stack.size() );
			String		tmp;

			for (usize i = 2; i < cnt; ++i, ++it)
			{
				if ( it->source_file().empty() )
					break;

				tmp << FileSystem::ToShortPath( it->source_file() ) << '(' << ToString( it->source_line() ) << "): " << it->description() << '\n';
			}
			self._SetDebugName( tmp );
		#endif
		}
		else
		{
			self._SetDebugName( dbgName );
		}
		Unused( dbgName );
	}
#endif
/*
=================================================
	_ResetCancellationState
=================================================
*/
	bool  AsyncTaskImpl::_ResetCancellationState () __NE___
	{
		ASSERT( _isRunning.load() );

		EStatus	expected = EStatus::Cancellation;
		for (;;)
		{
			if ( _status.CAS( INOUT expected, EStatus::InProgress ) or
				 expected == EStatus::InProgress )
				return true;

			ASSERT( AnyEqual( expected, EStatus::Cancellation, EStatus::Canceled ));

			if ( expected > EStatus::_Finished )
				break;
		}
		return false;
	}

} // AE::_Coro_
//-----------------------------------------------------------------------------



namespace AE::Threading
{
/*
=================================================
	GetChunkPool
=================================================
*/
	TaskScheduler::OutputChunkPool_t&  TaskScheduler::AsyncTaskApi::GetChunkPool () __NE___
	{
		return Scheduler()._chunkPool;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Instance
=================================================
*/
	INTERNAL_LINKAGE( InPlace<TaskScheduler>  s_TaskScheduler );

	TaskScheduler&  TaskScheduler::_Instance () __NE___
	{
		return s_TaskScheduler.Ref();
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
	TaskScheduler::TaskScheduler () __NE___
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
			_queues[uint( ETaskQueue::Main		)].ptr.reset( new LfTaskQueue{ POTValue{ 2u },						"main",			ETaskQueue::Main		});
			_queues[uint( ETaskQueue::PerFrame	)].ptr.reset( new LfTaskQueue{ POTValue{ cfg.maxPerFrameQueues },	"perFrame",		ETaskQueue::PerFrame	});
			_queues[uint( ETaskQueue::Renderer	)].ptr.reset( new LfTaskQueue{ POTValue{ cfg.maxRenderQueues },		"renderer",		ETaskQueue::Renderer	});
			_queues[uint( ETaskQueue::Background)].ptr.reset( new LfTaskQueue{ POTValue{ cfg.maxBackgroundQueues },	"background",	ETaskQueue::Background	});
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

		_fileIOService = null;

		ASSERT_Eq( TaskApi::AsyncTaskTotalCount(), 0 );

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
	Wait
----
	Warning: deadlock may occur if 'Wait()' is called in all threads,
	use it only for debugging and testing.
	'maxTasksPerTick' - number of tasks per thread type which
	will be processed before switching to the next thread type.
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
		const auto	end_time = TimePoint_t::clock::now() + timeout;

		return Wait( tasks, end_time );
	}

	bool  TaskScheduler::Wait (ArrayView<AsyncTask> tasks, const TimePoint_t endTime) __NE___
	{
		if_unlikely( tasks.empty() )
			return true;

		for (;;)
		{
			if ( _IsAllComplete( tasks ))
				return true;

			const auto	tp = TimePoint_t::clock::now();
			if_unlikely( tp >= endTime )
			{
				// time is out
				return _IsAllComplete( tasks );
			}

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
			{
				// time is out
				return _IsAllComplete( tasks );
			}

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
	bool  TaskScheduler::Cancel (const AsyncTask &task, Bool fastCancel) __NE___
	{
		if_unlikely( task == null )
			return false;

		if ( fastCancel )
			return TaskApi::SetCancellationState( *task );
		else{
			TaskApi::CancelAsDependency( *task );
			return task->Status() < ETaskStatus::_Finished;
		}
	}

/*
=================================================
	_AddTaskDependencies
-----
	returns 'false':
		- on allocation error
=================================================
*/
	bool  TaskScheduler::_AddTaskDependencies (Task &task, const AsyncTask &dep, Bool isStrong) __NE___
	{
		if_unlikely( dep == null )
			return true;

		auto&	output_chunks = TaskApi::OutputChunkRef( *dep );
		EXLOCK( output_chunks );	// TODO: optimize

		// if we have lock and task is not finished then we can add output dependency
		// even if status has been changed in another thread
		const ETaskStatus	status	= dep->Status();

		// add to output
		if ( status < ETaskStatus::_Finished )
		{
			OutputChunk_t*	root = output_chunks.get();

			for (OutputChunk_t* chunk = root, *prev = null;;)
			{
				if ( chunk == null )
				{
					uint	idx;
					CHECK_ERR( _chunkPool.Assign( OUT idx ));

					chunk = &_chunkPool[ idx ];
					chunk->Init();

					if ( prev )		prev->SetNext( chunk );
					else			root = chunk;
				}

				if ( uint cnt = chunk->Count(); cnt < OutputChunk_t::_chunkSize )
				{
					const uint	i = cnt++;

					chunk->SetCount( cnt );
					chunk->tasks[i]	= task.GetRC();
					chunk->tasks[i].SetExtra( uint{isStrong} );
					break;
				}

				prev	= chunk;
				chunk	= chunk->Next();
			}

			output_chunks.set( root );

			TaskApi::IncWaitCounter( task );
		}

		// cancel current task
		if_unlikely( isStrong and status > ETaskStatus::_Interrupted )
			return false;

		return true;
	}

/*
=================================================
	Enqueue
----
	warning: 'task->_waitCount' will not be changed, use 'EnqueueNew()' instead

	returns 'false' on:
		- task is null
		- task was canceled or failed
=================================================
*/
	bool  TaskScheduler::Enqueue (AsyncTask task) __NE___
	{
		CHECK_ERR( task != null );
		ASSERT( task->QueueType() < ETaskQueue::_Count );

		for (ETaskStatus expected = ETaskStatus::Initial;
			 not TaskApi::SetPending( *task, INOUT expected );)
		{
			// status has been changed in another thread
			if_unlikely( expected > ETaskStatus::_Finished )
				return false;

			// one of dependency is already cancelled, so current task is marked as cancelled before enqueue
			if_unlikely( expected == ETaskStatus::Cancellation )
				break;

			// 'CAS' can return 'false' even if expected value is the same as current value in atomic
			ASSERT( expected == ETaskStatus::Initial or expected == ETaskStatus::Continue );
			ThreadUtils::Pause();
		}

		PROFILE_ONLY( TaskApi::SetProfiler( *task, _profiler.load() ));

		const uint	tid = uint(task->QueueType());

		_queues[tid].ptr->Add( RVRef(task), SeedFromThreadID() );
		return true;
	}

	bool  TaskScheduler::Enqueue (AsyncTask task, ETaskQueue queue) __NE___
	{
		if ( task )
			TaskApi::SetQueueType( *task, queue );

		return Enqueue( RVRef(task) );
	}

/*
=================================================
	SuspendThread
=================================================
*/
	void  TaskScheduler::SuspendThread (uint iteration) __NE___
	{
		ThreadUtils::ProgressiveSleepInf( iteration );
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
		Unused( profiler );
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

		using TaskArr_t		= Array< AsyncTask >;
		using TaskArr2_t	= Array< String >;

		struct InOutDeps
		{
			AsyncTask	_self;
			TaskArr_t	in;
			TaskArr2_t	in2;
			TaskArr_t	out;
		};
		HashMap< const Task*, InOutDeps >	map;

		const auto	CheckTask = [&map] (AsyncTask task)
		{{
			ASSERT( task );

			auto&	output_chunks = TaskApi::OutputChunkRef( *task );
			EXLOCK( output_chunks );

			auto&	io = map[ task.get() ];
			io._self = task;
			for (const OutputChunk_t* chunk = output_chunks.get();  chunk != null; )
			{
				for (uint i = 0, cnt = chunk->Count(); i < cnt; ++i)
				{
					io.out.emplace_back( chunk->tasks[i].get() );

					AsyncTask	task2	{chunk->tasks[i].get()};
					auto&		io2		= map[ task2.get() ];

					io2._self = task2;
					io2.in.emplace_back( task );
				}
				chunk = chunk->Next();
			}
		}};

		for (auto& q : _queues)
		{
			q.totalProcessed = q.ptr->GetTotalProcessedTasks();
			q.ptr->DbgDetectDeadlock( CheckTask );
		}


		const auto	CheckTask2 = [&map] (StringView name, AsyncTask task)
		{{
			auto&	io = map[ task.get() ];
			io._self = task;
			io.in2.emplace_back( String{name} );
		}};

		{
			SHAREDLOCK( _taskDepsMngrsGuard );
			for (auto& mngr : _taskDepsMngrs) {
				mngr.second->DbgDetectDeadlock( CheckTask2 );
			}
		}

		String	log;

		for (auto& [task, io] : map)
		{
			const int	wait_count = TaskApi::GetWaitCounter( *task );

			log << "\n\n  '" << task->DbgName() << "' (" << ToString<16>(usize(task)) << ")"
				<< " wait for (" << ToString( wait_count ) << ")"
				<< (TaskApi::IsFastCancellation( *task ) ? ", fast canceled" : "");

			switch_enum( task->Status() )
			{
				case ETaskStatus::Pending :			break;

				case ETaskStatus::Initial :			log << ", status: Initial";		break;
				case ETaskStatus::InProgress :		log << ", status: InProgress";	break;
				case ETaskStatus::Cancellation :	log << ", status: Cancellation";break;
				case ETaskStatus::Continue :		log << ", status: Continue";	break;
				case ETaskStatus::Completed :		log << ", status: Completed";	break;
				case ETaskStatus::Canceled :		log << ", status: Canceled";	break;
				case ETaskStatus::Error :			log << ", status: Error";		break;

				case ETaskStatus::_Interrupted :
				case ETaskStatus::_Finished :
				default :							log << ", status: Unknown";		break;
			}
			switch_end

			if ( wait_count != 0 )
			{
				log	<< ", in:";
				int	i = 0;
				for (auto& dep : io.in)
				{
					log << "\n      [" << ToString(i++) << "] ";
					log << "'" << dep->DbgName() << "' (" << ToString<16>(usize(dep.get())) << ")";
				}
				for (auto& dep : io.in2)
				{
					log << "\n      [" << ToString(i++) << "] ";
					log << "'" << dep << "'";
				}
				//CHECK( i == wait_count );
			}

			if ( not io.out.empty() )
			{
				log	<< "\n    out:";

				for (auto& dep : io.out)
				{
					log << "\n      '" << dep->DbgName() << "' (" << ToString<16>(usize(dep.get())) << ")";
				}
			}
		}

		if ( not log.empty() )
		{
			log << "\n------------------------------------\n";
			AE_LOGW( log );
		}

	#endif // AE_DEBUG
	}

} // AE::Threading
