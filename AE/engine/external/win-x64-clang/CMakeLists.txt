# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

# not compiled with Clang
string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( (outPos GREATER -1) )
	set( AE_ENABLE_COMPRESSONATOR	OFF CACHE INTERNAL "" FORCE )
endif()

add_subdirectory( "../windows" "windows" )
