# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "PipelineCompiler")
	cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

	file( GLOB SOURCES "*.*" )
	add_executable( "Tests.PipelineCompiler" ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET "Tests.PipelineCompiler" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.PipelineCompiler" PUBLIC "PipelineCompiler" )
	target_compile_definitions( "Tests.PipelineCompiler" PRIVATE "AE_TEST_PIPELINE_COMPILER" )

	if (${AE_USE_PCH})
		target_precompile_headers( "Tests.PipelineCompiler" PRIVATE "../../tools/res_pack/pipeline_compiler/PipelineCompiler.pch.h" )
	endif()

	add_test( NAME "Tests.PipelineCompiler" COMMAND "Tests.PipelineCompiler" )
endif()
