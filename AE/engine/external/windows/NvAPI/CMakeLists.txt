# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use NVAPI

set( NVAPI_PATH "${AE_DST_SHARED_PATH}/NvAPI" )

if (NOT EXISTS "${NVAPI_PATH}/include")
	message( FATAL_ERROR "'${NVAPI_PATH}/include' path is not found" )
endif()

add_library( "NVAPI-lib" INTERFACE )
target_include_directories( "NVAPI-lib" INTERFACE "${NVAPI_PATH}/include" )
target_compile_definitions( "NVAPI-lib" INTERFACE "AE_ENABLE_NVAPI" )
set_property( TARGET "NVAPI-lib" PROPERTY INTERFACE_LINK_LIBRARIES "${NVAPI_PATH}/lib/nvapi64.lib" )
