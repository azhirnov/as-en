# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "PipelineCompiler-shared" OR TARGET "PipelineCompiler-shared" OR TARGET "InputActionsBinding-shared")

    set( PIPELINE_COMPILER_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/pipeline_compiler" )
    set( SHADER_TRACE_DIR       "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/shader_trace" )
    set( GRAPHICS_DIR           "${CMAKE_CURRENT_SOURCE_DIR}/../../src/graphics" )
    set( PLATFORM_DIR           "${CMAKE_CURRENT_SOURCE_DIR}/../../src/platform" )

    file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.txt" "*.glsl" "*.msl" )
    file( GLOB_RECURSE SHARED_DATA "${AE_ENGINE_SHARED_DATA}/*.*" )

    set( PIPELINE_COMPILER_SRC
        "${PIPELINE_COMPILER_DIR}/Packer/PipelinePackDeserializer.cpp"
        "${PIPELINE_COMPILER_DIR}/Packer/RenderPassPack.cpp"
        "${PIPELINE_COMPILER_DIR}/Packer/SamplerPack.cpp"
        "${PIPELINE_COMPILER_DIR}/Packer/FeatureSetPack.cpp" )
    set( GRAPHICS_SRC
        "${GRAPHICS_DIR}/Public/RenderState.h"
        "${GRAPHICS_DIR}/Private/RenderState.cpp"
        "${GRAPHICS_DIR}/Private/EnumUtils.h"
        "${GRAPHICS_DIR}/Private/EnumUtils.cpp"
        "${GRAPHICS_DIR}/Public/FeatureSet.h"
        "${GRAPHICS_DIR}/Private/FeatureSet.cpp" )
    set( PLATFORM_SRC
        "${PLATFORM_DIR}/Private/SerializableInputActions.h"
        "${PLATFORM_DIR}/Private/SerializableInputActions.cpp"
        "${PLATFORM_DIR}/GLFW/SerializableInputActionsGLFW.h"
        "${PLATFORM_DIR}/GLFW/SerializableInputActionsGLFW.cpp"
        "${PLATFORM_DIR}/WinAPI/SerializableInputActionsWinAPI.h"
        "${PLATFORM_DIR}/WinAPI/SerializableInputActionsWinAPI.cpp"
        "${PLATFORM_DIR}/Android/SerializableInputActionsAndroid.h"
        "${PLATFORM_DIR}/Android/SerializableInputActionsAndroid.cpp"
        "${PLATFORM_DIR}/OpenVR/SerializableInputActionsOpenVR.h"
        "${PLATFORM_DIR}/OpenVR/SerializableInputActionsOpenVR.cpp" )

    if (TARGET "ShaderTrace")
        set( SHADER_TRACE_SRC "" )
    else()
        set( SHADER_TRACE_SRC
            "${SHADER_TRACE_DIR}/Public/ShaderTrace.h"
            "${SHADER_TRACE_DIR}/Impl/ShaderTrace.cpp"
            "${SHADER_TRACE_DIR}/Impl/ParseShaderTrace.cpp" )
    endif()

    add_executable( "Tests.AssetPacker" ${SOURCES} ${SHARED_DATA} ${PIPELINE_COMPILER_SRC} ${GRAPHICS_SRC} ${PLATFORM_SRC} ${SHADER_TRACE_SRC} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    source_group( TREE "${AE_ENGINE_SHARED_DATA}/.." FILES ${SHARED_DATA} )
    source_group( "external/graphics" FILES ${GRAPHICS_SRC} )
    source_group( "external/platform" FILES ${PLATFORM_SRC} )
    source_group( "external/pipeline_compiler" FILES ${PIPELINE_COMPILER_SRC} )
    source_group( "external/shader_trace" FILES ${SHADER_TRACE_SRC} )

    set_property( TARGET "Tests.AssetPacker" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.AssetPacker" PUBLIC "Base" )

    if (TARGET "Vulkan-lib")
        target_link_libraries( "Tests.AssetPacker" PUBLIC "Vulkan-lib" )
    endif()

    target_include_directories( "Tests.AssetPacker" PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" "${AE_ENGINE_SHARED_DATA}/shaders" )

    if (TARGET "ShaderTrace")
        target_link_libraries( "Tests.AssetPacker" PUBLIC "ShaderTrace" )
    else()
        target_include_directories( "Tests.AssetPacker" PUBLIC "${SHADER_TRACE_DIR}/Public" )
        target_compile_definitions( "Tests.AssetPacker" PRIVATE AE_ENABLE_GLSL_TRACE )
    endif()

    target_include_directories( "Tests.AssetPacker" PRIVATE
        "../../tools/res_pack"
        "${PIPELINE_COMPILER_DIR}" )

    target_compile_definitions( "Tests.AssetPacker" PUBLIC
        AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
        AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
        AE_TEST_PIPELINE_COMPILER
    )

    if (TARGET "PipelineCompiler-shared")
        target_compile_definitions( "Tests.AssetPacker" PUBLIC AE_PIPELINE_COMPILER_LIBRARY="${MAIN_BINARY_DIR}" )
        add_dependencies( "Tests.AssetPacker" "PipelineCompiler-shared" )
        target_sources( "Tests.AssetPacker" PRIVATE "${PipelineCompiler.trigger}" )
        source_group( "_gen_" FILES "${PipelineCompiler.trigger}" )
        set_property( SOURCE "${PipelineCompiler.trigger}" PROPERTY GENERATED 1 )
    endif()

    if (TARGET "PipelineCompiler-shared")
        target_compile_definitions( "Tests.AssetPacker" PUBLIC AE_ASSET_PACKER_LIBRARY="${MAIN_BINARY_DIR}" )
        add_dependencies( "Tests.AssetPacker" "AssetPacker-shared" )
        target_sources( "Tests.AssetPacker" PRIVATE "${AssetPacker.trigger}" )
        source_group( "_gen_" FILES "${AssetPacker.trigger}" )
        set_property( SOURCE "${AssetPacker.trigger}" PROPERTY GENERATED 1 )
    endif()

    if (TARGET "InputActionsBinding-shared")
        target_compile_definitions( "Tests.AssetPacker" PUBLIC AE_INPUT_ACTIONS_BINDING_LIBRARY="${MAIN_BINARY_DIR}" )
        add_dependencies( "Tests.AssetPacker" "InputActionsBinding-shared" )
        target_sources( "Tests.AssetPacker" PRIVATE "${InputActionsBinding.trigger}" )
        source_group( "_gen_" FILES "${InputActionsBinding.trigger}" )
        set_property( SOURCE "${InputActionsBinding.trigger}" PROPERTY GENERATED 1 )
    endif()

    # must be same as in 'Graphics' module
    if (DEFINED AE_GRAPHICS_STRONG_VALIDATION)
        if (${AE_GRAPHICS_STRONG_VALIDATION})
            target_compile_definitions( "Tests.AssetPacker" PRIVATE AE_GRAPHICS_STRONG_VALIDATION=1 )
        else()
            target_compile_definitions( "Tests.AssetPacker" PRIVATE AE_GRAPHICS_STRONG_VALIDATION=0 )
        endif()
    else()
        message( FATAL_ERROR "AE_GRAPHICS_STRONG_VALIDATION is not defined" )
    endif()

    add_test( NAME "Tests.AssetPacker" COMMAND "Tests.AssetPacker" )

endif()
