// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
        Task Dependencies

    AsyncTask
    ArrayView<AsyncTask>
    WeakDep
    StrongDep
    WeakDepArray
    StrongDepArray
    <custom>            - use Scheduler().RegisterDependency< custom >(...)

    Weak dependency     - if task in weak dependency is canceled then dependent task will be executed.
                          Call chain:  task depends on weak,  weak.Cancel(),  weak.OnCancel(),  task.Run().

    Strong dependency   - if task in strong dependency is canceled then dependent task will be canceled.
                          Call chain:  task depends on strong,  strong.Cancel(),  strong.OnCancel(),  task.OnCancel().


        Coroutines

    CoroTask
    bool        co_await Coro_IsCanceled
    EStatus     co_await Coro_Status
    ETaskQueue  co_await Coro_TaskQueue
*/

#pragma once

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
            TaskType    _task;

            explicit _TaskDependency (TaskType &&task)                  __NE___ : _task{ RVRef(task) } {}
            explicit _TaskDependency (const TaskType &task)             __NE___ : _task{ task } {}
            _TaskDependency (_TaskDependency &&)                        __NE___ = default;
            _TaskDependency (const _TaskDependency &)                   __NE___ = default;
        };

        template <typename TaskType, bool IsStrongDep>
        struct _TaskDependencyArray : ArrayView< TaskType >
        {
            _TaskDependencyArray (_TaskDependencyArray &&)              __NE___ = default;
            _TaskDependencyArray (const _TaskDependencyArray &)         __NE___ = default;

            _TaskDependencyArray (TaskType const* ptr, usize count)     __NE___ : ArrayView<TaskType>{ptr, count} {}

            _TaskDependencyArray (std::initializer_list<TaskType> list) __NE___ : ArrayView<TaskType>{list} {}

            template <typename AllocT>
            _TaskDependencyArray (const Array<TaskType,AllocT> &vec)    __NE___ : ArrayView<TaskType>{vec} {}

            template <usize S>
            _TaskDependencyArray (const StaticArray<TaskType,S> &arr)   __NE___ : ArrayView<TaskType>{arr} {}

            template <usize S>
            _TaskDependencyArray (const FixedArray<TaskType,S> &arr)    __NE___ : ArrayView<TaskType>{arr} {}

            template <usize S>
            _TaskDependencyArray (const TaskType (&arr)[S])             __NE___ : ArrayView<TaskType>{arr} {}
        };

    } // _hidden_

    using WeakDep           = Threading::_hidden_::_TaskDependency< AsyncTask, false >;
    using StrongDep         = Threading::_hidden_::_TaskDependency< AsyncTask, true >;

    using WeakDepArray      = Threading::_hidden_::_TaskDependencyArray< AsyncTask, false >;
    using StrongDepArray    = Threading::_hidden_::_TaskDependencyArray< AsyncTask, true >;
