# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GraphicsHL")
	file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" )
	file( GLOB_RECURSE SHARED_DATA "${AE_ENGINE_SHARED_DATA}/*.*" )

	if (ANDROID)
		set( TEST_NAME "TestsGraphicsHL" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} ${SHARED_DATA} )
	elseif (APPLE)
		set( TEST_NAME "Tests.GraphicsHL" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} ${SHARED_DATA} )
	else()
		set( TEST_NAME "Tests.GraphicsHL" )
		add_executable( ${TEST_NAME} ${SOURCES} ${SHARED_DATA} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	source_group( TREE "${AE_ENGINE_SHARED_DATA}/.." FILES ${SHARED_DATA} )

	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "GraphicsHL" "GraphicsTestUtils" )

	target_include_directories( ${TEST_NAME} PUBLIC "." )
	target_include_directories( ${TEST_NAME} PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" )

	target_compile_definitions( ${TEST_NAME} PRIVATE
		AE_PIPELINE_COMPILER_LIBRARY="${MAIN_BINARY_DIR}"
		AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
		AE_CANVAS_VERTS="${MAIN_SOURCE_DIR}/engine/src/graphics_hl/Canvas/VertexTypes.as"
	)

	if (TARGET "OfflinePacker")
		add_custom_target( "Tests.GraphicsHL.PackRes"
			DEPENDS  "OfflinePacker"
			COMMAND  $<TARGET_FILE:OfflinePacker>
						-i "${CMAKE_CURRENT_SOURCE_DIR}/DrawTests/res_pack.as"
						-o "${AE_TEMP_FOLDER}/engine/graphics_hl"
			COMMENT  "pack resources for 'Tests.GraphicsHL' ..."
			VERBATIM
		)
		set_property( TARGET "Tests.GraphicsHL.PackRes" PROPERTY FOLDER "Engine/Tests" )
		add_dependencies( "PACK_RES" "Tests.GraphicsHL.PackRes" )
	endif()

	if (${AE_CI_BUILD_TEST})
		target_compile_definitions( ${TEST_NAME} PRIVATE AE_REF_IMG_PATH="tests_graphics_hl_ref" )
	elseif (ANDROID)
		target_compile_definitions( ${TEST_NAME} PRIVATE AE_REF_IMG_PATH="tests_graphics_hl_ref_" )
	else()
		target_compile_definitions( ${TEST_NAME} PRIVATE
			AE_REF_IMG_PATH="${AE_DATA_FOLDER}/tests/graphics_hl/"
			AE_RES_PACK_FOLDER="${AE_TEMP_FOLDER}/engine/graphics_hl/" )
	endif()

	EnablePCH2( ${TEST_NAME} "TestsGraphicsHL" )

	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endif()
