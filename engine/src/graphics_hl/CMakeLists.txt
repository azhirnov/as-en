if (TARGET "Graphics")

	file( GLOB_RECURSE CPP_SOURCES "*.*" )

	set( ASSET_PACKER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/asset_packer" )
	set( ASSET_PACKER_SRC
		"${ASSET_PACKER_DIR}/Packer/ImageAtlasPacker.h"
		#"${ASSET_PACKER_DIR}/Packer/ImageAtlasPacker.cpp"
		"${ASSET_PACKER_DIR}/Packer/ImagePacker.h"
		#"${ASSET_PACKER_DIR}/Packer/ImagePacker.cpp"
		"${ASSET_PACKER_DIR}/Packer/RasterFontPacker.h"
		#"${ASSET_PACKER_DIR}/Packer/RasterFontPacker.cpp"
		"${ASSET_PACKER_DIR}/Packer/UIPacker.h"
		#"${ASSET_PACKER_DIR}/Packer/UIPacker.cpp"
	)

	set( SOURCES ${CPP_SOURCES} ${ASSET_PACKER_SRC} )

	add_library( "GraphicsHL" STATIC ${SOURCES} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_SOURCES} )
	source_group( "asset_packer" FILES ${ASSET_PACKER_SRC} )

	set_property( TARGET "GraphicsHL" PROPERTY FOLDER "Engine" )
	target_link_libraries( "GraphicsHL" PUBLIC "Graphics" "VFS" "Platform" )
	target_include_directories( "GraphicsHL" PUBLIC "${ASSET_PACKER_DIR}" )

	install( TARGETS "GraphicsHL" ARCHIVE DESTINATION "lib" )
endif()
