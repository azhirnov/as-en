# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.*" )
add_executable( "ResourceUpdater" ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "ResourceUpdater" PUBLIC "ResourceLoaders" "Threading" )
target_include_directories( "ResourceUpdater" PUBLIC "." )
set_property( TARGET "ResourceUpdater" PROPERTY FOLDER "Engine/ToolApps" )
