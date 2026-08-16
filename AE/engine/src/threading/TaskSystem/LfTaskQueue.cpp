// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "threading/TaskSystem/LfTaskQueue.h"

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	LfTaskQueue::LfTaskQueue (POTValue seedMask, StringView name, ETaskQueue type) __Th___
	{
		for (auto& chunk : _chunks)
		{
			CHECK( chunk == null );

			chunk = new Chunk{};	// TODO: may throw
		}

		//_seedMask	= seedMask;

		Unused( seedMask, name, type );
		DEBUG_ONLY(
			_name		= name;
			_queueType	= type;
		)
	}

/*
=================================================
	Release
=================================================
*/
	void  LfTaskQueue::Release () __NE___
	{
		for (auto& chunk : _chunks)
		{
			Chunk*	chunk_ptr = chunk;
			chunk = null;

			for (; chunk_ptr != null;)
			{
				const PackedBits	prev = chunk_ptr->packed.exchange( PackedBits{}.Lock() );

				// chunk must be unlocked and empty
				CHECK_MSG( not prev.IsLocked(), "chunk is locked by another thread" );
				CHECK_MSG( prev.pack.count == 0, "queue must be empty" );

				for (usize i = 0, cnt = prev.pack.count; i < cnt; ++i)
				{
					AE_LOG_DBG( "Task '"s << chunk_ptr->array[i]->DbgName() << "' will not be executed" );

					chunk_ptr->array[i].~AsyncTask();
				}

				Chunk*	next = chunk_ptr->next.exchange( null );

				delete chunk_ptr;
				chunk_ptr = next;
			}
		}
	}

