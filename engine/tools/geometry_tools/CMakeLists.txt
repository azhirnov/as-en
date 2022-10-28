cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "GeometryTools" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "GeometryTools" PUBLIC "Base" )
target_include_directories( "GeometryTools" PUBLIC ".." )
set_property( TARGET "GeometryTools" PROPERTY FOLDER "Engine/Tools" )

if (TARGET "Graphics")
	target_link_libraries( "GeometryTools" PUBLIC "Graphics" )
	target_compile_definitions( "GeometryTools" PUBLIC "AE_GEOMTOOLS_HAS_GRAPHICS" )
endif()
