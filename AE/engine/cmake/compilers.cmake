# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# Build config:
#	- Debug    -- enable all checks
#	- Develop  -- enable some optimization but keep most debug checks, exclude long-time checks
#	- Profile  -- enable some optimizations but keep debug info for profiling
#	- Release  -- enable all optimizations
#
# feature support:
#	https://en.cppreference.com/w/cpp/compiler_support

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

# options:
#	AE_ENABLE_COMPILER_WARNINGS		BOOL :		TRUE - for engine,	FALSE - for external projects
#	AE_USE_SANITIZER				BOOL
#	AE_DISABLE_THREADS				BOOL :		for emscripten
#	AE_ENABLE_EXCEPTIONS			BOOL :		TRUE - enable,		FALSE - disable exception and RTTI
#	AE_ENABLE_LOGS					BOOL
#	AE_SIMD_AVX						STRING :	0, 1, 2, 3(AVX512)
#	AE_SIMD_SSE						STRING :	0, 20, 30, 31, 41, 42
#	AE_SIMD_AES						STRING :	0, 1


# detect target platform
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set( TARGET_PLATFORM "PLATFORM_LINUX" CACHE INTERNAL "" FORCE )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	set( TARGET_PLATFORM "PLATFORM_ANDROID" CACHE INTERNAL "" FORCE )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set( TARGET_PLATFORM "PLATFORM_MACOS" CACHE INTERNAL "" FORCE )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
	set( TARGET_PLATFORM "PLATFORM_IOS" CACHE INTERNAL "" FORCE ) # TODO: check

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set( TARGET_PLATFORM "PLATFORM_WINDOWS" CACHE INTERNAL "" FORCE )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
	set( TARGET_PLATFORM "PLATFORM_EMSCRIPTEN" CACHE INTERNAL "" FORCE )

elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD") OR (${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly"))
	set( TARGET_PLATFORM "PLATFORM_BSD" CACHE INTERNAL "" FORCE )

else ()
	message( FATAL_ERROR "unsupported platform ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_VERSION}" )
endif()
message( STATUS "TARGET_PLATFORM: ${TARGET_PLATFORM}" )
set( "AE_TARGET_${TARGET_PLATFORM}" ON CACHE INTERNAL "" FORCE )


# detect target platform bits
if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set( PLATFORM_BITS 64 CACHE INTERNAL "" FORCE )
elseif (${CMAKE_SIZEOF_VOID_P} EQUAL 4)
	set( PLATFORM_BITS 32 CACHE INTERNAL "" FORCE )
else ()
	message( FATAL_ERROR "unsupported platform bits!" )
endif()


# detect cpu architecture
if (MSVC)
	if (NOT DEFINED CMAKE_GENERATOR_PLATFORM)
		set( CMAKE_GENERATOR_PLATFORM "X64" )
	endif()
	string( TOUPPER ${CMAKE_GENERATOR_PLATFORM} PLATFORM_NAME )
	if ( (DEFINED PLATFORM_NAME) AND (NOT (PLATFORM_NAME STREQUAL "")) )
		if (${PLATFORM_NAME} STREQUAL "ARM64")
			set( TARGET_CPU_ARCH "ARM64" )
		elseif (${PLATFORM_NAME} STREQUAL "ARM")
			set( TARGET_CPU_ARCH "ARM32" )
		elseif (${PLATFORM_NAME} STREQUAL "WIN32")
			set( TARGET_CPU_ARCH "X86" )
		elseif (${PLATFORM_NAME} STREQUAL "X64")
			set( TARGET_CPU_ARCH "X64" )
		else()
			message( FATAL_ERROR "unknown platform '${CMAKE_GENERATOR_PLATFORM}'" )
		endif()
	else()
		set( TARGET_CPU_ARCH "X64" CACHE INTERNAL "" FORCE )
	endif()
else()
	string( TOUPPER ${CMAKE_SYSTEM_PROCESSOR} PLATFORM_NAME )
	if (${PLATFORM_NAME} STREQUAL "AMD64")
		set( TARGET_CPU_ARCH "X64" )
	elseif (${PLATFORM_NAME} STREQUAL "X86_64")
		set( TARGET_CPU_ARCH "X64" )
	elseif (${PLATFORM_NAME} STREQUAL "X86")
		set( TARGET_CPU_ARCH "X86" )
	elseif (${PLATFORM_NAME} STREQUAL "ARMV7-A")
		set( TARGET_CPU_ARCH "ARM32" )
	elseif (${PLATFORM_NAME} STREQUAL "AARCH64")
		set( TARGET_CPU_ARCH "ARM64" )
	elseif (${PLATFORM_NAME} STREQUAL "ARM64")
		set( TARGET_CPU_ARCH "ARM64" )
	elseif (${PLATFORM_NAME} STREQUAL "I686")
		set( TARGET_CPU_ARCH "i686" )	# P6 microarchitecture
	else()
		message( FATAL_ERROR "unknown processor '${CMAKE_SYSTEM_PROCESSOR}'" )
	endif()
endif()
message( STATUS "TARGET_CPU_ARCH: ${TARGET_CPU_ARCH}" )
set( "AE_CPU_ARCH_${TARGET_CPU_ARCH}" ON CACHE INTERNAL "" FORCE )


# default compiler flags
set( PROJECTS_SHARED_DEFINES
	 "AE_${TARGET_PLATFORM}" "AE_PLATFORM_NAME=\"${CMAKE_SYSTEM_NAME}\"" "AE_PLATFORM_BITS=${PLATFORM_BITS}"
	 "AE_CPU_ARCH_${TARGET_CPU_ARCH}" "AE_CPU_ARCH_NAME=\"${TARGET_CPU_ARCH}\"" )

if (${AE_ENABLE_EXCEPTIONS})
	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_ENABLE_RTTI" "AE_ENABLE_EXCEPTIONS" )
endif()
if (${AE_ENABLE_LOGS})
	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_ENABLE_LOGS" )
endif()

set( CMAKE_CONFIGURATION_TYPES Release Profile Develop Debug )
set( CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING "Configurations" FORCE )

set( PROJECTS_SHARED_DEFINES_RELEASE "AE_CFG_RELEASE"	"AE_RELEASE")
set( PROJECTS_SHARED_DEFINES_PROFILE "AE_CFG_PROFILE"	"AE_RELEASE")
set( PROJECTS_SHARED_DEFINES_DEVELOP "AE_CFG_DEVELOP"	"AE_DEBUG"	)
set( PROJECTS_SHARED_DEFINES_DEBUG   "AE_CFG_DEBUG"		"AE_DEBUG"	)


