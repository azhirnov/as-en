# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

#----------------------------------------------------------

set( AE_ENABLE_ENGINE_TESTS ON CACHE BOOL "enable engine tests" )
set( AE_ENABLE_ENGINE_PERF_TESTS OFF CACHE BOOL "enable engine performance tests" )
set( AE_ENABLE_VTUNE_API OFF CACHE BOOL "enable helper functions for Intel VTune profiling" )
include( "cmake/config.cmake" )

set( AE_EXTERNAL_REP_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Data" CACHE INTERNAL "" FORCE )

set( AE_ENABLE_COMPILER_WARNINGS ON CACHE INTERNAL "" FORCE )
add_subdirectory( "cmake/project_template" )

#----------------------------------------------------------

set( EXTERNAL_FOLDER "${CMAKE_SOURCE_DIR}/engine/external" )
include( "${EXTERNAL_FOLDER}/External.cmake" )

#----------------------------------------------------------

add_subdirectory( "src" )
add_subdirectory( "tools" )

if (${AE_ENABLE_ENGINE_TESTS})
	add_subdirectory( "tests" )
endif()
