# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# find installed Intel VTune

add_library( "VTune-lib" INTERFACE )
target_compile_definitions( "VTune-lib" INTERFACE "AE_ENABLE_VTUNE_API" )

if (EXISTS "$ENV{VTUNE_AMPLIFIER_2019_DIR}/include")
	message( STATUS "found Intel VTune 2019 in '$ENV{VTUNE_AMPLIFIER_2019_DIR}'" )
	target_include_directories( "VTune-lib" INTERFACE "$ENV{VTUNE_AMPLIFIER_2019_DIR}include" )
	
	if (WIN32)
		if (${AE_PLATFORM_BITS} EQUAL 64)
			set_property( TARGET "VTune-lib" PROPERTY INTERFACE_LINK_LIBRARIES "$ENV{VTUNE_AMPLIFIER_2019_DIR}lib64/libittnotify.lib" )
		elseif (${AE_PLATFORM_BITS} EQUAL 32)
			set_property( TARGET "VTune-lib" PROPERTY INTERFACE_LINK_LIBRARIES "$ENV{VTUNE_AMPLIFIER_2019_DIR}lib32/libittnotify.lib" )
		endif ()
	endif ()
else ()
	message( FATAL_ERROR "can't find Intel VTune" )
endif ()
