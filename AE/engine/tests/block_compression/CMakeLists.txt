# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if ((TARGET "PipelineCompiler") AND ${AE_ENABLE_VULKAN})
	file( GLOB SOURCES "*.*" )

	if (${AE_ANDROID_UI_MODE})
		set( TEST_NAME "TestsBlockCompression" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Tests.BlockCompression" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Tests.BlockCompression" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "PipelineCompiler" "GraphicsRHI" )

	target_compile_definitions( ${TEST_NAME} PRIVATE
		"AE_TEST_PIPELINE_COMPILER"
		AE_SHADER_PATH="${AE_ENGINE_SHARED_DATA}/shaders"
		AE_SHADER_PATH2="${AE_ENGINE_SHARED_DATA}"
	)

	UseEnginePCH2( ${TEST_NAME} )

	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endif()
