cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( GRAPHICS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/graphics" )
set( GRAPHICS_SRC
	"${GRAPHICS_DIR}/Public/ImageMemView.h"
	#"${GRAPHICS_DIR}/Private/ImageMemView.cpp"
	"${GRAPHICS_DIR}/Private/EnumUtils.h"
	#"${GRAPHICS_DIR}/Private/EnumUtils.cpp"
	"${GRAPHICS_DIR}/Scripting/GraphicsBindings.h"
	#"${GRAPHICS_DIR}/Scripting/GraphicsBindings.cpp"
)

set( VFS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../src/vfs" )
set( VFS_SRC
	"${VFS_DIR}/Archive/ArchivePacker.h"
	#"${VFS_DIR}/Archive/ArchivePacker.cpp"
)

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "AssetPacker" STATIC ${SOURCES} ${GRAPHICS_SRC} ${VFS_SRC} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "external/graphics" FILES ${GRAPHICS_SRC} )
source_group( "external/vfs" FILES ${VFS_SRC} )

target_link_libraries( "AssetPacker" PUBLIC "Serializing" "Scripting" "ResourceLoaders" "AtlasTools" "FreeType-lib" "msdfgen-lib" )
target_compile_definitions( "AssetPacker" PUBLIC "AE_BUILD_ASSET_PACKER" )
target_include_directories( "AssetPacker" PUBLIC "." )

add_library( "AssetPacker-shared" SHARED
	"AssetPacker.h"
	#"Lib/main.cpp"
	#"Lib/AssetPacker.def"
)
target_link_libraries( "AssetPacker-shared" PUBLIC "AssetPacker" )

set_property( TARGET "AssetPacker" PROPERTY FOLDER "Engine/ResPack" )
set_property( TARGET "AssetPacker-shared" PROPERTY FOLDER "Engine/ResPack" )

# generated cpp file used as dependency to trigger project rebuild
set( AssetPacker.trigger "${CMAKE_CURRENT_SOURCE_DIR}/AssetPacker.trigger.cpp" CACHE INTERNAL "" FORCE )
add_custom_command( TARGET "AssetPacker-shared" POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch "${AssetPacker.trigger}" )