#==================================================================================================
# setup SIMD
#==================================================================================================
if ( (${TARGET_CPU_ARCH} STREQUAL "X64") OR (${TARGET_CPU_ARCH} STREQUAL "X86") )
	set( AE_SIMD_AVX "0" CACHE STRING "AVX version: 0, 1, 2, 3(AVX512)" )
	set( AE_SIMD_SSE "0" CACHE STRING "SSE version: 0, 20, 30, 31, 41, 42" )
	set( AE_SIMD_AES "0" CACHE STRING "AES version: 0, 1" )

	if (${AE_SIMD_AVX} GREATER 0)
		set( AE_SIMD_SSE "42" CACHE INTERNAL "" FORCE )
		set( AE_SIMD_AES "1"  CACHE INTERNAL "" FORCE )
	endif()
	message( STATUS "AE_SIMD_AVX: ${AE_SIMD_AVX}" )
	message( STATUS "AE_SIMD_SSE: ${AE_SIMD_SSE}" )
	message( STATUS "AE_SIMD_AES: ${AE_SIMD_AES}" )

	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_SIMD_AVX=${AE_SIMD_AVX}" "AE_SIMD_SSE=${AE_SIMD_SSE}" "AE_SIMD_AES=${AE_SIMD_AES}" )

	if ( MSVC )
		if (${AE_SIMD_AVX} EQUAL 3)
			set( COMPILER_FLAGS ${COMPILER_FLAGS} /arch:AVX512 )
		elseif (${AE_SIMD_AVX} EQUAL 2)
			set( COMPILER_FLAGS ${COMPILER_FLAGS} /arch:AVX2 )
		elseif (${AE_SIMD_AVX} EQUAL 1)
			set( COMPILER_FLAGS ${COMPILER_FLAGS} /arch:AVX )
			set( AE_SIMD_SSE "42" )
		elseif (${AE_SIMD_AVX} EQUAL 0)
			if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
				# SSE2 enabled by default for x64
			elseif (${AE_SIMD_SSE} GREATER_EQUAL 20)
				set( COMPILER_FLAGS ${COMPILER_FLAGS} /arch:SSE2 )	# x86 only
			elseif (${AE_SIMD_SSE} EQUAL 0)
			else()
				message( FATAL_ERROR "unsupported AE_SIMD_SSE flags: ${AE_SIMD_SSE}" )
			endif()
		else()
			message( FATAL_ERROR "unsupported AE_SIMD_AVX flags: ${AE_SIMD_AVX}" )
		endif()
	else()
		if (${AE_SIMD_AVX} EQUAL 2)
			set( COMPILER_FLAGS ${COMPILER_FLAGS} -mavx2 )
		elseif (${AE_SIMD_AVX} EQUAL 1)
			set( COMPILER_FLAGS ${COMPILER_FLAGS} -mavx -mno-avx2 )
		elseif (${AE_SIMD_AVX} EQUAL 0)
			set( COMPILER_FLAGS ${COMPILER_FLAGS} -mno-avx )
			# SSE
			if (${AE_SIMD_SSE} EQUAL 42)
				set( COMPILER_FLAGS ${COMPILER_FLAGS} -msse4.2 )
			elseif (${AE_SIMD_SSE} EQUAL 41)
				set( COMPILER_FLAGS ${COMPILER_FLAGS} -msse4.1 )
			elseif (${AE_SIMD_SSE} EQUAL 31)
				set( COMPILER_FLAGS ${COMPILER_FLAGS} -mssse3 )
			elseif (${AE_SIMD_SSE} EQUAL 30)
				set( COMPILER_FLAGS ${COMPILER_FLAGS} -msse3 )
			elseif (${AE_SIMD_SSE} EQUAL 20)
				set( COMPILER_FLAGS ${COMPILER_FLAGS} -msse2 )
			elseif (${AE_SIMD_SSE} EQUAL 0)
			#	set( COMPILER_FLAGS ${COMPILER_FLAGS} -mno-sse )	# failed to compile on Mac
			else()
				message( FATAL_ERROR "unsupported AE_SIMD_SSE flags: ${AE_SIMD_SSE}" )
			endif()
		else()
			message( FATAL_ERROR "unsupported AE_SIMD_AVX flags: ${AE_SIMD_AVX}" )
		endif()
	endif()

elseif ( (${TARGET_CPU_ARCH} STREQUAL "ARM64") OR (${TARGET_CPU_ARCH} STREQUAL "ARM32") )
	# AE_SIMD_NEON defined in source
elseif (${TARGET_CPU_ARCH} STREQUAL "i686")
	# no SIMD
else()
	message( FATAL_ERROR "unknown platform '${TARGET_CPU_ARCH}' for SIMD flags" )
endif()


