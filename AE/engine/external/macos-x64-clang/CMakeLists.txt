# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

# not compatible with x64 on ARM
set( AE_ENABLE_ZSTD	 			OFF CACHE INTERNAL "" FORCE )

add_subdirectory( "../macos" "macos" )
