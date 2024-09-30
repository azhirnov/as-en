# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "ECS-st" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "ECS-st" PROPERTY FOLDER "Engine" )
target_link_libraries( "ECS-st" PUBLIC "Serializing" "Threading" )

EnablePCH( "ECS-st" )
EnablePrebuild( "ECS-st" )
EnableUnitBuild( "ECS-st" )

install( TARGETS "ECS-st" ARCHIVE DESTINATION "lib" )
