# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "GeometryTools" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "GeometryTools" PUBLIC "Base" )
target_include_directories( "GeometryTools" PUBLIC ".." )
set_property( TARGET "GeometryTools" PROPERTY FOLDER "Engine/Tools" )

if (TARGET "Graphics")
    target_link_libraries( "GeometryTools" PUBLIC "Graphics" )
    target_compile_definitions( "GeometryTools" PUBLIC "AE_GEOMTOOLS_HAS_GRAPHICS" )
endif()

EnablePCH( "GeometryTools" )
EnablePrebuild( "GeometryTools" )
