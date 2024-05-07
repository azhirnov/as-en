# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( CMAKE_INSTALL_PREFIX 	"${CMAKE_BINARY_DIR}/install" 							CACHE INTERNAL "installation directory" FORCE )
set( AE_DST_PATH			"${AE_EXTERNAL_BIN_PATH}/external/macos-x64-clang15"	CACHE INTERNAL ""						FORCE )
set( AE_DST_SHARED_PATH		"${AE_EXTERNAL_BIN_PATH}/external/macos-x64"			CACHE INTERNAL ""						FORCE )

# not compatible with x64 on ARM
set( AE_ENABLE_ZSTD	 			OFF CACHE INTERNAL "" FORCE )

add_subdirectory( "../macos" "macos" )
