file( GLOB_RECURSE SOURCES "*.*" )
add_library( "Profiler" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "Profiler" PROPERTY FOLDER "Engine" )
target_link_libraries( "Profiler" PUBLIC "GraphicsHL" )

if (TARGET "ImGUI-lib")
	target_link_libraries( "Profiler" PUBLIC "ImGUI-lib" )
endif()
