# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_ENABLE_SCRIPTING} AND ${AE_ENABLE_EXCEPTIONS})
	file( GLOB_RECURSE HPP_SOURCES "*.h" )
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
	set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

	add_library( "Scripting" STATIC ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	set_property( TARGET "Scripting" PROPERTY FOLDER "Engine" )
	target_link_libraries( "Scripting" PUBLIC "Base" )
	
	UseEnginePCH( "Scripting" )
	EnableUnityBuild( "Scripting" )

	if (${AE_USE_UNITY_BUILD})
		set( NON_UNITY_BUILD_SRC
			"${CMAKE_CURRENT_SOURCE_DIR}/Bindings/CoreBindings_BindVectorMath1.cpp"
			"${CMAKE_CURRENT_SOURCE_DIR}/Bindings/CoreBindings_BindVectorMath2.cpp"
		)
		set_property( SOURCE ${NON_UNITY_BUILD_SRC} PROPERTY SKIP_UNITY_BUILD_INCLUSION ON )
	endif()

	install( TARGETS "Scripting" ARCHIVE DESTINATION "lib" )
endif()
