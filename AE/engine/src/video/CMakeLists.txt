# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "Video" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "Video" PROPERTY FOLDER "Engine" )
target_link_libraries( "Video" PUBLIC "Platform" )

if (TARGET "Audio")
	target_link_libraries( "Video" PUBLIC "Audio" )
endif()

UseEnginePCH( "Video" )
EnableUnityBuild( "Video" )

install( TARGETS "Video" ARCHIVE DESTINATION "lib" )
