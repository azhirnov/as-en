# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

set( GRAPHICS_DIR "${MAIN_SOURCE_DIR}/engine/src/graphics_rhi" )
set( GRAPHICS_SRC
	"${GRAPHICS_DIR}/Public/ImageMemView.h"
	"${GRAPHICS_DIR}/Private/ImageMemView.cpp"
	"${GRAPHICS_DIR}/Public/RenderState.h"
	"${GRAPHICS_DIR}/Private/RenderState.cpp.h"
	"${GRAPHICS_DIR}/Private/EnumUtils.h"
	"${GRAPHICS_DIR}/Private/EnumUtils.cpp.h"
	"${GRAPHICS_DIR}/Scripting/GraphicsBindings.h"
	"${GRAPHICS_DIR}/Scripting/GraphicsBindings.cpp.h"
	"${GRAPHICS_DIR}/Private/Shared.cpp" )
set_property( SOURCE ${GRAPHICS_CPP_SRC} PROPERTY SKIP_UNITY_BUILD_INCLUSION ON )


set( UI_DIR "${MAIN_SOURCE_DIR}/engine/src/graphics/UI" )
set( UI_SRC
	"${UI_DIR}/Layout.h"
	"${UI_DIR}/Layout.cpp"
	"${UI_DIR}/Drawable.h"
	"${UI_DIR}/Drawable.cpp"
	"${UI_DIR}/Controller.h"
	"${UI_DIR}/Controller.cpp"
	"${UI_DIR}/Widget.h"
	"${UI_DIR}/Widget.cpp" )


file( GLOB_RECURSE HPP_SOURCES "*.h" )
file( GLOB_RECURSE CPP_SOURCES "*.cpp" )

set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

add_library( "AssetPacker" STATIC
	${SOURCES}
	${GRAPHICS_SRC}
	${UI_SRC}
)
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
source_group( "external/graphics" FILES ${GRAPHICS_SRC} )
source_group( "external/ui" FILES ${UI_SRC} )

target_link_libraries( "AssetPacker" PUBLIC "Serializing" "Scripting" "ResourceLoaders" "AtlasTools" "VFS" "Graphics" )
target_compile_definitions( "AssetPacker" PRIVATE "AE_BUILD_ASSET_PACKER" )

if (TARGET "FreeType-lib")
	target_link_libraries( "AssetPacker" PUBLIC "FreeType-lib" )
	if (UNIX)
		target_link_libraries( "AssetPacker" PUBLIC "bz2" )
	endif()
endif()
if (TARGET "msdfgen-lib")
	target_link_libraries( "AssetPacker" PUBLIC "msdfgen-lib" )
endif()
if (TARGET "Vulkan-lib")
	target_link_libraries( "AssetPacker" PUBLIC "Vulkan-lib" )
endif()
if (TARGET "Compressonator-lib")
	target_link_libraries( "AssetPacker" PUBLIC "Compressonator-lib" )
endif()
if (TARGET "AstcEncoder-lib")
	target_link_libraries( "AssetPacker" PUBLIC "AstcEncoder-lib" )
endif()
#if (TARGET "ETCPACK-lib")
#	target_link_libraries( "AssetPacker" PUBLIC "ETCPACK-lib" )
#endif()

UseEnginePCH2( "AssetPacker" )
EnableUnityBuild( "AssetPacker" )

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
