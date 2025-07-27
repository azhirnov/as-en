# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "GeometryTools" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
target_link_libraries( "GeometryTools" PUBLIC "Base" )
set_property( TARGET "GeometryTools" PROPERTY FOLDER "Engine/Tools" )

if (TARGET "GraphicsRHI")
	target_link_libraries( "GeometryTools" PUBLIC "GraphicsRHI" )
	target_compile_definitions( "GeometryTools" PUBLIC "AE_GEOMTOOLS_HAS_GRAPHICS" )
endif()
if (TARGET "CDT-lib")
	target_link_libraries( "GeometryTools" PUBLIC "CDT-lib" )
endif()

UseEnginePCH2( "GeometryTools" )
EnableUnityBuild( "GeometryTools" )
