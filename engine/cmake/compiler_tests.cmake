# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

include( CheckCXXSourceCompiles )

if (${COMPILER_MSVC})
	set( AE_DEFAULT_CPPFLAGS "/std:c++latest" )
else()
	list( FIND CMAKE_CXX_COMPILE_FEATURES "cxx_std_20" HAS_CPP20 )
	if ( ${HAS_CPP20} LESS 0 )
		set( AE_DEFAULT_CPPFLAGS "-std=c++17" )
	else()
		set( AE_DEFAULT_CPPFLAGS "-std=c++20" )
	endif()
endif()

set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS}" )
message( STATUS "Run compiler tests with flags: ${CMAKE_REQUIRED_FLAGS}" )

set( AE_COMPILER_DEFINITIONS "" )

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <string_view>
	int main () {
		std::string_view str{\"1234\"};
		(void)(str);
		return 0;
	}"
	STD_STRINGVIEW_SUPPORTED )

if (STD_STRINGVIEW_SUPPORTED)
	set( STD_STRINGVIEW_SUPPORTED ON CACHE INTERNAL "" FORCE )
else()
	set( STD_STRINGVIEW_SUPPORTED OFF CACHE INTERNAL "" FORCE )
endif()

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <optional>
	int main () {
		std::optional<int> opt = 1;
		return opt.has_value() ? 0 : 1;
	}"
	STD_OPTIONAL_SUPPORTED )

if (STD_OPTIONAL_SUPPORTED)
	set( STD_OPTIONAL_SUPPORTED ON CACHE INTERNAL "" FORCE )
else()
	set( STD_OPTIONAL_SUPPORTED OFF CACHE INTERNAL "" FORCE )
endif()

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <variant>
	int main () {
		std::variant<int, float> var;
		var = 1.0f;
		(void)(var);
		return 0;
	}"
	STD_VARIANT_SUPPORTED )

if (STD_VARIANT_SUPPORTED)
	set( STD_VARIANT_SUPPORTED ON CACHE INTERNAL "" FORCE )
else()
	set( STD_VARIANT_SUPPORTED OFF CACHE INTERNAL "" FORCE )
endif()

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <filesystem>
	int main () {
		(void)(std::filesystem::current_path());
		return 0;
	}"
	STD_FILESYSTEM_SUPPORTED )

if (STD_FILESYSTEM_SUPPORTED)
	set( STD_FILESYSTEM_SUPPORTED ON CACHE INTERNAL "" FORCE )
else()
	set( STD_FILESYSTEM_SUPPORTED OFF CACHE INTERNAL "" FORCE )
endif()

#------------------------------------------------------------------------------
check_cxx_source_compiles(
	"#include <new>
	static constexpr size_t Align = std::hardware_destructive_interference_size;
	int main () {
		return 0;
	}"
	STD_CACHELINESIZE_SUPPORTED )

if (STD_CACHELINESIZE_SUPPORTED)
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CACHE_LINE=std::hardware_destructive_interference_size" )

elseif (APPLE)
	string( TOUPPER ${CMAKE_SYSTEM_PROCESSOR} PLATFORM_NAME )
	if (${PLATFORM_NAME} STREQUAL "X86_64")
		set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CACHE_LINE=64" )
	else()
		set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CACHE_LINE=128" )
	endif()
else ()
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CACHE_LINE=64" ) # TODO
endif()

#------------------------------------------------------------------------------
if (NOT ${COMPILER_MSVC})
	set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS} -Werror=unknown-pragmas" )
endif()

check_cxx_source_compiles(
	"#pragma detect_mismatch( \"AE_DEBUG\", \"1\" )
	int main () {
		return 0;
	}"
	CPP_DETECT_MISMATCH_SUPPORTED )

if (CPP_DETECT_MISMATCH_SUPPORTED)
	set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CPP_DETECT_MISMATCH" )
endif()
set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS}" )

#------------------------------------------------------------------------------
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

set( CMAKE_REQUIRED_FLAGS "" )
set( CMAKE_REQUIRED_LIBRARIES "" )
set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" CACHE INTERNAL "" FORCE )

