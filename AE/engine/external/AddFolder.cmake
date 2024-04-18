
set( AE_EXTERNAL_SHARED_PATH "${AE_EXTERNAL_REP_PATH}/external/source" CACHE INTERNAL "" FORCE )

# detect target platform
if (MSVC)
	if ( (DEFINED CMAKE_GENERATOR_PLATFORM) AND (NOT (CMAKE_GENERATOR_PLATFORM STREQUAL "")) )
		if (${CMAKE_GENERATOR_PLATFORM} STREQUAL "ARM64")
			set( EXT_ARCH "arm64" )
		elseif (${CMAKE_GENERATOR_PLATFORM} STREQUAL "ARM")
			set( EXT_ARCH "arm" )
		elseif (${CMAKE_GENERATOR_PLATFORM} STREQUAL "Win32")
			set( EXT_ARCH "x86" )
		elseif (${CMAKE_GENERATOR_PLATFORM} STREQUAL "x64")
			set( EXT_ARCH "x64" )
		else()
			message( FATAL_ERROR "unknown platform '${CMAKE_GENERATOR_PLATFORM}'" )
		endif()
	else()
		set( EXT_ARCH "x64" )
	endif()

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	# ignore

elseif (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Android"))
	if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
		set( EXT_ARCH "x64" )
	elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
		set( EXT_ARCH "x64" )
	elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
		set( EXT_ARCH "x86" )
	elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "armv7-a")
		set( EXT_ARCH "armv7" )
	elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
		set( EXT_ARCH "arm64" )
	elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64")
		set( EXT_ARCH "arm64" )
	else()
		message( FATAL_ERROR "unknown processor '${CMAKE_SYSTEM_PROCESSOR}'" )
	endif()
endif()

# detect compiler
if (MSVC)
	set( EXT_COMP "" )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set( EXT_COMP "clang" )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "AppleClang")
	set( EXT_COMP "clang" )
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
	set( EXT_COMP "gcc" )
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	# ignore
else()
	message( FATAL_ERROR "unknown compiler '${CMAKE_CXX_COMPILER_ID}'" )
endif()

# get compiler version
string( REPLACE "." ";" EXT_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION} )
list( GET EXT_COMPILER_VERSION 0 EXT_COMP_VER )


# Android
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	set( PLATFORM_DEPENDENT "android-${EXT_COMP}" )

# Windows
elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "Windows") AND MSVC)
	if (${CMAKE_VS_PLATFORM_TOOLSET} STREQUAL "v142")
		set( PLATFORM_DEPENDENT "win-${EXT_ARCH}-msvc142" )
	elseif (${CMAKE_VS_PLATFORM_TOOLSET} STREQUAL "v143")
		set( PLATFORM_DEPENDENT "win-${EXT_ARCH}-msvc143" )
	elseif (${CMAKE_VS_PLATFORM_TOOLSET} STREQUAL "ClangCL")
		string( REPLACE "." ";" VERSION_LIST ${CMAKE_CXX_COMPILER_VERSION} )
		list( GET VERSION_LIST 0 CLANG_MAJ_VER )
		set( PLATFORM_DEPENDENT "win-${EXT_ARCH}-clang${CLANG_MAJ_VER}" )
	else()
		message( FATAL_ERROR "unsupported VC toolset '${CMAKE_VS_PLATFORM_TOOLSET}'" )
	endif()

# Linux
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set( PLATFORM_DEPENDENT "linux-${EXT_ARCH}-${EXT_COMP}${EXT_COMP_VER}" )

# Mac
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set( PLATFORM_DEPENDENT "macos-${EXT_ARCH}-${EXT_COMP}${EXT_COMP_VER}" )


# Emscripten
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set( PLATFORM_DEPENDENT "emscripten" )

endif()


if (NOT DEFINED PLATFORM_DEPENDENT)
	message( FATAL_ERROR "Unsupported platform '${CMAKE_SYSTEM_NAME}' or compiler '${CMAKE_CXX_COMPILER_ID}' with version '${CMAKE_CXX_COMPILER_VERSION}'" )
endif()

#if (NOT EXISTS "${EXTERNAL_FOLDER}/${PLATFORM_DEPENDENT}")
#	message( FATAL_ERROR "platform specific folder '${EXTERNAL_FOLDER}/${PLATFORM_DEPENDENT}' is not exists" )
#endif()

add_subdirectory( "${EXTERNAL_FOLDER}/${PLATFORM_DEPENDENT}" "${PLATFORM_DEPENDENT}" )
add_subdirectory( "${EXTERNAL_FOLDER}/shared" "shared" )
