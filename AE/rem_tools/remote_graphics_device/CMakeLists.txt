# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if ( (${AE_ENABLE_METAL} OR ${AE_ENABLE_VULKAN}) AND (TARGET "GraphicsRHI") )
	file( GLOB_RECURSE SOURCES "*.*" )

	set( RG_PATH "${MAIN_SOURCE_DIR}/engine/src/graphics_rhi/Remote" )
	set( RG_FILES "${RG_PATH}/RConnection.h" "${RG_PATH}/RConnection.cpp" "${RG_PATH}/RMessages.h" "${RG_PATH}/RMessages.cpp" "${RG_PATH}/RMessages.cpp.h" )

	if (${AE_ANDROID_UI_MODE})
		add_library( "RemoteGraphicsDevice" SHARED ${SOURCES} ${RG_FILES} )

	elseif (APPLE)
		add_executable( "RemoteGraphicsDevice" MACOSX_BUNDLE ${SOURCES} ${RG_FILES} )

	else()
		if (${AE_INCLUDE_ANDROID_SRC})
			set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/rg-device/src/main" )
			if (NOT EXISTS "${ANDROID_SRC_PATH}")
				message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
			endif()
			file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.*" )
			list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )
		endif()

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
	set_property( TARGET "RemoteGraphicsDevice" PROPERTY FOLDER "RemoteTools" )

	UseEnginePCH2( "RemoteGraphicsDevice" )
endif()
