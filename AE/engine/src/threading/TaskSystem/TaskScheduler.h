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
        - If can't create task or add to queue                      -> return default cancelled task
        - If null task used as dependency                           -> ignore
        - If null coroutine used as dependency (co_await noop_coro) -> cancel coroutine

    [docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/TaskScheduler-ru.md)
*/

#pragma once

#include "threading/TaskSystem/AsyncTask.h"
#include "threading/TaskSystem/Coroutine.h"
#include "threading/Containers/LfIndexedPool.h"
#include "threading/Memory/GlobalLinearAllocator.h"

namespace AE::Threading { class TaskScheduler; }
namespace AE
{
    Threading::TaskScheduler&  Scheduler () __NE___;

#ifdef AE_DEBUG
    static constexpr AE::Base::minutes      DefaultTimeout  {60};   // 60 min - for debugging
#else
    static constexpr AE::Base::milliseconds DefaultTimeout  {500};  // 0.5 sec
#endif
}

namespace AE::Threading
{
    enum class EThreadSeed : usize {};
    enum class ECpuCoreId  : ubyte  { Unknown = 0xFF };


    //
    // Thread interface
    //
    class IThread : public EnableRC< IThread >
    {
    // types
    public:
        struct ProfilingInfo
        {
            StringView      threadName;
            StringView      coreName;
            usize           threadId    = 0;
            ECpuCoreId      coreId      = ECpuCoreId(0);
            uint            curFreq     = 0;    // MHz
            uint            minFreq     = 0;    // MHz
            uint            maxFreq     = 0;    // MHz
        };


    // interface
    public:
            virtual bool  Attach (uint uid, ECpuCoreId coreId)  __NE___ = 0;
            virtual void  Detach ()                             __NE___ = 0;

        ND_ virtual usize           DbgID ()                    C_NE___ = 0;
        ND_ virtual ProfilingInfo   GetProfilingInfo ()         C_NE___ = 0;
    };
//-----------------------------------------------------------------------------



    //
    // Task Dependency Manager interface
    //
    class ITaskDependencyManager : public EnableRC< ITaskDependencyManager >
    {
    // types
    public:
        using CheckDepFn_t  = Function< void (StringView, AsyncTask, IAsyncTask::TaskDependency) >;


    // interface
    public:
        // returns 'true' if added dependency to task.
        // returns 'false' if dependency is cancelled or on error.
        ND_ virtual bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)   __NE___ = 0;

        // only for debugging
        DEBUG_ONLY(
            virtual void  DbgDetectDeadlock (const CheckDepFn_t &)                          __NE___ {};)

        // helper functions
        static void  _SetDependencyCompletionStatus (const AsyncTask &task, uint depIndex, bool cancel = false) __NE___;
    };
//-----------------------------------------------------------------------------



    enum class EIOServiceType : uint
    {
        File,   // async file IO
    };


    //
    // Input/Output Service
    //
    class IOService : public EnableRC< IOService >
    {
    // interface
    public:
        // returns number of processed events.
        //      thread-safe: yes
        ND_ virtual usize           ProcessEvents ()    __NE___ = 0;

        ND_ virtual EIOServiceType  GetIOServiceType () C_NE___ = 0;
    };
