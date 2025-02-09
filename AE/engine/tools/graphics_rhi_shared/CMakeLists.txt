# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ( (${AE_ENABLE_METAL} OR ${AE_ENABLE_VULKAN}) AND (TARGET "GraphicsRHI") )
	file( GLOB_RECURSE SOURCES "*.*" )
	add_library( "GraphicsRHI-shared" SHARED ${SOURCES} )

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "GraphicsRHI-shared" PUBLIC "GraphicsRHI" )
	target_include_directories( "GraphicsRHI-shared" PUBLIC ".." )
	target_compile_definitions( "GraphicsRHI-shared" PRIVATE AE_BUILD_GRAPHICS_LIB )
	set_property( TARGET "GraphicsRHI-shared" PROPERTY FOLDER "Engine/Tools" )

	if (${AE_ENABLE_VULKAN})
		target_link_libraries( "GraphicsRHI-shared" PUBLIC "VulkanSyncLog" )
	endif()

endif()
