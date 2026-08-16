# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "PipelineCompiler")
	file( GLOB SOURCES "*.*" )
	if (${AE_ANDROID_UI_MODE})
		set( TEST_NAME "TestsPipelineCompiler" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Tests.PipelineCompiler" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Tests.PipelineCompiler" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "PipelineCompiler" )
	target_compile_definitions( ${TEST_NAME} PRIVATE "AE_TEST_PIPELINE_COMPILER" )

	# same as for 'PipelineCompiler'
	if ( (TARGET "Vulkan-lib") OR (EXISTS "${AE_EXTERNAL_SHARED_PATH}/Vulkan/vulkan/vulkan.h") )
		target_include_directories( ${TEST_NAME} PRIVATE "${AE_EXTERNAL_SHARED_PATH}/Vulkan" )
		target_compile_definitions( ${TEST_NAME} PRIVATE "AE_ENABLE_VULKAN" )
	endif()

	UseEnginePCH2( ${TEST_NAME} )

	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endif()
