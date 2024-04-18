# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "ECS-st")
	file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
	if (DEFINED ANDROID)
		add_library( "Tests.ECS-st" STATIC ${SOURCES} )
	elseif (APPLE)
		add_executable( "Tests.ECS-st" MACOSX_BUNDLE ${SOURCES} )
	else()
		add_executable( "Tests.ECS-st" ${SOURCES} )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET "Tests.ECS-st" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.ECS-st" PUBLIC "ECS-st" )
	target_compile_definitions( "Tests.ECS-st" PUBLIC AE_TEST_ECS_ST )

	add_test( NAME "Tests.ECS-st" COMMAND "Tests.ECS-st" )
endif()
