# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Graphics")

    file( GLOB_RECURSE HPP_SOURCES "*.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
    endif()

    set( ASSET_PACKER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/asset_packer" )
    set( ASSET_PACKER_HPP_SRC
        "${ASSET_PACKER_DIR}/Packer/ImageAtlasPacker.h"
        "${ASSET_PACKER_DIR}/Packer/ImagePacker.h"
        "${ASSET_PACKER_DIR}/Packer/RasterFontPacker.h"
        "${ASSET_PACKER_DIR}/Packer/UIPacker.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        set( ASSET_PACKER_CPP_SRC
            "${ASSET_PACKER_DIR}/Packer/ImageAtlasPacker.cpp"
            "${ASSET_PACKER_DIR}/Packer/ImagePacker.cpp"
            "${ASSET_PACKER_DIR}/Packer/RasterFontPacker.cpp"
            "${ASSET_PACKER_DIR}/Packer/UIPacker.cpp" )
    endif()

    set( SOURCES
        ${HPP_SOURCES} ${CPP_SOURCES}
        ${ASSET_PACKER_HPP_SRC} ${ASSET_PACKER_CPP_SRC} )

    add_library( "GraphicsHL" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HPP_SOURCES} ${CPP_SOURCES} )
    source_group( "asset_packer" FILES ${ASSET_PACKER_HPP_SRC} ${ASSET_PACKER_CPP_SRC} )

    set_property( TARGET "GraphicsHL" PROPERTY FOLDER "Engine" )
    target_link_libraries( "GraphicsHL" PUBLIC "Graphics" "VFS" "Platform" )
    target_include_directories( "GraphicsHL" PUBLIC "${ASSET_PACKER_DIR}" )

    EnablePCH( "GraphicsHL" )
    EnablePrebuild( "GraphicsHL" )

    install( TARGETS "GraphicsHL" ARCHIVE DESTINATION "lib" )
endif()
