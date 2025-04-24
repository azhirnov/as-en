
if (NOT DEFINED AE_EXTERNAL_BIN_PATH)
	message( FATAL_ERROR "AE_EXTERNAL_BIN_PATH is not defined" )
endif()
if (NOT EXISTS "${AE_EXTERNAL_BIN_PATH}")
	message( FATAL_ERROR "AE_EXTERNAL_BIN_PATH: '${AE_EXTERNAL_BIN_PATH}' is not exists" )
endif()
if (NOT DEFINED AE_COMPILER_NAME)
	message( FATAL_ERROR "AE_COMPILER_NAME is not defined" )
endif()

set( AE_EXTERNAL_SHARED_PATH "${AE_EXTERNAL_BIN_PATH}/external/source" CACHE INTERNAL "" FORCE )

# detect target platform
if (AE_CPU_ARCH_X64)
	set( EXT_ARCH "x64" )
elseif (AE_CPU_ARCH_X86)
	set( EXT_ARCH "x86" )
elseif (AE_CPU_ARCH_ARM64)
	set( EXT_ARCH "arm64" )
elseif (AE_CPU_ARCH_ARM32)
	set( EXT_ARCH "armv7" )
elseif (AE_CPU_ARCH_E2K)
	set( EXT_ARCH "e2k" )
else()
	message( FATAL_ERROR "unsupported processor '${CMAKE_SYSTEM_PROCESSOR}'" )
endif()

# detect compiler
if ("${AE_COMPILER_NAME}" STREQUAL "MSVC")
	set( EXT_COMP "" )
elseif (("${AE_COMPILER_NAME}" STREQUAL "CLANG") OR ("${AE_COMPILER_NAME}" STREQUAL "MSVC_CLANG") OR
 		("${AE_COMPILER_NAME}" STREQUAL "CLANG_APPLE") OR ("${AE_COMPILER_NAME}" STREQUAL "CLANG_ANDROID"))
	set( EXT_COMP "clang" )
elseif ("${AE_COMPILER_NAME}" STREQUAL "GCC")
	set( EXT_COMP "gcc" )
elseif ("${AE_COMPILER_NAME}" STREQUAL "E2K_LCC")
	set( EXT_COMP "lcc" )
elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
	# ignore
else()
	message( FATAL_ERROR "unknown compiler '${CMAKE_CXX_COMPILER_ID}' / ${AE_COMPILER_NAME}" )
endif()

# get compiler version
string( REPLACE "." ";" EXT_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION} )
list( GET EXT_COMPILER_VERSION 0 EXT_COMP_VER1 )
list( GET EXT_COMPILER_VERSION 1 EXT_COMP_VER2 )


# Android
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	set( EXT_PLATFORM_AND_COMPILER 		"android-${EXT_COMP}" )
	set( EXT_PLATFORM_AND_COMPILER_VER 	${EXT_PLATFORM_AND_COMPILER} )
	set( EXT_PLATFORM 					"android" )

# Windows
elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND MSVC)
	set( EXT_PLATFORM "win-${EXT_ARCH}" )
	if ((${CMAKE_VS_PLATFORM_TOOLSET} STREQUAL "v142") OR (${CMAKE_VS_PLATFORM_TOOLSET} STREQUAL "v143"))
		set( EXT_PLATFORM_AND_COMPILER 		"win-${EXT_ARCH}-msvc" )
		set( EXT_PLATFORM_AND_COMPILER_VER 	"win-${EXT_ARCH}-msvc${EXT_COMP_VER1}.${EXT_COMP_VER2}" )
		
	elseif (${CMAKE_VS_PLATFORM_TOOLSET} STREQUAL "ClangCL")
		set( EXT_PLATFORM_AND_COMPILER 		"win-${EXT_ARCH}-clang" )
		set( EXT_PLATFORM_AND_COMPILER_VER 	"win-${EXT_ARCH}-clang${EXT_COMP_VER1}" )
	else()
		message( FATAL_ERROR "unsupported VC toolset '${CMAKE_VS_PLATFORM_TOOLSET}'" )
	endif()

# Linux
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set( EXT_PLATFORM 					"linux-${EXT_ARCH}" )
	set( EXT_PLATFORM_AND_COMPILER 		"linux-${EXT_ARCH}-${EXT_COMP}" )
	set( EXT_PLATFORM_AND_COMPILER_VER 	"linux-${EXT_ARCH}-${EXT_COMP}${EXT_COMP_VER1}" )

# Mac
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set( EXT_PLATFORM 					"macos-${EXT_ARCH}" )
	set( EXT_PLATFORM_AND_COMPILER 		"macos-${EXT_ARCH}-${EXT_COMP}" )
	set( EXT_PLATFORM_AND_COMPILER_VER 	"macos-${EXT_ARCH}-${EXT_COMP}${EXT_COMP_VER1}" )

# Emscripten
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set( EXT_PLATFORM 					"emscripten" )
	set( EXT_PLATFORM_AND_COMPILER 		${EXT_PLATFORM} )
	set( EXT_PLATFORM_AND_COMPILER_VER 	${EXT_PLATFORM} )

endif()


if (NOT DEFINED EXT_PLATFORM_AND_COMPILER)
	message( FATAL_ERROR "Unsupported platform '${CMAKE_SYSTEM_NAME}' or compiler '${CMAKE_CXX_COMPILER_ID}' with version '${CMAKE_CXX_COMPILER_VERSION}'" )
endif()

set( CMAKE_INSTALL_PREFIX 	"${CMAKE_BINARY_DIR}/install" 										CACHE INTERNAL "installation directory" 										FORCE )
set( AE_DST_PATH			"${AE_EXTERNAL_BIN_PATH}/external/${EXT_PLATFORM_AND_COMPILER_VER}"	CACHE INTERNAL "path to static libraries"										FORCE )
set( AE_DST_SHARED_PATH		"${AE_EXTERNAL_BIN_PATH}/external/${EXT_PLATFORM}"					CACHE INTERNAL "path to header-only and shared objects external dependencies"	FORCE )

if (NOT EXISTS "${AE_DST_SHARED_PATH}")
	message( FATAL_ERROR "platform specific folder '${AE_DST_SHARED_PATH}' is not exists" )
endif()
if (NOT EXISTS "${AE_DST_PATH}")
	message( FATAL_ERROR "platform and compiler specific folder '${AE_DST_PATH}' is not exists" )
endif()


add_subdirectory( "${AE_ENGINE_EXTERNAL_CMAKE}/${EXT_PLATFORM_AND_COMPILER}" 	"${EXT_PLATFORM_AND_COMPILER}" )
add_subdirectory( "${AE_ENGINE_EXTERNAL_CMAKE}/shared" 							"shared" )
