# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

#----------------------------------------------------------

set( AE_ENABLE_ENGINE_TESTS         ON  CACHE BOOL "enable engine tests" )
set( AE_ENABLE_ENGINE_PERF_TESTS    ON  CACHE BOOL "enable engine performance tests" )
set( AE_ENABLE_COMPILER_WARNINGS    ON  CACHE INTERNAL "" FORCE )

set( AE_EXTERNAL_REP_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Data" CACHE INTERNAL "" FORCE )
set( AE_TEMP_FOLDER         "${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Temp" CACHE INTERNAL "" FORCE )

include( "cmake/config.cmake" )
add_subdirectory( "cmake/project_template" )

#----------------------------------------------------------

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

add_subdirectory( "src" )
add_subdirectory( "tools" )

if (${AE_ENABLE_ENGINE_TESTS})
    add_subdirectory( "tests" )
endif()
