# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

file( GLOB_RECURSE SOURCES "*.*" )

if (APPLE)
	add_executable( "HeaderGen" MACOSX_BUNDLE ${SOURCES} )
else()
	add_executable( "HeaderGen" ${SOURCES} )
endif()

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "HeaderGen" PUBLIC "Base" )
set_property( TARGET "HeaderGen" PROPERTY FOLDER "Engine/ToolApps" )

if (${AE_ENABLE_VULKAN})
	set( VK_HEADER_PATH "${AE_EXTERNAL_SHARED_PATH}/Vulkan" )
	if (NOT EXISTS "${VK_HEADER_PATH}")
		message( FATAL_ERROR "Invalid vulkan headers path '${VK_HEADER_PATH}'" )
	endif()
	target_include_directories( "HeaderGen" PUBLIC "${VK_HEADER_PATH}" )
	target_compile_definitions( "HeaderGen" PUBLIC
		VULKAN_HEADER_PATH="${VK_HEADER_PATH}/vulkan"
		VULKAN_LOADER_PATH="${VK_HEADER_PATH}/vulkan_loader" )
endif()

if (${AE_ENABLE_OPENXR})
	set( XR_HEADER_PATH "${AE_DST_PATH}/OpenXR/include/openxr" )
	if (NOT EXISTS "${XR_HEADER_PATH}")
		message( FATAL_ERROR "Invalid OpenXR headers path '${XR_HEADER_PATH}'" )
	endif()
	target_include_directories( "HeaderGen" PUBLIC "${XR_HEADER_PATH}" )
	target_compile_definitions( "HeaderGen" PUBLIC
		XR_HEADER_PATH="${XR_HEADER_PATH}"
		XR_LOADER_PATH="${AE_EXTERNAL_SHARED_PATH}/OpenXRLoader" )
endif()

target_include_directories( "HeaderGen" PUBLIC "." )

UseEnginePCH2( "HeaderGen" )
