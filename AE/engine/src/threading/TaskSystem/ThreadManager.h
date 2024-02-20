// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

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
            FixedString<64>         name;

        // methods
            ThreadConfig (ThreadConfig &&)                              __NE___ = default;
            ThreadConfig (const ThreadConfig &)                         __NE___ = default;
            ThreadConfig (const EThreadArray &threads, StringView name) __NE___ : threads{threads},  name{name} {}
            ThreadConfig ()                                             __NE___ : ThreadConfig{ EThreadArray{ ETaskQueue::PerFrame }, "worker" } {}
        };


    // methods
    public:
        ND_ static RC<IThread>  CreateThread (const ThreadConfig &cfg)                          __NE___;

        ND_ static bool  SetupThreads (const TaskScheduler::Config  &cfg,
                                       EnumSet<EThread>              mask,
                                       uint                          maxThreads,
                                       Bool                          bindThreadToPhysicalCore,
                                       OUT EThreadArray             &allowProcessInMain)        __NE___;

    private:
        friend class TaskScheduler;
        ND_ static RC<IThread>  _CreateMainThread ()                                            __NE___;

        ND_ static bool  _SetupThreads_v1 (TaskScheduler::Config     cfg,
                                           const CpuArchInfo        &cpuInfo,
                                           const EnumSet<EThread>    mask,
                                           const uint                maxThreads,
                                           OUT EThreadArray         &allowProcessInMain)        __NE___;

        ND_ static bool  _SetupThreads_v2 (TaskScheduler::Config     cfg,
                                           const CpuArchInfo        &cpuInfo,
                                           const EnumSet<EThread>    mask,
                                           const uint                maxThreads,
                                           bool                      bindThreadToPhysicalCore,
                                           OUT EThreadArray         &allowProcessInMain)        __NE___;
    };


} // AE::Threading
