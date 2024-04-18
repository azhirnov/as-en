# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
if (MSVC OR APPLE)
	file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )

set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/engine/src/main" )
if (NOT EXISTS "${ANDROID_SRC_PATH}")
	message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
endif()
file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.*" )
list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )

add_library( "Platform" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} ${ANDROID_SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "Android/Apk" FILES ${ANDROID_SOURCES} )

set_property( TARGET "Platform" PROPERTY FOLDER "Engine" )
target_link_libraries( "Platform" PUBLIC "Threading" "VFS" "Networking" )

if (TARGET "Audio")
	target_link_libraries( "Platform" PUBLIC "Audio" )
endif()
if (TARGET "Graphics")
	target_link_libraries( "Platform" PUBLIC "Graphics" )
endif()

if (NOT APPLE)
	set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()
if (NOT ANDROID)
	set( ANDROID_SDK_ROOT "$ENV{ANDROID_SDK_ROOT}" )
	set( ANDROID_NDK_INCLUDE "${ANDROID_SDK_ROOT}/ndk/24.0.8215888/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr" )
	target_include_directories( "Platform" PRIVATE "${ANDROID_NDK_INCLUDE}" )
	set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

# choose window library
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	target_link_libraries( "Platform" PUBLIC "android" "camera2ndk" "mediandk" )	# "nativewindow" for API 30+

elseif (${AE_ENABLE_GLFW})
	target_link_libraries( "Platform" PUBLIC "GLFW-lib" )

	if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
		target_compile_definitions( "Platform" PRIVATE GLFW_EXPOSE_NATIVE_COCOA )

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		target_compile_definitions( "Platform" PRIVATE GLFW_EXPOSE_NATIVE_WIN32 )

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		target_compile_definitions( "Platform" PRIVATE GLFW_EXPOSE_NATIVE_X11 )	# GLFW_EXPOSE_NATIVE_WAYLAND
	endif()

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	# Raw WinAPI is supported
	target_link_libraries( "Platform" PUBLIC "Dxva2.lib" )
	target_compile_definitions( "Platform" PUBLIC AE_WINAPI_WINDOW )

else()
	message( FATAL_ERROR "At least one window library must be supported" )
endif()

if (TARGET "OpenVR-lib")
	target_link_libraries( "Platform" PUBLIC "OpenVR-lib" )
endif()
if (TARGET "OpenXR-lib")
	target_link_libraries( "Platform" PUBLIC "OpenXR-lib" )
endif()
if (${AE_ENABLE_VULKAN})
	target_link_libraries( "Platform" PUBLIC "VulkanSyncLog" )
endif()

EnablePCH( "Platform" )
EnablePrebuild( "Platform" )

install( TARGETS "Platform" ARCHIVE DESTINATION "lib" )
