
#----------------------------------------------------------
# Base

function( EngineDeps_Base projName )

	target_link_libraries( ${projName} PUBLIC "ProjectTemplate" )

	target_include_directories( ${projName} PUBLIC
		"${MAIN_SOURCE_DIR}/engine/src"
		"${MAIN_SOURCE_DIR}/engine/_experimental"
		"${MAIN_SOURCE_DIR}/engine/tools"
		"${MAIN_SOURCE_DIR}/engine/external"
	)

	if (TARGET "GLM-lib")
		target_link_libraries( ${projName} PUBLIC "GLM-lib" )
	else()
		message( FATAL_ERROR "GLM target is not exists" )
	endif()

	if (TARGET "Brotli-lib")
		target_link_libraries( ${projName} PUBLIC "Brotli-lib" )
	endif()
	if (TARGET "zstd-lib")
		target_link_libraries( ${projName} PUBLIC "zstd-lib" )
	endif()
	if (TARGET "lz4-lib")
		target_link_libraries( ${projName} PUBLIC "lz4-lib" )
	endif()

	if (TARGET "Utf8Proc-lib")
		target_link_libraries( ${projName} PUBLIC "Utf8Proc-lib" )
	endif()

	if (TARGET "Abseil-lib")
		target_link_libraries( ${projName} PUBLIC "Abseil-lib" )
	endif()

	if (TARGET "xxHash-lib")
		target_link_libraries( ${projName} PUBLIC "xxHash-lib" )
	endif()

	target_link_libraries( ${projName} PUBLIC "FametaCounter-lib" )

	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		target_link_libraries( ${projName} PUBLIC "dl" "pthread" )
		target_compile_definitions( ${projName} PUBLIC _LARGEFILE_SOURCE _LARGE_FILES _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE )
	endif()

	if (ANDROID)
		target_link_libraries( ${projName} PUBLIC "log" )
	endif()

	if (APPLE)
		find_library( COCOA_LIBRARY Cocoa REQUIRED )
		target_link_libraries( ${projName} PRIVATE ${COCOA_LIBRARY} )
	endif()

	if (WIN32)
		# for sockets
		target_link_libraries( ${projName} PUBLIC "ws2_32" ) # "Iphlpapi"
	endif()

	target_compile_definitions( ${projName} PUBLIC FP_FAST_FMA FP_FAST_FMAF FP_FAST_FMAL )	# TODO: remove
endfunction()

#----------------------------------------------------------
# GraphicsRHI