//-----------------------------------------------------------------------------



    //
    // Async Task interface
    //
    class alignas(AE_CACHE_LINE) IAsyncTask : public EnableRC< IAsyncTask >
    {
        friend class ITaskDependencyManager;    // can change '_waitBits' and '_canceledDepsCount'
        friend class LfTaskQueue;               // can change '_status'
        friend class TaskScheduler;             // can change '_status'
        friend class IThread;                   // can change '_status'

    // types
    public:
        enum class EStatus : uint
        {
            Initial,
            Pending,        // task has been added to the queue and is waiting until input dependencies complete
            InProgress,     // task was acquired by thread
            Cancellation,   // task required to be canceled
            Continue,       // task will be returned to scheduler

            _Finished,
            Completed,      // successfully completed

            _Interrupted,
            Canceled,       // task was externally canceled
            Failed,         // task has internal error and has been failed
        };

        struct TaskDependency
        {
            ubyte   bitIndex : 7;   // to reset bit in '_waitBits'
            ubyte   isStrong : 1;   // to increment '_canceledDepsCount'

            TaskDependency ()                                   __NE___ : bitIndex{0x7F}, isStrong{0} {}
            explicit TaskDependency (uint index, bool strong)   __NE___ : bitIndex{ubyte(index)}, isStrong{strong} {}
        };

    private:
        static constexpr uint   ElemInChunk = 12;

        struct OutputChunk
        {
        // variables
            OutputChunk *                               next        = null;
            uint                                        selfIndex   = UMax;
            uint                                        count       = 0;
            StaticArray< AsyncTask, ElemInChunk >       tasks       {};
            StaticArray< TaskDependency, ElemInChunk >  deps        {};

        // methods
            OutputChunk ()          __NE___ {}

            void  Init (uint idx)   __NE___;
        };

        STATIC_ASSERT_64( sizeof(OutputChunk) == 128 );

        using WaitBits_t = ulong;


    // variables
    private:
        ETaskQueue                  _queueType          = ETaskQueue::PerFrame; // packed with atomic counter in 'EnableRC<>'
        Atomic< EStatus >           _status             {EStatus::Initial};
        Atomic< uint >              _canceledDepsCount  {0};                    // > 0 if canceled      // TODO: pack with '_status'
        Atomic< WaitBits_t >        _waitBits           {~WaitBits_t{0}};       // 0 - all complete, otherwise - has uncomplete dependencies

        PtrWithSpinLock< OutputChunk >  _output             {null};

        PROFILE_ONLY(
            RC<ITaskProfiler>       _profiler;
        )
        DEBUG_ONLY(
            Atomic<bool>            _isRunning          {false};
        )


    // methods
    public:
        virtual ~IAsyncTask ()                          __NE___;

        ND_ ETaskQueue  QueueType ()                    C_NE___ { return _queueType; }

        ND_ EStatus     Status ()                       C_NE___ { return _status.load(); }

        ND_ bool        IsInQueue ()                    C_NE___ { return Status() <  EStatus::_Finished; }
        ND_ bool        IsFinished ()                   C_NE___ { return Status() >  EStatus::_Finished; }      // status: Completed / Failed / Canceled
        ND_ bool        IsInterrupted ()                C_NE___ { return Status() >  EStatus::_Interrupted; }   // status: Failed / Canceled
        ND_ bool        IsCompleted ()                  C_NE___ { return Status() == EStatus::Completed; }      // status: Completed

        ND_ virtual StringView  DbgName ()              C_NE___ = 0;

        DEBUG_ONLY( ND_ bool  DbgIsRunning ()           C_NE___ { return _isRunning.load(); })


    protected:
        explicit IAsyncTask (ETaskQueue type)           __NE___;

            // can throw exception
            virtual void  Run ()                        __Th___ = 0;

            virtual void  OnCancel ()                   __NE___ { ASSERT( not _isRunning.load() ); }

            // call this only inside 'Run()' method.
        ND_ bool  IsCanceled () const                   __NE___ { ASSERT( _isRunning.load() );  return Status() == EStatus::Cancellation; }

            // call this only inside 'Run()' method
            void  OnFailure ()                          __NE___;

            // call this only inside 'Run()' method.
            // doesn't restart if have been canceled in another thread.
            // throw exception if failed to add dependencies.
            template <typename ...Deps>
            void  Continue (const Tuple<Deps...> &)     __NE___;
            void  Continue ()                           __NE___ { return Continue( Tuple{} ); }

            // call this before reusing task
        ND_ bool  _ResetState ()                        __NE___;

            // Only for debugging!
            void  _DbgSet (EStatus status)              __NE___;

            // Only during initialization
            void  _SetQueueType (ETaskQueue type)       __NE___;

            // Only in constructor!
            void  _MakeCompleted ()                     __NE___;

            bool  _SetCancellationState ()              __NE___;

    private:
        // call this methods only after 'Run()' method
        void  _OnFinish (OUT bool& rerun)               __NE___;
        void  _Cancel ()                                __NE___;
        void  _FreeOutputChunks (bool isCanceled)       __NE___;

        DEBUG_ONLY( ND_ static slong  _AsyncTaskTotalCount () __NE___;)
    };
//-----------------------------------------------------------------------------



    //
    // Function as Async Task
    //

    class AsyncTaskFn final : public IAsyncTask
    {
    // types
    public:
        using Func_t    = Function< void () >;


    // variables
    private:
        Func_t      _fn;

      #ifdef AE_DEBUG
        String      _dbgName;
      #endif


    // methods
    public:
        template <typename Fn>
        explicit AsyncTaskFn (Fn && fn, StringView dbgName = Default, ETaskQueue type = ETaskQueue::PerFrame) :
            IAsyncTask{ type },
            _fn{ FwdArg<Fn>(fn) }
            #ifdef AE_DEBUG
            , _dbgName{ dbgName }
            #endif
        {
            Unused( dbgName );
        }


    private:
        void  Run ()            __Th_OV
        {
            return _fn();
        }

    #ifdef AE_DEBUG
        StringView  DbgName ()  C_NE_OV { return _dbgName; }
    #else
        StringView  DbgName ()  C_NE_OV { return "AsyncTaskFn"; }
    #endif
    };
