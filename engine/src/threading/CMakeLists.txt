file( GLOB_RECURSE SOURCES "*.*" )
add_library( "Threading" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Threading" PROPERTY FOLDER "Engine" )
target_link_libraries( "Threading" PUBLIC "Base" )

install( TARGETS "Threading" ARCHIVE DESTINATION "lib" )
