// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: yes
*/

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{

    //
    // Task Profiler interface
    //

    class ITaskProfiler : public EnableRC<ITaskProfiler>
    {
    // interface
    public:
        // Before task execution in current thread.
        virtual void  Begin (const IAsyncTask &)                        __NE___ = 0;

        // After task execution in current thread.
        virtual void  End (const IAsyncTask &)                          __NE___ = 0;

        // Task added to the queue at first time or when rerunning, when used 'Continue()'.
        virtual void  Enqueue (const IAsyncTask &)                      __NE___ = 0;

        // When new thread are added to task scheduler.
        virtual void  AddThread (RC<IThread>)                           __NE___ = 0;

        // Used for work outside of task.
        virtual void  BeginNonTaskWork (const void *id, StringView name)__NE___ = 0;
        virtual void  EndNonTaskWork (const void *id, StringView name)  __NE___ = 0;
    };


} // AE::Threading
