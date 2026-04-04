# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download slang (Apache-2.0 license)

set( SLANG_PATH "${AE_DST_SHARED_PATH}/slang" )

if (NOT EXISTS "${SLANG_PATH}/lib")
	message( FATAL_ERROR "'${SLANG_PATH}/lib' path is not found" )
endif()

if (AE_TARGET_PLATFORM_LINUX AND AE_CPU_ARCH_X64 )
	if (NOT EXISTS "${SLANG_PATH}/lib/libslang-compiler.so")
		message( FATAL_ERROR "'${SLANG_PATH}/lib/slang-compiler.so' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_LINUX AND AE_CPU_ARCH_ARM64 )
	if (NOT EXISTS "${SLANG_PATH}/lib/libslang-compiler.so")
		message( FATAL_ERROR "'${SLANG_PATH}/lib/slang-compiler.so' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_WINDOWS AND AE_CPU_ARCH_X64 )
	if (NOT EXISTS "${SLANG_PATH}/lib/slang-compiler.dll")
		message( FATAL_ERROR "'${SLANG_PATH}/lib/slang-compiler.dll' is not found" )
	endif()

elseif (AE_TARGET_PLATFORM_MACOS )
	if (NOT EXISTS "${SLANG_PATH}/lib/slang-compiler.dylib")
		message( FATAL_ERROR "'${SLANG_PATH}/lib/slang-compiler.dylib' is not found" )
	endif()

else()
	message( FATAL_ERROR "slang is not supported for current platform: ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}" )
endif()
#------------------------------------------------

add_library( "SLang-lib" INTERFACE )
target_include_directories( "SLang-lib" INTERFACE "${SLANG_PATH}/include" )
target_compile_definitions( "SLang-lib" INTERFACE
	"AE_ENABLE_SLANG" "AE_LICENSE_APACHE_2"
	"SLANG_STATIC" # actually it is dynamic, but for dynamic loading it is better to use 'SLANG_STATIC'
)

add_custom_target( "SLang-copy-dll"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${SLANG_PATH}/lib" "${MAIN_BINARY_DIR}${AE_RUNTIME_CONFIG}"
	COMMENT "Copying SLang DLL..." )

set_property( TARGET "SLang-copy-dll" PROPERTY FOLDER "Engine/External" )
add_dependencies( "SLang-lib" "SLang-copy-dll" )

install( FILES "${SLANG_PATH}/slang-LICENSE.txt" DESTINATION "3party_license" RENAME "slang-LICENSE" )
#------------------------------------------------

#[[
if (${AE_ENABLE_EXTERNAL_TESTS})
	add_executable( "Tests.SLang" "tests/main.cpp" )
	set_property( TARGET "Tests.SLang" PROPERTY FOLDER "Engine/External/Tests" )
	target_link_libraries( "Tests.SLang" PUBLIC "SLang-lib" )
	#add_test( NAME "Tests.SLang" COMMAND "Tests.SLang" )
	set_target_properties( "Tests.SLang" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
endif ()
]]
