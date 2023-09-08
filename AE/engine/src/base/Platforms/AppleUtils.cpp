// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS)

# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <pthread.h>
# include <sched.h>

# include "base/Platforms/AppleUtils.h"
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
        return Base::_hidden_::SecureZeroMemFallback( OUT ptr, size );
    }

/*
=================================================
    SetCurrentThreadName
=================================================
*/
    void  AppleUtils::SetCurrentThreadName (NtStringView name) __NE___
    {
        StaticLogger::SetCurrentThreadName( StringView{name} );

        //ASSERT( name.length() <= 16 );
        //int   res = prctl( PR_SET_NAME, (unsigned long) name.c_str(), 0, 0, 0 );
        //ASSERT( res == 0 );
    }

/*
=================================================
    GetCurrentThreadName
=================================================
*/
    String  AppleUtils::GetCurrentThreadName ()
    {
        char    buf [16] = {};
        //int       res = prctl( PR_GET_NAME, buf, 0, 0, 0 );
        //ASSERT( res == 0 );
        return String{buf};
    }

/*
=================================================
    GetCurrentThreadHandle
=================================================
*/
    ThreadHandle  AppleUtils::GetCurrentThreadHandle () __NE___
    {
        return ::pthread_self();
    }

/*
=================================================
    SetThreadAffinity
=================================================
*/
    bool  AppleUtils::SetThreadAffinity (const ThreadHandle &handle, uint coreIdx) __NE___
    {
        ASSERT_Lt( coreIdx, std::thread::hardware_concurrency() );

        //cpu_set_t cpuset;
        //CPU_ZERO( &cpuset );
        //CPU_SET( coreIdx, &cpuset );
        //return ::pthread_setaffinity_np( handle, sizeof(cpu_set_t), &cpuset ) == 0;

        Unused( handle, coreIdx );
        return false;
    }

    bool  AppleUtils::SetCurrentThreadAffinity (uint coreIdx) __NE___
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
    bool  AppleUtils::SetThreadPriority (const ThreadHandle &handle, float priority) __NE___
    {
        // TODO
        Unused( handle, priority );
        return false;
    }

    bool  AppleUtils::SetCurrentThreadPriority (float priority) __NE___
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
    uint  AppleUtils::GetProcessorCoreIndex () __NE___
    {
        return ::sched_getcpu();
    }

/*
=================================================
    ThreadPause
=================================================
*/
    void  AppleUtils::ThreadPause () __NE___
    {
    #if defined(AE_CPU_ARCH_X86)
        #if AE_SIMD_SSE >= 2
            _mm_pause();    // requires SSE2
        #endif
    #elif defined(AE_CPU_ARCH_X64)
        _mm_pause();        // SSE2 always supported on x64

    #elif defined(AE_CPU_ARCH_ARM32) or defined(AE_CPU_ARCH_ARM64)
        //__builtin_arm_yield();
        //__yield();
        asm volatile("yield");
        //dispatch_hardware_pause();

    #else
        // TODO
    #endif
    }


} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
