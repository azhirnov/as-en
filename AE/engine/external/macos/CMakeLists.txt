# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

if (${AE_ENABLE_VULKAN})
    message( FATAL_ERROR "Vulkan is not supported on Apple" )
endif()

# not supported on M1
string( TOUPPER ${CMAKE_SYSTEM_PROCESSOR} PLATFORM_NAME )
if (${PLATFORM_NAME} STREQUAL "ARM64")
    set( AE_ENABLE_ANGELSCRIPT OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_GLSLANG     OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_SPIRV_CROSS OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_FREETYPE    OFF CACHE INTERNAL "" FORCE )
endif()
#----------------------------------------------------------

if (NOT DEFINED AE_DST_PATH)
    message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT DEFINED AE_DST_SHARED_PATH)
    message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()
