# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (NOT ${AE_ENABLE_REMOTE_GRAPHICS})
	file( GLOB_RECURSE CPP_SOURCES "*.*" )

	set( PROFILER_SRC_PATH "${MAIN_SOURCE_DIR}/engine/src/profiler" )
	file( GLOB_RECURSE PROFILER_SOURCES "${PROFILER_SRC_PATH}/Profilers/*.*" "${PROFILER_SRC_PATH}/Remote/*.*" )

	set( JNI_SRC_PATH "${MAIN_SOURCE_DIR}/engine/src/platform/Android" )
	file( GLOB_RECURSE JNI_SOURCES "${JNI_SRC_PATH}/Java.h" "${JNI_SRC_PATH}/Java.cpp" )

	set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/remote-ctrl/src/main" )
	if (NOT EXISTS "${ANDROID_SRC_PATH}")
		message( FATAL_ERROR "Android source is not exists in '${ANDROID_SRC_PATH}'" )
	endif()
	file( GLOB_RECURSE ANDROID_SOURCES "${ANDROID_SRC_PATH}/*.*" )
	list( APPEND ANDROID_SOURCES "${ANDROID_SRC_PATH}/../../build.gradle" )

	add_library( "AndroidRemoteControl" SHARED EXCLUDE_FROM_ALL ${CPP_SOURCES} ${ANDROID_SOURCES} ${PROFILER_SOURCES} ${JNI_SOURCES} )

	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_SOURCES} )
	source_group( "Apk" FILES ${ANDROID_SOURCES} )
	source_group( "Utils" FILES ${JNI_SOURCES} )
	source_group( "Profilers" FILES ${PROFILER_SOURCES} )
	set_source_files_properties( ${ANDROID_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )

	target_link_libraries( "AndroidRemoteControl" PUBLIC "Networking" )
	set_property( TARGET "AndroidRemoteControl" PROPERTY FOLDER "Samples" )

	if (TARGET "Mali-HWCPipe-lib")
		target_link_libraries( "AndroidRemoteControl" PUBLIC "Mali-HWCPipe-lib" )
	endif()
	if (TARGET "PowerVRScopeStats-lib")
		target_link_libraries( "AndroidRemoteControl" PUBLIC "PowerVRScopeStats-lib" )
	endif()
	if (ANDROID)
		target_compile_definitions( "AndroidRemoteControl" PUBLIC "AE_ENABLE_ADRENO_PERFCOUNTER" "AE_ENABLE_ARM_PMU" )
	endif()
	
	UseEnginePCH2( "AndroidRemoteControl" )
endif()
