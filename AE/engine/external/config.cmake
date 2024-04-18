# helper file to redirect to engine config

include( "${AE_EXTERNAL_PATH}/../cmake/config.cmake" )
include( "${AE_EXTERNAL_PATH}/../cmake/compilers.cmake" )

if (DEFINED ANDROID)
	set( CMAKE_DEBUG_POSTFIX   	"" 	CACHE INTERNAL "" FORCE )
	set( CMAKE_PROFILE_POSTFIX 	""	CACHE INTERNAL "" FORCE )
	set( CMAKE_DEVELOP_POSTFIX 	"" 	CACHE INTERNAL "" FORCE )
	set( CMAKE_RELEASE_POSTFIX 	""	CACHE INTERNAL "" FORCE )
else()
	set( CMAKE_DEBUG_POSTFIX   	"d" 	CACHE INTERNAL "" FORCE )
	set( CMAKE_PROFILE_POSTFIX 	"_prof"	CACHE INTERNAL "" FORCE )
	set( CMAKE_DEVELOP_POSTFIX 	"_dev" 	CACHE INTERNAL "" FORCE )
	set( CMAKE_RELEASE_POSTFIX 	"" 		CACHE INTERNAL "" FORCE )
endif()
