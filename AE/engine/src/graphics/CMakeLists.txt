# Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

if (TARGET "GraphicsRHI")

	file( GLOB_RECURSE HPP_SOURCES "*.h" "*.as" )
	file( GLOB_RECURSE CPP_SOURCES "*.cpp" )

	set( ASSET_PACKER_DIR "${MAIN_SOURCE_DIR}/engine/tools/res_pack/asset_packer" )
	set( ASSET_PACKER_HPP_SRC
		"${ASSET_PACKER_DIR}/Packer/ImageAtlasPacker.h"
		"${ASSET_PACKER_DIR}/Packer/ImageAtlasPacker.cpp.h"
		"${ASSET_PACKER_DIR}/Packer/ImagePacker.h"
		"${ASSET_PACKER_DIR}/Packer/ImagePacker.cpp.h"
		"${ASSET_PACKER_DIR}/Packer/RasterFontPacker.h"
		"${ASSET_PACKER_DIR}/Packer/RasterFontPacker.cpp.h"
		"${ASSET_PACKER_DIR}/Packer/UIPacker.h" )
	set( ASSET_PACKER_CPP_SRC
		"${ASSET_PACKER_DIR}/Packer/UIPacker.cpp" )

	set( SOURCES
		${HPP_SOURCES} ${CPP_SOURCES}
		${ASSET_PACKER_HPP_SRC} ${ASSET_PACKER_CPP_SRC} )

	add_library( "Graphics" STATIC ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HPP_SOURCES} ${CPP_SOURCES} )
	source_group( "asset_packer" FILES ${ASSET_PACKER_HPP_SRC} ${ASSET_PACKER_CPP_SRC} )

	set_property( TARGET "Graphics" PROPERTY FOLDER "Engine" )
	target_link_libraries( "Graphics" PUBLIC "GraphicsRHI" "VFS" "Platform" )

	UseEnginePCH( "Graphics" )
	EnableUnityBuild( "Graphics" )

	install( TARGETS "Graphics" ARCHIVE DESTINATION "lib" )
endif()
