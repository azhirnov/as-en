# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Networking")
	file( GLOB SOURCES "*.*" )

	if (ANDROID)
		set( TEST_NAME "NetworkStressTest" )
		add_library( ${TEST_NAME} SHARED ${SOURCES} )
	elseif (APPLE)
		set( TEST_NAME "Tests.NetworkStressTest" )
		add_executable( ${TEST_NAME} MACOSX_BUNDLE ${SOURCES} )
	else()
		set( TEST_NAME "Tests.NetworkStressTest" )
		add_executable( ${TEST_NAME} ${SOURCES} )
		set_target_properties( ${TEST_NAME} PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET ${TEST_NAME} PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( ${TEST_NAME} PUBLIC "Networking" )

	UseEnginePCH2( ${TEST_NAME} )
endif()