#==================================================================================================
# Visual Studio Compilation settings
#==================================================================================================
set( COMPILER_MSVC OFF )
set( COMPILER_MSVC_CLANG OFF )
if ( MSVC )
	if (DEFINED DETECTED_COMPILER)
		message( FATAL_ERROR "multiple compiler types detected, previous: '${DETECTED_COMPILER}'" )
	endif()

	string( FIND "${CMAKE_CXX_COMPILER_ID}" "MSVC" outPos )
	if ( (outPos GREATER -1) )
		set( COMPILER_MSVC ON )
		set( DETECTED_COMPILER "COMPILER_MSVC" )
	endif()
	string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
	if ( (outPos GREATER -1) )
		set( COMPILER_MSVC_CLANG ON )
		set( DETECTED_COMPILER "COMPILER_MSVC_CLANG" )
	endif()

	set( TEMP_CXX_FLAGS ${CMAKE_CXX_FLAGS} )
	string( REPLACE "/GR-"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/GR"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" ) # RTTI
	string( REPLACE "/EHa-"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/EHa"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" ) # catches both structured (asynchronous) and standard C++ (synchronous) exceptions
	string( REPLACE "/EHsc-"				" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/EHsc"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" ) # functions declared as extern "C" never throw a C++ exception.
	string( REPLACE "/EHc-"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/EHc"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" ) # ignored without /EHs
	string( REPLACE "/EHs-"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/EHs"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" ) # catches only standard C++ exceptions, functions declared as extern "C" may throw a C++ exception.
	string( REPLACE "/EHr-"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/EHr"					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" ) # forcing compiler to generate runtime checks for unhandled exceptions that escape a noexcept function
	string( REPLACE "/D_HAS_EXCEPTIONS=0"	" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "/D_HAS_EXCEPTIONS=1"	" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )
	string( REPLACE "   "					" " TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS}" )

	if (${AE_ENABLE_EXCEPTIONS})
		set( TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS} /D_HAS_EXCEPTIONS=1 /EHsc /GR" )
	else ()
		# disable c++ exceptions
		# _HAS_EXCEPTIONS=0 means 'there are no exceptions' and that the standard library can assume no exceptions.
		# The standard library team does not test any _HAS_EXCEPTIONS=0 configuration.
		set( TEMP_CXX_FLAGS "${TEMP_CXX_FLAGS} /D_HAS_EXCEPTIONS=0 /EHs- /GR-" )  # /EHc- /EHr- are not needed
	endif()
	#--------------------------------------------

	set( CURRENT_C_FLAGS ${CMAKE_C_FLAGS} CACHE STRING "" FORCE )
	set( CURRENT_CXX_FLAGS "${TEMP_CXX_FLAGS}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS "${TEMP_CXX_FLAGS}" CACHE STRING "" FORCE )
	set( CURRENT_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} CACHE STRING "" FORCE )
	set( CURRENT_STATIC_LINKER_FLAGS ${CMAKE_STATIC_LINKER_FLAGS} CACHE STRING "" FORCE )
	set( CURRENT_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} CACHE STRING "" FORCE )

	set( CONFIGURATION_DEPENDENT_PATH ON CACHE INTERNAL "" FORCE )

	#--------------------------------------------
	if (NOT CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION)
		message( FATAL_ERROR "CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION is not defined, Windows SDK is required" )
	endif()
	string( FIND "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}" "." outLength )
	string( SUBSTRING "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}" 0 ${outLength}  WINDOWS_TARGET_VERSION_MAJ )
	MATH( EXPR outLength "${outLength}+1" )
	string( SUBSTRING "${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}" ${outLength} -1 WINDOWS_TARGET_VERSION_MIN2 )
	string( FIND "${WINDOWS_TARGET_VERSION_MIN2}" "." outLength )
	MATH( EXPR outLength "${outLength}+1" )
	string( SUBSTRING "${WINDOWS_TARGET_VERSION_MIN2}" ${outLength} -1 WINDOWS_TARGET_VERSION_MIN3 )
	string( FIND "${WINDOWS_TARGET_VERSION_MIN3}" "." outLength )
	string( SUBSTRING "${WINDOWS_TARGET_VERSION_MIN3}" 0 ${outLength}  WINDOWS_TARGET_VERSION_MIN )
	#--------------------------------------------
	message( STATUS "CMAKE_C_FLAGS: ${CURRENT_C_FLAGS}" )
	message( STATUS "CMAKE_CXX_FLAGS: ${CURRENT_CXX_FLAGS}" )
	message( STATUS "CMAKE_EXE_LINKER_FLAGS: ${CURRENT_EXE_LINKER_FLAGS}" )
	message( STATUS "CMAKE_STATIC_LINKER_FLAGS: ${CURRENT_STATIC_LINKER_FLAGS}" )
	message( STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CURRENT_SHARED_LINKER_FLAGS}" )

	set( MSVC_WARNING_LIST
		/W4 /WX-
		# errors
		/we4002 /we4099 /we4129 /we4130 /we4172 /we4201 /we4238 /we4239 /we4240 /we4251 /we4263 /we4264 /we4266 /we4273 /we4293
		/we4305 /we4390 /we4455 /we4456 /we4457 /we4458 /we4459 /we4473 /we4474 /we4522 /we4552 /we4553 /we4554 /we4700 /we4706 /we4715 /we4716 /we4717
		/we4927 /we5062 /we5054 /we4565 /we5054 /we4291 /we4297 /we4584 /we4566
		# disable warnings
		/wd4061 /wd4062 /wd4063 /wd4310 /wd4324 /wd4365 /wd4503 /wd4514 /wd4530 /wd4623 /wd4625 /wd4626 /wd4710 /wd4714 /wd5026 /wd5027
	)
	if (${AE_ENABLE_COMPILER_WARNINGS})
		set( MSVC_WARNING_LIST ${MSVC_WARNING_LIST}
			# warnings
			/w14834 /w14018 /w14127 /w14189 /w14244 /w14245 /w14287 /w14389 /w14505 /w14668 /w14701 /w14702 /w14703 /w14838 /w14946 /w14996 /w15038
		)
	else()
		set( MSVC_WARNING_LIST ${MSVC_WARNING_LIST}
			# disable warnings
			/wd4267 /wd4100 /wd4127 /wd4996
		)
	endif()

	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES}
		 "_SILENCE_CXX20_CISO646_REMOVED_WARNING"
		 "_SILENCE_ALL_CXX23_DEPRECATION_WARNINGS"	# for Abseil
		 "AE_COMPILER_MSVC" "AE_PLATFORM_TARGET_VERSION_MAJOR=${WINDOWS_TARGET_VERSION_MAJ}" "AE_PLATFORM_TARGET_VERSION_MINOR=${WINDOWS_TARGET_VERSION_MIN}"
		 "UNICODE=1" )

	if (${COMPILER_MSVC_CLANG})
		set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_COMPILER_CLANG_CL" )
		set( MSVC_WARNING_LIST ${MSVC_WARNING_LIST}
			# errors
			-Werror=deprecated-copy -Werror=unknown-attributes -Werror=self-assign -Werror=self-move -Werror=conditional-uninitialized -Werror=uninitialized
			-Werror=init-self -Werror=range-loop-bind-reference -Werror=implicit-exception-spec-mismatch -Werror=return-stack-address -Werror=address
			-Werror=unsupported-friend -Werror=unknown-warning-option -Werror=user-defined-literals -Werror=instantiation-after-specialization
			-Werror=keyword-macro -Werror=large-by-value-copy -Werror=method-signatures -Werror=infinite-recursion -Werror=pessimizing-move -Werror=dangling-else
			-Werror=return-std-move -Werror=deprecated-increment-bool -Werror=abstract-final-class -Werror=parentheses -Werror=return-type -Werror=array-bounds
			-Werror=div-by-zero -Werror=missing-field-initializers -Werror=cast-qual -Werror=cast-align -Werror=invalid-pch -Werror=defaulted-function-deleted
			-Werror=ignored-qualifiers -Werror=microsoft-template -Werror=nonportable-include-path -Werror=inconsistent-missing-override
			-Werror=microsoft-cast -Werror=invalid-token-paste -Werror=sign-compare -Werror=bitwise-instead-of-logical
			# warnings
			-Wunused-parameter -Wnarrowing -Wlogical-op-parentheses  -Wunused  -Wloop-analysis -Wincrement-bool -Wc++14-extensions -Wc++17-extensions
			-Wunused-private-field -Wdelete-non-virtual-dtor -Wrange-loop-analysis -Wundefined-bool-conversion -Wincrement-bool
			-Wunused-lambda-capture -Wundef -Wformat-security
			-Wdouble-promotion -Wchar-subscripts -Wformat -Wmain -Wmissing-braces  -Wmissing-include-dirs -Wunknown-pragmas -Wpragmas -Wstrict-overflow
			-Wstrict-aliasing -Wendif-labels -Wpointer-arith -Wwrite-strings -Wconversion-null -Wenum-compare -Wsizeof-pointer-memaccess
			# disable warnings
			-Wno-comment -Wno-ambiguous-reversed-operator -Wno-unneeded-internal-declaration -Wno-undefined-inline
			-Wno-unused-function -Wno-unused-const-variable -Wno-unused-local-typedef -Wno-switch
		)
	else()
		set( COMPILER_FLAGS ${COMPILER_FLAGS} /fp:strict /fp:except- )
	endif()

	if (${COMPILER_MSVC_CLANG} AND NOT ${AE_ENABLE_COMPILER_WARNINGS})
		set( CURRENT_C_FLAGS "${CURRENT_C_FLAGS} -Wno-comment -Wno-ambiguous-reversed-operator -Wno-unneeded-internal-declaration -Wno-undefined-inline -Wno-unused-function -Wno-unused-const-variable -Wno-unused-local-typedef -Wno-switch -Wno-deprecated-copy-with-user-provided-copy -Wno-unknown-argument -Wno-deprecated-declarations -Wno-deprecated-non-prototype -Wno-deprecated-copy" )
		set( CURRENT_CXX_FLAGS "${CURRENT_CXX_FLAGS} -Wno-comment -Wno-ambiguous-reversed-operator -Wno-unneeded-internal-declaration -Wno-undefined-inline -Wno-unused-function -Wno-unused-const-variable -Wno-unused-local-typedef -Wno-switch -Wno-deprecated-copy-with-user-provided-copy -Wno-unknown-argument -Wno-deprecated-declarations -Wno-deprecated-non-prototype -Wno-deprecated-copy" )
	endif()

	set( MSVC_SHARED_OPTS /std:c++latest /MP /Gm- /Zc:inline /Gy- /JMC /volatile:iso
		 ${COMPILER_FLAGS} ${MSVC_WARNING_LIST} )

	if (${AE_USE_SANITIZER})
		set( MSVC_SHARED_OPTS ${MSVC_SHARED_OPTS} /fsanitize=address )
	endif()

	set( MSVC_SHARED_OPTS_DBG ${MSVC_SHARED_OPTS} )
	if (${AE_ENABLE_COMPILER_WARNINGS})
		list( APPEND MSVC_SHARED_OPTS_DBG /w14100 )
	endif()

	# Release
	set( CMAKE_C_FLAGS_RELEASE "${CURRENT_C_FLAGS} /D_NDEBUG /DNDEBUG /MT /Ox /MP " CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "${CURRENT_CXX_FLAGS} /D_NDEBUG /DNDEBUG /MT /Ox /MP " CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CURRENT_EXE_LINKER_FLAGS} /LTCG /RELEASE " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CURRENT_STATIC_LINKER_FLAGS} /LTCG " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CURRENT_SHARED_LINKER_FLAGS} /LTCG /RELEASE " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_RELEASE ${MSVC_SHARED_OPTS} /Ob2 /Oi /Ot /Oy /GT /GL /GF /GS- /Ox /analyze- CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_RELEASE " /OPT:REF /OPT:ICF /LTCG /RELEASE /DYNAMICBASE" CACHE INTERNAL "" FORCE )
	# Profile
	set( CMAKE_C_FLAGS_PROFILE "${CURRENT_C_FLAGS} /D_NDEBUG /DNDEBUG /MT /Ox /MP " CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_PROFILE "${CURRENT_CXX_FLAGS} /D_NDEBUG /DNDEBUG /MT /Ox /Zi /MP " CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_PROFILE "${CURRENT_EXE_LINKER_FLAGS} /LTCG /DEBUG /PROFILE " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CURRENT_STATIC_LINKER_FLAGS} /LTCG " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CURRENT_SHARED_LINKER_FLAGS} /LTCG /DEBUG /PROFILE " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_PROFILE ${MSVC_SHARED_OPTS_DBG} /Ob2 /Oi /Ot /Oy /GT /GL /GF /GS- /Ox /analyze- /Zi CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_PROFILE " /OPT:REF /OPT:ICF /LTCG /DEBUG /PROFILE" CACHE INTERNAL "" FORCE )
	# Develop
	set( CMAKE_C_FLAGS_DEVELOP "${CURRENT_C_FLAGS} /D_NDEBUG /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0 /MT /Od /MP " CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEVELOP "${CURRENT_CXX_FLAGS} /D_NDEBUG /DNDEBUG /D_ITERATOR_DEBUG_LEVEL=0 /MT /Od /Zi /MP " CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CURRENT_EXE_LINKER_FLAGS} /LTCG /DEBUG " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CURRENT_STATIC_LINKER_FLAGS} /LTCG " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CURRENT_SHARED_LINKER_FLAGS} /LTCG /DEBUG " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEVELOP ${MSVC_SHARED_OPTS_DBG} /Ob2 /Oi /Ot /Oy /GT /GL /GF /GS- /Od /analyze- /Zi CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEVELOP " /OPT:REF /OPT:ICF /LTCG /DEBUG" CACHE INTERNAL "" FORCE )
	# Debug
	set( CMAKE_C_FLAGS_DEBUG "${CURRENT_C_FLAGS} /D_DEBUG /D_ITERATOR_DEBUG_LEVEL=${AE_ITERATOR_DEBUG_LEVEL} /MTd /Od /MP " CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEBUG "${CURRENT_CXX_FLAGS} /D_DEBUG /D_ITERATOR_DEBUG_LEVEL=${AE_ITERATOR_DEBUG_LEVEL} /MTd /Od /Zi /MP " CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CURRENT_EXE_LINKER_FLAGS} /DEBUG:FULL " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CURRENT_SHARED_LINKER_FLAGS} /DEBUG:FULL " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEBUG ${MSVC_SHARED_OPTS_DBG} /sdl /Od /Ob0 /Oy- /GF- /GS /analyze- /Zi /RTC1 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEBUG " /OPT:REF /OPT:ICF /INCREMENTAL:NO /DEBUG:FULL" CACHE INTERNAL "" FORCE )
