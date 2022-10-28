cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "AtlasTools" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "AtlasTools" PUBLIC "Base" )
target_include_directories( "AtlasTools" PUBLIC ".." )
set_property( TARGET "AtlasTools" PROPERTY FOLDER "Engine/Tools" )

if (TARGET "STB-lib")
	target_link_libraries( "AtlasTools" PUBLIC "STB-lib" )
endif()
