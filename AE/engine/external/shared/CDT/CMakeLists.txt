# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download CDT (MPL-2.0 license)

set( CDT_PATH "${AE_DST_PATH}/CDT" )

if (NOT EXISTS "${CDT_PATH}/lib")
	message( FATAL_ERROR "'${CDT_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "CDT-lib" INTERFACE )
target_include_directories( "CDT-lib" INTERFACE "${CDT_PATH}/include" )
target_compile_definitions( "CDT-lib" INTERFACE "AE_ENABLE_CDT" "AE_LICENSE_MPL_2" )
install( FILES "${CDT_PATH}/LICENSE" DESTINATION "3party_license" RENAME "CDT-LICENSE" )

set_property( TARGET "CDT-lib" PROPERTY INTERFACE_LINK_LIBRARIES
	$<$<CONFIG:Release>: "${CDT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}CDT${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Profile>: "${CDT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}CDT${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Develop>: "${CDT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}CDT${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Debug>:   "${CDT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}CDTd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
)
#------------------------------------------------

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.CDT" "tests/main.cpp" )
	#add_test( NAME "Tests.CDT" COMMAND "Tests.CDT" )
	set_property( TARGET "Tests.CDT" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.CDT" PUBLIC "CDT-lib" )
endif ()
