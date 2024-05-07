# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ( (${AE_ENABLE_METAL} OR ${AE_ENABLE_VULKAN}) AND (TARGET "Graphics") )
	file( GLOB_RECURSE SOURCES "*.*" )

	set( RG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../src/graphics/Remote" )
	set( RG_FILES "${RG_PATH}/RConnection.h" "${RG_PATH}/RConnection.cpp" "${RG_PATH}/RMessages.h" "${RG_PATH}/RMessages.cpp" "${RG_PATH}/RMessages.cpp.h" )

	if (ANDROID)
		add_library( "RemoteGraphicsDevice" SHARED ${SOURCES} ${RG_FILES} )

	elseif (APPLE)
		add_executable( "RemoteGraphicsDevice" MACOSX_BUNDLE ${SOURCES} ${RG_FILES} )

	else()
		set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/rg-device/src/main" )
		if (NOT EXISTS "${ANDROID_SRC_PATH}")
			message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
		endif()
		file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.*" )
		list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )

		add_executable( "RemoteGraphicsDevice" ${SOURCES} ${ANDROID_SOURCES} ${RG_FILES} )

		source_group( "Apk" FILES ${ANDROID_SOURCES} )
		set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
	endif()

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	source_group( "Remote" FILES ${RG_FILES} )

	target_link_libraries( "RemoteGraphicsDevice" PUBLIC "Platform" "Networking" )
	if (TARGET "Profiler")
		target_link_libraries( "RemoteGraphicsDevice" PUBLIC "Profiler" )
	endif()

	target_include_directories( "RemoteGraphicsDevice" PUBLIC ".." )
	set_property( TARGET "RemoteGraphicsDevice" PROPERTY FOLDER "Engine/ToolApps" )
endif()
