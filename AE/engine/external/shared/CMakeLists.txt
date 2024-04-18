# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

#----------------------------------------------------------

# External dependencies
set( AE_ENABLE_ABSEIL 			ON  CACHE BOOL "use Absel" )
set( AE_ENABLE_ANGELSCRIPT 		ON  CACHE BOOL "use AngelScript" )
set( AE_ENABLE_ASSIMP	 		ON  CACHE BOOL "use Assimp" )

set( AE_ENABLE_BASS 			OFF CACHE BOOL "use BASS" )
set( AE_ENABLE_BROTLI 			ON  CACHE BOOL "use brotli" )

set( AE_ENABLE_CDT	 			ON  CACHE BOOL "use CDT" )

set( AE_ENABLE_GLFW 			ON  CACHE BOOL "use GLFW" )
set( AE_ENABLE_GLSLANG 			ON  CACHE BOOL "use GLSLang" )
set( AE_ENABLE_GLM 				ON  CACHE BOOL "use GLM" )

set( AE_ENABLE_FREETYPE 		ON  CACHE BOOL "use FreeType" )

set( AE_ENABLE_IMGUI 			ON  CACHE BOOL "use ImGUI" )

set( AE_ENABLE_KTX	 			ON  CACHE BOOL "use KTX" )

set( AE_ENABLE_LZ4	 			ON  CACHE BOOL "use lz4" )

set( AE_ENABLE_MSDFGEN 			ON  CACHE BOOL "use msdfgen" )
set( AE_ENABLE_METAL 			OFF CACHE BOOL "enable Metal API" )
set( AE_ENABLE_MESH_OPTIMIZER	ON  CACHE BOOL "use MeshOptimizer" )

set( AE_ENABLE_REMOTE_GRAPHICS	OFF CACHE BOOL "enable Remote Graphics" )
set( AE_ENABLE_RGRAPHICS_SERVER	ON  CACHE BOOL "enable Remote Graphics server" )

set( AE_ENABLE_SPIRV_CROSS 		ON  CACHE BOOL "use SPIRV-Cross" )
set( AE_ENABLE_STB 				ON  CACHE BOOL "use STB" )

set( AE_ENABLE_TINYGLTF 		ON  CACHE BOOL "use tinygltf" )

set( AE_ENABLE_VULKAN 			OFF CACHE BOOL "enable Vulkan API" )
set( AE_ENABLE_VMA 				ON  CACHE BOOL "use Vulkan Memory Allocator" )

set( AE_ENABLE_UTF8PROC 		ON  CACHE BOOL "use Utf8Proc" )

set( AE_ENABLE_XXHASH	 		ON  CACHE BOOL "use xxHash" )

set( AE_ENABLE_ZSTD	 			ON  CACHE BOOL "use zstd" )

if (${AE_ENABLE_REMOTE_GRAPHICS} AND ${AE_ENABLE_RGRAPHICS_SERVER})
	message( FATAL_ERROR "AE_ENABLE_REMOTE_GRAPHICS and AE_ENABLE_RGRAPHICS_SERVER are not compatible" )
endif()
#----------------------------------------------------------


if (${AE_CI_BUILD_NO_GRAPHICS})
	set( AE_ENABLE_BASS 		OFF CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_GLSLANG 		OFF CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_FREETYPE 	OFF CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_MSDFGEN 		OFF CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_SPIRV_CROSS 	OFF CACHE INTERNAL "" FORCE )
endif()
#----------------------------------------------------------


if (NOT DEFINED AE_DST_PATH)
	message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT EXISTS "${AE_DST_PATH}")
	message( FATAL_ERROR "AE_DST_PATH: '${AE_DST_PATH}' is not exists" )
endif()

if (NOT DEFINED AE_DST_SHARED_PATH)
	message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()
if (NOT EXISTS "${AE_DST_SHARED_PATH}")
	message( FATAL_ERROR "AE_DST_SHARED_PATH: '${AE_DST_SHARED_PATH}' is not exists" )
endif()


if (${AE_ENABLE_ABSEIL})
	add_subdirectory( "Abseil" )
endif()

if (${AE_ENABLE_ANGELSCRIPT})
	add_subdirectory( "AngelScript" )
endif()

if (${AE_ENABLE_ASSIMP})
	add_subdirectory( "Assimp" )
endif()

if (${AE_ENABLE_BASS})
	add_subdirectory( "BASS" )
endif()

if (${AE_ENABLE_BROTLI})
	add_subdirectory( "Brotli" )
endif()

if (${AE_ENABLE_CDT})
	add_subdirectory( "CDT" )
endif()

if (${AE_ENABLE_GLFW})
	add_subdirectory( "GLFW" )
endif()

if (${AE_ENABLE_GLSLANG})
	add_subdirectory( "glslang" )
endif()

if (${AE_ENABLE_GLM})
	add_subdirectory( "GLM" )
endif()

if (${AE_ENABLE_FREETYPE})
	add_subdirectory( "FreeType" )
endif()

add_subdirectory( "fameta-counter" )

if (${AE_ENABLE_IMGUI})
	add_subdirectory( "imgui" )
endif()

if (${AE_ENABLE_KTX})
	add_subdirectory( "KTX-Software" )
endif()

if (${AE_ENABLE_LZ4})
	add_subdirectory( "lz4" )
endif()

if (${AE_ENABLE_MSDFGEN})
	add_subdirectory( "msdfgen" )
endif()

if (${AE_ENABLE_MESH_OPTIMIZER})
	add_subdirectory( "MeshOptimizer" )
endif()

if (${AE_ENABLE_SPIRV_CROSS})
	add_subdirectory( "SPIRV-Cross" )
endif()

if (${AE_ENABLE_STB})
	add_subdirectory( "stb" )
endif()

if (${AE_ENABLE_VULKAN})
	add_subdirectory( "Vulkan" )
endif()

if (${AE_ENABLE_VMA} AND ${AE_ENABLE_VULKAN})
	add_subdirectory( "VMA" )
endif()

if (${AE_ENABLE_UTF8PROC})
	add_subdirectory( "Utf8Proc" )
endif()

if (${AE_ENABLE_TINYGLTF})
	add_subdirectory( "tinygltf" )
endif()

if (${AE_ENABLE_XXHASH})
	add_subdirectory( "xxHash" )
endif()

if (${AE_ENABLE_ZSTD})
	add_subdirectory( "zstd" )
endif()
