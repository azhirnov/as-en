# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

file( GLOB_RECURSE SOURCES "*.*" )
add_executable( "VulkanHeaderGen" ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "VulkanHeaderGen" PUBLIC "Base" )
target_include_directories( "VulkanHeaderGen" PUBLIC ".." )
set_property( TARGET "VulkanHeaderGen" PROPERTY FOLDER "Engine/ToolApps" )

set( VK_HEADER_PATH "${AE_EXTERNAL_SHARED_PATH}/Vulkan" )
if (NOT EXISTS "${VK_HEADER_PATH}")
    message( FATAL_ERROR "Invalid vulkan headers path '${VK_HEADER_PATH}'" )
endif()

target_include_directories( "VulkanHeaderGen" PUBLIC "${VK_HEADER_PATH}" )
target_compile_definitions( "VulkanHeaderGen" PUBLIC
    VULKAN_HEADER_PATH="${VK_HEADER_PATH}/vulkan"
    VULKAN_LOADER_PATH="${VK_HEADER_PATH}/vulkan_loader"
)
