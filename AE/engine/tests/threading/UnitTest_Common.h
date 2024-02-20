// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Threading.h"
#include "../shared/UnitTest_Shared.h"
#include "../shared/DebugInstanceCounter.h"

using namespace AE::Threading;

enum class WorkerQueueCount : uint {};
enum class IOThreadCount    : uint {};

static constexpr seconds  c_MaxTimeout {100};


struct LocalTaskScheduler
{
    explicit LocalTaskScheduler (WorkerQueueCount count)
    {
        TaskScheduler::Config   cfg;
        cfg.maxPerFrameQueues   = ubyte(Max( 2u, uint(count) ));
        cfg.mainThreadCoreId    = ECpuCoreId(0);

        TaskScheduler::InstanceCtor::Create();
        TEST( Scheduler().Setup( cfg ));
    }

    explicit LocalTaskScheduler (IOThreadCount count, ECpuCoreId mainThreadCoreId = ECpuCoreId(0))
    {
        TaskScheduler::Config   cfg;
        cfg.maxPerFrameQueues   = 1;
        cfg.maxIOAccessThreads  = ubyte(Max( 1u, uint(count) ));
        cfg.mainThreadCoreId    = mainThreadCoreId;

        TaskScheduler::InstanceCtor::Create();
        TEST( Scheduler().Setup( cfg ));
    }

    ~LocalTaskScheduler ()
    {
        Scheduler().Release();
        TaskScheduler::InstanceCtor::Destroy();
    }

    TaskScheduler* operator -> ()
    {
        return &Scheduler();
    }
};
