// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_ANDROID
# include "base/Platforms/UnixUtils.h"

namespace AE::Base
{

    //
    // Android Utils
    //

    struct AndroidUtils final : UnixUtils
    {
        // Thread //
            static void     SetCurrentThreadName (NtStringView name)                        __NE___;
        ND_ static String   GetCurrentThreadName ()                                         __Th___;

        ND_ static ThreadHandle  GetCurrentThreadHandle ()                                  __NE___;

            static bool     SetThreadAffinity (const ThreadHandle &handle, uint coreIdx)    __NE___;
            static bool     SetThreadPriority (const ThreadHandle &handle, float priority)  __NE___;

            static bool     SetCurrentThreadAffinity (uint coreIdx)                         __NE___;
            static bool     SetCurrentThreadPriority (float priority)                       __NE___;

        ND_ static uint     GetProcessorCoreIndex ()                                        __NE___;    // current logical CPU core


        // OS //
        ND_ static Version3         GetOSVersion ()                                         __NE___;
        ND_ static uint             GetSDKVersion ()                                        __NE___;
        ND_ static uint             GetTargetSDKVersion ()                                  __NE___;
        ND_ static constexpr uint   GetMinSDKVersion ()                                     __NE___ { return __ANDROID_API__; }
        ND_ static StringView       GetOSName ()                                            __NE___ { return "Android"; }
    };

} // AE::Base

#endif // AE_PLATFORM_ANDROID
