# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "Video" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "Video" PROPERTY FOLDER "Engine" )
target_link_libraries( "Video" PUBLIC "Platform" )

if (${AE_ENABLE_FFMPEG})
    target_link_libraries( "Video" PUBLIC "FFmpeg-lib" )
endif()

EnablePCH( "Video" )
EnablePrebuild( "Video" )

install( TARGETS "Video" ARCHIVE DESTINATION "lib" )
