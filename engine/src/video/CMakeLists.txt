file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
add_library( "Video" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "Video" PROPERTY FOLDER "Engine" )
target_link_libraries( "Video" PUBLIC "Platform" )

if (${AE_ENABLE_FFMPEG})
    target_link_libraries( "Video" PUBLIC "FFmpeg-lib" )
endif()

install( TARGETS "Video" ARCHIVE DESTINATION "lib" )
