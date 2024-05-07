# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( AE_ENABLE_MEMLEAK_CHECKS ON CACHE BOOL "enable memory leak checks" )

#----------------------------------------------------------
# advanced settings

set( AE_USE_SANITIZER		OFF CACHE BOOL "enable (address/...) sanitizer" )
set( AE_CI_BUILD_TEST		OFF CACHE BOOL "CI settings for tests" )
set( AE_CI_BUILD_PERF		OFF CACHE BOOL "CI settings for performance tests" )
set( AE_ENABLE_EXCEPTIONS	ON  CACHE BOOL "enable exception and RTTI" )
set( AE_ENABLE_LOGS			ON  CACHE BOOL "enable logging, disable to remove a lot of strings" )

mark_as_advanced( AE_USE_SANITIZER AE_CI_BUILD_TEST AE_CI_BUILD_PERF AE_ENABLE_EXCEPTIONS AE_ENABLE_LOGS )

#----------------------------------------------------------
# internal constants

set( AE_ITERATOR_DEBUG_LEVEL 1 CACHE INTERNAL "" FORCE )

if (ANDROID)
	set( AE_MOBILE  ON  CACHE INTERNAL "" FORCE )
	set( AE_DESKTOP OFF CACHE INTERNAL "" FORCE )
else ()
	set( AE_MOBILE  OFF CACHE INTERNAL "" FORCE )
	set( AE_DESKTOP ON  CACHE INTERNAL "" FORCE )
endif()
