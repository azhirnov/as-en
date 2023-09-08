# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use DevIL (LGPL v2.1)

set( DEVIL_PATH "${AE_DST_SHARED_PATH}/DevIL" )

add_custom_target( "DevIL-copy-dll"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${DEVIL_PATH}/bin" "${MAIN_BINARY_DIR}/$<CONFIG>"
    COMMENT "Copying DevIL DLLs..." )
set_property( TARGET "DevIL-copy-dll" PROPERTY FOLDER "Engine/External" )

add_library( "DevIL-lib" INTERFACE )
target_include_directories( "DevIL-lib" INTERFACE "${DEVIL_PATH}/include" )
target_compile_definitions( "DevIL-lib" INTERFACE "AE_ENABLE_DEVIL" )
add_dependencies( "DevIL-lib" "DevIL-copy-dll" )

target_compile_definitions( "DevIL-lib" INTERFACE "AE_LICENSE_LGPLv2_1_SHAREDLIB" )
