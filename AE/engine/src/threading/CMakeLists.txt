# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )

add_library( "Threading" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Threading" PROPERTY FOLDER "Engine" )
target_link_libraries( "Threading" PUBLIC "Base" )

if (NOT APPLE)
	set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

UseEnginePCH( "Threading" )
EnableUnityBuild( "Threading" )

install( TARGETS "Threading" ARCHIVE DESTINATION "lib" )
