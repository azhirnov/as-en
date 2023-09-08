// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_LINUX
# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <sys/prctl.h>
# include <pthread.h>
# include <sched.h>

#include <sys/utsname.h>

# include "base/Platforms/LinuxUtils.h"
# include "base/Algorithms/ArrayUtils.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
    SecureZeroMem
=================================================
*/
    void  SecureZeroMem (OUT void* ptr, Bytes size) __NE___
    {
        ::explicit_bzero( ptr, usize(size) );
    }

/*
=================================================
    SetCurrentThreadName
=================================================
*/
    void  LinuxUtils::SetCurrentThreadName (NtStringView name) __NE___
    {
        StaticLogger::SetCurrentThreadName( StringView{name} );

        ASSERT( name.length() <= 16 );
        int res = ::prctl( PR_SET_NAME, (unsigned long) name.c_str(), 0, 0, 0 );
        ASSERT( res == 0 );
    }

/*
=================================================
    GetCurrentThreadName
=================================================
*/
    String  LinuxUtils::GetCurrentThreadName ()
    {
        char    buf [16];
        int     res = ::prctl( PR_GET_NAME, buf, 0, 0, 0 );
        ASSERT( res == 0 );
        return String{buf};
    }

/*
=================================================
    GetCurrentThreadHandle
=================================================
*/
    ThreadHandle  LinuxUtils::GetCurrentThreadHandle () __NE___
    {
        return ::pthread_self();
    }

/*
=================================================
    SetThreadAffinity
=================================================
*/
    bool  LinuxUtils::SetThreadAffinity (const ThreadHandle &handle, uint coreIdx) __NE___
    {
        ASSERT_Lt( coreIdx, std::thread::hardware_concurrency() );

        cpu_set_t cpuset;
        CPU_ZERO( &cpuset );
        CPU_SET( coreIdx, &cpuset );
        return ::pthread_setaffinity_np( handle, sizeof(cpu_set_t), &cpuset ) == 0;
    }

    bool  LinuxUtils::SetCurrentThreadAffinity (uint coreIdx) __NE___
    {
        ASSERT_Lt( coreIdx, std::thread::hardware_concurrency() );

        ::cpu_set_t  mask;
        CPU_ZERO( OUT &mask );
        CPU_SET( coreIdx, INOUT &mask );

        return ::sched_setaffinity( 0, sizeof(mask), &mask ) == 0;
    }

/*
=================================================
    SetThreadPriority
=================================================
*/
    bool  LinuxUtils::SetThreadPriority (const ThreadHandle &handle, float priority) __NE___
    {
        // TODO:
        //  pthread_setschedprio
        //  https://pubs.opengroup.org/onlinepubs/007904875/functions/xsh_chap02_08.html#tag_02_08_04_01
        Unused( handle, priority );
        return false;
    }

    bool  LinuxUtils::SetCurrentThreadPriority (float priority) __NE___
    {
        // TODO
        Unused( priority );
        return false;
    }

/*
=================================================
    GetProcessorCoreIndex
=================================================
*/
    uint  LinuxUtils::GetProcessorCoreIndex () __NE___
    {
        return ::sched_getcpu();
    }

/*
=================================================
    ThreadPause
=================================================
*/
    void  LinuxUtils::ThreadPause () __NE___
    {
    #if defined(AE_CPU_ARCH_X86)
        #if AE_SIMD_SSE >= 2
            _mm_pause();    // requires SSE2
        #endif
    #elif defined(AE_CPU_ARCH_X64)
        _mm_pause();        // SSE2 always supported on x64
        // __builtin_ia32_pause 

    #elif defined(AE_CPU_ARCH_ARM32) or defined(AE_CPU_ARCH_ARM64)
        //__builtin_arm_yield();
        __yield();
        //asm volatile("yield")

    #else
        // TODO
    #endif
    }

    // TODO:
    //  pthread_getcpuclockid() - check (allow to get frequency per core?)
    // https://gist.github.com/stevedoyle/1319053
    // https://android.stackexchange.com/questions/19810/how-can-i-determine-max-cpu-speed-at-runtime
    // http://www.java2s.com/Code/Android/Hardware/GetCPUFrequencyCurrent.htm
    // https://stackoverflow.com/questions/3021054/how-to-read-cpu-frequency-on-android-device
    //  all code also valid for MacOS/iOS/Android

/*
=================================================
    GetOSName
=================================================
*/
# ifndef AE_RELEASE
    String  LinuxUtils::GetOSName () __NE___
    {
        utsname     os_info = {};
        CHECK_ERR( ::uname( OUT &os_info ) == 0 );

        CATCH_ERR(
            String  result;
            result  << os_info.sysname << ' '
                    << os_info.nodename << ' '
                    << os_info.release << ' '
                    << os_info.version;
            return result;
        )
    }
# endif

/*
=================================================
    GetOSVersion
=================================================
*/
    Version3  LinuxUtils::GetOSVersion () __NE___
    {
        //utsname       os_info = {};
        //CHECK_ERR( ::uname( OUT &os_info ) == 0 );

        // TODO
        return Version3{};
    }


} // AE::Base

#endif // AE_PLATFORM_LINUX
