# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( CMAKE_INSTALL_PREFIX 	"${CMAKE_BINARY_DIR}/install" 						CACHE INTERNAL "installation directory" FORCE )
set( AE_DST_PATH			"${AE_EXTERNAL_BIN_PATH}/external/win-x64-clang17"	CACHE INTERNAL ""						FORCE )
set( AE_DST_SHARED_PATH		"${AE_EXTERNAL_BIN_PATH}/external/win-x64"			CACHE INTERNAL ""						FORCE )

# not compiled with Clang
string( FIND "${CMAKE_CXX_COMPILER_ID}" "Clang" outPos )
if ( (outPos GREATER -1) )
	set( AE_ENABLE_COMPRESSONATOR	OFF CACHE INTERNAL "" FORCE )
endif()

add_subdirectory( "../windows" "windows" )
