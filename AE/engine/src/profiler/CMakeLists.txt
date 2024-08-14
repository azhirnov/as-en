# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( AE_ENABLE_PROFILER  ON  CACHE BOOL "enable engine profiler UI" )

if ( (TARGET "GraphicsHL") AND (${AE_ENABLE_PROFILER}) )

	file( GLOB_RECURSE HPP_SOURCES "*.h" )
	if (NOT (DEFINED ENGINE_LIBS_PATH))
		file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
	endif()
	set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

	add_library( "Profiler" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

	set_property( TARGET "Profiler" PROPERTY FOLDER "Engine" )
	target_link_libraries( "Profiler" PUBLIC "GraphicsHL" "Networking" )

	if (TARGET "Mali-HWCPipe-lib")
		target_link_libraries( "Profiler" PUBLIC "Mali-HWCPipe-lib" )
	endif()
	if (TARGET "PowerVRScopeStats-lib")
		target_link_libraries( "Profiler" PUBLIC "PowerVRScopeStats-lib" )
	endif()
	if ( TARGET "NVML-lib" )
		target_link_libraries( "Profiler" PUBLIC "NVML-lib" )
	endif()
	if (ANDROID)
		target_compile_definitions( "Profiler" PUBLIC "AE_ENABLE_ADRENO_PERFCOUNTER" "AE_ENABLE_ARM_PMU" )
	endif()

	EnablePCH( "Profiler" )
	EnablePrebuild( "Profiler" )

	install( TARGETS "Profiler" ARCHIVE DESTINATION "lib" )
endif()
