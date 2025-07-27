# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "Serializing" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Serializing" PROPERTY FOLDER "Engine" )
target_link_libraries( "Serializing" PUBLIC "Base" )

UseEnginePCH( "Serializing" )
EnableUnityBuild( "Serializing" )

install( TARGETS "Serializing" ARCHIVE DESTINATION "lib" )
