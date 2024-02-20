// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <jni.h>
# include <android/asset_manager_jni.h>
# include <android/native_window.h>
# include <android/native_window_jni.h>
# include <android/keycodes.h>
# include <android/sensor.h>

# include <camera/NdkCameraManager.h>
# include "base/Defines/Undef.h"

# include "base/Platforms/AndroidApi26.h"
# include "platform/Android/Java.h"

namespace AE::App
{
    class ApplicationAndroid;
    class InputActionsAndroid;
    class WindowAndroid;

} // AE::App

#endif // AE_PLATFORM_ANDROID
