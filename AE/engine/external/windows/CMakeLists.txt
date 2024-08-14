# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

# External dependencies
set( AE_ENABLE_BREAKPAD			ON  CACHE BOOL "use breakpad" )
set( AE_ENABLE_COMPRESSONATOR	ON  CACHE BOOL "use Compressonator" )
set( AE_ENABLE_DEVIL			ON  CACHE BOOL "use DevIL" )
set( AE_ENABLE_FFMPEG			ON  CACHE BOOL "use FFmpeg (GPL or LGPL)" )
set( AE_ENABLE_NVIDIA_API		ON  CACHE BOOL "use NVPerf & NVAPI" )
set( AE_ENABLE_OPENVR			ON  CACHE BOOL "use OpenVR" )
#----------------------------------------------------------

if (NOT DEFINED AE_DST_PATH)
	message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT DEFINED AE_DST_SHARED_PATH)
	message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()

if (${AE_ENABLE_COMPRESSONATOR})
	add_subdirectory( "Compressonator" )
endif()

if (${AE_ENABLE_OPENVR})
	add_subdirectory( "OpenVR" )
endif ()

if (${AE_ENABLE_FFMPEG})
	add_subdirectory( "ffmpeg" )
endif ()

if (${AE_ENABLE_BREAKPAD})
#	add_subdirectory( "breakpad" )
endif ()

if (${AE_ENABLE_DEVIL})
	add_subdirectory( "DevIL" )
endif ()

if (${AE_ENABLE_NVIDIA_API})
	add_subdirectory( "NVidia" )
endif ()
