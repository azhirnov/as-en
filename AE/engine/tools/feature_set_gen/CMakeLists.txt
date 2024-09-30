# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_ENABLE_VULKAN} AND WIN32)

	file( GLOB_RECURSE SOURCES "*.*" )
	add_executable( "FeatureSetGen" ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET "FeatureSetGen" PROPERTY FOLDER "Engine/ToolApps" )

	target_link_libraries( "FeatureSetGen" PRIVATE "Serializing" "Vulkan-lib" )
	target_include_directories( "FeatureSetGen" PRIVATE "." )

	target_compile_definitions( "FeatureSetGen" PRIVATE
		"FEATURE_SET_FOLDER=Path{\"${AE_ENGINE_SHARED_DATA}/feature_set\"}"
		"DEVICE_INFO_FOLDER=Path{\"${CMAKE_CURRENT_SOURCE_DIR}/../dev_info\"}"
		"OUTPUT_FOLDER=Path{\"${CMAKE_CURRENT_SOURCE_DIR}/_output\"}"
		AE_GRAPHICS_STRONG_VALIDATION=0
	)
endif()
