cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( AE_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
add_subdirectory( "../.." "main" )

if (NOT EXISTS "${ANDROID_NDK}")
    message( FATAL_ERROR "NDK path '${ANDROID_NDK}' is not exists" )
endif()

add_library( "AndroidTest" SHARED
    "main.cpp"
    "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c"
)
set_property( TARGET "AndroidTest" PROPERTY FOLDER "Engine/External/Tests" )
set_target_properties( "AndroidTest" PROPERTIES LINK_FLAGS_RELEASE "-u ANativeActivity_onCreate" )
target_include_directories( "AndroidTest" PUBLIC "${ANDROID_NDK}/sources/android/native_app_glue" )
target_link_libraries( "AndroidTest" PUBLIC android log )

if (TARGET "Tests.AngelScript")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.AngelScript" )
endif ()

if (TARGET "Tests.Abseil")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.Abseil" )
endif ()

if (TARGET "Tests.Brotli")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.Brotli" )
endif ()

if (TARGET "Tests.GLM")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.GLM" )
endif ()

if (TARGET "Tests.FreeType")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.FreeType" )
endif ()

if (TARGET "Tests.Vulkan")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.Vulkan" )
endif ()

if (TARGET "Tests.OpenGL")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.OpenGL" )
endif ()

if (TARGET "Tests.FileSystem")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.FileSystem" )
endif ()

if (TARGET "Tests.VMA")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.VMA" )
endif ()

if (TARGET "Tests.Utf8Proc")
    target_link_libraries( "AndroidTest" PUBLIC "Tests.Utf8Proc" )
endif ()
