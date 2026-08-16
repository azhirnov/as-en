# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "LangModel")
	file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
	add_executable( "Tests.LangModel" ${SOURCES} )
	set_target_properties( "Tests.LangModel" PROPERTIES SUFFIX "${AE_EXECUTABLE_SUFFIX}" )

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET "Tests.LangModel" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.LangModel" PUBLIC "LangModel" "GraphicsRHI" "ResourceLoaders" )

	target_include_directories( "Tests.LangModel" PUBLIC ".." )	# from '_experimental' to 'tests' folder

	UseEnginePCH2( "Tests.LangModel" )

	add_test( NAME "Tests.LangModel" COMMAND "Tests.LangModel" )
endif()
