# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GraphicsHL")
    file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" )
    file( GLOB_RECURSE SHARED_DATA "${AE_ENGINE_SHARED_DATA}/*.*" )

    if (DEFINED ANDROID)
        add_library( "Tests.GraphicsHL" STATIC ${SOURCES} ${SHARED_DATA} )
    elseif (APPLE)
        add_executable( "Tests.GraphicsHL" MACOSX_BUNDLE ${SOURCES} ${SHARED_DATA} )
    else()
        add_executable( "Tests.GraphicsHL" ${SOURCES} ${SHARED_DATA} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    source_group( TREE "${AE_ENGINE_SHARED_DATA}/.." FILES ${SHARED_DATA} )

    set_property( TARGET "Tests.GraphicsHL" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.GraphicsHL" PUBLIC "Platform" "GraphicsHL" "GraphicsTestUtils" )
    target_compile_definitions( "Tests.GraphicsHL" PUBLIC AE_TEST_GRAPHICS_HL )

    target_include_directories( "Tests.GraphicsHL" PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" )

    target_compile_definitions( "Tests.GraphicsHL" PRIVATE
        AE_PIPELINE_COMPILER_LIBRARY="${MAIN_BINARY_DIR}"
        AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
        AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        AE_CANVAS_VERTS="${MAIN_SOURCE_DIR}/engine/src/graphics_hl/Canvas/VertexTypes.as"
    )

    if (TARGET "OfflinePacker")
        add_custom_target( "Tests.GraphicsHL.PackRes"
            DEPENDS  "OfflinePacker"
            COMMAND  $<TARGET_FILE:OfflinePacker>
                        -i "${CMAKE_CURRENT_SOURCE_DIR}/DrawTests/res_pack.as"
                        -o "${AE_TEMP_FOLDER}/engine/graphics_hl"
            COMMENT  "pack resources for 'Tests.GraphicsHL' ..."
            VERBATIM
        )
        set_property( TARGET "Tests.GraphicsHL.PackRes" PROPERTY FOLDER "Engine/Tests" )
        add_dependencies( "PACK_RES" "Tests.GraphicsHL.PackRes" )
    endif()

    if (DEFINED ANDROID AND ${AE_ENABLE_VULKAN})
        target_compile_definitions( "Tests.GraphicsHL" PRIVATE
            AE_REFDUMP_PATH="tests_graphicshl_vulkan_refdump"
            AE_REF_IMG_PATH="tests_graphicshl_vulkan_refimg"
            AE_RES_PACK="pipelines.bin" )

    elseif (${AE_ENABLE_VULKAN} OR ${AE_ENABLE_REMOTE_GRAPHICS})
        target_compile_definitions( "Tests.GraphicsHL" PRIVATE
            AE_REFDUMP_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics_hl/vulkan"
            AE_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics_hl/vulkan"
            AE_RES_PACK="${AE_TEMP_FOLDER}/engine/graphics_hl/vk/pipelines.bin" )

    elseif (${AE_ENABLE_METAL})
        target_compile_definitions( "Tests.GraphicsHL" PRIVATE
            AE_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics_hl/metal"
            AE_RES_PACK="${AE_TEMP_FOLDER}/engine/graphics_hl/mac/pipelines.bin" )
    endif()

    if (NOT ${AE_CI_BUILD_NO_GRAPHICS})
        add_test( NAME "Tests.GraphicsHL" COMMAND "Tests.GraphicsHL" )
    endif()
endif()
