# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "GraphicsTestUtils" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "GraphicsTestUtils" PUBLIC "Graphics" "ResourceLoaders" )
target_include_directories( "GraphicsTestUtils" PUBLIC ".." )
set_property( TARGET "GraphicsTestUtils" PROPERTY FOLDER "Engine/Tools" )
