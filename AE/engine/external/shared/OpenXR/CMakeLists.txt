# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#
# download OpenXR headers (Apache 2.0 license)

set( OPENXR_PATH "${AE_DST_PATH}/OpenXR" )

add_library( "OpenXR-lib" INTERFACE )
target_include_directories( "OpenXR-lib" INTERFACE "${OPENXR_PATH}/include" "${AE_EXTERNAL_SHARED_PATH}/OpenXRLoader" )
target_compile_definitions( "OpenXR-lib" INTERFACE "AE_ENABLE_OPENXR" "AE_LICENSE_APACHE_2" )
install( FILES "${OPENXR_PATH}/share/doc/openxr/LICENSE" DESTINATION "3party_license" RENAME "OpenXR-SDK-LICENSE" )

#------------------------------------------------

add_custom_target( "OpenXR-copy-dll"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${OPENXR_PATH}/bin/api_layers" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
	COMMAND ${CMAKE_COMMAND} -E copy "${OPENXR_PATH}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}openxr_loader${CMAKE_SHARED_LIBRARY_SUFFIX}" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
	COMMENT "Copying OpenXR DLL..." )
set_property( TARGET "OpenXR-copy-dll" PROPERTY FOLDER "Engine/External" )
add_dependencies( "OpenXR-lib" "OpenXR-copy-dll" )
		
#------------------------------------------------