/*
=================================================
	Process
=================================================
*/
	bool  LfTaskQueue::Process (const EThreadSeed seed) __NE___
	{
		if_likely( AsyncTask task = Pull( seed ))
		{
			DEBUG_ONLY(
				const auto	start_time = TimePoint_t::clock::now();
			)

			bool	rerun = false;
			task->Run( OUT rerun );

			#ifdef AE_DEBUG
			{
				auto	dt = TimePoint_t::clock::now() - start_time;
				_workTime += dt.count();

				switch_enum( _queueType )
				{
					case ETaskQueue::Main :
					case ETaskQueue::PerFrame :
					case ETaskQueue::Renderer :
						if ( dt > milliseconds{100} )
							AE_LOGW( "Task '"s << task->DbgName() << "' executed " << ToString(dt) << " in " << ToString(_queueType) << " queue" );
						break;

					case ETaskQueue::Background :
						if ( dt > milliseconds{5'000} )
							AE_LOGW( "Task '"s << task->DbgName() << "' executed " << ToString(dt) << " in " << ToString(_queueType) << " queue" );
						break;

					case ETaskQueue::_Count :
						break;
				}
				switch_end
			}
			#endif

			if_unlikely( rerun )
			{
				Scheduler().Enqueue( RVRef(task) );		// TODO: check error
			}
			return true;
		}
		return false;
	}

/*
=================================================
	_RemoveTask
----
	returns 'true' to continue search
=================================================
*/
	inline bool  LfTaskQueue::_RemoveTask (TaskArr_t& arr, INOUT usize& pos, INOUT usize& count, OUT AsyncTask& task) __NE___
	{
		ASSERT( pos < count );
		ASSERT( task == null );

		using EFlags = TaskApi::EFlags;

		auto&			curr			= arr[ pos ];
		const EFlags	flags			= TaskApi::GetFlags( *curr );
		const bool		will_cancel		= TaskApi::WillBeCanceled( *curr );
		const bool		fast_cancel		= will_cancel and NoBits( flags, EFlags::RunCancelled );
		const bool		do_not_run		= AllBits( flags, EFlags::DoNotRun );	// TODO

		// check input dependencies
		if_unlikely( (not fast_cancel) and TaskApi::InputDepsAreNotFinished( *curr ))
		{
			// input dependencies is not complete
			return true;	// continue search
		}

		// remove task
		task.Swap( curr );

		if ( --count > 0 )
			curr.Swap( arr[ count ]);

		// try to start task
		ETaskStatus		status	= task->Status();

		if_likely( status == ETaskStatus::Pending								and
				   (not fast_cancel)											and
				   TaskApi::SetInProgress( *task, will_cancel, INOUT status )	and	// Pending -> InProgress / Cancellation
				   not do_not_run )
		{
			return false;	// stop search and run task
		}

		// fast cancellation, task will not resume
		// calls 'OnCancel()' and free output dependencies
		if_unlikely( (status == ETaskStatus::Cancellation) or will_cancel )
		{
			// TODO: cancel used with locked chunk - bad for performance
			// TODO: check if task has overloaded 'OnCancel()' method, or add task to queue inside '_Cancel()' ?
			TaskApi::Cancel( *task );
		}
		else
		if ( do_not_run )
		{
			TaskApi::MakeComplete( *task );
		}
		else
		// task was or will be run in another thread
		{
			ASSERT( status == ETaskStatus::InProgress or
					status >  ETaskStatus::_Finished );
		}

		task = null;

		return true;	// continue search
	}

/*
=================================================
	Pull
=================================================
*/
	AsyncTask  LfTaskQueue::Pull (const EThreadSeed seed) __NE___
	{
		DEBUG_ONLY(
			const auto	start_time = TimePoint_t::clock::now();
		)

		for (usize j = 0; j < MaxChunks; ++j)
		{
			Chunk*	chunk_ptr = _chunks[ (j + usize(seed)) % MaxChunks ];

			for (; chunk_ptr != null; chunk_ptr = chunk_ptr->next.load())
			{
				PackedBits	packed	= chunk_ptr->packed.load();

				if ( packed.pack.count == 0 )
					continue;

				// try to acquire spinlock
				bool	locked	= false;
				for (uint i = 0; i < SpinlockWaitCount; ++i)
				{
					if ( packed.IsLocked() )
						break; // locked by another thread

					if_likely( chunk_ptr->packed.CAS( INOUT packed, packed.Lock() ))
					{
						locked = true;
						break;
					}

					ThreadUtils::Pause();
				}

				// if spin-lock is acquired
				if_unlikely( not locked )
					continue;

				// load changes in 'Chunk::array'
				MemoryBarrier( EMemoryOrder::Acquire );
				ASSERT( not packed.IsLocked() );

				const PackedBits	old_packed	= packed.Lock();
				usize				count		= packed.pack.count;
				usize				pos			= packed.pack.pos;
				AsyncTask			task;

				for (; (pos < count) and _RemoveTask( chunk_ptr->array, pos, count, OUT task ); ++pos) {}

				if ( task == null )
				{
					const usize		start_pos = Min( packed.pack.pos, count );
					for (pos = 0; ((pos < start_pos) and (pos < count)) and _RemoveTask( chunk_ptr->array, pos, count, OUT task ); ++pos) {}
				}

				packed.pack.pos		= pos;
				packed.pack.count	= count;

				const bool	array_changed	= (old_packed.pack.count != count);
				const auto	order			=
					(array_changed and task != null) ?	EMemoryOrder::AcquireRelease :	// both
					array_changed					 ?	EMemoryOrder::Release :			// flush changes in 'Chunk::array'
														EMemoryOrder::Relaxed;

				// unlock
				const PackedBits	prev_packed = chunk_ptr->packed.exchange( packed, order );
				CHECK( old_packed == prev_packed );

				DEBUG_ONLY(
					_taskCount.Sub( old_packed.pack.count - packed.pack.count );
					_totalProcessed.fetch_add( old_packed.pack.count - packed.pack.count );
				)

				if ( task != null )
				{
					DEBUG_ONLY(
						_searchTime += (TimePoint_t::clock::now() - start_time).count();
					)
					return task;
				}
			}
		}

		DEBUG_ONLY(
			_searchTime += (TimePoint_t::clock::now() - start_time).count();
		)
		return null;
	}

/*
=================================================
	Add
=================================================
*/
	void  LfTaskQueue::Add (AsyncTask task, const EThreadSeed seed) __NE___
	{
		ASSERT( task );

		DEBUG_ONLY(
			const auto	start_time = TimePoint_t::clock::now();
		)

		for (;;)
		{
			for (usize j = 0; j < MaxChunks; ++j)
			{
				Chunk*	chunk_ptr = _chunks[ (j + usize(seed)) % MaxChunks ];

				for (uint depth = 0; chunk_ptr != null; ++depth)
				{
					// try to acquire spinlock
					PackedBits	packed	= chunk_ptr->packed.load();
					bool		locked	= false;

					for (uint i = 0; i < SpinlockWaitCount; ++i)
					{
						if ( packed.IsLocked() )
							break; // locked by another thread

						if_likely( chunk_ptr->packed.CAS( INOUT packed, packed.Lock() ))
						{
							locked = true;
							break;
						}

						ThreadUtils::Pause();
					}

					// if spin-lock is acquired
					if_likely( locked )
					{
						// load changes in 'Chunk::array'
						MemoryBarrier( EMemoryOrder::Acquire );

						ASSERT( not packed.IsLocked() );
						const PackedBits	old_packed	= packed.Lock();

						// insert task
						if_likely( packed.pack.count < chunk_ptr->array.size() )
						{
							chunk_ptr->array[ packed.pack.count ].Swap( task );
							++packed.pack.count;

							// flush changes in 'Chunk::array'
							MemoryBarrier( EMemoryOrder::Release );
						}

						// unlock
						const PackedBits	prev_packed = chunk_ptr->packed.exchange( packed );
						CHECK( old_packed == prev_packed );

						// if inserted
						if_likely( old_packed.pack.count != packed.pack.count )
						{
							DEBUG_ONLY(
								_insertionTime += (TimePoint_t::clock::now() - start_time).count();

								_maxTasks.fetch_max( _taskCount.Add( 1 ));
							)
							return;  // ok
						}
					}

					Chunk*	next = chunk_ptr->next.load();

					// add new chunk
					if_unlikely( next == null )
					{
						if ( depth >= MaxDepth )
						{
							AE_LOG_DBG( "task queue overflow" );
							break;
						}

						next = new Chunk{};  // throw

						for (Chunk* exp_chunk = null;;)
						{
							if_likely( chunk_ptr->next.CAS( INOUT exp_chunk, next ))
								break;

							// new chunk was added by another thread
							if_unlikely( exp_chunk != null )
							{
								delete next;
								next = exp_chunk;
								break;
							}

							ThreadUtils::Pause();
						}
					}

					chunk_ptr = next;
				}
			}

			ThreadUtils::Sleep_500us();
		}
	}

/*
=================================================
	CancelAll
=================================================
*
	LfTaskQueue::CancelAllResult  LfTaskQueue::CancelAll () __NE___
	{
		DEBUG_ONLY(
			const auto	start_time = TimePoint_t::clock::now();
		)

		CancelAllResult	result;

		for (usize j = 0; j < MaxChunks; ++j)
		{
			Chunk*	chunk_ptr = _chunks[ (j + usize(seed)) % MaxChunks ];

			for (; chunk_ptr != null; chunk_ptr = chunk_ptr->next.load())
			{
				PackedBits	packed	= chunk_ptr->packed.load();

				if ( packed.pack.count == 0 )
					continue;

				// try to acquire spinlock
				bool	locked	= false;
				for (uint i = 0; i < SpinlockWaitCount; ++i)
				{
					if ( packed.IsLocked() )
						break; // locked by another thread

					if_likely( chunk_ptr->packed.CAS( INOUT packed, packed.Lock() ))
					{
						locked = true;
						break;
					}

					ThreadUtils::Pause();
				}

				// if spin-lock is acquired
				if_unlikely( not locked )
				{
					++ result.skippedChunks;
					continue;
				}

				// load changes in 'Chunk::array'
				MemoryBarrier( EMemoryOrder::Acquire );
				ASSERT( not packed.IsLocked() );

				const PackedBits	old_packed	= packed.Lock();
				usize				count		= packed.pack.count;
				usize				pos			= packed.pack.pos;

				// TODO

				packed.pack.pos		= pos;
				packed.pack.count	= count;

				const bool	array_changed	= (old_packed.pack.count != count);
				const auto	order			=
					(array_changed and task != null) ?	EMemoryOrder::AcquireRelease :	// both
					array_changed					 ?	EMemoryOrder::Release :			// flush changes in 'Chunk::array'
														EMemoryOrder::Relaxed;

				// unlock
				const PackedBits	prev_packed = chunk_ptr->packed.exchange( packed, order );
				CHECK( old_packed == prev_packed );

				DEBUG_ONLY(
					_taskCount.Sub( old_packed.pack.count - packed.pack.count );
					_totalProcessed.fetch_add( old_packed.pack.count - packed.pack.count );
				)

				if ( task != null )
				{
					DEBUG_ONLY(
						_searchTime += (TimePoint_t::clock::now() - start_time).count();
					)
					return task;
				}
			}
		}

		DEBUG_ONLY(
			_searchTime += (TimePoint_t::clock::now() - start_time).count();
		)
		return result;
	}

/*
=================================================
	WriteProfilerStat
=================================================
*/
	void  LfTaskQueue::WriteProfilerStat () __NE___
	{
		DEBUG_ONLY(
			auto	work_time	= _workTime.exchange( 0 );
			auto	search_time	= _searchTime.exchange( 0 );
			auto	insert_time	= _insertionTime.exchange( 0 );
			auto	max_tasks	= _maxTasks.exchange( 0 );
			auto	task_count	= _totalProcessed.exchange( 0 );

			if ( (work_time == 0 and search_time == 0) or max_tasks == 0 )
				return;

			double	work		= double(work_time);
			double	search		= (work_time ? double(search_time) /  work : 1.0);
			double	insertion	= (work_time ? double(insert_time) /  work : 1.0);

			AE_LOGI( String{_name} << " queue "
				<< "\n  total work: " << ToString( nanoseconds{work_time} )
				<< "\n  search:     " << ToString( search * 100.0, 2 ) << " %"
				<< "\n  insertion:  " << ToString( insertion * 100.0, 2 ) << " %"
				<< "\n  max tasks:  " << ToString( max_tasks )
				<< "\n  avg task t: " << ToString( nanosecondsd{ work / task_count }));
		)
	}

/*
=================================================
	DbgDetectDeadlock
=================================================
*/
#ifdef AE_DEBUG

	void  LfTaskQueue::DbgDetectDeadlock (const Function<void (AsyncTask)> &fn) __NE___
	{
		for (usize j = 0; j < MaxChunks; ++j)
		{
			Chunk*	chunk_ptr = _chunks[j];

			for (; chunk_ptr != null; chunk_ptr = chunk_ptr->next.load())
			{
				PackedBits	packed	= chunk_ptr->packed.load();

				if ( packed.pack.count == 0 )
					continue;

				// lock spinlock
				[&]() {
					for (uint p = 0;; ++p)
					{
						for (uint i = 0; i < ThreadUtils::SpinBeforeLock(); ++i)
						{
							// expect in unlocked state
							packed.pack.locked = 0;

							if_likely( chunk_ptr->packed.CAS( INOUT packed, packed.Lock() ))
								return;

							ThreadUtils::Pause();
						}
						ThreadUtils::ProgressiveSleep( p );
					}
				}();

				// load changes in 'Chunk::array'
				MemoryBarrier( EMemoryOrder::Acquire );
				ASSERT( not packed.IsLocked() );

				const PackedBits	old_packed	= packed.Lock();
				const usize			count		= packed.pack.count;

				for (usize i = 0; i < count; ++i)
				{
					fn( chunk_ptr->array[i] );
				}


				// unlock
				const PackedBits	prev_packed = chunk_ptr->packed.exchange( packed );
				CHECK( old_packed == prev_packed );
			}
		}
	}

#endif // AE_DEBUG

} // AE::Threading
