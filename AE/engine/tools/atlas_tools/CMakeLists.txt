# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "AtlasTools" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "AtlasTools" PUBLIC "Base" )
set_property( TARGET "AtlasTools" PROPERTY FOLDER "Engine/Tools" )

if (TARGET "STB-lib")
	target_link_libraries( "AtlasTools" PUBLIC "STB-lib" )
endif()

UseEnginePCH2( "AtlasTools" )
EnableUnityBuild( "AtlasTools" )
