file( GLOB_RECURSE CPP_SOURCES "*.h" "*.cpp" )
if (MSVC OR APPLE)
	file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
endif()
set( SOURCES ${CPP_SOURCES} ${OBJC_SOURCES} )

add_library( "Platform" STATIC ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

set_property( TARGET "Platform" PROPERTY FOLDER "Engine" )
target_link_libraries( "Platform" PUBLIC "Threading" "Graphics" "VFS" )

if (MSVC)
	set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

# choose window library
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	target_link_libraries( "Platform" PUBLIC "android" )

elseif (${AE_ENABLE_GLFW})
	target_link_libraries( "Platform" PUBLIC "GLFW-lib" )

	if (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
		target_compile_definitions( "Platform" PRIVATE GLFW_EXPOSE_NATIVE_COCOA )

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		target_compile_definitions( "Platform" PRIVATE GLFW_EXPOSE_NATIVE_WIN32 )

	elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		target_compile_definitions( "Platform" PRIVATE GLFW_EXPOSE_NATIVE_X11 )	# GLFW_EXPOSE_NATIVE_WAYLAND
	endif()

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	# Raw WinAPI is supported
	target_link_libraries( "Platform" PUBLIC "Shcore.lib" "Dxva2.lib" )

else()
	message( FATAL_ERROR "At least one window library must be supported" )
endif()

if (${AE_ENABLE_OPENVR})
	target_link_libraries( "Platform" PUBLIC "OpenVR-lib" )
endif()

if (${AE_ENABLE_OPENXR})
	target_link_libraries( "Platform" PUBLIC "OpenXR-lib" )
endif()

install( TARGETS "Platform" ARCHIVE DESTINATION "lib" )
