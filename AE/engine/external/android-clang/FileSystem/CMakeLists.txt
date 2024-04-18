# download filesystem (MIT license)

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( GFS_PATH "${AE_EXTERNAL_SHARED_PATH}/FileSystem" )

add_library( "FileSystem-lib" INTERFACE )
target_include_directories( "FileSystem-lib" INTERFACE "${GFS_PATH}/include/ghc" )
target_compile_definitions( "FileSystem-lib" INTERFACE "AE_ENABLE_GFS" "AE_LICENSE_MIT" )

if (${AE_ENABLE_EXTERNAL_TESTS})
	add_library( "Tests.FileSystem" STATIC "tests/main.cpp" )
	set_property( TARGET "Tests.FileSystem" PROPERTY FOLDER "Engine/External/Tests" )
	target_include_directories( "Tests.FileSystem" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AndroidTest" )
	target_link_libraries( "Tests.FileSystem" PUBLIC "FileSystem-lib" )
endif ()
