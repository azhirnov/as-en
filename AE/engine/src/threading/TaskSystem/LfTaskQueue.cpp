// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/TaskSystem/LfTaskQueue.h"

namespace AE::Threading
{

/*
=================================================
    constructor
=================================================
*/
    LfTaskQueue::LfTaskQueue (POTValue seedMask, StringView name) __Th___
    {
        for (auto& chunk : _chunks)
        {
            CHECK( chunk == null );

            chunk = new Chunk{};    // TODO: may throw
        }

        //_seedMask = seedMask;

        Unused( seedMask, name );
        AE_SCHEDULER_PROFILING(
            _name   = name;
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
            Chunk*  chunk_ptr = chunk;
            chunk = null;

            for (; chunk_ptr != null;)
            {
                const PackedBits    prev = chunk_ptr->packed.exchange( PackedBits{}.Lock() );

                // chunk must be unlocked and empty
                CHECK( not prev.IsLocked() );
                CHECK( prev.pack.count == 0 );

                for (usize i = 0, cnt = prev.pack.count; i < cnt; ++i)
                {
                    chunk_ptr->array[i].~AsyncTask();
                }

                Chunk*  next = chunk_ptr->next.exchange( null );

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
    bool  LfTaskQueue::Process (usize seed) __NE___
    {
        if_likely( AsyncTask task = Pull( seed ))
        {
            AE_SCHEDULER_PROFILING(
                const auto  start_time = TimePoint_t::clock::now();
            )

            DEBUG_ONLY( task->_isRunning.store( true ));
            PROFILE_ONLY(
                if ( task->_profiler )
                    task->_profiler->Begin( *task );
            )
            //AE_LOG_DBG( "begin: "s << task->DbgName() );

            try {
                task->Run();    // throw
            }
            catch(...) {
                task->_SetCancellationState();
            }

            DEBUG_ONLY( task->_isRunning.store( false ));
            PROFILE_ONLY(
                if ( task->_profiler )
                    task->_profiler->End( *task );
            )

            bool    rerun = false;
            task->_OnFinish( OUT rerun );   // TODO

            AE_SCHEDULER_PROFILING(
                _workTime += (TimePoint_t::clock::now() - start_time).count();
            )
            //AE_LOG_DBG( "--end: "s << task->DbgName() );

            if_unlikely( rerun )
            {
                Scheduler().Enqueue( RVRef(task) );     // TODO: check error
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
    forceinline bool  LfTaskQueue::_RemoveTask (TaskArr_t& arr, INOUT usize& pos, INOUT usize& count, OUT AsyncTask& task) __NE___
    {
        ASSERT( pos < count );
        ASSERT( task == null );

        auto&       curr        = arr[ pos ];
        const uint  canceled    = curr->_canceledDepsCount.load();

        // check input dependencies
        if_unlikely( (canceled == 0) & (curr->_waitBits.load() != 0) )
        {
            // input dependencies is not complete
            return true;
        }

        // remove task
        task.Swap( curr );

        if ( --count > 0 )
            curr.Swap( arr[ count ]);

        // try to start task
        EStatus     status = task->Status();
        if_likely( ((status == EStatus::Pending) &
                    (canceled == 0))            and
                   task->_status.compare_exchange_strong( INOUT status, EStatus::InProgress ))
        {
            return false;   // stop search
        }

        // task was canceled
        if_unlikely( (status == EStatus::Cancellation) | (canceled > 0) )
        {
            // TODO: cancel used with locked chunk - bad for performance
            // TODO: check if task has overloaded 'OnCancel()' method, or add task to queue inside '_Cancel()' ?
            task->_Cancel();
        }
        else
        // task was or will be run in another thread
        {
            ASSERT( status == EStatus::InProgress or
                    status >  EStatus::_Finished );
        }

        task = null;

        return true;    // continue search
    }

/*
=================================================
    Pull
=================================================
*/
    AsyncTask  LfTaskQueue::Pull (usize seed) __NE___
    {
        AE_SCHEDULER_PROFILING(
            const auto  start_time = TimePoint_t::clock::now();
        )

        for (usize j = 0; j < MaxChunks; ++j)
        {
            Chunk*  chunk_ptr = _chunks[ (j + seed) % MaxChunks ];

            for (; chunk_ptr != null; chunk_ptr = chunk_ptr->next.load())
            {
                PackedBits  packed  = chunk_ptr->packed.load();

                if ( packed.pack.count == 0 )
                    continue;

                // try to acquire spinlock
                bool    locked  = false;
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

                const PackedBits    old_packed  = packed.Lock();
                usize               count       = packed.pack.count;
                usize               pos         = packed.pack.pos;
                AsyncTask           task;

                for (; (pos < count) and _RemoveTask( chunk_ptr->array, pos, count, OUT task ); ++pos) {}

                if ( task == null )
                {
                    const usize     start_pos = Min( packed.pack.pos, count );
                    for (pos = 0; ((pos < start_pos) & (pos < count)) and _RemoveTask( chunk_ptr->array, pos, count, OUT task ); ++pos) {}
                }

                packed.pack.pos     = pos;
                packed.pack.count   = count;

                const bool  array_changed   = (old_packed.pack.count != count);
                const auto  order           = 
                    (array_changed and task != null) ?  EMemoryOrder::AcquireRelease :  // both
                    array_changed                    ?  EMemoryOrder::Release :         // flush changes in 'Chunk::array'
                                                        EMemoryOrder::Relaxed;

                // unlock
                const PackedBits    prev_packed = chunk_ptr->packed.exchange( packed, order );
                CHECK( old_packed == prev_packed );

                AE_SCHEDULER_PROFILING(
                    _taskCount.Sub( old_packed.pack.count - packed.pack.count );
                    _totalProcessed.fetch_add( old_packed.pack.count - packed.pack.count );
                )

                if ( task != null )
                {
                    AE_SCHEDULER_PROFILING(
                        _searchTime += (TimePoint_t::clock::now() - start_time).count();
                    )
                    return task;
                }
            }
        }

        AE_SCHEDULER_PROFILING(
            _searchTime += (TimePoint_t::clock::now() - start_time).count();
        )
        return null;
    }

/*
=================================================
    Add
----
    TODO: insert tasks without dependencies to the front,
        with dependencies - to the back
=================================================
*/
    void  LfTaskQueue::Add (AsyncTask task, usize seed) __NE___
    {
        ASSERT( task != null );

        AE_SCHEDULER_PROFILING(
            const auto  start_time = TimePoint_t::clock::now();
        )

        for (;;)
        {
            for (usize j = 0; j < MaxChunks; ++j)
            {
                Chunk*  chunk_ptr = _chunks[ (j + seed) % MaxChunks ];

                for (uint depth = 0; chunk_ptr != null; ++depth)
                {
                    // try to acquire spinlock
                    PackedBits  packed  = chunk_ptr->packed.load();
                    bool        locked  = false;

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
                        const PackedBits    old_packed  = packed.Lock();

                        // insert task
                        if_likely( packed.pack.count < chunk_ptr->array.size() )
                        {
                            chunk_ptr->array[ packed.pack.count ].Swap( task );
                            ++packed.pack.count;

                            // flush changes in 'Chunk::array'
                            MemoryBarrier( EMemoryOrder::Release );
                        }

                        // unlock
                        const PackedBits    prev_packed = chunk_ptr->packed.exchange( packed );
                        CHECK( old_packed == prev_packed );

                        // if inserted
                        if_likely( old_packed.pack.count != packed.pack.count )
                        {
                            AE_SCHEDULER_PROFILING(
                                _insertionTime += (TimePoint_t::clock::now() - start_time).count();

                                _maxTasks.Max( _taskCount.Add( 1 ));
                            )
                            return;
                        }
                    }

                    Chunk*  next = chunk_ptr->next.load();

                    // add new chunk
                    if_unlikely( next == null )
                    {
                        if ( depth >= MaxDepth )
                            break;

                        next = new Chunk{};

                        for (Chunk* exp_chunk = null;;)
                        {
                            if ( chunk_ptr->next.CAS( INOUT exp_chunk, next ))
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

            //AE_LOGI( "task queue overflow" );
            ThreadUtils::YieldOrSleep();

            // TODO: wait for event or sleep
        }
    }

/*
=================================================
    WriteProfilerStat
=================================================
*/
    void  LfTaskQueue::WriteProfilerStat () __NE___
    {
        AE_SCHEDULER_PROFILING(
            auto    work_time   = _workTime.exchange( 0 );
            auto    search_time = _searchTime.exchange( 0 );
            auto    insert_time = _insertionTime.exchange( 0 );
            auto    max_tasks   = _maxTasks.exchange( 0 );

            if ( (work_time == 0 and search_time == 0) or max_tasks == 0 )
                return;

            double  work        = double(work_time);
            double  search      = (work_time ? double(search_time) /  work : 1.0);
            double  insertion   = (work_time ? double(insert_time) /  work : 1.0);

            AE_LOGI( _name
                << " queue total work: " << ToString( nanoseconds(work_time) )
                << ", search: " << ToString( search * 100.0, 2 ) << " %"
                << ", insertion: " << ToString( insertion * 100.0, 2 ) << " %"
                << ", max tasks: " << ToString( max_tasks ));
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
            Chunk*  chunk_ptr = _chunks[j];

            for (; chunk_ptr != null; chunk_ptr = chunk_ptr->next.load())
            {
                PackedBits  packed  = chunk_ptr->packed.load();

                if ( packed.pack.count == 0 )
                    continue;

                // lock spinlock
                for (uint i = 0;; ++i)
                {
                    // expect in unlocked state
                    packed.pack.locked = 0;

                    if_likely( chunk_ptr->packed.CAS( INOUT packed, packed.Lock() ))
                        break;

                    if_unlikely( i > ThreadUtils::SpinBeforeLock() )
                    {
                        i = 0;
                        ThreadUtils::YieldOrSleep();
                    }

                    ThreadUtils::Pause();
                }

                // load changes in 'Chunk::array'
                MemoryBarrier( EMemoryOrder::Acquire );
                ASSERT( not packed.IsLocked() );

                const PackedBits    old_packed  = packed.Lock();
                const usize         count       = packed.pack.count;

                for (usize i = 0; i < count; ++i)
                {
                    fn( chunk_ptr->array[i] );
                }


                // unlock
                const PackedBits    prev_packed = chunk_ptr->packed.exchange( packed );
                CHECK( old_packed == prev_packed );
            }
        }
    }

#endif // AE_DEBUG

} // AE::Threading
