# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

# External dependencies
set( AE_ENABLE_BREAKPAD			OFF CACHE BOOL "use breakpad" )
set( AE_ENABLE_COMPRESSONATOR	ON  CACHE BOOL "use Compressonator" )
set( AE_ENABLE_DEVIL			ON  CACHE BOOL "use DevIL" )
set( AE_ENABLE_NVIDIA_API		ON  CACHE BOOL "use NVPerf & NVAPI" )
set( AE_ENABLE_OPENVR			OFF CACHE BOOL "use OpenVR" )

set( AE_ENABLE_OPENXR			ON  CACHE INTERNAL "" FORCE )
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
endif()

if (${AE_ENABLE_BREAKPAD})
#	add_subdirectory( "breakpad" )
endif()

if (${AE_ENABLE_DEVIL})
	add_subdirectory( "DevIL" )
endif()

if (${AE_ENABLE_NVIDIA_API})
	add_subdirectory( "NvAPI" )
	add_subdirectory( "NvPerf" )
endif()
