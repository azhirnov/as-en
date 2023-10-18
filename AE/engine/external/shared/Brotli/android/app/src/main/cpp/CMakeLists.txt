# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )
project( "AndroidTest" LANGUAGES C CXX )

if (NOT EXISTS "${ANDROID_NDK}")
    message( FATAL_ERROR "NDK path '${ANDROID_NDK}' is not exists" )
endif()

include( "android.cmake" )

add_library( "AndroidTest" SHARED
    "../../../../../tests/main.cpp"
    "android_main.cpp"
    "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c" )

target_compile_definitions( "AndroidTest" PUBLIC "ANDROID" )
target_include_directories( "AndroidTest" PUBLIC "${ANDROID_NDK}/sources/android/native_app_glue" "." )

set_target_properties( "AndroidTest" PROPERTIES LINK_FLAGS_RELEASE "-u ANativeActivity_onCreate" )
target_link_libraries( "AndroidTest" PUBLIC android log "Static-lib" )
