# download google/hardware-perfcounter (Apache 2.0 license)

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( POWERVR_SCOPESTATS_PATH "${AE_DST_SHARED_PATH}/PowerVRScopeStats" )

add_library( "PowerVRScopeStats-lib" INTERFACE )
target_include_directories( "PowerVRScopeStats-lib" INTERFACE "${POWERVR_SCOPESTATS_PATH}/include" )
target_compile_definitions( "PowerVRScopeStats-lib" INTERFACE "AE_ENABLE_PVRCOUNTER" "AE_LICENSE_MIT" )

set_property( TARGET "PowerVRScopeStats-lib" PROPERTY INTERFACE_LINK_LIBRARIES
	"${POWERVR_SCOPESTATS_PATH}/lib/${ANDROID_ABI}/libPVRScopeDeveloper.a" )

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_library( "Tests.PowerVRScopeStats" STATIC "tests/main.c" )
	set_property( TARGET "Tests.PowerVRScopeStats" PROPERTY FOLDER "Engine/External/Tests" )
	target_include_directories( "Tests.PowerVRScopeStats" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AndroidTest" )
	target_link_libraries( "Tests.PowerVRScopeStats" PUBLIC "PowerVRScopeStats-lib" )
endif ()
