# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Platform")
	file( GLOB_RECURSE SOURCES "*.*" )
	if (DEFINED ANDROID)
		add_library( "Tests.Platform" STATIC ${SOURCES} )
	elseif (APPLE)
		add_executable( "Tests.Platform" MACOSX_BUNDLE ${SOURCES} )
	else()
		add_executable( "Tests.Platform" ${SOURCES} )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET "Tests.Platform" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.Platform" PUBLIC "Platform" )
	target_compile_definitions( "Tests.Platform" PUBLIC AE_TEST_PLATFORM )

	add_test( NAME "Tests.Platform" COMMAND "Tests.Platform" )
endif()
