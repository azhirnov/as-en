# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB SOURCES "*.*" )
add_executable( "LLM.Console" ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	
set_property( TARGET "LLM.Console" PROPERTY FOLDER "LLM" )
target_link_libraries( "LLM.Console" PUBLIC "LangModel" )
	
UseEnginePCH2( "LLM.Console" )

target_compile_definitions( "LLM.Console" PUBLIC
	OUTPUT_FOLDER="${MAIN_SOURCE_DIR}/../AE-Data/llm_response"
)
