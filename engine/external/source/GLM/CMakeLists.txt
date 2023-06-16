# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download glm (MIT license)

set( GLM_PATH "${AE_EXTERNAL_SHARED_PATH}/GLM" )

add_library( "GLM-lib" INTERFACE )
target_include_directories( "GLM-lib" INTERFACE "${GLM_PATH}" )
target_compile_definitions( "GLM-lib" INTERFACE "AE_ENABLE_GLM" "AE_LICENSE_MIT" )
install( FILES "${GLM_PATH}/copying.txt" DESTINATION "3party_license" RENAME "GLM-LICENSE.md" )
