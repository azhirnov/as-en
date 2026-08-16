# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

file( GLOB_RECURSE SOURCES "*.*" )
add_executable( "ResourceUpdater" ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "ResourceUpdater" PUBLIC "ResourceLoaders" "Threading" )
set_property( TARGET "ResourceUpdater" PROPERTY FOLDER "Engine/ToolApps" )
UseEnginePCH2( "ResourceUpdater" )
