# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( CMAKE_INSTALL_PREFIX 	"${CMAKE_BINARY_DIR}/install" 						CACHE INTERNAL "installation directory" FORCE )
set( AE_DST_PATH 			"${AE_EXTERNAL_REP_PATH}/external/linux-x64-gcc13"	CACHE INTERNAL "" 						FORCE )
set( AE_DST_SHARED_PATH 	"${AE_EXTERNAL_REP_PATH}/external/linux-x64"		CACHE INTERNAL "" 						FORCE )

add_subdirectory( "../linux" "linux" )
