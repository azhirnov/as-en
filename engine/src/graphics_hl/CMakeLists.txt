file( GLOB_RECURSE SOURCES "*.*" )
add_library( "GraphicsHL" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "GraphicsHL" PROPERTY FOLDER "Engine" )
target_link_libraries( "GraphicsHL" PUBLIC "Graphics" )

if (${AE_ENABLE_FREETYPE})
#	target_link_libraries( "GraphicsHL" PUBLIC "FreeType-lib" )
endif ()
