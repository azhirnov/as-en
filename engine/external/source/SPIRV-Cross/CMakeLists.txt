# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download SPIRV-Cross (Apache-2.0 license)

set( SPIRVCROSS_PATH "${AE_EXTERNAL_SHARED_PATH}/SPIRV-Cross" )

add_library( "SPIRV-Cross-lib" INTERFACE )
target_include_directories( "SPIRV-Cross-lib" INTERFACE "${SPIRVCROSS_PATH}/include" )
target_compile_definitions( "SPIRV-Cross-lib" INTERFACE "AE_ENABLE_SPIRV_CROSS" "AE_LICENSE_APACHE_2" )
install( FILES "${SPIRVCROSS_PATH}/LICENSE" DESTINATION "3party_license" RENAME "SPIRV-Cross-LICENSE" )