//-----------------------------------------------------------------------------



# ifdef AE_HAS_COROUTINE
  namespace _hidden_
  {
    template <typename DepsType>
    class AsyncTaskCoro_Awaiter;


    //
    // Async Task Coroutine
    //
    class AsyncTaskCoro final
    {
    public:
        class promise_type;
        using Handle_t  = std::coroutine_handle< promise_type >;

        //
        // promise_type
        //
        class promise_type final : public IAsyncTask
        {
            friend class AsyncTaskCoro;

        // variables
        private:
          #ifdef AE_DEBUG
            String      _dbgName;
          #endif


        // methods
        public:
            promise_type ()                                 __NE___ : IAsyncTask{ ETaskQueue::PerFrame } {}

            ND_ AsyncTaskCoro       get_return_object ()    __NE___ { return AsyncTaskCoro{ *this }; }

            ND_ std::suspend_always initial_suspend ()      C_NE___ { return {}; }          // delayed start
            ND_ std::suspend_always final_suspend ()        C_NE___ { return {}; }          // must not be 'suspend_never'  // TODO: don't suspend

                void                return_void ()          C_NE___ {}

                void                unhandled_exception ()  C_Th___ { throw; }              // rethrow exceptions

            #ifdef AE_DEBUG
                StringView          DbgName ()              C_NE_OV { return _dbgName; }
            #else
                StringView          DbgName ()              C_NE_OV { return "AsyncTaskCoro"; }
            #endif


        public:
                void  Cancel ()                             __NE___ { Unused( IAsyncTask::_SetCancellationState() ); }
                void  Fail ()                               __NE___ { IAsyncTask::OnFailure(); }
            ND_ bool  IsCanceled ()                         __NE___ { return IAsyncTask::IsCanceled(); }

            template <typename ...Deps>
            void  Continue (const Tuple<Deps...> &deps)     __NE___ { return IAsyncTask::Continue( deps ); }


        private:
            void  Run ()                                    __Th_OV
            {
                auto    coro_handle = Handle_t::from_promise( *this );
                coro_handle.resume();   // throw

                if_unlikely( bool{coro_handle} and not coro_handle.done() )
                    ASSERT( AnyEqual( Status(), EStatus::Cancellation, EStatus::Continue, EStatus::Failed ));
            }

            void  _ReleaseObject ()                         __NE_OV
            {
                MemoryBarrier( EMemoryOrder::Acquire );
                ASSERT( IsFinished() );

                auto    coro_handle = Handle_t::from_promise( *this );

                // internally calls 'promise_type' dtor
                coro_handle.destroy();
            }
        };


    // variables
    private:
        RC<promise_type>    _coro;


    // methods
    public:
        AsyncTaskCoro ()                                    __NE___ {}
        explicit AsyncTaskCoro (promise_type &p)            __NE___ : _coro{ p.GetRC<promise_type>() } {}
        explicit AsyncTaskCoro (Handle_t handle)            __NE___ : _coro{ handle.promise().GetRC<promise_type>() } {}
        ~AsyncTaskCoro ()                                   __NE___ {}

        AsyncTaskCoro (AsyncTaskCoro &&)                    __NE___ = default;
        AsyncTaskCoro (const AsyncTaskCoro &)               __NE___ = default;

        AsyncTaskCoro&  operator = (AsyncTaskCoro &&)       __NE___ = default;
        AsyncTaskCoro&  operator = (const AsyncTaskCoro &)  __NE___ = default;

        operator AsyncTask ()                               C_NE___ { return _coro; }
        explicit operator bool ()                           C_NE___ { return bool{_coro}; }

    private:
        friend class Threading::TaskScheduler;
        void  _InitCoro (ETaskQueue type, StringView name)  __NE___
        {
            _coro->_SetQueueType( type );
            Unused( name );
          #ifdef AE_DEBUG
            _coro->_dbgName = String{name};
          #endif
        }
    };


    //
    // Coroutine Awaiter
    //
    class AsyncTaskCoro_AwaiterImpl
    {
    // methods
    private:
        ND_ forceinline static IAsyncTask::EStatus  _GetStatus (const AsyncTask &task)  __NE___ { return task ? task->Status() : IAsyncTask::EStatus::Canceled; }

        template <typename T>
        ND_ forceinline static IAsyncTask::EStatus  _GetStatus (const T &other)         __NE___ { return _GetStatus( AsyncTask{ other }); }


    public:
        template <typename P>
        ND_ static bool  AwaitSuspendImpl (std::coroutine_handle<P> curCoro, const AsyncTask &dep) __NE___
        {
            // compatible with all 'promise_type' which is inhertited from 'IAsyncTask'
            STATIC_ASSERT( IsBaseOf< IAsyncTask, P >);

            using EStatus = IAsyncTask::EStatus;

            const EStatus   stat = _GetStatus( dep );

            if ( stat == EStatus::Completed )
                return false;   // resume

            if ( stat > EStatus::_Interrupted )
            {
                curCoro.promise().Cancel();
                return true;    // suspend & cancel
            }

            curCoro.promise().Continue( Tuple{ dep });

            // task may be cancelled
            if_unlikely( curCoro.promise().IsFinished() )
                return false;   // resume

            return true;    // suspend
        }


        template <typename P, typename ...Deps>
        ND_ static bool  AwaitSuspendImpl2 (std::coroutine_handle<P> curCoro, const Tuple<Deps...> &deps) __NE___
        {
            // compatible with all 'promise_type' which is inhertited from 'IAsyncTask'
            STATIC_ASSERT( IsBaseOf< IAsyncTask, P >);

            using EStatus = IAsyncTask::EStatus;

            const auto  stats_arr   = deps.Apply( [] (auto&& ...args) { return StaticArray< EStatus, CountOf<Deps...>() >{ _GetStatus( args ) ... }; });
            const auto  stats       = ArrayView<EStatus>{ stats_arr };

            if ( stats.AllEqual( EStatus::Completed ))
                return false;   // resume

            if ( stats.AllGreater( EStatus::_Finished ) and
                 stats.AllGreater( EStatus::_Interrupted ))
            {
                curCoro.promise().Cancel();
                return true;    // suspend & cancel
            }

            curCoro.promise().Continue( deps.Apply( [] (auto&& ...args) { return Tuple{ AsyncTask{args} ... }; }));

            // task may be cancelled
            if_unlikely( curCoro.promise().IsFinished() )
                return false;   // resume

            return true;    // suspend
        }
    };


    template <>
    class AsyncTaskCoro_Awaiter< AsyncTask >
    {
    // variables
    private:
        AsyncTask const&    _dep;


    // methods
    public:
        explicit AsyncTaskCoro_Awaiter (const AsyncTask &dep)           __NE___ : _dep{dep} {}

        ND_ bool    await_ready ()                                      C_NE___ { return false; }

            void    await_resume ()                                     __NE___ {}

        // return task to scheduler with new dependencies
        template <typename P>
        ND_ bool    await_suspend (std::coroutine_handle<P> curCoro)    __NE___
        {
            return AsyncTaskCoro_AwaiterImpl::AwaitSuspendImpl( curCoro, _dep );
        }
    };


    template <typename ...Deps>
    class AsyncTaskCoro_Awaiter< Tuple<Deps...> >
    {
    // variables
    private:
        Tuple<Deps...> const&   _deps;


    // methods
    public:
        explicit AsyncTaskCoro_Awaiter (const Tuple<Deps...> &deps)     __NE___ : _deps{deps} {}

        ND_ bool    await_ready ()                                      C_NE___ { return false; }

            void    await_resume ()                                     __NE___ {}

        // return task to scheduler with new dependencies
        template <typename P>
        ND_ bool    await_suspend (std::coroutine_handle<P> curCoro)    __NE___
        {
            return AsyncTaskCoro_AwaiterImpl::AwaitSuspendImpl2( curCoro, _deps );
        }
    };


    //
    // Coroutine Error (for CHECK_CE)
    //
    class AsyncTaskCoro_Error final
    {
    private:
        struct Awaiter
        {
            ND_ bool    await_ready ()                                  C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
                void    await_resume ()                                 __NE___ {}

            template <typename P>
            ND_ bool    await_suspend (std::coroutine_handle<P> curCoro)__NE___
            {
                // compatible with all 'promise_type' which is inhertited from 'IAsyncTask'
                STATIC_ASSERT( IsBaseOf< IAsyncTask, P >);

                curCoro.promise().Fail();
                return false;   // always resume coroutine
            }
        };

    public:
        explicit AsyncTaskCoro_Error ()                                 __NE___ {}

        ND_ auto  operator co_await ()                                  C_NE___ { return Awaiter{}; }
    };


    //
    // Coroutine Is Canceled
    //
    class Coroutine_IsCanceled final
    {
    private:
        struct Awaiter
        {
        private:
            bool    _isCanceled = false;

        public:
            ND_ bool    await_ready ()                                  C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
            ND_ bool    await_resume ()                                 __NE___ { return _isCanceled; }

            template <typename P>
            ND_ bool    await_suspend (std::coroutine_handle<P> curCoro)__NE___
            {
                // compatible with all 'promise_type' which is inhertited from 'IAsyncTask'
                STATIC_ASSERT( IsBaseOf< IAsyncTask, P >);

                _isCanceled = curCoro.promise().IsCanceled();   
                return false;   // always resume coroutine
            }
        };

    public:
        constexpr Coroutine_IsCanceled ()                               __NE___ {}

        ND_ auto  operator co_await ()                                  C_NE___ { return Awaiter{}; }
    };


    //
    // Coroutine Status
    //
    class Coroutine_Status final
    {
    private:
        struct Awaiter
        {
        private:
            IAsyncTask::EStatus     _status = IAsyncTask::EStatus::Initial;

        public:
            ND_ bool    await_ready ()                                  C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
            ND_ auto    await_resume ()                                 __NE___ { return _status; }

            template <typename P>
            ND_ bool    await_suspend (std::coroutine_handle<P> curCoro)__NE___
            {
                // compatible with all 'promise_type' which is inhertited from 'IAsyncTask'
                STATIC_ASSERT( IsBaseOf< IAsyncTask, P >);

                _status = curCoro.promise().Status();   
                return false;   // always resume coroutine
            }
        };

    public:
        constexpr Coroutine_Status ()                                   __NE___ {}

        ND_ auto  operator co_await ()                                  C_NE___ { return Awaiter{}; }
    };


    //
    // Coroutine Queue
    //
    class Coroutine_Queue final
    {
    private:
        struct Awaiter
        {
        private:
            ETaskQueue      _queue  = ETaskQueue::_Count;

        public:
            ND_ bool    await_ready ()                                  C_NE___ { return false; }   // call 'await_suspend()' to get coroutine handle
            ND_ auto    await_resume ()                                 __NE___ { return _queue; }

            template <typename P>
            ND_ bool    await_suspend (std::coroutine_handle<P> curCoro)__NE___
            {
                // compatible with all 'promise_type' which is inhertited from 'IAsyncTask'
                STATIC_ASSERT( IsBaseOf< IAsyncTask, P >);

                _queue = curCoro.promise().QueueType(); 
                return false;   // always resume coroutine
            }
        };

    public:
        constexpr Coroutine_Queue ()                                    __NE___ {}

        ND_ auto  operator co_await ()                                  C_NE___ { return Awaiter{}; }
    };

  } // _hidden_
//-----------------------------------------------------------------------------



    static constexpr Threading::_hidden_::Coroutine_IsCanceled  Coro_IsCanceled {};
    static constexpr Threading::_hidden_::Coroutine_Status      Coro_Status     {};
    static constexpr Threading::_hidden_::Coroutine_Queue       Coro_TaskQueue  {};

    using CoroTask = Threading::_hidden_::AsyncTaskCoro;

/*
=================================================
    operator co_await (task dependencies)
----
    it is safe to use reference because 'AsyncTaskCoro_Awaiter' object
    destroyed before 'Tuple' destruction.
=================================================
*/
    ND_ inline auto  operator co_await (const AsyncTask &dep) __NE___
    {
        return Threading::_hidden_::AsyncTaskCoro_Awaiter< AsyncTask >{ dep };
    }

    template <typename ...Deps>
    ND_ auto  operator co_await (const Tuple<Deps...> &deps) __NE___
    {
        return Threading::_hidden_::AsyncTaskCoro_Awaiter< Tuple<Deps...> >{ deps };
    }

# endif // AE_HAS_COROUTINE
//-----------------------------------------------------------------------------

} // AE::Threading
