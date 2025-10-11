#pragma once

#ifndef ANDROID
# error only for Android
#endif

#include <android/log.h>
#include <cstdlib>
#include <cstddef>

#define printf(...) \
    __android_log_print(ANDROID_LOG_ERROR, "AE-External", __VA_ARGS__);

