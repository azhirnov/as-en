# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if ( (${AE_ENABLE_METAL} OR ${AE_ENABLE_VULKAN}) AND (TARGET "GraphicsRHI") )
	file( GLOB_RECURSE SOURCES "*.*" )
	add_library( "GraphicsRHI-shared" SHARED ${SOURCES} )

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "GraphicsRHI-shared" PUBLIC "GraphicsRHI" )
	target_compile_definitions( "GraphicsRHI-shared" PRIVATE AE_BUILD_GRAPHICS_LIB )
	set_property( TARGET "GraphicsRHI-shared" PROPERTY FOLDER "Engine/Tools" )

	if (${AE_ENABLE_VULKAN})
		target_link_libraries( "GraphicsRHI-shared" PUBLIC "VulkanSyncLog" )
	endif()

	UseEnginePCH2( "GraphicsRHI-shared" )

endif()
