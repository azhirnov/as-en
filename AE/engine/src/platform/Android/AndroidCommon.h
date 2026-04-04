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
//# include <android_native_app_glue.h>

# include <camera/NdkCameraManager.h>

// encoder
# include <media/NdkMediaCodec.h>
# include <media/NdkMediaFormat.h>
# include <media/NdkMediaMuxer.h>

// decoder
# include <media/NdkMediaExtractor.h>
# include <media/NdkImageReader.h>

# include "base/Defines/Undef.h"

# include "base/Platforms/AndroidApi26.h"
# include "platform/Android/Java.h"

namespace AE::App
{
	class InputActionsAndroid;

	class ApplicationAndroid;
	class WindowAndroid;

	class GameAppAndroid;
	class GameWindowAndroid;

} // AE::App

#endif // AE_PLATFORM_ANDROID
