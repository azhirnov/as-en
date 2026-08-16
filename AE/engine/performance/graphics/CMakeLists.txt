# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "Graphics")
	file( GLOB_RECURSE SOURCES "*.*" )
	if (${AE_ANDROID_UI_MODE})
		set( TEST_NAME "PerfGraphics" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Perf.Graphics" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Perf.Graphics" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/PerfTests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "Graphics" "Profiler" )
	target_include_directories( ${TEST_NAME} PUBLIC
		"."
		"${AE_ENGINE_SHARED_DATA}/scripts"
		"${AE_ENGINE_SHARED_DATA}/shaders"
		"Resources/cpp"
	)

	if (TARGET "OfflinePacker")
		add_custom_target( "Perf.Graphics.PackRes"
			DEPENDS  "OfflinePacker"
			COMMAND  $<TARGET_FILE:OfflinePacker>
						-i "${CMAKE_CURRENT_SOURCE_DIR}/Resources/res_pack.as"
						-o "${AE_TEMP_FOLDER}/engine/perf_graphics"
			COMMENT  "pack resources for 'Perf.Graphics' ..."
			VERBATIM
		)
		set_property( TARGET "Perf.Graphics.PackRes" PROPERTY FOLDER "Engine/PerfTests" )
		add_dependencies( "PACK_RES" "Perf.Graphics.PackRes" )
	endif()

	target_compile_definitions( ${TEST_NAME} PRIVATE AE_RES_PACK_FOLDER="${AE_TEMP_FOLDER}/engine/perf_graphics/" )

	if (NOT (${AE_CI_BUILD_TEST} OR ANDROID))
		target_compile_definitions( ${TEST_NAME} PRIVATE AE_RESULT_PATH="${AE_DATA_FOLDER}/perf/graphics/" )
	endif()

	UseEnginePCH2( ${TEST_NAME} )

	if (${AE_ANDROID_CONSOLE_MODE})
		add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
	endif()
endif()
