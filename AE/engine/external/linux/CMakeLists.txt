# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

set( AE_ENABLE_KTX	 				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_OPENXR				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_FFMPEG				OFF CACHE INTERNAL "" FORCE )
set( AE_ENABLE_STABLE_DIFFUSION		OFF CACHE INTERNAL "" FORCE )

#----------------------------------------------------------

if (NOT DEFINED AE_DST_PATH)
	message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT DEFINED AE_DST_SHARED_PATH)
	message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()
