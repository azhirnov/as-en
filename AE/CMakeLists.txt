# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

project( "AE"
         VERSION 23.10.199   # year, month, version
         LANGUAGES C CXX )
if (APPLE)
    enable_language(OBJC)
endif()

message( STATUS "==========================================================================\n" )
message( STATUS "project '${PROJECT_NAME}' generation started" )

set_property( GLOBAL PROPERTY USE_FOLDERS ON )
message( STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})" )
message( STATUS "host system: ${CMAKE_HOST_SYSTEM_NAME} (${CMAKE_HOST_SYSTEM_VERSION})" )
message( STATUS "target system: ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_VERSION})" )
message( STATUS "host processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}" )
message( STATUS "target processor: ${CMAKE_SYSTEM_PROCESSOR}" )

#----------------------------------------------------------

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    set( AE_USE_PCH ON  CACHE BOOL "use precompiled headers" )
else()
    set( AE_USE_PCH OFF CACHE INTERNAL "" FORCE )
endif()
message( STATUS "Precompiled headers: ${AE_USE_PCH}" )

#----------------------------------------------------------

set( CMAKE_DEBUG_POSTFIX    "" CACHE INTERNAL "" FORCE )
set( CMAKE_PROFILE_POSTFIX  "" CACHE INTERNAL "" FORCE )
set( CMAKE_DEVELOP_POSTFIX  "" CACHE INTERNAL "" FORCE )
set( CMAKE_RELEASE_POSTFIX  "" CACHE INTERNAL "" FORCE )
set( MAIN_SOURCE_DIR        "${CMAKE_CURRENT_SOURCE_DIR}"   CACHE INTERNAL "" FORCE )
set( MAIN_BINARY_DIR        "${CMAKE_BINARY_DIR}/bin"       CACHE INTERNAL "" FORCE )

# this settings breaks native debugger on android
if (NOT DEFINED ANDROID)
    set( CMAKE_INSTALL_PREFIX           "${CMAKE_BINARY_DIR}/install"   CACHE PATH      "installation directory" FORCE )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${MAIN_BINARY_DIR}"            CACHE INTERNAL  ""                       FORCE )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${MAIN_BINARY_DIR}"            CACHE INTERNAL  ""                       FORCE )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${MAIN_BINARY_DIR}"            CACHE INTERNAL  ""                       FORCE )
endif()

enable_testing()

#----------------------------------------------------------

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    add_subdirectory( "web" )

else()
    add_subdirectory( "engine" )
    add_subdirectory( "samples" )

endif()

#----------------------------------------------------------

message( STATUS "project '${PROJECT_NAME}' generation ended" )
message( STATUS "\n==========================================================================" )
