# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( EXP_DIR "${MAIN_SOURCE_DIR}/engine/_experimental/platform" )

file( GLOB_RECURSE HPP_EXP		"${EXP_DIR}/*.h" )
file( GLOB_RECURSE CPP_EXP		"${EXP_DIR}/*.cpp" )

file( GLOB_RECURSE HPP_SOURCES	"*.h" )
file( GLOB_RECURSE CPP_SOURCES	"*.cpp" )

if (MSVC OR APPLE)
	file( GLOB_RECURSE OBJC_SOURCES	"*.mm" )
	file( GLOB_RECURSE OBJC_EXP		"${EXP_DIR}/*.mm" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )
set( EXP_SOURCES ${HPP_EXP} ${CPP_EXP} ${OBJC_EXP} )

if (${AE_INCLUDE_ANDROID_SRC})
	set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/engine/src/main" )
	if (NOT EXISTS "${ANDROID_SRC_PATH}")
		message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
	endif()
	file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.java" "${ANDROID_SRC_PATH}/*.xml" )
	list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )

	set( ANDROID_SRC_PATH2 "${MAIN_SOURCE_DIR}/android/engine_java/src/main" )
	if (NOT EXISTS "${ANDROID_SRC_PATH2}")
		message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH2}" )
	endif()

	file( GLOB_RECURSE ANDROID_SOURCES2 "${ANDROID_SRC_PATH2}/*.java" "${ANDROID_SRC_PATH2}/*.xml" )
	list( APPEND ANDROID_SOURCES ${ANDROID_SOURCES2} "${ANDROID_SRC_PATH2}/../../build.gradle" )
endif()


add_library( "Platform" STATIC ${SOURCES} ${ANDROID_SOURCES} ${EXP_SOURCES} ${ANDROID_NATIVE_APP} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( TREE ${EXP_DIR} FILES ${EXP_SOURCES} )
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

# for IDE
if (FALSE) #WIN32)
	target_include_directories( "Platform" PRIVATE
		"${ANDROID_NDK_INCLUDE}"
		"${ANDROID_NDK_PATH}/sources/android/native_app_glue"
	)
endif()

if (WIN32)
	set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

if (${AE_ENABLE_VULKAN})
	target_link_libraries( "Platform" PUBLIC "VulkanSyncLog" )
endif()

UseEnginePCH( "Platform" )
EnableUnityBuild( "Platform" )

install( TARGETS "Platform" ARCHIVE DESTINATION "lib" )
