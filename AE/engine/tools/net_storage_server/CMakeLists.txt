# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Networking")
	file( GLOB_RECURSE SOURCES "*.*" )
	add_executable( "NetStorageServer" ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "NetStorageServer" PUBLIC "VFS" )
	target_include_directories( "NetStorageServer" PUBLIC ".." )
	set_property( TARGET "NetStorageServer" PROPERTY FOLDER "Engine/ToolApps" )
endif()
