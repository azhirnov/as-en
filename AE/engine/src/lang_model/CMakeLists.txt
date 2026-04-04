# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "LangModel" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "LangModel" PROPERTY FOLDER "Engine" )
target_link_libraries( "LangModel" PUBLIC "Base" "Networking" )

if (TARGET "llama-lib")
	target_link_libraries( "LangModel" PUBLIC "llama-lib" )
endif()
if (TARGET "StableDiffusion-lib")
	target_link_libraries( "LangModel" PUBLIC "StableDiffusion-lib" )
endif()

UseEnginePCH( "LangModel" )
EnableUnityBuild( "LangModel" )

install( TARGETS "LangModel" ARCHIVE DESTINATION "lib" )
