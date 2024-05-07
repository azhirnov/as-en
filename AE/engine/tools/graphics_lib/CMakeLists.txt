# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ( (${AE_ENABLE_METAL} OR ${AE_ENABLE_VULKAN}) AND (TARGET "Graphics") )
	file( GLOB_RECURSE SOURCES "*.*" )
	add_library( "GraphicsLib" SHARED ${SOURCES} )

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "GraphicsLib" PUBLIC "Graphics" )
	target_include_directories( "GraphicsLib" PUBLIC ".." )
	target_compile_definitions( "GraphicsLib" PRIVATE AE_BUILD_GRAPHICS_LIB )
	set_property( TARGET "GraphicsLib" PROPERTY FOLDER "Engine/Tools" )

	if (${AE_ENABLE_VULKAN})
		target_link_libraries( "GraphicsLib" PUBLIC "VulkanSyncLog" )
	endif()

endif()
