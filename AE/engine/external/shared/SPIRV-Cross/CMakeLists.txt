# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download SPIRV-Cross (Apache-2.0 license)

set( SPIRVCROSS_PATH "${AE_DST_PATH}/SPIRV-Cross" )

if (NOT EXISTS "${SPIRVCROSS_PATH}/lib")
	message( FATAL_ERROR "'${SPIRVCROSS_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "SPIRV-Cross-lib" INTERFACE )
target_include_directories( "SPIRV-Cross-lib" INTERFACE "${SPIRVCROSS_PATH}/include" )
target_compile_definitions( "SPIRV-Cross-lib" INTERFACE "AE_ENABLE_SPIRV_CROSS" "AE_LICENSE_APACHE_2" )
install( FILES "${SPIRVCROSS_PATH}/LICENSE" DESTINATION "3party_license" RENAME "SPIRV-Cross-LICENSE" )

set_property( TARGET "SPIRV-Cross-lib" PROPERTY INTERFACE_LINK_LIBRARIES
	$<$<CONFIG:Release>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-core${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Release>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-glsl${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Release>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-msl${CMAKE_STATIC_LIBRARY_SUFFIX}" >

	$<$<CONFIG:Profile>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-core${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Profile>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-glsl${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Profile>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-msl${CMAKE_STATIC_LIBRARY_SUFFIX}" >

	$<$<CONFIG:Develop>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-core${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Develop>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-glsl${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Develop>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-msl${CMAKE_STATIC_LIBRARY_SUFFIX}" >

	$<$<CONFIG:Debug>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-cored${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Debug>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-glsld${CMAKE_STATIC_LIBRARY_SUFFIX}" >
	$<$<CONFIG:Debug>: "${SPIRVCROSS_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}spirv-cross-msld${CMAKE_STATIC_LIBRARY_SUFFIX}" >
)
#------------------------------------------------

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.SPIRV-Cross" "tests/main.cpp" )
	set_property( TARGET "Tests.SPIRV-Cross" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.SPIRV-Cross" PUBLIC "SPIRV-Cross-lib" )
	#add_test( NAME "Tests.SPIRV-Cross" COMMAND "Tests.SPIRV-Cross" )
endif ()
