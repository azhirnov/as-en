# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

#----------------------------------------------------------

set( AE_ENABLE_ENGINE_TESTS         ON  CACHE BOOL "enable engine tests" )
set( AE_ENABLE_ENGINE_PERF_TESTS    ON  CACHE BOOL "enable engine performance tests" )
set( AE_ENABLE_COMPILER_WARNINGS    ON  CACHE INTERNAL "" FORCE )

set( AE_EXTERNAL_REP_PATH   "${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Data" CACHE INTERNAL "" FORCE )
set( AE_TEMP_FOLDER         "${CMAKE_CURRENT_SOURCE_DIR}/../../AE-Temp" CACHE INTERNAL "" FORCE )
set( AE_ENGINE_SHARED_DATA  "${CMAKE_CURRENT_SOURCE_DIR}/shared_data"   CACHE INTERNAL "" FORCE )

if (NOT EXISTS ${AE_EXTERNAL_REP_PATH})
    message( FATAL_ERROR "Path '${AE_EXTERNAL_REP_PATH}' is not exists" )
endif()
if (NOT EXISTS ${AE_TEMP_FOLDER})
    message( FATAL_ERROR "Path '${AE_TEMP_FOLDER}' is not exists" )
endif()
if (NOT EXISTS ${AE_ENGINE_SHARED_DATA})
    message( FATAL_ERROR "Path '${AE_ENGINE_SHARED_DATA}' is not exists" )
endif()

include( "cmake/config.cmake" )
include( "cmake/utils.cmake" )
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

set( DUMMY_CPP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/project_template/main.cpp" )
if (DEFINED ENGINE_LIBS_PATH)
    set( PREBUILD_CPP_FILE "${DUMMY_CPP_FILE}" )
endif()

add_subdirectory( "src" )
add_subdirectory( "tools" )

if (${AE_ENABLE_ENGINE_TESTS})
    add_subdirectory( "tests" )
endif()
