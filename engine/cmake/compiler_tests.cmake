# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

include( CheckCXXSourceCompiles )

list( FIND CMAKE_CXX_COMPILE_FEATURES "cxx_std_20" HAS_CPP20 )
if ( ${HAS_CPP20} LESS 0 )
    set( AE_DEFAULT_CPPFLAGS "-std=c++17" )
else()
    set( AE_DEFAULT_CPPFLAGS "-std=c++20" )
endif()

string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( outPos GREATER -1 )
    set( AE_DEFAULT_CPPFLAGS "${AE_DEFAULT_CPPFLAGS} -stdlib=libc++" )
endif()

set( CMAKE_REQUIRED_FLAGS "${AE_DEFAULT_CPPFLAGS}" )
message( STATUS "Run compiler tests with flags: ${CMAKE_REQUIRED_FLAGS}" )

set( AE_COMPILER_DEFINITIONS "" )

#------------------------------------------------------------------------------
if (NOT DEFINED AE_STD_FILESYSTEM_SUPPORTED)
    message( STATUS "Performing Test STD_FILESYSTEM_SUPPORTED" )
    set( STD_FILESYSTEM_SUPPORTED_SRC 
        "#include <filesystem>
        int main () {
            (void)(std::filesystem::current_path());
            return 0;
        }"
    )
    try_compile(
        STD_FILESYSTEM_SUPPORTED
        SOURCE_FROM_VAR         "main.cpp" STD_FILESYSTEM_SUPPORTED_SRC
        CXX_STANDARD            17
        CXX_STANDARD_REQUIRED   YES
    )
    set( AE_STD_FILESYSTEM_SUPPORTED ${STD_FILESYSTEM_SUPPORTED} CACHE INTERNAL "" FORCE )
    if (STD_FILESYSTEM_SUPPORTED)
        message( STATUS "Performing Test STD_FILESYSTEM_SUPPORTED - Success" )
    else()
        message( STATUS "Performing Test STD_FILESYSTEM_SUPPORTED - Failed" )
    endif()
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
    set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" "AE_CACHE_LINE=64" ) # TODO: check
endif()

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
if (NOT DEFINED AE_HAS_CXX_COROUTINE)
    message( STATUS "Performing Test CPP_COROUTINE_SUPPORTED" )
    set( CPP_COROUTINE_SUPPORTED_SRC 
        "#include <coroutine>
        #ifndef __cpp_impl_coroutine
        #   error coroutines are not supported by compiler
        #endif
        #ifndef __cpp_lib_coroutine
        #   error coroutines are not implemented in std
        #endif
        int main () {
            return 0;
        }"
    )
    try_compile(
        CPP_COROUTINE_SUPPORTED
        SOURCE_FROM_VAR         "main.cpp" CPP_COROUTINE_SUPPORTED_SRC
        CXX_STANDARD            20
        CXX_STANDARD_REQUIRED   YES
    )
    set( AE_HAS_CXX_COROUTINE ${CPP_COROUTINE_SUPPORTED} CACHE INTERNAL "" FORCE )
    if (CPP_COROUTINE_SUPPORTED)
        message( STATUS "Performing Test CPP_COROUTINE_SUPPORTED - Success" )
    else()
        message( STATUS "Performing Test CPP_COROUTINE_SUPPORTED - Failed" )
    endif()
endif()

#------------------------------------------------------------------------------

set( CMAKE_REQUIRED_FLAGS "" )
set( CMAKE_REQUIRED_LIBRARIES "" )
set( AE_COMPILER_DEFINITIONS "${AE_COMPILER_DEFINITIONS}" CACHE INTERNAL "" FORCE )

