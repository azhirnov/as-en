# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

add_library( "ETCPACK-lib" INTERFACE )
target_include_directories( "ETCPACK-lib" INTERFACE "." )
target_compile_definitions( "ETCPACK-lib" INTERFACE "AE_ENABLE_ETCPACK" )