function( EngineDeps_GraphicsRHI projName )

	# Vulkan
	if (${AE_ENABLE_VULKAN})
		target_link_libraries( ${projName} PUBLIC "Vulkan-lib" )
		target_compile_definitions( ${projName} PUBLIC AE_ENABLE_GLSL_TRACE )

		if (${AE_ENABLE_VMA})
			target_link_libraries( ${projName} PUBLIC "VMA-lib" )
		endif()
		if (TARGET "RenderDoc-lib")
			target_link_libraries( ${projName} PUBLIC "RenderDoc-lib" )
		endif()
		if (WIN32 AND TARGET "NVPerf-lib")
			target_link_libraries( ${projName} PUBLIC "NVPerf-lib" )
		endif()
		if ( ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux") OR WIN32) AND (TARGET "AMDGpuPerf-lib") )
			target_link_libraries( ${projName} PUBLIC "AMDGpuPerf-lib" )
		endif()

		if (${AE_VK_TIMELINE_SEMAPHORE})
			target_compile_definitions( ${projName} PUBLIC AE_VK_TIMELINE_SEMAPHORE=1 )
		else()
			target_compile_definitions( ${projName} PUBLIC AE_VK_TIMELINE_SEMAPHORE=0 )
		endif()

		if (APPLE)
			target_compile_definitions( ${projName} PUBLIC VK_USE_PLATFORM_METAL_EXT=1 )
		endif()

		if (ANDROID)
			target_compile_definitions( ${projName} PUBLIC VK_USE_PLATFORM_ANDROID_KHR=1 )
		endif()
	endif()

	# Remote Graphics
	if (${AE_ENABLE_REMOTE_GRAPHICS})
		target_compile_definitions( ${projName} PUBLIC AE_ENABLE_REMOTE_GRAPHICS )
		target_compile_definitions( ${projName} PUBLIC AE_ENABLE_GLSL_TRACE )
	endif()

endfunction()

#----------------------------------------------------------
# Audio

function( EngineDeps_Audio projName )

	if (TARGET "BASS-lib")
		target_link_libraries( ${projName} PUBLIC "BASS-lib" )
		target_compile_definitions( ${projName} PUBLIC AE_ENABLE_AUDIO )

	elseif (ANDROID)
		target_compile_definitions( ${projName} PUBLIC AE_ENABLE_OPENSLES AE_ENABLE_AUDIO )

	endif()
endfunction()

#----------------------------------------------------------
# Platform

function( EngineDeps_Platform projName )

	# choose window library
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
		target_link_libraries( ${projName} PUBLIC "android" "camera2ndk" "mediandk" )	# "nativewindow" for API 30+

		target_include_directories( ${projName} PUBLIC "${ANDROID_NDK}/sources/android/native_app_glue" )
	elseif (${AE_ENABLE_GLFW})
		target_link_libraries( ${projName} PUBLIC "GLFW-lib" )

		if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
			target_compile_definitions( ${projName} PUBLIC GLFW_EXPOSE_NATIVE_COCOA )

		elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
			target_compile_definitions( ${projName} PUBLIC GLFW_EXPOSE_NATIVE_WIN32 )

		elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
			target_compile_definitions( ${projName} PUBLIC GLFW_EXPOSE_NATIVE_X11 GLFW_EXPOSE_NATIVE_WAYLAND )
		endif()

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		# Raw WinAPI is supported
		target_link_libraries( ${projName} PUBLIC "Dxva2.lib" )
		target_compile_definitions( ${projName} PUBLIC AE_WINAPI_WINDOW )

	else()
		message( FATAL_ERROR "At least one window library must be supported" )
	endif()

	if (${AE_ENABLE_VULKAN})
		if (TARGET "OpenVR-lib")
			target_link_libraries( ${projName} PUBLIC "OpenVR-lib" )
		endif()
		if (TARGET "OpenXR-lib")
			target_link_libraries( ${projName} PUBLIC "OpenXR-lib" )
		endif()
		if (ANDROID AND TARGET "PicoOpenXR-lib")
			target_link_libraries( ${projName} PUBLIC "PicoOpenXR-lib" )
		endif()
	endif()
	if (TARGET "NVAPI-lib")
		target_link_libraries( ${projName} PUBLIC "NVAPI-lib" )
	endif()
endfunction()

#----------------------------------------------------------
# Scripting

function( EngineDeps_Scripting projName )
	target_compile_definitions( ${projName} PUBLIC "AE_ENABLE_SCRIPTING" )

	if (${AE_ENABLE_ANGELSCRIPT})
		target_link_libraries( ${projName} PUBLIC "AngelScript-lib" )
	endif()
endfunction()

#----------------------------------------------------------
# Video

function( EngineDeps_Video projName )

	if (${AE_ENABLE_FFMPEG})
		target_link_libraries( ${projName} PUBLIC "FFmpeg-lib" )
	endif()
endfunction()

#----------------------------------------------------------
# Graphics

function( EngineDeps_Graphics projName )

	if (TARGET "ImGUI-lib")
		target_link_libraries( ${projName} PUBLIC "ImGUI-lib" )
	endif()
endfunction()

#----------------------------------------------------------
# HuLang

function( EngineDeps_HuLang projName )

endfunction()

#----------------------------------------------------------
# Networking

function( EngineDeps_Networking projName )

	target_compile_definitions( ${projName} PUBLIC "AE_ENABLE_NETWORKING" )

endfunction()

#----------------------------------------------------------
# Profiler

function( EngineDeps_Profiler projName )

	if (TARGET "Mali-HWCPipe-lib")
		target_link_libraries( ${projName} PUBLIC "Mali-HWCPipe-lib" )
	endif()
	if (TARGET "PowerVRScopeStats-lib")
		target_link_libraries( ${projName} PUBLIC "PowerVRScopeStats-lib" )
	endif()
	if ( TARGET "NVML-lib" )
		target_link_libraries( ${projName} PUBLIC "NVML-lib" )
	endif()
	if (ANDROID)
		target_compile_definitions( ${projName} PUBLIC "AE_ENABLE_ADRENO_PERFCOUNTER" "AE_ENABLE_ARM_PMU" )
	endif()

endfunction()

#----------------------------------------------------------
# Serializing

function( EngineDeps_Serializing projName )

endfunction()

#----------------------------------------------------------
# Threading

function( EngineDeps_Threading projName )
	if (APPLE)
		# true async
		target_compile_definitions( ${projName} PUBLIC AE_ASYNCIO_USE_POSIX_AIO )

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
		target_compile_definitions( ${projName} PUBLIC AE_ASYNCIO_USE_LINUX_AIO )

	elseif ((${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD") OR
			(${CMAKE_SYSTEM_NAME} STREQUAL "DragonFly"))
		target_compile_definitions( ${projName} PUBLIC AE_ASYNCIO_USE_BSD_AIO )

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		if (${AE_ENABLE_IO_URING})
			target_compile_definitions( ${projName} PUBLIC AE_ASYNCIO_USE_IO_URING )
			target_link_libraries( ${projName} PUBLIC "uring" )
		else()
			# true async
			target_compile_definitions( ${projName} PUBLIC AE_ASYNCIO_USE_LINUX_AIO )

			# impl dependent, may have thread pool inside
			#target_compile_definitions( ${projName} PUBLIC AE_ASYNCIO_USE_POSIX_AIO )
		endif()
	endif()
endfunction()

#----------------------------------------------------------
# VFS

function( EngineDeps_VFS projName )

endfunction()

#----------------------------------------------------------
# Video

function( EngineDeps_Video projName )

	if (${AE_ENABLE_FFMPEG})
		target_link_libraries( ${projName} PUBLIC "FFmpeg-lib" )
	endif()

endfunction()

#----------------------------------------------------------
# ECS

function( EngineDeps_ECS projName )

endfunction()

#----------------------------------------------------------
# LangModel

function( EngineDeps_LangModel projName )

endfunction()

#----------------------------------------------------------

if (${AE_WHOLE_ENGINE_PCH} OR ${AE_ENGINE_BASE_PCH})
	set( AE_ENGINE_PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/EnginePCH.dir" CACHE INTERNAL "" FORCE )

	function( SetEnginePCHSourceDir )
		source_group( "_pch_" FILES
			"${AE_ENGINE_PCH_DIR}/cmake_pch.cxx"
			"${AE_ENGINE_PCH_DIR}/Debug/cmake_pch.hxx"
			"${AE_ENGINE_PCH_DIR}/Develop/cmake_pch.hxx"
			"${AE_ENGINE_PCH_DIR}/Profile/cmake_pch.hxx"
			"${AE_ENGINE_PCH_DIR}/Release/cmake_pch.hxx"
		)
	endfunction()

	file( GLOB CPP_SOURCES	"*.*" )

	add_library( "EnginePCH" STATIC ${CPP_SOURCES} )

	set_property( TARGET "EnginePCH" PROPERTY FOLDER "Engine/Tools" )

	source_group( "" FILES ${CPP_SOURCES} )
	SetEnginePCHSourceDir()

	if (${AE_WHOLE_ENGINE_PCH})
		target_precompile_headers( "EnginePCH" PRIVATE "Engine.h" )
	else()
		target_precompile_headers( "EnginePCH" PRIVATE "Base.h" )
	endif()

	EngineDeps_Base( "EnginePCH" )
endif()

if (${AE_WHOLE_ENGINE_PCH})
	EngineDeps_Scripting( "EnginePCH" )
	EngineDeps_Serializing( "EnginePCH" )
	EngineDeps_Threading( "EnginePCH" )
	EngineDeps_Networking( "EnginePCH" )
	EngineDeps_GraphicsRHI( "EnginePCH" )
	EngineDeps_VFS( "EnginePCH" )
	EngineDeps_Audio( "EnginePCH" )
	EngineDeps_Platform( "EnginePCH" )
	EngineDeps_Graphics( "EnginePCH" )
	EngineDeps_Video( "EnginePCH" )
	EngineDeps_HuLang( "EnginePCH" )
	EngineDeps_Profiler( "EnginePCH" )
	EngineDeps_ECS( "EnginePCH" )
	EngineDeps_LangModel( "EnginePCH" )
endif()
#----------------------------------------------------------

function( UseEnginePCH projName )

	if (${AE_WHOLE_ENGINE_PCH})
		target_link_libraries( ${projName} PUBLIC "EnginePCH" )
		target_precompile_headers( ${projName} REUSE_FROM "EnginePCH" )
		SetEnginePCHSourceDir()

	elseif(${AE_ENGINE_BASE_PCH})
		target_link_libraries( ${projName} PUBLIC "EnginePCH" )
		target_precompile_headers( ${projName} REUSE_FROM "EnginePCH" )
		SetEnginePCHSourceDir()
		cmake_language( EVAL CODE "EngineDeps_${projName}( ${projName} )" )

	else()
		cmake_language( EVAL CODE "EngineDeps_${projName}( ${projName} )" )
	endif()

endfunction()

function( UseEnginePCH2 projName )

	if (${AE_WHOLE_ENGINE_PCH} OR ${AE_ENGINE_BASE_PCH})
		target_link_libraries( ${projName} PUBLIC "EnginePCH" )
		target_precompile_headers( ${projName} REUSE_FROM "EnginePCH" )
		SetEnginePCHSourceDir()
	endif()

endfunction()
#----------------------------------------------------------

# add working directory for Android console mode
function( AddTest projName )
	if (ANDROID AND ${AE_ANDROID_CONSOLE_MODE})
		add_test( NAME ${projName} COMMAND ${projName} WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )
	else()
		add_test( NAME ${projName} COMMAND ${projName} )
	endif()
endfunction()
#----------------------------------------------------------
