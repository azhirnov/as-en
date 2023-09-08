# download HWCPipe (MIT license)

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( ARM_HWCPIPE_PATH "${AE_DST_SHARED_PATH}/HWCPipe" )

add_library( "ARM-HWCPipe-lib" INTERFACE )
target_include_directories( "ARM-HWCPipe-lib" INTERFACE "${ARM_HWCPIPE_PATH}/include" )
target_compile_definitions( "ARM-HWCPipe-lib" INTERFACE "AE_ENABLE_ARM_HWCPIPE" "AE_LICENSE_MIT" "HWCPIPE_NO_JSON" )

set_property( TARGET "ARM-HWCPipe-lib" PROPERTY INTERFACE_LINK_LIBRARIES
    "${ARM_HWCPIPE_PATH}/lib/${ANDROID_ABI}/libhwcpipe.a" )

if (${AE_ENABLE_EXTERNAL_TESTS})
    add_library( "Tests.ARM-HWCPipe" STATIC "tests/main.cpp" )
    set_property( TARGET "Tests.ARM-HWCPipe" PROPERTY FOLDER "Engine/External/Tests" )
    target_include_directories( "Tests.ARM-HWCPipe" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AndroidTest" )
    target_link_libraries( "Tests.ARM-HWCPipe" PUBLIC "ARM-HWCPipe-lib" )
endif ()
