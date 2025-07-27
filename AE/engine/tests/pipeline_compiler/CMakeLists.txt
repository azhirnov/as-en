# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "PipelineCompiler")
	file( GLOB SOURCES "*.*" )
	add_executable( "Tests.PipelineCompiler" ${SOURCES} )
	set_target_properties( "Tests.PipelineCompiler" PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET "Tests.PipelineCompiler" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.PipelineCompiler" PUBLIC "PipelineCompiler" )
	target_compile_definitions( "Tests.PipelineCompiler" PRIVATE "AE_TEST_PIPELINE_COMPILER" )

	# same as for 'PipelineCompiler'
	if ( (TARGET "Vulkan-lib") OR (EXISTS "${AE_EXTERNAL_SHARED_PATH}/Vulkan/vulkan/vulkan.h") )
		target_include_directories( "Tests.PipelineCompiler" PRIVATE "${AE_EXTERNAL_SHARED_PATH}/Vulkan" )
		target_compile_definitions( "Tests.PipelineCompiler" PRIVATE "AE_ENABLE_VULKAN" )
	endif()
	
	UseEnginePCH2( "Tests.PipelineCompiler" )

	add_test( NAME "Tests.PipelineCompiler" COMMAND "Tests.PipelineCompiler" )
endif()