endif()


#==================================================================================================
# GCC/Clang settings:
#	global - only for external projects
#	local - only for AE projects
#==================================================================================================
set( GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX "-Wno-unused -Wno-switch -Wno-undef -Wno-comment -fPIC" )
set( GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX  -Wdouble-promotion -Wchar-subscripts -Wformat -Wmain -Wmissing-braces -Werror=uninitialized -Wmissing-include-dirs -Wunknown-pragmas -Wpragmas -Wstrict-overflow -Wstrict-aliasing -Wendif-labels -Wpointer-arith -Wwrite-strings -Wconversion-null -Wenum-compare -Wsign-compare -Wno-unused -Wsizeof-pointer-memaccess -Wno-zero-as-null-pointer-constant -Wundef -Werror=init-self -Werror=parentheses -Werror=return-type -Werror=array-bounds -Werror=div-by-zero -Werror=missing-field-initializers -Werror=cast-qual -Werror=cast-align -Wno-switch -Werror=invalid-pch -Wformat-security -fvisibility-inlines-hidden -fvisibility=hidden -fPIC )

if (${AE_ENABLE_EXCEPTIONS})
	set( GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_CXX "${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX} -frtti -fexceptions" )
	set( GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX  ${GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX}     -frtti -fexceptions )
else()
	# TODO: -fno-unwind-tables -fno-asynchronous-unwind-tables
	set( GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_CXX "${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX} -fno-rtti -fno-exceptions" )
	set( GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX  ${GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX}     -fno-rtti -fno-exceptions )
