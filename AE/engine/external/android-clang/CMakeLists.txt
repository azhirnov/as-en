# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

# External dependencies
set( AE_ENABLE_ARM_HWCPIPE 		ON  CACHE BOOL "enable ARM (Mali) performance counters" )
set( AE_ENABLE_PVRCOUNTER 		ON  CACHE BOOL "enable PowerVR performance counters" )
set( AE_ENABLE_PICO_OPENXR 		ON  CACHE BOOL "enable PICO OpenXR SDK" )

set( AE_ENABLE_ASSIMP			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_ASTC_ENCODER	 	OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_AMD_GPU_PERF 	OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_BASS 			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_CDT				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_GLSLANG			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_SPIRV_CROSS 		OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_GLFW				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_FREETYPE			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_LLAMA			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_MSDFGEN			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_MESH_OPTIMIZER	OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_NVML				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_KTX				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_TINYGLTF			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_SLANG			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_OPENXR			OFF CACHE INTERNAL "" FORCE )

#----------------------------------------------------------

set( AE_ANDROID_JNILIB_PATH	"${MAIN_SOURCE_DIR}/android/engine/src/main/jniLibs"	CACHE INTERNAL "" FORCE )

if (${AE_ENABLE_EXTERNAL_TESTS})
	if (NOT EXISTS "${ANDROID_NDK}")
		message( FATAL_ERROR "NDK path '${ANDROID_NDK}' is not exists" )
	endif()
	add_library( native_app_glue STATIC "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c" )
	target_include_directories( native_app_glue PUBLIC "${ANDROID_NDK}/sources/android/native_app_glue" )
	target_link_libraries( native_app_glue PUBLIC android log )
endif()

#----------------------------------------------------------

if (${AE_ENABLE_ARM_HWCPIPE})
	add_subdirectory( "HWCPipe" )
endif()

if (${AE_ENABLE_PVRCOUNTER})
	add_subdirectory( "PowerVRScopeStats" )
endif()

if (${AE_ENABLE_PICO_OPENXR})
	add_subdirectory( "PicoOpenXR" )
endif()
