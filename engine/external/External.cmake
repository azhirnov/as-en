
set( AE_EXTERNAL_SHARED_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/source" CACHE INTERNAL "" FORCE )

set( AE_ENABLE_REMOTE_GRAPHICS	OFF CACHE BOOL "enable Remote Graphics" )
set( AE_ENABLE_RGRAPHICS_SERVER	OFF CACHE BOOL "enable Remote Graphics server" )
set( AE_ENABLE_ANGELSCRIPT		ON  CACHE BOOL "" )

if (APPLE)
	set( AE_ENABLE_METAL  ON  CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_VULKAN OFF CACHE INTERNAL "" FORCE )
else()
	set( AE_ENABLE_METAL  OFF CACHE INTERNAL "" FORCE )
	set( AE_ENABLE_VULKAN ON  CACHE INTERNAL "" FORCE )
endif()

add_subdirectory( "external/source/GLM" 		"GLM" )
add_subdirectory( "external/source/AngelScript" "AngelScript" )
add_subdirectory( "external/source/imgui" 		"imgui" )
add_subdirectory( "external/source/glslang" 	"glslang" )
add_subdirectory( "external/source/SPIRV-Cross" "SPIRV-Cross" )

if (${AE_ENABLE_VULKAN})
	add_subdirectory( "external/source/Vulkan" "Vulkan" )
endif()