endif()

# TODO:
#	-ffast-math
#	-ffp-contract=fast
#	-fvisibility-inlines-hidden -fvisibility=hidden
#	-ffunction-sections -fdata-sections
#	-no-canonical-prefixes


#==================================================================================================
# GCC Compilation settings
# https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html
#==================================================================================================
set( COMPILER_GCC OFF )
string( FIND "${CMAKE_CXX_COMPILER_ID}" "GNU" outPos )
if ( (outPos GREATER -1) )
	set( COMPILER_GCC ON )
endif()
if ( COMPILER_GCC )
	if (DEFINED DETECTED_COMPILER)
		message( FATAL_ERROR "multiple compiler types detected, previous: '${DETECTED_COMPILER}'" )
	endif()
	set( DETECTED_COMPILER "COMPILER_GCC" )
	#--------------------------------------------
	set( CONFIGURATION_DEPENDENT_PATH OFF CACHE INTERNAL "" FORCE )

	# -Wno-shadow -Wno-enum-compare -Wno-narrowing -Wno-attributes
	set( GCC_SHARED_OPTS         ${COMPILER_FLAGS} -Wmaybe-uninitialized -Wfree-nonheap-object -Wcast-align -Wlogical-op -Waddress -Wno-non-template-friend -Werror=return-local-addr -Werror=placement-new -Werror=sign-compare -Werror=literal-suffix -Werror=shadow=local -Werror=delete-incomplete -Werror=odr -Werror=subobject-linkage -Werror=multichar -Winvalid-offsetof ${GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX} )
	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_COMPILER_GCC" )

	# Release  TODO: -Ofast ?
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>: > )
	set( CMAKE_C_FLAGS_RELEASE "-O3 -finline-functions ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-O3 -finline-functions ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_RELEASE  ${GCC_SHARED_OPTS} -O3 -Ofast -fomit-frame-pointer -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_RELEASE " -static-libgcc -static-libstdc++" CACHE INTERNAL "" FORCE )
	# Profile
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Profile>: > )
	set( CMAKE_C_FLAGS_PROFILE "-O2 ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_PROFILE "-O2 ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_PROFILE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_PROFILE  ${GCC_SHARED_OPTS} -O2 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_PROFILE " -static-libgcc -static-libstdc++" CACHE INTERNAL "" FORCE )
	# Develop
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Develop>: > )
	set( CMAKE_C_FLAGS_DEVELOP "-O2 ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEVELOP "-O2 ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEVELOP  ${GCC_SHARED_OPTS} -g -ggdb -O2 -Wno-terminate  CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEVELOP " -static-libgcc -static-libstdc++" CACHE INTERNAL "" FORCE )
	# Debug
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>: > )
	set( CMAKE_C_FLAGS_DEBUG "-O0 ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEBUG  ${GCC_SHARED_OPTS} -g -ggdb -O0 -Wno-terminate  CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEBUG " -static-libgcc -static-libstdc++" CACHE INTERNAL "" FORCE )
endif()


#==================================================================================================
# Clang shared settings
# https://clang.llvm.org/docs/DiagnosticsReference.html
#==================================================================================================
set( CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX "${GCC_CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX} -Wnarrowing -stdlib=libc++" ) # -Wno-deprecated-builtins
set( CLANG_SHARED_LOCAL_WARNING_LIST_CXX  ${GCC_CLANG_SHARED_LOCAL_WARNING_LIST_CXX} -Wnarrowing -Wlogical-op-parentheses  -Wunused -Werror=conditional-uninitialized -Wloop-analysis -Wincrement-bool -Wno-undefined-inline -Wc++14-extensions -Wc++17-extensions -Wno-comment -Wunused-private-field -Werror=return-stack-address -Werror=address -Werror=unsupported-friend -Werror=unknown-warning-option -Werror=user-defined-literals -Werror=instantiation-after-specialization -Werror=keyword-macro -Werror=large-by-value-copy -Werror=method-signatures -Werror=self-assign -Werror=self-move -Werror=infinite-recursion -Werror=pessimizing-move -Werror=dangling-else -Werror=return-std-move -Werror=deprecated-increment-bool -Werror=abstract-final-class -Wno-ambiguous-reversed-operator -Wno-unneeded-internal-declaration -Wno-unused-function -Wno-unused-const-variable -Wno-unused-local-typedef -Wdelete-non-virtual-dtor -Wrange-loop-analysis -Wundefined-bool-conversion -Winconsistent-missing-override -Wincrement-bool -Wunused-lambda-capture -fno-short-enums -Werror=implicit-exception-spec-mismatch -Werror=range-loop-bind-reference )

#==================================================================================================
# Clang Compilation settings
#==================================================================================================
set( COMPILER_CLANG OFF )
string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( (outPos GREATER -1) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Linux") )
	set( COMPILER_CLANG ON )
endif()
if ( COMPILER_CLANG )
	if (DEFINED DETECTED_COMPILER)
		message( FATAL_ERROR "multiple compiler types detected, previous: '${DETECTED_COMPILER}'" )
	endif()
	set( DETECTED_COMPILER "COMPILER_CLANG" )
	#--------------------------------------------
	set( CONFIGURATION_DEPENDENT_PATH OFF CACHE INTERNAL "" FORCE )
	#--------------------------------------------

	set( CLANG_SHARED_OPTS       ${COMPILER_FLAGS} ${CLANG_SHARED_LOCAL_WARNING_LIST_CXX} )
	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_COMPILER_CLANG" )

	if (${AE_USE_SANITIZER})
		set( CLANG_SHARED_OPTS ${CLANG_SHARED_OPTS} -fsanitize=address )
		#  -fsanitize=thread -fsanitize=undefined
	endif()

	# Release
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>: > )
	set( CMAKE_C_FLAGS_RELEASE "-O3 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-O3 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_RELEASE  ${CLANG_SHARED_OPTS} -O3 -Ofast -fomit-frame-pointer -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "" FORCE )
	# Profile
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Profile>: > )
	set( CMAKE_C_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_PROFILE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_PROFILE  ${CLANG_SHARED_OPTS} -O2 -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_PROFILE "" CACHE INTERNAL "" FORCE )
	# Develop
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Develop>: > )
	set( CMAKE_C_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEVELOP  ${CLANG_SHARED_OPTS} -g -glldb -O2 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEVELOP "" CACHE INTERNAL "" FORCE )
	# Debug
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>: > )
	set( CMAKE_C_FLAGS_DEBUG "-O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEBUG  ${CLANG_SHARED_OPTS} -g -glldb -O0 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "" FORCE )
endif()


#==================================================================================================
# Emscripten Clang Compilation settings
#==================================================================================================
set( COMPILER_CLANG_EMSCRIPTEN OFF )
string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( (outPos GREATER -1) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten") )
	set( COMPILER_CLANG_EMSCRIPTEN ON )
endif()
if ( COMPILER_CLANG_EMSCRIPTEN )
	if (DEFINED DETECTED_COMPILER)
		message( FATAL_ERROR "multiple compiler types detected, previous: '${DETECTED_COMPILER}'" )
	endif()
	set( DETECTED_COMPILER "COMPILER_CLANG_EMSCRIPTEN" )
	#--------------------------------------------
	set( CONFIGURATION_DEPENDENT_PATH OFF CACHE INTERNAL "" FORCE )
	#--------------------------------------------

	set( CLANG_SHARED_OPTS ${COMPILER_FLAGS} ${CLANG_SHARED_LOCAL_WARNING_LIST_CXX} -pthread )
	set( CURRENT_EXE_LINKER_FLAGS "-s WASM=1" )
	#set( CURRENT_EXE_LINKER_FLAGS "-s DISABLE_EXCEPTION_CATCHING=0" )						# exceptions are turned off by default in -O1 and above
	#set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=0 -s INITIAL_MEMORY=268435456" )
	set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -s ALLOW_MEMORY_GROWTH=1" )
	set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -s MAX_WEBGL_VERSION=2" )

	set( AE_DISABLE_THREADS		ON  CACHE BOOL "disable std::thread for compatibility with emscripten" )
	set( AE_EMS_NATIVE_SOCKETS	OFF CACHE BOOL "use native WebSockets, otherwise - emulate POSIX sockets" )

	if (${AE_DISABLE_THREADS})
		set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_DISABLE_THREADS" )
	else()
		#set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -s USE_PTHREADS -s PTHREAD_POOL_SIZE=1" )
		set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -s USE_PTHREADS -s PROXY_TO_PTHREAD" )
	endif()

	if (${AE_EMS_NATIVE_SOCKETS})
		set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_EMS_NATIVE_SOCKETS" )
		set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -lwebsocket.js" )
	else()
		if (${AE_DISABLE_THREADS})
			message( FATAL_ERROR "POSIX sockets requires '-s USE_PTHREADS -s PROXY_TO_PTHREAD' flags" )
		endif()
		set( CURRENT_EXE_LINKER_FLAGS "${CURRENT_EXE_LINKER_FLAGS} -lwebsocket.js -sPROXY_POSIX_SOCKETS" )
	endif()

	set( CURRENT_EXE_LINKER_FLAGS_DBG "${CURRENT_EXE_LINKER_FLAGS} -s DEMANGLE_SUPPORT=1 --source-map-base http://localhost:9090/_build_ems/bin/" )

	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES}
		 "AE_COMPILER_CLANG" "AE_PLATFORM_TARGET_VERSION_MAJOR=1" "AE_PLATFORM_TARGET_VERSION_MINOR=0" )

	# Release
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>: > )
	set( CMAKE_C_FLAGS_RELEASE "-O3 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-O3 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_RELEASE ${CLANG_SHARED_OPTS} -O3 -Ofast -fomit-frame-pointer -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "" FORCE )
	# Profile
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Profile>: > )
	set( CMAKE_C_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_PROFILE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_PROFILE ${CLANG_SHARED_OPTS} -O2 -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_PROFILE "" CACHE INTERNAL "" FORCE )
	# Develop
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Develop>: > )
	set( CMAKE_C_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEVELOP ${CLANG_SHARED_OPTS} -O2 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEVELOP "" CACHE INTERNAL "" FORCE )
	# Debug
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>: > )
	set( CMAKE_C_FLAGS_DEBUG "-g2 -gsource-map -O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEBUG "-g2 -gsource-map  -O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CURRENT_EXE_LINKER_FLAGS_DBG} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEBUG ${CLANG_SHARED_OPTS} -O0 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "" FORCE )
