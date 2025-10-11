# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_ENABLE_AUDIO})
	file( GLOB_RECURSE HPP_SOURCES "*.h" )
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
	set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

	add_library( "Audio" STATIC ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET "Audio" PROPERTY FOLDER "Engine" )
	target_link_libraries( "Audio" PUBLIC "Threading" "Serializing" )

	if (FALSE) #WIN32)
		target_include_directories( "Audio" PRIVATE "${ANDROID_NDK_INCLUDE}" )
	endif()

	UseEnginePCH( "Audio" )
	EnableUnityBuild( "Audio" )

	install( TARGETS "Audio" ARCHIVE DESTINATION "lib" )
endif()
