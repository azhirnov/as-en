# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "GraphicsRHI")
	file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" )

	if (${AE_ANDROID_UI_MODE})
		set( TEST_NAME "TestsGraphicsRHI" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Tests.GraphicsRHI" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Tests.GraphicsRHI" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "GraphicsRHI" "VFS" "GraphicsTestUtils" )

	target_include_directories( ${TEST_NAME} PUBLIC "." "${AE_ENGINE_SHARED_DATA}/scripts" "${AE_ENGINE_SHARED_DATA}/shaders" )

	# disable if pipelines are compiled on linux
	# enable on all platforms if pipelines are precompiled
	if (WIN32 AND ${AE_ENABLE_VULKAN})
		target_compile_definitions( ${TEST_NAME} PRIVATE AE_TEST_SHADER_DEBUGGER )
	endif()

	target_compile_definitions( ${TEST_NAME} PRIVATE
		AE_PIPELINE_COMPILER_LIBRARY="${MAIN_BINARY_DIR}"
		AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
	)

	if (TARGET "OfflinePacker")
		add_custom_target( "Tests.GraphicsRHI.PackRes"
			DEPENDS  "OfflinePacker"
			COMMAND  $<TARGET_FILE:OfflinePacker>
						-i "${CMAKE_CURRENT_SOURCE_DIR}/RenderGraph/res_pack.as"
						-o "${AE_TEMP_FOLDER}/engine/graphics_rhi"
			COMMENT  "pack resources for 'Tests.GraphicsRHI' ..."
			VERBATIM
		)
		set_property( TARGET "Tests.GraphicsRHI.PackRes" PROPERTY FOLDER "Engine/Tests" )
		add_dependencies( "PACK_RES" "Tests.GraphicsRHI.PackRes" )
	endif()

	if (${AE_ENABLE_VULKAN})
		target_link_libraries( ${TEST_NAME} PUBLIC "VulkanSyncLog" )
	endif()

	UseEnginePCH2( ${TEST_NAME} )

	AddTest( ${TEST_NAME} )
endif()
