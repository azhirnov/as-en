# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if ( ${AE_ENABLE_VULKAN} AND ${AE_ENABLE_LOGS} )
	file( GLOB_RECURSE SOURCES "*.*" )
	add_library( "VulkanSyncLog" STATIC ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "VulkanSyncLog" PUBLIC "Base" "Vulkan-lib" )
	set_property( TARGET "VulkanSyncLog" PROPERTY FOLDER "Engine/Tools" )
endif()
