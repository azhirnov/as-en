# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

# External dependencies
set( AE_ENABLE_FILESYSTEM  		ON  CACHE BOOL "enable alternative std::filesystem" )
set( AE_ENABLE_ARM_HWCPIPE 		ON  CACHE BOOL "enable ARM (Mali) performance counters" )
set( AE_ENABLE_PVRCOUNTER 		ON  CACHE BOOL "enable PowerVR performance counters" )

set( AE_ENABLE_ASSIMP			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_ASTC_ENCODER	 	OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_AMD_GPU_PERF 	OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_BASS 			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_CDT				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_GLSLANG			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_SPIRV_CROSS 		OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_GLFW				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_FREETYPE			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_MSDFGEN			OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_MESH_OPTIMIZER	OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_NVML				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_KTX				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_TINYGLTF			OFF CACHE INTERNAL "" FORCE )

#----------------------------------------------------------

set( AE_DST_PATH			"${AE_EXTERNAL_BIN_PATH}/external/android-clang"		CACHE INTERNAL "" FORCE )
set( AE_DST_SHARED_PATH		"${AE_EXTERNAL_BIN_PATH}/external/android-clang"		CACHE INTERNAL "" FORCE )
set( AE_ANDROID_JNILIB_PATH	"${MAIN_SOURCE_DIR}/android/engine/src/main/jniLibs"	CACHE INTERNAL "" FORCE )

if (NOT EXISTS "${ANDROID_NDK}")
	message( FATAL_ERROR "NDK path '${ANDROID_NDK}' is not exists" )
endif()
if (${AE_ENABLE_EXTERNAL_TESTS})
	add_library( native_app_glue STATIC "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c" )
	target_include_directories( native_app_glue PUBLIC "${ANDROID_NDK}/sources/android/native_app_glue" )
	target_link_libraries( native_app_glue PUBLIC android log )
endif()

#----------------------------------------------------------

if (${AE_ENABLE_FILESYSTEM})
	add_subdirectory( "FileSystem" )
endif()

if (${AE_ENABLE_ARM_HWCPIPE})
	add_subdirectory( "HWCPipe" )
endif()

if (${AE_ENABLE_PVRCOUNTER})
	add_subdirectory( "PowerVRScopeStats" )
endif()
