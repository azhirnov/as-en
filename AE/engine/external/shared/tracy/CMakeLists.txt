# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#
# tracy (3-clause BSD license)

set( TRACY_PATH "${AE_DST_PATH}/tracy" )

if (NOT EXISTS "${TRACY_PATH}/lib")
	message( FATAL_ERROR "'${TRACY_PATH}/lib' path is not found" )
endif()
#------------------------------------------------

add_library( "Tracy-lib" INTERFACE )
target_include_directories( "Tracy-lib" INTERFACE "${TRACY_PATH}/include/tracy" )
install( FILES "${TRACY_PATH}/LICENSE" DESTINATION "3party_license" RENAME "Tracy-LICENSE" )

target_compile_definitions( "Tracy-lib" INTERFACE
	$<$<CONFIG:Profile>: "AE_ENABLE_TRACY" >
	$<$<CONFIG:Profile>: "TRACY_ENABLE" >
	$<$<CONFIG:Profile>: "AE_LICENSE_BSD3" >
)

set_property( TARGET "Tracy-lib" PROPERTY INTERFACE_LINK_LIBRARIES
#	$<$<CONFIG:Release>: "${TRACY_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}TracyClient${CMAKE_STATIC_LIBRARY_SUFFIX}" >	# disabled on Release config
	$<$<CONFIG:Profile>: "${TRACY_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}TracyClient${CMAKE_STATIC_LIBRARY_SUFFIX}" >
#	$<$<CONFIG:Develop>: "${TRACY_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}TracyClient${CMAKE_STATIC_LIBRARY_SUFFIX}" >
#	$<$<CONFIG:Debug>:   "${TRACY_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}TracyClientd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
)
#------------------------------------------------

if (WIN32)
	if (NOT EXISTS "${TRACY_PATH}/bin/tracy-profiler.exe")
		message( FATAL_ERROR "'${TRACY_PATH}/bin/tracy-profiler.exe' executable is not found" )
	endif()
	file( TO_NATIVE_PATH "${TRACY_PATH}/bin/tracy-profiler.exe" TRACY_PROFILER_EXE )

	add_custom_target( TracyProfiler
		COMMAND powershell -NoProfile -ExecutionPolicy Bypass -Command
			"Invoke-CimMethod -ClassName Win32_Process -MethodName Create -Arguments @{ CommandLine = '\"${TRACY_PROFILER_EXE}\"' } | Out-Null"
		VERBATIM
	)
	set_property( TARGET "TracyProfiler" PROPERTY FOLDER "Engine/ToolApps" )
endif()
