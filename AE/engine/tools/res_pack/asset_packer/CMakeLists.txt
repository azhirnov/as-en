# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

set( GRAPHICS_DIR "${MAIN_SOURCE_DIR}/engine/src/graphics" )
set( GRAPHICS_HPP_SRC
	"${GRAPHICS_DIR}/Public/ImageMemView.h"
	"${GRAPHICS_DIR}/Public/RenderState.h"
	"${GRAPHICS_DIR}/Private/EnumUtils.h"
	"${GRAPHICS_DIR}/Scripting/GraphicsBindings.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	set( GRAPHICS_CPP_SRC
		"${GRAPHICS_DIR}/Private/ImageMemView.cpp"
		"${GRAPHICS_DIR}/Private/EnumUtils.cpp"
		"${GRAPHICS_DIR}/Private/RenderState.cpp"
		"${GRAPHICS_DIR}/Scripting/GraphicsBindings.cpp" )
endif()


set( UI_DIR "${MAIN_SOURCE_DIR}/engine/src/graphics_hl/UI" )
set( UI_HPP_SRC
	"${UI_DIR}/Layout.h"
	"${UI_DIR}/Drawable.h"
	"${UI_DIR}/Controller.h"
	"${UI_DIR}/Widget.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	set( UI_CPP_SRC
		"${UI_DIR}/Layout.cpp"
		"${UI_DIR}/Drawable.cpp"
		"${UI_DIR}/Controller.cpp"
		"${UI_DIR}/Widget.cpp" )
endif()


file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "AssetPacker" STATIC
	${SOURCES}
	${GRAPHICS_HPP_SRC} ${GRAPHICS_CPP_SRC}
	${UI_HPP_SRC} ${UI_CPP_SRC}
	${PREBUILD_CPP_FILE}
)
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "external/graphics" FILES ${GRAPHICS_HPP_SRC} ${GRAPHICS_CPP_SRC} )
source_group( "external/ui" FILES ${UI_HPP_SRC} ${UI_CPP_SRC} )

target_link_libraries( "AssetPacker" PUBLIC "Serializing" "Scripting" "ResourceLoaders" "AtlasTools" "VFS" )
target_compile_definitions( "AssetPacker" PRIVATE "AE_BUILD_ASSET_PACKER" "AE_GRAPHICS_STRONG_VALIDATION=0" "AE_UI_NO_GRAPHICS" )
target_include_directories( "AssetPacker" PUBLIC "." )

if (TARGET "FreeType-lib")
	target_link_libraries( "AssetPacker" PUBLIC "FreeType-lib" )
endif()
if (TARGET "msdfgen-lib")
	target_link_libraries( "AssetPacker" PUBLIC "msdfgen-lib" )
endif()
if (TARGET "Vulkan-lib")
	target_link_libraries( "AssetPacker" PUBLIC "Vulkan-lib" )
endif()

EnablePCH( "AssetPacker" )
EnablePrebuild( "AssetPacker" )

add_library( "AssetPacker-shared" SHARED
	"AssetPacker.h"
	"Lib/Export.cpp"
	"Lib/AssetPacker.def"
)
target_link_libraries( "AssetPacker-shared" PUBLIC "AssetPacker" )

set_property( TARGET "AssetPacker" PROPERTY FOLDER "Engine/ResPack" )
set_property( TARGET "AssetPacker-shared" PROPERTY FOLDER "Engine/ResPack" )

# generated cpp file used as dependency to trigger project rebuild
set( AssetPacker.trigger "${CMAKE_CURRENT_SOURCE_DIR}/AssetPacker.trigger.cpp" CACHE INTERNAL "" FORCE )
add_custom_command( TARGET "AssetPacker-shared" POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch "${AssetPacker.trigger}" )
set_source_files_properties( "${AssetPacker.trigger}" PROPERTIES GENERATED TRUE )
