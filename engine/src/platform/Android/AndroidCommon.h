// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <android/asset_manager_jni.h>
# include <android/native_window.h>

namespace AE::App
{
    class ApplicationAndroid;
    class InputActionsAndroid;
    class WindowAndroid;
}

#endif // AE_PLATFORM_ANDROID
