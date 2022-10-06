# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( AE_ENABLE_MEMLEAK_CHECKS ON CACHE BOOL "enable memory leak checks" )

#----------------------------------------------------------
# advanced settings

set( AE_NO_EXCEPTIONS ON CACHE BOOL "disable C++ exceptions" )
set( AE_CI_BUILD OFF CACHE BOOL "CI settings" )
mark_as_advanced( AE_NO_EXCEPTIONS AE_CI_BUILD )

#----------------------------------------------------------
# internal constants

set( AE_ITERATOR_DEBUG_LEVEL 1 CACHE INTERNAL "" FORCE )

if (DEFINED ANDROID)
	set( AE_MOBILE  ON  CACHE INTERNAL "" FORCE )
	set( AE_DESKTOP OFF CACHE INTERNAL "" FORCE )
else ()
	set( AE_MOBILE  OFF CACHE INTERNAL "" FORCE )
	set( AE_DESKTOP ON  CACHE INTERNAL "" FORCE )
endif ()
