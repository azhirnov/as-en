# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_ENABLE_VULKAN})
    file( GLOB_RECURSE SOURCES "*.*" )
    add_library( "VulkanSyncLog" STATIC ${SOURCES} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    target_link_libraries( "VulkanSyncLog" PUBLIC "Base" "Vulkan-lib" )
    target_include_directories( "VulkanSyncLog" PUBLIC "." )
    set_property( TARGET "VulkanSyncLog" PROPERTY FOLDER "Engine/Tools" )
endif()
