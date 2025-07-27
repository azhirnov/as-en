# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#----------------------------------------------------------
# engine components

set( AE_ENABLE_PROFILER				ON  CACHE BOOL "enable engine profiler UI" )

if (${AE_ENABLE_ANGELSCRIPT})
	set( AE_ENABLE_SCRIPTING		ON  CACHE INTERNAL "" FORCE )
else()
	set( AE_ENABLE_SCRIPTING		OFF CACHE INTERNAL "" FORCE )
endif()

if ( (TARGET "BASS-lib") )
	set( AE_ENABLE_AUDIO			TRUE CACHE INTERNAL "" FORCE )
endif()

if (${AE_ENABLE_VULKAN} OR ${AE_ENABLE_METAL} OR ${AE_ENABLE_REMOTE_GRAPHICS})
	set( AE_ENABLE_GRAPHICS			ON  CACHE INTERNAL "" FORCE )
else()
	set( AE_ENABLE_GRAPHICS			OFF CACHE INTERNAL "" FORCE )
endif()

#----------------------------------------------------------