endif()


#==================================================================================================
# Apple Clang Compilation settings
#==================================================================================================
set( COMPILER_CLANG_APPLE OFF )
string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( (outPos GREATER -1) AND ((${CMAKE_SYSTEM_NAME} STREQUAL "Darwin") OR (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")) )
	set( COMPILER_CLANG_APPLE ON )
endif()
if ( COMPILER_CLANG_APPLE )
	if (DEFINED DETECTED_COMPILER)
		message( FATAL_ERROR "multiple compiler types detected, previous: '${DETECTED_COMPILER}'" )
	endif()
	set( DETECTED_COMPILER "COMPILER_CLANG_APPLE" )
	#--------------------------------------------
	set( CONFIGURATION_DEPENDENT_PATH ON CACHE INTERNAL "" FORCE )
	#--------------------------------------------
	if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
		message( FATAL_ERROR "CMAKE_OSX_DEPLOYMENT_TARGET (${CMAKE_OSX_DEPLOYMENT_TARGET}) is not defined" )
	endif()
	string( FIND "${CMAKE_OSX_DEPLOYMENT_TARGET}" "." outLength )
	string( SUBSTRING "${CMAKE_OSX_DEPLOYMENT_TARGET}" 0 ${outLength}  APPLE_VER_MAJ )
	string( SUBSTRING "${CMAKE_OSX_DEPLOYMENT_TARGET}" ${outLength} -1 APPLE_VER_MIN )
	#--------------------------------------------

	set( CLANG_SHARED_OPTS ${COMPILER_FLAGS} ${CLANG_SHARED_LOCAL_WARNING_LIST_CXX} -Werror=objc-method-access -Werror=unguarded-availability )

	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES}
		 "AE_COMPILER_CLANG" "AE_PLATFORM_TARGET_VERSION_MAJOR=${APPLE_VER_MAJ}" "AE_PLATFORM_TARGET_VERSION_MINOR=${APPLE_VER_MIN}" )

	if (${AE_USE_SANITIZER})
		set( CLANG_SHARED_OPTS	${CLANG_SHARED_OPTS} -fsanitize=address )
		#  -fsanitize=thread -fsanitize=undefined
	endif()

	# Release
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>: > )
	set( CMAKE_C_FLAGS_RELEASE "-O3 -finline-functions ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-O3 -finline-functions ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_RELEASE ${CLANG_SHARED_OPTS} -O3 -Ofast -fomit-frame-pointer -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_RELEASE "" CACHE INTERNAL "" FORCE )
	# Profile
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Profile>: > )
	set( CMAKE_C_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_PROFILE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_PROFILE ${CLANG_SHARED_OPTS} -O2 -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_PROFILE "" CACHE INTERNAL "" FORCE )
	# Develop
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Develop>: > )
	set( CMAKE_C_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEVELOP ${CLANG_SHARED_OPTS} -g -glldb -O0 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEVELOP "" CACHE INTERNAL "" FORCE )
	# Debug
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>: > )
	set( CMAKE_C_FLAGS_DEBUG "-O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEBUG ${CLANG_SHARED_OPTS} -g -glldb -O0 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEBUG "" CACHE INTERNAL "" FORCE )
endif()


#==================================================================================================
# Android Clang Compilation settings
#==================================================================================================
set( COMPILER_CLANG_ANDROID OFF )
string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( (outPos GREATER -1) AND (${CMAKE_SYSTEM_NAME} STREQUAL "Android") )
	set( COMPILER_CLANG_ANDROID ON )
endif()
if ( COMPILER_CLANG_ANDROID )
	if (DEFINED DETECTED_COMPILER)
		message( FATAL_ERROR "multiple compiler types detected, previous: '${DETECTED_COMPILER}'" )
	endif()
	set( DETECTED_COMPILER "COMPILER_CLANG_ANDROID" )
	#--------------------------------------------
	set( CONFIGURATION_DEPENDENT_PATH OFF CACHE INTERNAL "" FORCE )
	#--------------------------------------------

	set( CLANG_SHARED_OPTS ${COMPILER_FLAGS} ${CLANG_SHARED_LOCAL_WARNING_LIST_CXX} -fstack-protector-strong -fPIC -fcoroutines-ts )
	# -mfloat-abi=hard

	set( PROJECTS_SHARED_DEFINES ${PROJECTS_SHARED_DEFINES} "AE_COMPILER_CLANG" )

	if (${AE_USE_SANITIZER})
		set( CLANG_SHARED_OPTS	${CLANG_SHARED_OPTS} -fsanitize=address )
		#  -fsanitize=thread -fsanitize=undefined
	endif()

	# Release
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Release>: > )
	set( CMAKE_C_FLAGS_RELEASE "-O3 -finline-functions ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_RELEASE "-O3 -finline-functions ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_RELEASE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_RELEASE ${CLANG_SHARED_OPTS} -O3 -Ofast -fomit-frame-pointer -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_RELEASE " -static" CACHE INTERNAL "" FORCE )
	# Profile
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Profile>: > )
	set( CMAKE_C_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_PROFILE "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_PROFILE "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_PROFILE ${CLANG_SHARED_OPTS} -O2 -finline-functions CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_PROFILE " -static" CACHE INTERNAL "" FORCE )
	# Develop
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Develop>: > )
	set( CMAKE_C_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEVELOP "-O2 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEVELOP "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEVELOP "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEVELOP "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEVELOP ${CLANG_SHARED_OPTS} -g -glldb -O2 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEVELOP " -static" CACHE INTERNAL "" FORCE )
	# Debug
	set_property( DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>: > )
	set( CMAKE_C_FLAGS_DEBUG "-O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_CXX_FLAGS_DEBUG "-O0 ${CLANG_SHARED_GLOBAL_WARNING_LIST_C_CXX}" CACHE STRING "" FORCE )
	set( CMAKE_EXE_LINKER_FLAGS_DEBUG "${CURRENT_EXE_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CURRENT_STATIC_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CURRENT_SHARED_LINKER_FLAGS} " CACHE STRING "" FORCE )
	set( PROJECTS_SHARED_CXX_FLAGS_DEBUG ${CLANG_SHARED_OPTS} -g -glldb -O0 CACHE INTERNAL "" FORCE )
	set( PROJECTS_SHARED_LINKER_FLAGS_DEBUG " -static" CACHE INTERNAL "" FORCE )

	#message( FATAL_ERROR "PROJECTS_SHARED_CXX_FLAGS_DEBUG: ${PROJECTS_SHARED_CXX_FLAGS_DEBUG}" )
endif()


#==================================================================================================
if ( DEFINED DETECTED_COMPILER )
	message( STATUS "current compiler: ${DETECTED_COMPILER}" )
else()
	message( FATAL_ERROR "current compiler: '${CMAKE_CXX_COMPILER_ID}' is not configured for this project!" )
endif()

set( PROJECTS_SHARED_DEFINES			${PROJECTS_SHARED_DEFINES}			CACHE INTERNAL "" FORCE )
set( PROJECTS_SHARED_DEFINES_RELEASE	${PROJECTS_SHARED_DEFINES_RELEASE}	CACHE INTERNAL "" FORCE )
set( PROJECTS_SHARED_DEFINES_PROFILE	${PROJECTS_SHARED_DEFINES_PROFILE}	CACHE INTERNAL "" FORCE )
set( PROJECTS_SHARED_DEFINES_DEVELOP	${PROJECTS_SHARED_DEFINES_DEVELOP}	CACHE INTERNAL "" FORCE )
set( PROJECTS_SHARED_DEFINES_DEBUG		${PROJECTS_SHARED_DEFINES_DEBUG}	CACHE INTERNAL "" FORCE )


# print
if (FALSE)
	message( STATUS "-------------------------------------------" )
	message( STATUS "PROJECTS_SHARED_DEFINES: ${PROJECTS_SHARED_DEFINES}" )
	#	Release
	message( STATUS "PROJECTS_SHARED_DEFINES_RELEASE: ${PROJECTS_SHARED_DEFINES_RELEASE}" )
	message( STATUS "PROJECTS_SHARED_CXX_FLAGS_RELEASE: ${PROJECTS_SHARED_CXX_FLAGS_RELEASE}" )
	message( STATUS "PROJECTS_SHARED_LINKER_FLAGS_RELEASE: ${PROJECTS_SHARED_LINKER_FLAGS_RELEASE}" )
	#	Profile
	message( STATUS "PROJECTS_SHARED_DEFINES_PROFILE: ${PROJECTS_SHARED_DEFINES_PROFILE}" )
	message( STATUS "PROJECTS_SHARED_CXX_FLAGS_PROFILE: ${PROJECTS_SHARED_CXX_FLAGS_PROFILE}" )
	message( STATUS "PROJECTS_SHARED_LINKER_FLAGS_PROFILE: ${PROJECTS_SHARED_LINKER_FLAGS_PROFILE}" )
	#	Develop
	message( STATUS "PROJECTS_SHARED_DEFINES_DEVELOP: ${PROJECTS_SHARED_DEFINES_DEVELOP}" )
	message( STATUS "PROJECTS_SHARED_CXX_FLAGS_DEVELOP: ${PROJECTS_SHARED_CXX_FLAGS_DEVELOP}" )
	message( STATUS "PROJECTS_SHARED_LINKER_FLAGS_DEVELOP: ${PROJECTS_SHARED_LINKER_FLAGS_DEVELOP}" )
	#	Debug
	message( STATUS "PROJECTS_SHARED_DEFINES_DEBUG: ${PROJECTS_SHARED_DEFINES_DEBUG}" )
	message( STATUS "PROJECTS_SHARED_CXX_FLAGS_DEBUG: ${PROJECTS_SHARED_CXX_FLAGS_DEBUG}" )
	message( STATUS "PROJECTS_SHARED_LINKER_FLAGS_DEBUG: ${PROJECTS_SHARED_LINKER_FLAGS_DEBUG}" )
	message( STATUS "-------------------------------------------" )
endif()

if (FALSE)
	message( STATUS "-------------------------------------------" )
	#	Release
	message( STATUS "CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}" )
	message( STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}" )
	message( STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE: ${CMAKE_EXE_LINKER_FLAGS_RELEASE}" )
	message( STATUS "CMAKE_STATIC_LINKER_FLAGS_RELEASE: ${CMAKE_STATIC_LINKER_FLAGS_RELEASE}" )
	message( STATUS "CMAKE_SHARED_LINKER_FLAGS_RELEASE: ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}" )
	#	Profile
	message( STATUS "CMAKE_C_FLAGS_PROFILE: ${CMAKE_C_FLAGS_PROFILE}" )
	message( STATUS "CMAKE_CXX_FLAGS_PROFILE: ${CMAKE_CXX_FLAGS_PROFILE}" )
	message( STATUS "CMAKE_EXE_LINKER_FLAGS_PROFILE: ${CMAKE_EXE_LINKER_FLAGS_PROFILE}" )
	message( STATUS "CMAKE_STATIC_LINKER_FLAGS_PROFILE: ${CMAKE_STATIC_LINKER_FLAGS_PROFILE}" )
	message( STATUS "CMAKE_SHARED_LINKER_FLAGS_PROFILE: ${CMAKE_SHARED_LINKER_FLAGS_PROFILE}" )
	#	Develop
	message( STATUS "CMAKE_C_FLAGS_DEVELOP: ${CMAKE_C_FLAGS_DEVELOP}" )
	message( STATUS "CMAKE_CXX_FLAGS_DEVELOP: ${CMAKE_CXX_FLAGS_DEVELOP}" )
	message( STATUS "CMAKE_EXE_LINKER_FLAGS_DEVELOP: ${CMAKE_EXE_LINKER_FLAGS_DEVELOP}" )
	message( STATUS "CMAKE_STATIC_LINKER_FLAGS_DEVELOP: ${CMAKE_STATIC_LINKER_FLAGS_DEVELOP}" )
	message( STATUS "CMAKE_SHARED_LINKER_FLAGS_DEVELOP: ${CMAKE_SHARED_LINKER_FLAGS_DEVELOP}" )
	#	Debug
	message( STATUS "CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}" )
	message( STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}" )
	message( STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG: ${CMAKE_EXE_LINKER_FLAGS_DEBUG}" )
	message( STATUS "CMAKE_STATIC_LINKER_FLAGS_DEBUG: ${CMAKE_STATIC_LINKER_FLAGS_DEBUG}" )
	message( STATUS "CMAKE_SHARED_LINKER_FLAGS_DEBUG: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" )
	message( STATUS "-------------------------------------------" )
endif()
