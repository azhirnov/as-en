# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use AMD GPUPerfAPI

set( AMDGPUPERF_PATH "${AE_DST_SHARED_PATH}/AMDGpuPerf" )

if (NOT EXISTS "${AMDGPUPERF_PATH}/bin")
	message( FATAL_ERROR "'${AMDGPUPERF_PATH}/bin' path is not found" )
endif()
#------------------------------------------------

add_library( "AMDGpuPerf-lib" INTERFACE )
target_include_directories( "AMDGpuPerf-lib" INTERFACE "${AMDGPUPERF_PATH}/include" )
target_compile_definitions( "AMDGpuPerf-lib" INTERFACE "AE_ENABLE_AMDGPUPERF" )
#------------------------------------------------

if (WIN32)
	add_custom_target( "AMDGpuPerf-copy-dll"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${AMDGPUPERF_PATH}/bin" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
		COMMENT "Copying AMD-GpuPerf DLLs..." )
	set_property( TARGET "AMDGpuPerf-copy-dll" PROPERTY FOLDER "Engine/External" )
	add_dependencies( "AMDGpuPerf-lib" "AMDGpuPerf-copy-dll" )
else()
	add_custom_target( "AMDGpuPerf-copy-so"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${AMDGPUPERF_PATH}/bin" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
		COMMENT "Copying AMD-GpuPerf Libs..." )
	set_property( TARGET "AMDGpuPerf-copy-so" PROPERTY FOLDER "Engine/External" )
	add_dependencies( "AMDGpuPerf-lib" "AMDGpuPerf-copy-so" )
endif()
