# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download GLSLANG (BSD-like license)

set( GLSLANG_PATH "${AE_EXTERNAL_SHARED_PATH}/glslang" )

add_library( "GLSLang-lib" INTERFACE )
target_include_directories( "GLSLang-lib" INTERFACE "${GLSLANG_PATH}/include" )
target_compile_definitions( "GLSLang-lib" INTERFACE "AE_ENABLE_GLSLANG" "ENABLE_OPT" "AE_LICENSE_BSD" )

install( FILES "${GLSLANG_PATH}/glslang-LICENSE.txt" DESTINATION "3party_license" RENAME "glslang-LICENSE" )
install( FILES "${GLSLANG_PATH}/SPIRV-Tools-LICENSE.txt" DESTINATION "3party_license" RENAME "SPIRV-Tools-LICENSE" )
install( FILES "${GLSLANG_PATH}/SPIRV-Headers-LICENSE.txt" DESTINATION "3party_license" RENAME "SPIRV-Headers-LICENSE" )
