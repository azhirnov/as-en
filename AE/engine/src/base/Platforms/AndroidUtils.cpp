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
    #if 0
        char    sdk_ver_str [92];
        if ( __system_property_get( "ro.build.version.sdk", OUT sdk_ver_str )) {
            return std::atoi( sdk_ver_str );
        }
        return 0;
    #else
        return android_get_device_api_level();
    #endif
    }

/*
=================================================
    GetTargetSDKVersion
=================================================
*/
    uint  AndroidUtils::GetTargetSDKVersion () __NE___
    {
        ASSERT( GetSDKVersion() >= 24 );
        return android_get_application_target_sdk_version();
    }

/*
=================================================
    GetOSVersion
=================================================
*/
    Version3  AndroidUtils::GetOSVersion () __NE___
    {
    #if 0
        char    sdk_ver_str [92];
        if ( __system_property_get( "ro.build.version.release", OUT sdk_ver_str ))
        {
            StringView  str {sdk_ver_str};
            usize       pos = str.find( '.' );
            return Version3{ uint(std::atoi(str.data())), uint(std::atoi(str.data() + pos)), 0 };
        }
        return Default;
    #else
        switch ( android_get_device_api_level() )
        {
            case 34 :   return Version3{ 14, 0, 0 };
            case 33 :   return Version3{ 13, 0, 0 };
            case 32 :
            case 31 :   return Version3{ 12, 0, 0 };
            case 30 :   return Version3{ 11, 0, 0 };
            case 29 :   return Version3{ 10, 0, 0 };
            case 28 :   return Version3{  9, 0, 0 };
            case 27 :   return Version3{  8, 1, 0 };
            case 26 :   return Version3{  8, 0, 0 };
            case 25 :   return Version3{  7, 1, 0 };
            case 24 :   return Version3{  7, 0, 0 };
        }
        return Default;
    #endif
    }


} // AE::Base

#endif // AE_PLATFORM_ANDROID
