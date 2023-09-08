# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

if (${AE_DESKTOP})
    #add_subdirectory( "vulkan_header_gen" )
    add_subdirectory( "vulkan_image_zcurve" )
    add_subdirectory( "lfas" )
endif()

add_subdirectory( "atlas_tools" )
add_subdirectory( "geometry_tools" )
add_subdirectory( "vulkan_sync_log" )
add_subdirectory( "vulkan_ext_emulator" )
add_subdirectory( "res_loaders" )
add_subdirectory( "graphics_test" )

if (${AE_DESKTOP} AND (DEFINED AE_CPU_ARCH_X64))
    add_subdirectory( "res_pack" )
endif()
