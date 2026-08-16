# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (${AE_ENABLE_VULKAN})
	file( GLOB_RECURSE SOURCES "*.*" )
	add_executable( "VulkanImageZCurve" ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "VulkanImageZCurve" PUBLIC "GraphicsRHI" )
	set_property( TARGET "VulkanImageZCurve" PROPERTY FOLDER "Engine/ToolApps" )
	UseEnginePCH2( "VulkanImageZCurve" )
endif()
