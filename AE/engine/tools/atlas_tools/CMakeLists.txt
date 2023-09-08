# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "AtlasTools" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "AtlasTools" PUBLIC "Base" )
target_include_directories( "AtlasTools" PUBLIC ".." )
set_property( TARGET "AtlasTools" PROPERTY FOLDER "Engine/Tools" )

if (TARGET "STB-lib")
    target_link_libraries( "AtlasTools" PUBLIC "STB-lib" )
endif()

EnablePCH( "AtlasTools" )
EnablePrebuild( "AtlasTools" )
