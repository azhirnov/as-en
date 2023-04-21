
set( AE_EXTERNAL_SHARED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/source" CACHE INTERNAL "" FORCE )

set( AE_ENABLE_REMOTE_GRAPHICS	OFF CACHE BOOL "enable Remote Graphics" )
set( AE_ENABLE_RGRAPHICS_SERVER	OFF CACHE BOOL "enable Remote Graphics server" )

if (APPLE)
	set( AE_ENABLE_METAL  ON  CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_VULKAN OFF CACHE INTERNAL "" FORCE )
else()
	set( AE_ENABLE_METAL  OFF CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_VULKAN ON  CACHE INTERNAL "" FORCE )
endif()

add_subdirectory( "external/source/GLM" "GLM" )

if (${AE_ENABLE_VULKAN})
	add_subdirectory( "external/source/Vulkan" "Vulkan" )
endif()
