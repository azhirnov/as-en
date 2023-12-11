// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_ANDROID
# include "base/Defines/StdInclude.h"

# undef  _GNU_SOURCE
# define _GNU_SOURCE 1
# include <sys/prctl.h>
# include <pthread.h>
# include <sched.h>
# include <sys/system_properties.h>
# include <arm_acle.h>

# include "base/Platforms/AndroidUtils.h"
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
        return Base::_hidden_::SecureZeroMemFallback( OUT ptr, size );
    }

/*
=================================================
    SetCurrentThreadName
=================================================
*/
    void  AndroidUtils::SetCurrentThreadName (NtStringView name) __NE___
    {
        StaticLogger::SetCurrentThreadName( StringView{name} );

        ASSERT( name.length() <= 16 );
        int res = prctl( PR_SET_NAME, (unsigned long) name.c_str(), 0, 0, 0 );
        ASSERT( res == 0 );  Unused( res );
    }

/*
=================================================
    GetCurrentThreadName
=================================================
*/
    String  AndroidUtils::GetCurrentThreadName ()
    {
        char    buf [16];
        int     res = prctl( PR_GET_NAME, buf, 0, 0, 0 );
        ASSERT( res == 0 );  Unused( res );
        return String{buf};
    }
/*
=================================================
    GetCurrentThreadHandle
=================================================
*/
    ThreadHandle  AndroidUtils::GetCurrentThreadHandle () __NE___
    {
        return ::pthread_self();
    }

/*
=================================================
    SetThreadAffinity
=================================================
*/
    bool  AndroidUtils::SetThreadAffinity (const ThreadHandle &handle, uint coreIdx) __NE___
    {
        ASSERT( handle == GetCurrentThreadHandle() );
        return SetCurrentThreadAffinity( coreIdx );
    }

    bool  AndroidUtils::SetCurrentThreadAffinity (uint coreIdx) __NE___
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
    bool  AndroidUtils::SetThreadPriority (const ThreadHandle &handle, float priority) __NE___
    {
        // TODO
        Unused( handle, priority );
        return false;
    }

    bool  AndroidUtils::SetCurrentThreadPriority (float priority) __NE___
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
    uint  AndroidUtils::GetProcessorCoreIndex () __NE___
    {
        return ::sched_getcpu();
    }

/*
=================================================
    GetSDKVersion
=================================================
*/
    uint  AndroidUtils::GetSDKVersion () __NE___
    {
        char    sdk_ver_str [92];
        if ( __system_property_get( "ro.build.version.sdk", OUT sdk_ver_str )) {
            return std::atoi( sdk_ver_str );
        }
        return 0;
    }

/*
=================================================
    GetOSVersion
=================================================
*/
    Version3  AndroidUtils::GetOSVersion () __NE___
    {
        char    sdk_ver_str [92];
        if ( __system_property_get( "ro.build.version.release", OUT sdk_ver_str ))
        {
            StringView  str {sdk_ver_str};
            usize       pos = str.find( '.' );
            return Version3{ uint(std::atoi(str.data())), uint(std::atoi(str.data() + pos)), 0 };
        }
        return Default;
    }


} // AE::Base

#endif // AE_PLATFORM_ANDROID
