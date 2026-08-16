# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )

set( EXT_SOURCES "${MAIN_SOURCE_DIR}/engine/src/graphics_rhi/Private/Shared.cpp" )

file( GLOB_RECURSE AS_SOURCES "*.as" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${AS_SOURCES} )

if (${AE_ANDROID_UI_MODE})
	add_library( "CICD" SHARED ${SOURCES} ${EXT_SOURCES} )
	target_compile_definitions( "CICD" PUBLIC CICD_TEST_MACHINE )
	target_link_libraries( "CICD" PUBLIC "Platform" )

else()
	if (${AE_INCLUDE_ANDROID_SRC})
		set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/cicd/src/main" )
		if (NOT EXISTS "${ANDROID_SRC_PATH}")
			message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
		endif()
		file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.*" )
		list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )
	endif()

	if (APPLE)
		add_executable( "CICD" MACOSX_BUNDLE ${SOURCES} ${EXT_SOURCES} )
	else()
		add_executable( "CICD" ${SOURCES} ${ANDROID_SOURCES} ${EXT_SOURCES} )
	endif()

	source_group( "Apk" FILES ${ANDROID_SOURCES} )
	set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )

	target_compile_definitions( "CICD" PUBLIC CICD_TEST_MACHINE CICD_BUILD_MACHINE CICD_CLIENT CICD_SERVER )
	target_compile_definitions( "CICD" PUBLIC "AE_SCRIPT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/scripts\"" )
	target_link_libraries( "CICD" PUBLIC "Scripting" )
endif()

if (${AE_ENABLE_VULKAN})
	target_link_libraries( "CICD" PUBLIC "Vulkan-lib" )

	if (APPLE)
		#if (NOT EXISTS "/usr/local/lib/libMoltenVK.dylib")
		#	message( FATAL_ERROR "'/usr/local/lib/libMoltenVK.dylib' is not exists, instal VulkanSDK with MoltenVk and try again" )
		#endif()
		#if (NOT EXISTS "/usr/local/lib/libvulkan.dylib")
		#	message( FATAL_ERROR "'/usr/local/lib/libvulkan.dylib' is not exists, instal VulkanSDK with MoltenVk and try again" )
		#endif()
	endif()
endif()

set_property( SOURCE ${EXT_SOURCES} PROPERTY SKIP_UNITY_BUILD_INCLUSION ON )

source_group( "external" FILES ${EXT_SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

target_link_libraries( "CICD" PUBLIC "Networking" )
target_include_directories( "CICD" PUBLIC "scripts" )
set_property( TARGET "CICD" PROPERTY FOLDER "Engine/ToolApps" )

UseEnginePCH2( "CICD" )
EnableUnityBuild( "CICD" )
