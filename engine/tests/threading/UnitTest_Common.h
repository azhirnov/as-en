// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Threading.h"
#include "../shared/UnitTest_Shared.h"
#include "../shared/DebugInstanceCounter.h"

using namespace AE::Threading;

enum class WorkerQueueCount : uint {};
enum class IOThreadCount    : uint {};


struct LocalTaskScheduler
{
    explicit LocalTaskScheduler (WorkerQueueCount)
    {
        TaskScheduler::Config   cfg;
        cfg.maxPerFrameQueues   = 2;

        TaskScheduler::CreateInstance();
        TEST( Scheduler().Setup( cfg ));
    }

    explicit LocalTaskScheduler (IOThreadCount count)
    {
        TaskScheduler::Config   cfg;
        cfg.maxPerFrameQueues   = 1;
        cfg.maxIOThreads        = ubyte(count);

        TaskScheduler::CreateInstance();
        TEST( Scheduler().Setup( cfg ));
    }

    ~LocalTaskScheduler ()
    {
        Scheduler().Release();
        TaskScheduler::DestroyInstance();
    }

    TaskScheduler* operator -> ()
    {
        return &Scheduler();
    }
};
