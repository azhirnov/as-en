cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "Networking" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Networking" PROPERTY FOLDER "Engine" )
target_link_libraries( "Networking" PUBLIC "Threading" )
target_compile_definitions( "Networking" PUBLIC "AE_ENABLE_NETWORKING" )
