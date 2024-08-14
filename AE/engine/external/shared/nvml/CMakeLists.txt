# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use NVIDIA Management Library (NVML)

set( NVML_PATH "${AE_EXTERNAL_SHARED_PATH}/nvml" )

if (NOT EXISTS "${NVML_PATH}/nvml.h")
	message( FATAL_ERROR "'${NVML_PATH}/nvml.h' header is not found" )
endif()
#------------------------------------------------
	
add_library( "NVML-lib" INTERFACE )
target_include_directories( "NVML-lib" INTERFACE "${NVML_PATH}" )
target_compile_definitions( "NVML-lib" INTERFACE "AE_ENABLE_NVML" )
