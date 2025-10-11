# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Networking")
	file( GLOB_RECURSE SOURCES "*.*" )
	add_executable( "LLM.Server" ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	target_link_libraries( "LLM.Server" PUBLIC "LangModel" )
	set_property( TARGET "LLM.Server" PROPERTY FOLDER "LLM" )
	UseEnginePCH2( "LLM.Server" )
endif()
