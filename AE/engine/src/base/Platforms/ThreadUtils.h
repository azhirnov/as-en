// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/Helpers.h"
#include "base/Utils/Threading.h"
#include "base/Platforms/Platform.h"

namespace AE::Base
{

    //
    // Thread Utils
    //

    struct ThreadUtils : Noninstanceable
    {
        // Name //
            static void     SetName (NtStringView name)                                     __NE___ { return PlatformUtils::SetCurrentThreadName( name ); }
        ND_ static String   GetName ()                                                      __Th___ { return PlatformUtils::GetCurrentThreadName(); }


        // Sleep //
            static void     MilliSleep (milliseconds relativeTime)                          __NE___ { return PlatformUtils::ThreadMilliSleep( relativeTime ); }
            static bool     MicroSleep (nanoseconds relativeTime)                           __NE___ { return PlatformUtils::ThreadMicroSleep( relativeTime ); }
            static void     NanoSleep (nanoseconds relativeTime)                            __NE___ { return PlatformUtils::ThreadNanoSleep( relativeTime ); }

            static void     ProgressiveSleep (uint)                                         __NE___;

            static void     Pause ()                                                        __NE___ { return PlatformUtils::ThreadPause(); }        // keep high CPU frequency
            static void     Sleep_1us ()                                                    __NE___ { return PlatformUtils::ThreadSleep_1us(); }    // ARM64: low power mode
            static void     Sleep_500us ()                                                  __NE___ { return PlatformUtils::ThreadSleep_500us(); }  // All: low power mode
            static void     Sleep_15ms ()                                                   __NE___ { return PlatformUtils::ThreadSleep_15ms(); }   // All: low power mode

        ND_ static constexpr auto  NanoSleepTimeStep ()                                     __NE___ { return PlatformUtils::NanoSleepTimeStep(); }
        ND_ static constexpr auto  MicroSleepTimeStep ()                                    __NE___ { return PlatformUtils::MicroSleepTimeStep(); }
        ND_ static constexpr auto  MilliSleepTimeStep ()                                    __NE___ { return PlatformUtils::MilliSleepTimeStep(); }

        ND_ static constexpr uint  SpinBeforeLock ()                                        __NE___ { return 1'000; }


        // Handle / Affinity / Priority //
        ND_ static auto     GetHandle ()                                                    __NE___ { return PlatformUtils::GetCurrentThreadHandle(); }
        ND_ static bool     IsCurrent (const ThreadHandle &handle)                          __NE___ { return handle == PlatformUtils::GetCurrentThreadHandle(); }

            static bool     SetAffinity (const ThreadHandle &handle, uint coreIdx)          __NE___ { return PlatformUtils::SetThreadAffinity( handle, coreIdx ); }
            static bool     SetPriority (const ThreadHandle &handle, float priority)        __NE___ { return PlatformUtils::SetThreadPriority( handle, priority ); }

            static bool     SetAffinity (uint coreIdx)                                      __NE___ { return PlatformUtils::SetCurrentThreadAffinity( coreIdx ); }
            static bool     SetPriority (float priority)                                    __NE___ { return PlatformUtils::SetCurrentThreadPriority( priority ); }


        // ID //
        ND_ static uint     GetCoreIndex ()                                                 __NE___ { return PlatformUtils::GetProcessorCoreIndex(); }

        ND_ static auto     GetID ()                                                        __NE___ { return std::this_thread::get_id(); }
        ND_ static usize    GetIntID ()                                                     __NE___ { return usize(HashOf( std::this_thread::get_id() )); }

        #ifndef AE_DISABLE_THREADS
        ND_ static auto     GetID (const StdThread &t)                                      __NE___ { return t.get_id(); }
        ND_ static usize    GetIntID (const StdThread &t)                                   __NE___ { return usize(HashOf( t.get_id() )); }
        #endif

        ND_ static uint     MaxThreadCount ()                                               __NE___ { return std::thread::hardware_concurrency(); }
    };


/*
=================================================
    ProgressiveSleep
=================================================
*/
    inline void  ThreadUtils::ProgressiveSleep (const uint iteration) __NE___
    {
        if_likely( iteration < 8 )
        {
            Sleep_1us();
            return;
        }

        // power safe mode
        if_likely( iteration < 16 )
        {
            Sleep_500us();
            return;
        }

        // multiple threads at the same core is rare case
        if ( not PlatformUtils::SwitchToPendingThread() )
            Sleep_15ms();
    }


} // AE::Base
