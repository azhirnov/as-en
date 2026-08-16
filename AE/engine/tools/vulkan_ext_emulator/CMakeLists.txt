# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (${AE_ENABLE_VULKAN})
	file( GLOB_RECURSE HPP_SOURCES "*.h" )
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )

	set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

	add_library( "VulkanExtEmulation" STATIC ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "VulkanExtEmulation" PUBLIC "Base" "Vulkan-lib" )
	set_property( TARGET "VulkanExtEmulation" PROPERTY FOLDER "Engine/Tools" )
endif()
