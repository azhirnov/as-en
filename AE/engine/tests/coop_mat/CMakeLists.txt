# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ((TARGET "PipelineCompiler") AND ${AE_ENABLE_VULKAN})
	file( GLOB SOURCES "*.*" )
	add_executable( "Tests.CoopMat" ${SOURCES} )
	set_target_properties( "Tests.CoopMat" PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET "Tests.CoopMat" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.CoopMat" PUBLIC "PipelineCompiler" "GraphicsRHI" )
	target_compile_definitions( "Tests.CoopMat" PRIVATE "AE_TEST_PIPELINE_COMPILER" )

	UseEnginePCH2( "Tests.CoopMat" )

	add_test( NAME "Tests.CoopMat" COMMAND "Tests.CoopMat" )
endif()
