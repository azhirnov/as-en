# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use OpenVR (BSD-3-Clause license)

set( OPENVR_PATH "${AE_DST_SHARED_PATH}/OpenVR" )

add_custom_target( "OpenVR-copy-dll"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${OPENVR_PATH}/bin" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
	COMMENT "Copying OpenVR DLLs..." )
set_property( TARGET "OpenVR-copy-dll" PROPERTY FOLDER "Engine/External" )

add_library( "OpenVR-lib" INTERFACE )
target_include_directories( "OpenVR-lib" INTERFACE "${OPENVR_PATH}/include" )
target_compile_definitions( "OpenVR-lib" INTERFACE "AE_ENABLE_OPENVR" "AE_LICENSE_BSD3" )
add_dependencies( "OpenVR-lib" "OpenVR-copy-dll" )
