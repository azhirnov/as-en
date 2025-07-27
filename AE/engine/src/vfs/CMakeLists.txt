# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "VFS" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "VFS" PROPERTY FOLDER "Engine" )
target_link_libraries( "VFS" PUBLIC "Threading" "Networking" )

UseEnginePCH( "VFS" )
EnableUnityBuild( "VFS" )

install( TARGETS "VFS" ARCHIVE DESTINATION "lib" )
