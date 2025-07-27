# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
if (MSVC OR APPLE)
	file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )

if (${AE_INCLUDE_ANDROID_SRC})
	set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/engine/src/main" )
	if (NOT EXISTS "${ANDROID_SRC_PATH}")
		message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
	endif()
	file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.java" "${ANDROID_SRC_PATH}/*.xml" )
	list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )
endif()

add_library( "Platform" STATIC ${SOURCES} ${ANDROID_SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "Android/Apk" FILES ${ANDROID_SOURCES} )

set_property( TARGET "Platform" PROPERTY FOLDER "Engine" )
target_link_libraries( "Platform" PUBLIC "Threading" "VFS" "Networking" )

if (TARGET "Audio")
	target_link_libraries( "Platform" PUBLIC "Audio" )
endif()
if (TARGET "GraphicsRHI")
	target_link_libraries( "Platform" PUBLIC "GraphicsRHI" )
endif()

if (NOT APPLE)
	set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()
if (WIN32)
	target_include_directories( "Platform" PRIVATE "${ANDROID_NDK_INCLUDE}/sysroot/usr" )	# for IDE
	set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

if (${AE_ENABLE_VULKAN})
	target_link_libraries( "Platform" PUBLIC "VulkanSyncLog" )
endif()

UseEnginePCH( "Platform" )
EnableUnityBuild( "Platform" )

install( TARGETS "Platform" ARCHIVE DESTINATION "lib" )
