# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

#----------------------------------------------------------

if (NOT DEFINED AE_DST_PATH)
    message( FATAL_ERROR "AE_DST_PATH is not defined" )
endif()
if (NOT DEFINED AE_DST_SHARED_PATH)
    message( FATAL_ERROR "AE_DST_SHARED_PATH is not defined" )
endif()
