# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#
# download PICO OpenXR SDK

set( PICO_OPENXR_PATH "${AE_DST_PATH}/PicoOpenXR" )

if (NOT EXISTS "${PICO_OPENXR_PATH}/openxr")
	message( FATAL_ERROR "'${PICO_OPENXR_PATH}/openxr' path is not found" )
endif()
#------------------------------------------------

add_library( "PicoOpenXR-lib" INTERFACE )
target_include_directories( "PicoOpenXR-lib" INTERFACE "${PICO_OPENXR_PATH}" "${AE_EXTERNAL_SHARED_PATH}/OpenXRLoader" )
target_compile_definitions( "PicoOpenXR-lib" INTERFACE "AE_ENABLE_OPENXR" "AE_ENABLE_PICO_OPENXR" )

set_property( TARGET "PicoOpenXR-lib" PROPERTY INTERFACE_LINK_LIBRARIES
	"${PICO_OPENXR_PATH}/pico/loader/lib/${ANDROID_ABI}/libopenxr_loader.so"
)

#------------------------------------------------

#[[
add_custom_target( "PicoOpenXR-copy-so"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${PICO_OPENXR_PATH}/pico/loader/lib/${ANDROID_ABI}" "${AE_ANDROID_JNILIB_PATH}/${ANDROID_ABI}"
	COMMENT "Copying PICO OpenXR shared libs..." )
set_property( TARGET "PicoOpenXR-copy-so" PROPERTY FOLDER "Engine/External" )
add_dependencies( "PicoOpenXR-lib" "PicoOpenXR-copy-so" )
]]
#------------------------------------------------
