# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( AE_ENABLE_BASS             OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_ASSIMP           OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_CDT              OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_KTX              OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_MESH_OPTIMIZER   OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_MSDFGEN          OFF CACHE INTERNAL "" FORCE )

# not supported on M1
string( TOUPPER ${CMAKE_SYSTEM_PROCESSOR} PLATFORM_NAME )
if (${PLATFORM_NAME} STREQUAL "ARM64")
    set( AE_ENABLE_ANGELSCRIPT  OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_GLSLANG      OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_SPIRV_CROSS  OFF CACHE INTERNAL "" FORCE )
    set( AE_ENABLE_FREETYPE     OFF CACHE INTERNAL "" FORCE )
endif()
#----------------------------------------------------------

if (NOT DEFINED AE_DST_PATH)
    message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT DEFINED AE_DST_SHARED_PATH)
    message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()
