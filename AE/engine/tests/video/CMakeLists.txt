# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Video")
	file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
	add_executable( "Tests.Video" ${SOURCES} )
	set_target_properties( "Tests.Video" PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET "Tests.Video" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.Video" PUBLIC "Video" )

	add_test( NAME "Tests.Video" COMMAND "Tests.Video" )
endif()
