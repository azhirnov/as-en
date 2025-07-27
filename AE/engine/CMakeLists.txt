# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( AE_ENABLE_ENGINE_TESTS			ON  CACHE BOOL "enable engine tests" )
set( AE_ENABLE_ENGINE_PERF_TESTS	ON  CACHE BOOL "enable engine performance tests" )
set( AE_ENABLE_COMPILER_WARNINGS	ON  CACHE INTERNAL "" FORCE )

set( AE_EXTERNAL_BIN_PATH	"${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Bin"	CACHE INTERNAL "" FORCE )
set( AE_DATA_FOLDER			"${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Data" CACHE INTERNAL "" FORCE )
set( AE_TEMP_FOLDER			"${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Temp"	CACHE INTERNAL "" FORCE )
set( AE_ENGINE_SHARED_DATA	"${CMAKE_CURRENT_SOURCE_DIR}/shared_data" 	CACHE INTERNAL "" FORCE )

if (NOT EXISTS ${AE_EXTERNAL_BIN_PATH})
	message( FATAL_ERROR "Path '${AE_EXTERNAL_BIN_PATH}' is not exists. Clone 'AE-Bin' repositoty or download and build external dependencies, see 'docs/Build.md' for details." )
endif()
if (NOT EXISTS ${AE_TEMP_FOLDER})
	file( MAKE_DIRECTORY "${AE_TEMP_FOLDER}" )
	message( STATUS "Create directory '${AE_TEMP_FOLDER}'" )
endif()
if (NOT EXISTS ${AE_ENGINE_SHARED_DATA})
	message( FATAL_ERROR "Path '${AE_ENGINE_SHARED_DATA}' is not exists" )
endif()

include( "cmake/config.cmake" )
include( "cmake/utils.cmake" )
add_subdirectory( "cmake/deps" )
add_subdirectory( "cmake/project_template" )

#----------------------------------------------------------
# external dependencies

set( EXTERNAL_FOLDER "${CMAKE_SOURCE_DIR}/engine/external" )
include( "${EXTERNAL_FOLDER}/External.cmake" )

if (${AE_ENABLE_VULKAN})
	target_compile_definitions( "ProjectTemplate" PUBLIC "AE_ENABLE_VULKAN" )
endif()
if (${AE_ENABLE_METAL})
	target_compile_definitions( "ProjectTemplate" PUBLIC "AE_ENABLE_METAL" )
endif()
if (${AE_ENABLE_REMOTE_GRAPHICS})
	target_compile_definitions( "ProjectTemplate" PUBLIC "AE_ENABLE_REMOTE_GRAPHICS" )
endif()
#----------------------------------------------------------

include( "cmake/engine_config.cmake" )
add_subdirectory( "src/pch" )

include( "tools/EngineTools.cmake" )
add_subdirectory( "src" )
add_subdirectory( "tools" )

if (${AE_ENABLE_ENGINE_PERF_TESTS})
	add_subdirectory( "performance" )
endif()

if (${AE_ENABLE_ENGINE_TESTS})
	add_subdirectory( "tests" )
endif()
