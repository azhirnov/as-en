# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

set( AE_ENABLE_MEMLEAK_CHECKS		ON  CACHE BOOL "enable memory leak checks" )
set( AE_GRAPHICS_STRONG_VALIDATION	OFF CACHE BOOL "Safe mode: validate all arguments, throw exception in context..." )

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set( AE_ENABLE_IO_URING			ON  CACHE BOOL "use io_uring instead of Linux AIO, io_uring may be unsupported" )
endif()

set( AE_EXCLUDE_PACK_RES			OFF CACHE BOOL "don't pack resources when build all" )
set( AE_PORTABLE_APP				OFF CACHE BOOL "create portable version, must not include absolute paths" )

#----------------------------------------------------------
# C++

set( AE_FORCE_CXX20					OFF CACHE BOOL "use C++20 even if C++23 supported" )
set( AE_FORCE_CXX23					OFF CACHE BOOL "use C++23 even if C++26 supported" )

#----------------------------------------------------------
# cmake

set( AE_WHOLE_ENGINE_PCH			OFF CACHE BOOL "use precompiled headers for whole engine" )
set( AE_ENGINE_BASE_PCH				OFF CACHE BOOL "use precompiled headers only for engine base module" )
set( AE_USE_UNITY_BUILD				OFF CACHE BOOL "use unity build" )
set( AE_ENABLE_ENGINE_TESTS			ON  CACHE BOOL "enable engine tests" )
set( AE_ENABLE_ENGINE_PERF_TESTS	ON  CACHE BOOL "enable engine performance tests" )
set( AE_ENABLE_COMPILER_WARNINGS	ON  CACHE INTERNAL "" FORCE )

if (${AE_WHOLE_ENGINE_PCH} AND ${AE_ENGINE_BASE_PCH})
	message( FATAL_ERROR "select one of AE_WHOLE_ENGINE_PCH or AE_ENGINE_BASE_PCH" )
endif()

if (${AE_WHOLE_ENGINE_PCH})
	message( STATUS "Enabled precompiled headers for whole engine" )
elseif( ${AE_ENGINE_BASE_PCH} )
	message( STATUS "Enabled precompiled headers for Engine.Base" )
endif()

#----------------------------------------------------------
# advanced settings

set( AE_USE_SANITIZER				OFF CACHE BOOL "enable (address/...) sanitizer" )
set( AE_CI_BUILD_TEST				OFF CACHE BOOL "CI settings for tests" )
set( AE_CI_BUILD_PERF				OFF CACHE BOOL "CI settings for performance tests" )
set( AE_ENABLE_EXCEPTIONS			ON  CACHE BOOL "enable exception and RTTI" )
set( AE_ENABLE_LOGS					ON  CACHE BOOL "enable logging, disable to remove a lot of strings" )
set( AE_ENABLE_EXTERNAL_TESTS		OFF CACHE BOOL "" )

if (MSVC)
	set( AE_MSVC_JUSTMYCODE			ON  CACHE BOOL "disable STL debugging" )
endif()

if (ANDROID)
	set( AE_INCLUDE_ANDROID_SRC		OFF CACHE INTERNAL "" FORCE )
else()
	set( AE_INCLUDE_ANDROID_SRC		ON  CACHE BOOL "" )
endif()

if (ANDROID)
	set( AE_VK_TIMELINE_SEMAPHORE	OFF CACHE BOOL "Use Vulkan timeline semaphore" )
else()
	set( AE_VK_TIMELINE_SEMAPHORE	ON  CACHE BOOL "Use Vulkan timeline semaphore" )
endif()

if (ANDROID)
    set( AE_ANDROID_CONSOLE_MODE	OFF CACHE BOOL "allow to run console programs from Termux" )
    if (${AE_ANDROID_CONSOLE_MODE})
        set( AE_ANDROID_UI_MODE     OFF CACHE INTERNAL "" FORCE )
        message( STATUS "AE_ANDROID_CONSOLE_MODE enabled" )
    else()
        set( AE_ANDROID_UI_MODE     ON  CACHE INTERNAL "" FORCE )
        message( STATUS "AE_ANDROID_UI_MODE enabled" )
    endif()
else()
    set( AE_ANDROID_CONSOLE_MODE    OFF CACHE INTERNAL "" FORCE )
    set( AE_ANDROID_UI_MODE         OFF CACHE INTERNAL "" FORCE )
endif()

mark_as_advanced( AE_USE_SANITIZER AE_CI_BUILD_TEST AE_CI_BUILD_PERF AE_ENABLE_EXCEPTIONS AE_ENABLE_LOGS AE_INCLUDE_ANDROID_SRC
				  AE_MSVC_JUSTMYCODE AE_VK_TIMELINE_SEMAPHORE AE_ENABLE_EXTERNAL_TESTS )

#----------------------------------------------------------
# internal constants

if (MSVC)
	set( AE_ITERATOR_DEBUG_LEVEL 1 CACHE INTERNAL "can be 0, 1, 2" FORCE )
endif()

if (ANDROID)
	set( AE_MOBILE  ON  CACHE INTERNAL "" FORCE )
	set( AE_DESKTOP OFF CACHE INTERNAL "" FORCE )
else ()
	set( AE_MOBILE  OFF CACHE INTERNAL "" FORCE )
	set( AE_DESKTOP ON  CACHE INTERNAL "" FORCE )
endif()

#----------------------------------------------------------
# print
if (FALSE)
	message( STATUS "AE_USE_UNITY_BUILD:	${AE_USE_UNITY_BUILD}" )
	message( STATUS "AE_CI_BUILD_TEST:		${AE_CI_BUILD_TEST}" )
	message( STATUS "AE_CI_BUILD_PERF:		${AE_CI_BUILD_PERF}" )
	message( STATUS "AE_ENABLE_EXCEPTIONS:	${AE_ENABLE_EXCEPTIONS}" )
	message( STATUS "AE_PORTABLE_APP:		${AE_PORTABLE_APP}" )
	message( STATUS "AE_VK_TIMELINE_SEMAPHORE:		${AE_VK_TIMELINE_SEMAPHORE}" )
	message( STATUS "AE_GRAPHICS_STRONG_VALIDATION:	${AE_ENABLE_EXCEPTIONS}" )
endif()
