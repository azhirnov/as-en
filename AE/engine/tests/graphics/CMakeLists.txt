# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Graphics")
    file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" )
    file( GLOB_RECURSE SHARED_DATA "${AE_ENGINE_SHARED_DATA}/*.*" )

    if (DEFINED ANDROID)
        add_library( "Tests.Graphics" STATIC ${SOURCES} ${SHARED_DATA} )
    elseif (APPLE)
        add_executable( "Tests.Graphics" MACOSX_BUNDLE ${SOURCES} ${SHARED_DATA} )
    else()
        add_executable( "Tests.Graphics" ${SOURCES} ${SHARED_DATA} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    source_group( TREE "${AE_ENGINE_SHARED_DATA}/.." FILES ${SHARED_DATA} )

    set_property( TARGET "Tests.Graphics" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.Graphics" PUBLIC "Graphics" "Platform" "GraphicsTestUtils" )

    target_include_directories( "Tests.Graphics" PUBLIC "." )
    target_include_directories( "Tests.Graphics" PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" "${AE_ENGINE_SHARED_DATA}/shaders" )

    target_compile_definitions( "Tests.Graphics" PUBLIC AE_TEST_GRAPHICS )

    target_compile_definitions( "Tests.Graphics" PRIVATE
        AE_PIPELINE_COMPILER_LIBRARY="${MAIN_BINARY_DIR}"
        AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
        AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )

    if (TARGET "OfflinePacker")
        add_custom_target( "Tests.Graphics.PackRes"
            DEPENDS  "OfflinePacker"
            COMMAND  "${MAIN_BINARY_DIR}/$<CONFIG>/OfflinePacker${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/RenderGraph/res_pack.as" "${AE_TEMP_FOLDER}/engine/graphics"
            COMMENT  "pack resources for 'Tests.Graphics' ..."
            VERBATIM
        )
        set_property( TARGET "Tests.Graphics.PackRes" PROPERTY FOLDER "Engine/Tests" )
        add_dependencies( "PACK_RES" "Tests.Graphics.PackRes" )
    endif()

    if (${AE_ENABLE_VULKAN} OR ${AE_ENABLE_REMOTE_GRAPHICS})
        target_compile_definitions( "Tests.Graphics" PRIVATE
            AE_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics/vulkan"
            AE_RES_PACK="${AE_TEMP_FOLDER}/engine/graphics/vk/pipelines.bin" )

    elseif (${AE_ENABLE_METAL})
        target_compile_definitions( "Tests.Graphics" PRIVATE
            AE_REF_IMG_PATH="${AE_EXTERNAL_REP_PATH}/tests/graphics/metal"
            AE_RES_PACK="${AE_TEMP_FOLDER}/engine/graphics/mac/pipelines.bin"  )
    endif()

    if (NOT ${AE_CI_BUILD_NO_GRAPHICS})
        add_test( NAME "Tests.Graphics" COMMAND "Tests.Graphics" )
    endif()
endif()