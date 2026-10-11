# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#
# download SPIRV-Headers

set( SPIRV_PATH "${AE_EXTERNAL_SHARED_PATH}/spirv" )

if (NOT EXISTS "${SPIRV_PATH}/spirv.h")
	message( FATAL_ERROR "'${SPIRV_PATH}/spirv.h' path is not found" )
endif()
#------------------------------------------------

add_library( "SPIRV-lib" INTERFACE )
target_include_directories( "SPIRV-lib" INTERFACE "${SPIRV_PATH}" )
target_compile_definitions( "SPIRV-lib" INTERFACE "AE_ENABLE_SPIRV" )
