# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use NVAPI & NVPerf (license ???)

# NVAPI
if (FALSE)
	set( NVAPI_PATH "${AE_EXTERNAL_SHARED_PATH}/NVAPI" )
	add_library( "NVAPI-lib" INTERFACE )
	target_include_directories( "NVAPI-lib" INTERFACE "${NVAPI_PATH}" )
	target_compile_definitions( "NVAPI-lib" INTERFACE "AE_ENABLE_NVAPI" )
endif()

# NVPerf
if (TRUE)
	set( NVPERF_PATH "${AE_DST_SHARED_PATH}/NvPerf" )

	add_custom_target( "NVPerf-copy-dll"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${NVPERF_PATH}/bin" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
		COMMENT "Copying NVPerf DLLs..." )
	set_property( TARGET "NVPerf-copy-dll" PROPERTY FOLDER "Engine/External" )

	if (NOT EXISTS "${NVPERF_PATH}/bin")
		message( FATAL_ERROR "'${NVPERF_PATH}/bin' path is not found" )
	endif()

	add_library( "NVPerf-lib" INTERFACE )
	target_include_directories( "NVPerf-lib" INTERFACE "${NVPERF_PATH}/include" )
	target_compile_definitions( "NVPerf-lib" INTERFACE "AE_ENABLE_NVPERF" )
	add_dependencies( "NVPerf-lib" "NVPerf-copy-dll" )
endif()
