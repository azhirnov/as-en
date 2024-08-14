# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# BASS (free non-commercial)
# https://www.un4seen.com/

set( BASS_PATH "${AE_DST_SHARED_PATH}/BASS" )

if (NOT EXISTS "${BASS_PATH}")
	message( FATAL_ERROR "'${BASS_PATH}' path is not found" )
endif()

if (AE_TARGET_PLATFORM_LINUX AND AE_CPU_ARCH_X64 )
	if (NOT EXISTS "${BASS_PATH}/lib/libbass.so")
		message( FATAL_ERROR "'${BASS_PATH}/lib/libbass.so' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_LINUX AND AE_CPU_ARCH_ARM64 )
	if (NOT EXISTS "${BASS_PATH}/lib/libbass.so")
		message( FATAL_ERROR "'${BASS_PATH}/lib/libbass.so' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_ANDROID AND AE_CPU_ARCH_ARM32 )
	if (NOT EXISTS "${BASS_PATH}/lib/armeabi-v7a/libbass.so")
		message( FATAL_ERROR "'${BASS_PATH}/libs/armeabi-v7a/libbass.so' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_ANDROID AND AE_CPU_ARCH_ARM64 )
	if (NOT EXISTS "${BASS_PATH}/lib/arm64-v8a/libbass.so")
		message( FATAL_ERROR "'${BASS_PATH}/libs/arm64-v8a/libbass.so' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_WINDOWS AND AE_CPU_ARCH_X64 )
	if (NOT EXISTS "${BASS_PATH}/lib/bass.dll")
		message( FATAL_ERROR "'${BASS_PATH}/lib/bass.dll' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_MACOS )
	if (NOT EXISTS "${BASS_PATH}/lib/libbass.dylib")
		message( FATAL_ERROR "'${BASS_PATH}/lib/libbass.dylib' is not found" )
	endif()

else()
	message( FATAL_ERROR "BASS is not supported for current platform: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}" )
endif()
#------------------------------------------------

add_library( "BASS-lib" INTERFACE )
target_include_directories( "BASS-lib" INTERFACE "${BASS_PATH}/include" )
target_compile_definitions( "BASS-lib" INTERFACE "AE_ENABLE_BASS" "AE_LICENSE_FREE_NON_COMMERCIAL" )

if (ANDROID)
	add_custom_target( "BASS-copy-dll"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${BASS_PATH}/lib/${ANDROID_ABI}" "${AE_ANDROID_JNILIB_PATH}/${ANDROID_ABI}"
		COMMENT "Copying Android BASS shared libs..." )
else()
	add_custom_target( "BASS-copy-dll"
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${BASS_PATH}/lib" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
		COMMENT "Copying BASS DLL..." )
endif()
set_property( TARGET "BASS-copy-dll" PROPERTY FOLDER "Engine/External" )
add_dependencies( "BASS-lib" "BASS-copy-dll" )

#install( FILES "${BASS_PATH}/LICENSE" DESTINATION "3party_license" RENAME "BASS-LICENSE" )
