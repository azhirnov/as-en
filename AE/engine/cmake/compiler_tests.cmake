# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

include( CheckCXXSourceCompiles )

if (NOT DEFINED AE_CXX_20)
	message( FATAL_ERROR "'compiler_tests.cmake' included before C++ version check" )
endif()

if( ${AE_CXX_20} )
	set( AE_DEFAULT_CPPFLAGS "-std=c++20" )
elseif( ${AE_CXX_23} )
	set( AE_DEFAULT_CPPFLAGS "-std=c++23" )
elseif( ${AE_CXX_26} )
	set( AE_DEFAULT_CPPFLAGS "-std=c++26" )
endif()

string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( outPos GREATER -1 )
	set( AE_DEFAULT_CPPFLAGS "${AE_DEFAULT_CPPFLAGS} -stdlib=libc++" )
endif()

set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS}" )
message( STATUS "Run compiler tests with flags: ${CMAKE_REQUIRED_FLAGS}" )

set( AE_COMPILER_DEFINITIONS "" )

#------------------------------------------------------------------------------
if (NOT ${COMPILER_MSVC})
	set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS} -Werror=unknown-pragmas" )
endif()

check_cxx_source_compiles(
	"#pragma detect_mismatch( \"TEST_MACRO\", \"1\" )
	int main () {
		return 0;
	}"
	CPP_DETECT_MISMATCH_SUPPORTED )

if (CPP_DETECT_MISMATCH_SUPPORTED)
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CPP_DETECT_MISMATCH" )
endif()
set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS}" )

#==============================================================================
check_cxx_source_compiles(
	"#include <functional>
	int main () {
		char buffer[128] = {};
		(void)(std::_Hash_array_representation( reinterpret_cast<const unsigned char*>(buffer), std::size(buffer) ));
		return 0;
	}"
	HAS_HASHFN_HashArrayRepresentation )

if (HAS_HASHFN_HashArrayRepresentation)
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_HAS_HASHFN_HashArrayRepresentation" )
endif()

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <functional>
	int main () {
		char buffer[128] = {};
		(void)(std::__murmur2_or_cityhash<size_t>()( buffer, std::size(buffer) ));
		return 0;
	}"
	HAS_HASHFN_Murmur2OrCityhash )

if (HAS_HASHFN_Murmur2OrCityhash)
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_HAS_HASHFN_Murmur2OrCityhash" )
endif()

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <functional>
	int main () {
		char buffer[128] = {};
		(void)(std::_Hash_bytes( buffer, std::size(buffer), 0 ));
		return 0;
	}"
	HAS_HASHFN_HashBytes )

if (HAS_HASHFN_HashBytes)
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_HAS_HASHFN_HashBytes" )
endif()

#------------------------------------------------------------------------------
if (NOT (HAS_HASHFN_HashArrayRepresentation OR HAS_HASHFN_Murmur2OrCityhash OR HAS_HASHFN_HashBytes))
	message( STATUS "Warning: used fallback hash function" )
endif()

#==============================================================================

if ( ${AE_CXX_23} )
	set( CPP_IF_CONSTEVAL_SUPPORTED_SRC
		"constexpr bool is_constant_evaluated() noexcept
		{
			if consteval {
				return true;
			}else{
				return false;
			}
		}
		int main () {
			return is_constant_evaluated();
		}" )

	if (${CMAKE_VERSION} VERSION_LESS "3.25.0")
		check_cxx_source_compiles(
			"${CPP_IF_CONSTEVAL_SUPPORTED_SRC}"
			CPP_IF_CONSTEVAL_SUPPORTED )
	else()
		# use CXX_STANDARD instead of flags
		if (NOT DEFINED CPP_IF_CONSTEVAL_SUPPORTED)
			message( STATUS "Performing Test CPP_IF_CONSTEVAL_SUPPORTED" )
			try_compile(
				CPP_IF_CONSTEVAL_SUPPORTED
				SOURCE_FROM_VAR 		"main.cpp" CPP_IF_CONSTEVAL_SUPPORTED_SRC
				CXX_STANDARD  			23
				CXX_STANDARD_REQUIRED 	YES
			)
			set( CPP_IF_CONSTEVAL_SUPPORTED ${CPP_IF_CONSTEVAL_SUPPORTED} CACHE INTERNAL "" FORCE )
			if (CPP_IF_CONSTEVAL_SUPPORTED)
				message( STATUS "Performing Test CPP_IF_CONSTEVAL_SUPPORTED - Success" )
			else()
				message( STATUS "Performing Test CPP_IF_CONSTEVAL_SUPPORTED - Failed" )
			endif()
		endif()
	endif()

	if (CPP_IF_CONSTEVAL_SUPPORTED)
		set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CPP_IF_CONSTEVAL" )
	endif()
endif()
#==============================================================================

set( CMAKE_REQUIRED_FLAGS "" )
set( CMAKE_REQUIRED_LIBRARIES "" )
set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" CACHE INTERNAL "" FORCE )

