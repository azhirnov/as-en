# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download HWCPipe (MIT license)

set( MALI_HWCPIPE_PATH "${AE_DST_SHARED_PATH}/HWCPipe" )

if (NOT EXISTS "${MALI_HWCPIPE_PATH}/lib")
	message( FATAL_ERROR "'${MALI_HWCPIPE_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "Mali-HWCPipe-lib" INTERFACE )
target_include_directories( "Mali-HWCPipe-lib" INTERFACE "${MALI_HWCPIPE_PATH}/include" )
target_compile_definitions( "Mali-HWCPipe-lib" INTERFACE "AE_ENABLE_MALI_HWCPIPE" "AE_LICENSE_MIT" "HWCPIPE_NO_JSON" )

set_property( TARGET "Mali-HWCPipe-lib" PROPERTY INTERFACE_LINK_LIBRARIES
	"${MALI_HWCPIPE_PATH}/lib/${ANDROID_ABI}/libhwcpipe.a"
	"${MALI_HWCPIPE_PATH}/lib/${ANDROID_ABI}/libdevice.a"
)
#------------------------------------------------

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_library( "Tests.Mali-HWCPipe" STATIC "tests/main.cpp" )
	set_property( TARGET "Tests.Mali-HWCPipe" PROPERTY FOLDER "Engine/External/Tests" )
	target_include_directories( "Tests.Mali-HWCPipe" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AndroidTest" )
	target_link_libraries( "Tests.Mali-HWCPipe" PUBLIC "Mali-HWCPipe-lib" )
endif ()
