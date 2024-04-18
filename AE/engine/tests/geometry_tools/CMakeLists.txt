# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GeometryTools")
	file( GLOB SOURCES "*.*" )
	if (DEFINED ANDROID)
		add_library( "Tests.GeometryTools" STATIC ${SOURCES} )
	else()
		add_executable( "Tests.GeometryTools" ${SOURCES} )
	endif()
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET "Tests.GeometryTools" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.GeometryTools" PUBLIC "GeometryTools" )
	target_compile_definitions( "Tests.GeometryTools" PUBLIC AE_TEST_GEOMETRY_TOOLS )

	add_test( NAME "Tests.GeometryTools" COMMAND "Tests.GeometryTools" )
endif()
