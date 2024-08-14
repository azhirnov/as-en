# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()

file( GLOB_RECURSE AS_SOURCES "*.as" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${AS_SOURCES} )

if (ANDROID)
	add_library( "CICD" SHARED ${SOURCES} )
	target_compile_definitions( "CICD" PUBLIC CICD_TEST_MACHINE )
	target_link_libraries( "CICD" PUBLIC "Platform" )

elseif (NOT (TARGET "Scripting"))
	add_executable( "CICD" MACOSX_BUNDLE ${SOURCES} )
	target_compile_definitions( "CICD" PUBLIC CICD_TEST_MACHINE CICD_BUILD_MACHINE CICD_CLIENT )

else()
	set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/cicd/src/main" )
	if (NOT EXISTS "${ANDROID_SRC_PATH}")
		message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
	endif()
	file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.*" )
	list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )

	add_executable( "CICD" ${SOURCES} ${PREBUILD_CPP_FILE} ${ANDROID_SOURCES} )

	source_group( "Apk" FILES ${ANDROID_SOURCES} )
	set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )

	target_compile_definitions( "CICD" PUBLIC CICD_TEST_MACHINE CICD_BUILD_MACHINE CICD_CLIENT CICD_SERVER )
	target_compile_definitions( "CICD" PUBLIC "AE_SCRIPT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/scripts\"" )
	target_link_libraries( "CICD" PUBLIC "Scripting" )
endif()

if (${AE_ENABLE_VULKAN})
	target_link_libraries( "CICD" PUBLIC "Vulkan-lib" )

	if (APPLE)
		if (NOT EXISTS "/usr/local/lib/libMoltenVK.dylib")
			message( FATAL_ERROR "'/usr/local/lib/libMoltenVK.dylib' is not exists, instal VulkanSDK with MoltenVk and try again" )
		endif()
		if (NOT EXISTS "/usr/local/lib/libvulkan.dylib")
			message( FATAL_ERROR "'/usr/local/lib/libvulkan.dylib' is not exists, instal VulkanSDK with MoltenVk and try again" )
		endif()
	endif()
endif()

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "CICD" PUBLIC "Networking" )
target_include_directories( "CICD" PUBLIC ".." "scripts" )
set_property( TARGET "CICD" PROPERTY FOLDER "Engine/ToolApps" )

EnablePCH( "CICD" )
EnablePrebuild( "CICD" )
