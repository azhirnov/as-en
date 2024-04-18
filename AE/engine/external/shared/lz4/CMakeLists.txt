# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download lz4 (BSD 2 license)

set( LZ4_PATH "${AE_DST_PATH}/lz4" )

add_library( "lz4-lib" INTERFACE )
target_include_directories( "lz4-lib" INTERFACE "${LZ4_PATH}/include" )
target_compile_definitions( "lz4-lib" INTERFACE "AE_ENABLE_LZ4" "AE_LICENSE_BSD_2" )
install( FILES "${LZ4_PATH}/LICENSE" DESTINATION "3party_license" RENAME "lz4-LICENSE" )

if (DEFINED ANDROID)
	set_property( TARGET "lz4-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${LZ4_PATH}/lib/${ANDROID_ABI}/liblz4.a")
else()
	set_property( TARGET "lz4-lib" PROPERTY INTERFACE_LINK_LIBRARIES
		"${LZ4_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}lz4${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.lz4" "tests/main.cpp" )
	#add_test( NAME "Tests.lz4" COMMAND "Tests.lz4" )
	set_property( TARGET "Tests.lz4" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.lz4" PUBLIC "lz4-lib" )
endif ()