//-----------------------------------------------------------------------------



    //
    // Task Scheduler
    //
    class TaskScheduler final : public Noncopyable
    {
        friend class IAsyncTask;    // calls '_AddDependencies()', '_GetChunkPool()'
        friend struct InPlace<TaskScheduler>;

    // types
    public:
        struct Config
        {
            ubyte       maxPerFrameQueues   = 2;
            ubyte       maxBackgroundQueues = 2;
            ubyte       maxRenderQueues     = 2;
            ubyte       maxIOAccessThreads  = 0;
            ECpuCoreId  mainThreadCoreId    = Default;
        };

        class InstanceCtor {
        public:
            static void  Create ()  __NE___;
            static void  Destroy () __NE___;
        };

        using LoopingFlag_t = Atomic<uint>;
        using TimePoint_t   = std::chrono::high_resolution_clock::time_point;


        struct ThreadWakeup : Noncopyable
        {
        // variables
        private:
            ConditionVariable   _cv;
            Mutex               _mutex;
            EThreadBits         _activeThreads;

        // methods
        public:
            ThreadWakeup ()                                         __NE___ {}

            void  Wakeup (ETaskQueueBits)                           __NE___;
            void  Wakeup (EThreadBits)                              __NE___;

            void  Wakeup (ETaskQueue type)                          __NE___ { Wakeup( ETaskQueueBits{ type }); }
            void  Wakeup (EThread type)                             __NE___ { Wakeup( EThreadBits{ type }); }

            void  WakeupAndDetach (LoopingFlag_t &)                 __NE___;

            void  Suspend (ETaskQueueBits, LoopingFlag_t &)         __NE___;
            void  Suspend (EThreadBits, LoopingFlag_t &)            __NE___;
            void  Suspend (const EThreadArray &, LoopingFlag_t &)   __NE___;
        };

    private:
        using EStatus           = IAsyncTask::EStatus;
        using OutputChunk_t     = IAsyncTask::OutputChunk;
        using WaitBits_t        = IAsyncTask::WaitBits_t;

        struct PerQueue
        {
            Unique<class LfTaskQueue>   ptr;

            DEBUG_ONLY( ulong           totalProcessed  = 0;)
        };

        using TaskQueues_t      = StaticArray< PerQueue, uint(ETaskQueue::_Count) >;
        using TaskDepsMngr_t    = FlatHashMap< TypeId, RC<ITaskDependencyManager> >;
        using OutputChunkPool_t = LfIndexedPool< IAsyncTask::OutputChunk, uint, 64*64, 64, GlobalLinearAllocatorRef >;

        class _CanceledTask;
        class _WaitAsyncTask;
        class _DummyRequest;


    // variables
    private:
      #if AE_USE_THREAD_WAKEUP
        ThreadWakeup        _wakeup;
      #endif
        TaskQueues_t        _queues;

        AsyncTask           _canceledTask;          //                  readonly
        RC<>                _cancelledRequest;      // (AsyncDSRequest) readonly

        OutputChunkPool_t   _chunkPool;

        SharedMutex         _taskDepsMngrsGuard;    // TODO: init on start, remove lock
        TaskDepsMngr_t      _taskDepsMngrs;

        Mutex               _threadGuard;
        Array<RC<IThread>>  _threads;
        RC<IThread>         _mainThread;

        RC<IOService>       _fileIOService;

        PROFILE_ONLY(
            AtomicRC<ITaskProfiler> _profiler;
        )
        DEBUG_ONLY( struct{
            StructAtomic<TimePoint_t>   lastUpdate;
            Atomic<ulong>               numChecks   {0};
            Atomic<ulong>               numLocks    {0};
            const secondsf              interval    {10.f};
            const double                minRate     {0.01};
        }                           _deadlockCheck;)


    // methods
    public:
        ND_ bool  Setup (const Config &cfg)                                         __NE___;
            void  SetProfiler (RC<ITaskProfiler> profiler)                          __NE___;
            void  Release ()                                                        __NE___;

            template <typename T>
            bool  RegisterDependency (RC<ITaskDependencyManager> mngr)              __NE___;

            template <typename T>
            bool  UnregisterDependency ()                                           __NE___;


    // thread api //
            bool  AddThread (RC<IThread> thread, ECpuCoreId coreId = Default)       __NE___;

            bool  ProcessTask (ETaskQueue type, EThreadSeed seed)                   __NE___;
            bool  ProcessTasks (const EThreadArray &threads, EThreadSeed seed)      __NE___;
            bool  ProcessTasks (const EThreadArray &threads, EThreadSeed seed,
                                uint maxTasks)                                      __NE___;
            bool  ProcessFileIO ()                                                  __NE___;

        ND_ AsyncTask  PullTask (ETaskQueue type, EThreadSeed seed)                 __NE___;

            void  SuspendThread (const EThreadArray &,
                                 LoopingFlag_t      &,
                                 uint               iteration)                      __NE___;
            void  WakeupAndDetach (LoopingFlag_t &)                                 __NE___;

      #if AE_USE_THREAD_WAKEUP
        ND_ Ptr<ThreadWakeup>   GetThreadWakeup ()                                  __NE___ { return &_wakeup; }
      #endif
        ND_ static EThreadSeed  GetDefaultSeed ()                                   __NE___;


    // task api //
        template <typename TaskType,
                  typename ...Ctor,
                  typename ...Deps
                 >
            AsyncTask     Run (Tuple<Ctor...>       &&  ctor = Default,
                               const Tuple<Deps...> &   deps = Default)             __NE___;

        template <typename ...Deps>
            bool          Run (AsyncTask                task,
                               const Tuple<Deps...> &   deps = Default)             __NE___;


      #ifdef AE_HAS_COROUTINE
        template <typename ...Deps>
            AsyncTask     Run (ETaskQueue               queueType,
                               CoroTask                 coro,
                               const Tuple<Deps...> &   deps    = Default,
                               StringView               dbgName = Default)          __NE___;

        template <typename ...Deps>
            AsyncTask     Run (CoroTask                 coro,
                               const Tuple<Deps...> &   deps    = Default)          __NE___;

        template <typename T,
                  typename ...Deps
                 >
        ND_ Coroutine<T>  Run (ETaskQueue               queueType,
                               Coroutine<T>             coro,
                               const Tuple<Deps...> &   deps    = Default,
                               StringView               dbgName = Default)          __NE___;

        template <typename T,
                  typename ...Deps
                 >
        ND_ Coroutine<T>  Run (Coroutine<T>             coro,
                               const Tuple<Deps...> &   deps    = Default)          __NE___;
      #endif // AE_HAS_COROUTINE


            bool  Cancel (const AsyncTask &task)                                    __NE___;

            bool  Enqueue (AsyncTask task)                                          __NE___;


    // synchronizations //
        template <typename ...Deps>
        ND_ AsyncTask   WaitAsync (ETaskQueue queue, const Tuple<Deps...> &deps)    __NE___;

        ND_ bool        Wait (ArrayView<AsyncTask>  tasks,
                              const EThreadArray &  threads,
                              nanoseconds           timeout)                        __NE___;
        ND_ bool        Wait (ArrayView<AsyncTask>  tasks,
                              nanoseconds           timeout)                        __NE___;
        ND_ bool        Wait (ArrayView<AsyncTask>  tasks,
                              const EThreadArray &  threads,
                              TimePoint_t           endTime,
                              uint                  maxTasksPerTick)                __NE___;


    // other //
        ND_ Ptr<IOService>      GetFileIOService ()                                 C_NE___ { return _fileIOService.get(); }

        ND_ AsyncTask           GetCanceledTask ()                                  C_NE___ { return _canceledTask; }
        ND_ RC<>                GetCanceledDSRequest ()                             C_NE___ { return _cancelledRequest; }

        ND_ IThread const*      GetMainThread ()                                    C_NE___ { return _mainThread.get(); }

        friend TaskScheduler&   AE::Scheduler ()                                    __NE___;

        PROFILE_ONLY(
            ND_ RC<ITaskProfiler>   GetProfiler ()                                  __NE___ { return _profiler.load(); }
        )


    // debugging //
            void  DbgDetectDeadlock ()                                              __NE___;


    private:
        TaskScheduler ()                                                            __NE___;
        ~TaskScheduler ()                                                           __NE___;

        ND_ static TaskScheduler&  _Instance ()                                     __NE___;

        ND_ bool  _InitIOServices (const Config &cfg)                               __NE___;

        ND_ bool  _InsertTask (AsyncTask task, uint bitIndex)                       __NE___;

        ND_ OutputChunkPool_t&  _GetChunkPool ()                                    __NE___ { return _chunkPool; }

        ND_ static bool _IsAllComplete (ArrayView<AsyncTask> tasks)                 __NE___;

        template <usize I, typename ...Args>
        ND_ constexpr bool  _AddDependencies (const AsyncTask &task, const Tuple<Args...> &args, INOUT uint &bitIndex)      __NE___;

        template <typename T>
        ND_ bool  _AddCustomDependency (const AsyncTask &task, const T &dep, INOUT uint &bitIndex)                          __NE___;

        ND_ bool  _AddTaskDependencies (const AsyncTask &task, const AsyncTask &deps, Bool isStrong, INOUT uint &bitIndex)  __NE___;
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
        return _taskDepsMngrs.insert_or_assign( TypeIdOf<T>(), RVRef(mngr) ).second;    // should not throw
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
    template <typename TaskType, typename ...Ctor, typename ...Deps>
    AsyncTask  TaskScheduler::Run (Tuple<Ctor...> &&ctorArgs, const Tuple<Deps...> &deps) __NE___
    {
        StaticAssert( IsBaseOf< IAsyncTask, TaskType > );

        uint        bit_index   = 0;
        AsyncTask   task        = ctorArgs.Apply([] (auto&& ...args) __NE___
                                                { return MakeRCNe<TaskType>( FwdArg<decltype(args)>(args)... ); });
        if_unlikely( not task )
            return GetCanceledTask();

        if_unlikely( not _AddDependencies<0>( task, deps, INOUT bit_index ))
        {
            // add task to queue only to call 'OnCancel()'
            task->_SetCancellationState();
            bit_index = 0;  // no dependencies
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

        uint    bit_index = 0;

        if_unlikely( not _AddDependencies<0>( task, deps, INOUT bit_index ))
        {
            // add task to queue only to call 'OnCancel()'
            task->_SetCancellationState();
            bit_index = 0;  // no dependencies
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

        AsyncTask   task = AsyncTask{coro};

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

        AsyncTask   task        = AsyncTask{coro};
        uint        bit_index   = 0;

        ASSERT( task->Status() == EStatus::Initial );

        if_unlikely( not _AddDependencies<0>( task, deps, INOUT bit_index ))
        {
            // add task to queue only to call 'OnCancel()'
            task->_SetCancellationState();
            bit_index = 0;  // no dependencies
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
                    if_unlikely( not _AddTaskDependencies( task, dep, False{"weak"}, INOUT bitIndex )) return false;
                }
            else
            // implicitly it is strong dependency
            if constexpr( IsSpecializationOf< T, RC > and IsBaseOf< IAsyncTask, RemoveRC<T> >) {
                if_unlikely( not _AddTaskDependencies( task, args.template Get<I>(), True{"strong"}, INOUT bitIndex )) return false;
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
        StaticAssert( not IsConst<T> );
        SHAREDLOCK( _taskDepsMngrsGuard );

        auto    iter = _taskDepsMngrs.find( TypeIdOf<T>() );
        CHECK_ERR_MSG( iter != _taskDepsMngrs.end(),
            "Can't find dependency manager for type: "s << TypeNameOf<T>() );

        return iter->second->Resolve( AnyTypeCRef{dep}, task, INOUT bitIndex );
    }

/*
=================================================
    WaitAsync
=================================================
*/
    class TaskScheduler::_WaitAsyncTask final : public IAsyncTask
    {
    public:
        explicit _WaitAsyncTask (ETaskQueue type) __NE___ : IAsyncTask{type} {}

        void        Run ()      __Th_OV {}
        StringView  DbgName ()  C_NE_OV { return "WaitAsync"; }
    };

    template <typename ...Deps>
    AsyncTask  TaskScheduler::WaitAsync (ETaskQueue queue, const Tuple<Deps...> &deps) __NE___
    {
        return Run<_WaitAsyncTask>( Tuple{queue}, deps );
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
        ASSERT( _waitBits.load() == 0 );    // all input dependencies must complete

        if constexpr( CountOf<Deps...>() > 0 )
        {
            // in '_AddDependencies()' current task has been added to the input dependencies
            // and they may remove bits from '_waitBits' at any time
            _waitBits.store( UMax );

            uint    bit_index = 0;
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
            if_unlikely( (expected == EStatus::Cancellation) or (expected > EStatus::_Finished) )
                return;

            // 'CAS' can return 'false' even if expected value is the same as current value in atomic
            ASSERT( expected == EStatus::InProgress );
            ThreadUtils::Pause();
        }
    }

/*
=================================================
    MakeTask
=================================================
*/
    template <typename Fn, typename ...Deps>
    AsyncTask  MakeTask (Fn &&                  fn,
                         const Tuple<Deps...> & dependsOn,
                         StringView             dbgName,
                         ETaskQueue             queueType) __NE___
    {
        auto    task = MakeRC<AsyncTaskFn>( FwdArg<Fn>(fn), dbgName, queueType );
        Scheduler().Run( AsyncTask{task}, dependsOn );
        return RVRef(task);
    }

} // AE::Threading


namespace AE
{
/*
=================================================
    Scheduler
=================================================
*/
    ND_ inline Threading::TaskScheduler&  Scheduler () __NE___
    {
        return Threading::TaskScheduler::_Instance();
    }

} // AE
