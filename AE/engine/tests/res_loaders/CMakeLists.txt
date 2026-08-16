# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "ResourceLoaders")
	file( GLOB_RECURSE SOURCES "*.*" )
	if (${AE_ANDROID_UI_MODE})
		set( TEST_NAME "TestsResourceLoaders" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Tests.ResourceLoaders" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Tests.ResourceLoaders" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "ResourceLoaders" "Threading" )

	UseEnginePCH2( ${TEST_NAME} )

	add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )
endif()
