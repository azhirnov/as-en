// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Platforms/Platform.h"

namespace AE::Base
{

    bool  ThreadUtils::_NanoSleep (nanoseconds relativeTime)                    __NE___ { return PlatformUtils::NanoSleep( relativeTime ); }

    void  ThreadUtils::SetName (NtStringView name)                              __NE___ { return PlatformUtils::SetCurrentThreadName( name ); }

    String  ThreadUtils::GetName ()                                             __Th___ { return PlatformUtils::GetCurrentThreadName(); }

    bool  ThreadUtils::_WaitIOms (milliseconds relativeTime)                    __NE___ { return PlatformUtils::WaitIO( relativeTime ); }

    ThreadHandle    ThreadUtils::GetCurrentThreadHandle ()                      __NE___ { return PlatformUtils::GetCurrentThreadHandle(); }
    bool            ThreadUtils::IsCurrentThread (const ThreadHandle &handle)   __NE___ { return handle == PlatformUtils::GetCurrentThreadHandle(); }

    bool  ThreadUtils::SetAffinity (const ThreadHandle &handle, uint coreIdx)   __NE___ { return PlatformUtils::SetThreadAffinity( handle, coreIdx ); }
    bool  ThreadUtils::SetPriority (const ThreadHandle &handle, float priority) __NE___ { return PlatformUtils::SetThreadPriority( handle, priority ); }

    bool  ThreadUtils::SetAffinity (uint coreIdx)                               __NE___ { return PlatformUtils::SetCurrentThreadAffinity( coreIdx ); }
    bool  ThreadUtils::SetPriority (float priority)                             __NE___ { return PlatformUtils::SetCurrentThreadPriority( priority ); }

    uint  ThreadUtils::GetCoreIndex ()                                          __NE___ { return PlatformUtils::GetProcessorCoreIndex(); }

    bool  ThreadUtils::Yield ()                                                 __NE___ { return PlatformUtils::ThreadYield(); }

    void  ThreadUtils::Pause ()                                                 __NE___ { return PlatformUtils::ThreadPause(); }


} // AE::Base
