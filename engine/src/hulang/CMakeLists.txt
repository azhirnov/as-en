file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )

add_library( "HuLang" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "HuLang" PROPERTY FOLDER "Engine" )
target_include_directories( "HuLang" PUBLIC ".." )
target_link_libraries( "HuLang" PUBLIC "Base" "Threading" )

install( TARGETS "HuLang" ARCHIVE DESTINATION "lib" )
