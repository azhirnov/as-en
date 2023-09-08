# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Scripting")
    add_executable( "OfflinePacker" "main.cpp" )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

    set_property( TARGET "OfflinePacker" PROPERTY FOLDER "Engine/ResPack" )

    target_link_libraries( "OfflinePacker" PUBLIC "Scripting" "VFS" )
    target_link_libraries( "OfflinePacker" PUBLIC "PipelineCompiler" "InputActionsBinding" "AssetPacker" )
    target_include_directories( "OfflinePacker" PUBLIC ".." )

    target_compile_definitions( "OfflinePacker" PRIVATE
        AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
        AE_CANVAS_VERTS="${CMAKE_CURRENT_SOURCE_DIR}/../../../src/graphics_hl/Canvas/VertexTypes.as"
        AE_BUILD_ASSET_PACKER
        AE_BUILD_INPUT_ACTIONS_BINDING
        AE_BUILD_PIPELINE_COMPILER
    )

    add_executable( "PACK_RES" EXCLUDE_FROM_ALL "empty.cpp" )
    set_property( TARGET "PACK_RES" PROPERTY FOLDER "CMakePredefinedTargets" )
    add_dependencies( "PACK_RES" "OfflinePacker" )
endif()
