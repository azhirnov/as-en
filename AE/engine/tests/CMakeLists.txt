# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

add_subdirectory( "base" )
add_subdirectory( "scripting" )
add_subdirectory( "threading" )
add_subdirectory( "networking" )
add_subdirectory( "serializing" )
add_subdirectory( "platform" )
add_subdirectory( "graphics" )
add_subdirectory( "graphics_hl" )
add_subdirectory( "ecs-st" )
add_subdirectory( "vfs" )
add_subdirectory( "video" )

if (${AE_DESKTOP})
    add_subdirectory( "atlas_tools" )
    add_subdirectory( "geometry_tools" )
    add_subdirectory( "asset_packer" )
    add_subdirectory( "shader_trace" )
    add_subdirectory( "pipeline_compiler" )
endif ()

if (DEFINED ANDROID)
    add_subdirectory( "test_launcher" )
endif ()
