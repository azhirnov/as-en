file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
add_library( "VFS" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "VFS" PROPERTY FOLDER "Engine" )
target_link_libraries( "VFS" PUBLIC "Threading" "Networking" )

install( TARGETS "VFS" ARCHIVE DESTINATION "lib" )
