# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

# TODO: failed to link in Linux
if ( (TARGET "GLSLang-lib") AND WIN32 )
	file( GLOB_RECURSE SOURCES1 "*.*" )
	add_library( "ShaderTrace" STATIC ${SOURCES1} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES1} )
	set_property( TARGET "ShaderTrace" PROPERTY FOLDER "Engine/ResPack" )

	target_include_directories( "ShaderTrace" PUBLIC "Public" )
	target_compile_definitions( "ShaderTrace" PUBLIC "AE_ENABLE_GLSL_TRACE" )
	target_link_libraries( "ShaderTrace" PUBLIC "Serializing" "GLSLang-lib" )

	#------------------------------------------------------

	set( SOURCES2
		"Public/ShaderTrace.h"
		"Impl/ShaderTrace.cpp"
		"Impl/ParseShaderTrace.cpp"
	)
	add_library( "ShaderTraceParser" STATIC ${SOURCES2} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES2} )
	set_property( TARGET "ShaderTraceParser" PROPERTY FOLDER "Engine/ResPack" )

	target_include_directories( "ShaderTraceParser" PUBLIC "Public" )
	target_compile_definitions( "ShaderTraceParser" PUBLIC "AE_ENABLE_GLSL_TRACE" )
	target_link_libraries( "ShaderTraceParser" PUBLIC "Serializing" )
endif()
