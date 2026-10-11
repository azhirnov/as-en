# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "Base")
	set( EXP_DIR "${MAIN_SOURCE_DIR}/engine/tests/_experimental/base" )

	file( GLOB CUR_SOURCES		"*.*" )
	file( GLOB EXP_SOURCES		"${EXP_DIR}/*.*" )
	file( GLOB SHARED_SOURCES	"${CMAKE_CURRENT_SOURCE_DIR}/../shared/*.*" )

	set( SOURCES ${CUR_SOURCES} ${EXP_SOURCES} ${SHARED_SOURCES} )

	if (${AE_ANDROID_UI_MODE})
		set( TEST_NAME "TestsBase" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Tests.Base" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Tests.Base" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CUR_SOURCES} )
	source_group( TREE ${EXP_DIR} FILES ${EXP_SOURCES} )
	source_group( "shared" FILES ${SHARED_SOURCES} )

	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "Base" )

	if (EXISTS "${EXP_DIR}")
		target_include_directories( ${TEST_NAME} PUBLIC "." )
		target_compile_definitions( ${TEST_NAME} PUBLIC AE_EXPERIMENTAL )
	endif()

	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )

	UseEnginePCH2( ${TEST_NAME} )
endif()
