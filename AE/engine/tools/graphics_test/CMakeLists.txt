# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "GraphicsTestUtils" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "GraphicsTestUtils" PUBLIC "GraphicsRHI" "ResourceLoaders" )
set_property( TARGET "GraphicsTestUtils" PROPERTY FOLDER "Engine/Tools" )
