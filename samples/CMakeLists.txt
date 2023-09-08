# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

# supported on all platforms
add_subdirectory( "demo" )

if (${AE_DESKTOP} AND (DEFINED AE_CPU_ARCH_X64))
    add_subdirectory( "res_editor" )
endif()
