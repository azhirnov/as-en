// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

    //
    // Thread Wakeup
    //
    struct ThreadWakeup
    {
    // types
    public:
        using LoopingFlag_t     = Atomic<uint>;


    // variables
    private:
        ConditionVariable   _cv;
        Mutex               _mutex;
        EThreadBits         _activeThreads;


    // methods
    public:
        ThreadWakeup ()                                             __NE___ {}

        void  Wakeup (ETaskQueueBits)                               __NE___;
        void  Wakeup (EThreadBits)                                  __NE___;

        void  Wakeup (ETaskQueue type)                              __NE___ { Wakeup( ETaskQueueBits{ type }); }
        void  Wakeup (EThread type)                                 __NE___ { Wakeup( EThreadBits{ type }); }

        void  WakeupAndDetach (LoopingFlag_t &)                     __NE___;

        void  Suspend (ETaskQueueBits, LoopingFlag_t &)             __NE___;
        void  Suspend (EThreadBits, LoopingFlag_t &)                __NE___;
        void  Suspend (const EThreadArray &, LoopingFlag_t &)       __NE___;
    };



    //
    // Thread Manager
    //
    class ThreadMngr final : public Noninstanceable
    {
    // types
    public:

        //
        // Thread Config
        //
        struct ThreadConfig
        {
        // variables
            EThreadArray            threads;
            Ptr<ThreadWakeup>       wakeup; 
            FixedString<64>         name;

        // methods
            ThreadConfig ()                                                                         __NE___ {}
            ThreadConfig (ThreadConfig &&)                                                          __NE___ = default;
            ThreadConfig (const ThreadConfig &)                                                     __NE___ = default;

            ThreadConfig (const EThreadArray &threads, StringView name)                             __NE___ : threads{threads},  name{name} {}
            ThreadConfig (const EThreadArray &threads, Ptr<ThreadWakeup> wakeup, StringView name)   __NE___ : threads{threads},  wakeup{wakeup},  name{name} {}

            ND_ static ThreadConfig  CreateNonSleep ()                                              __NE___ { return ThreadConfig( EThreadArray{ ETaskQueue::PerFrame }, "worker" ); }
        };


    // methods
    public:
        ND_ static RC<IThread>  CreateThread (const ThreadConfig &cfg)                          __NE___;

        ND_ static bool  SetupThreads (const TaskScheduler::Config  &cfg,
                                       EnumBitSet<EThread>           mask,
                                       Ptr<ThreadWakeup>             wakeup,            
                                       uint                          maxThreads,
                                       OUT EThreadArray             &allowProcessInMain)        __NE___;

    private:
        friend class TaskScheduler;
        ND_ static RC<IThread>  _CreateMainThread ()                                            __NE___;

        ND_ static bool  _SetupThreads_v1 (TaskScheduler                &scheduler,
                                           const EnumBitSet<EThread>     mask,
                                           OUT EThreadArray             &allowProcessInMain)    __NE___;

        ND_ static bool  _SetupThreads_v2 (TaskScheduler                &scheduler,
                                           const CpuArchInfo            &cpuInfo,
                                           const EnumBitSet<EThread>     mask,
                                           Ptr<ThreadWakeup>             wakeup,
                                           const uint                    maxThreads,
                                           OUT EThreadArray             &allowProcessInMain)    __NE___;
    };


} // AE::Threading
