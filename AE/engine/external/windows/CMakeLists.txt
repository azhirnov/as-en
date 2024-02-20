# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

# External dependencies
set( AE_ENABLE_OPENVR       ON  CACHE BOOL "use OpenVR" )
set( AE_ENABLE_FFMPEG       ON  CACHE BOOL "use FFmpeg (GPL or LGPL)" )
set( AE_ENABLE_DEVIL        ON  CACHE BOOL "use DevIL" )
set( AE_ENABLE_NVIDIA_API   OFF CACHE BOOL "use NVPerf & NVAPI" )
#----------------------------------------------------------


if (${AE_CI_BUILD_NO_GRAPHICS})
    set( AE_ENABLE_OPENVR       OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_FFMPEG       OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_BREAKPAD     OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_DEVIL        OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_NVIDIA_API   OFF CACHE INTERNAL "" FORCE )
endif()
#----------------------------------------------------------

if (NOT DEFINED AE_DST_PATH)
    message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT DEFINED AE_DST_SHARED_PATH)
    message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()

if (${AE_ENABLE_OPENVR})
    add_subdirectory( "OpenVR" )
endif ()

if (${AE_ENABLE_FFMPEG})
    add_subdirectory( "ffmpeg" )
endif ()

if (${AE_ENABLE_DEVIL})
    add_subdirectory( "DevIL" )
endif ()

if (${AE_ENABLE_NVIDIA_API})
    add_subdirectory( "NVidia" )
endif ()
