# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GLSLang-lib")
    set( GRAPHICS_DIR "${MAIN_SOURCE_DIR}/engine/src/graphics" )
    set( GRAPHICS_HPP_SRC
        "${GRAPHICS_DIR}/Scripting/GraphicsBindings.h"
        "${GRAPHICS_DIR}/Public/FeatureSet.h"
        "${GRAPHICS_DIR}/Public/RenderState.h"
        "${GRAPHICS_DIR}/Public/PipelineDesc.h"
        "${GRAPHICS_DIR}/Public/SamplerDesc.h"
        "${GRAPHICS_DIR}/Private/EnumUtils.h"
        "${GRAPHICS_DIR}/Private/EnumToString.h"
        "${GRAPHICS_DIR}/Public/IDs.h"
        "${GRAPHICS_DIR}/Vulkan/VEnumCast.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        set( GRAPHICS_CPP_SRC
            "${GRAPHICS_DIR}/Scripting/GraphicsBindings.cpp"
            "${GRAPHICS_DIR}/Private/FeatureSet.cpp"
            "${GRAPHICS_DIR}/Private/RenderState.cpp"
            "${GRAPHICS_DIR}/Private/EnumUtils.cpp"
            "${GRAPHICS_DIR}/Vulkan/VEnumCast.cpp" )
    endif()
    file( GLOB_RECURSE SOURCES "*.*" )

    add_library( "PipelineCompiler" STATIC ${SOURCES} ${GRAPHICS_HPP_SRC} ${GRAPHICS_CPP_SRC} ${PREBUILD_CPP_FILE} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    source_group( "external/graphics" FILES ${GRAPHICS_HPP_SRC} ${GRAPHICS_CPP_SRC} )

    target_link_libraries( "PipelineCompiler" PUBLIC "Serializing" "Scripting" "Threading" )
    target_compile_definitions( "PipelineCompiler" PRIVATE "AE_BUILD_PIPELINE_COMPILER" "AE_TEST_PIPELINE_COMPILER" )
    target_include_directories( "PipelineCompiler" PUBLIC "." )

    if (WIN32)
        set( METAL_TOOLS "${AE_EXTERNAL_REP_PATH}/external/macos/MetalTools/bin" )
        if ( EXISTS "${METAL_TOOLS}" )
            message( STATUS "Metal tools '${METAL_TOOLS}' is not exists" )
        endif()
        if ( (EXISTS "${METAL_TOOLS}") AND (TARGET "SPIRV-Cross-lib") )
            target_compile_definitions( "PipelineCompiler" PUBLIC "AE_METAL_TOOLS=\"${METAL_TOOLS}\"" )
            target_link_libraries( "PipelineCompiler" PUBLIC "SPIRV-Cross-lib" )
        endif()
    endif()

    if (TARGET "Vulkan-lib")
        target_link_libraries( "PipelineCompiler" PUBLIC "Vulkan-lib" )
    endif()

    target_link_libraries( "PipelineCompiler" PUBLIC "GLSLang-lib" )

    if (TARGET "ShaderTrace")
        target_link_libraries( "PipelineCompiler" PUBLIC "ShaderTrace" )
    endif()

    # must be same as in 'Graphics' module
    if (DEFINED AE_GRAPHICS_STRONG_VALIDATION)
        if (${AE_GRAPHICS_STRONG_VALIDATION})
            target_compile_definitions( "PipelineCompiler" PRIVATE AE_GRAPHICS_STRONG_VALIDATION=1 )
        else()
            target_compile_definitions( "PipelineCompiler" PRIVATE AE_GRAPHICS_STRONG_VALIDATION=0 )
        endif()
    else()
        message( FATAL_ERROR "AE_GRAPHICS_STRONG_VALIDATION is not defined" )
    endif()

    EnablePCH( "PipelineCompiler" )
    EnablePrebuild( "PipelineCompiler" )

    add_library( "PipelineCompiler-shared" SHARED
        "PipelineCompiler.h"
        "Lib/Export.cpp"
        "Lib/PipelineCompiler.def"
    )
    target_link_libraries( "PipelineCompiler-shared" PUBLIC "PipelineCompiler" )

    set_property( TARGET "PipelineCompiler" PROPERTY FOLDER "Engine/ResPack" )
    set_property( TARGET "PipelineCompiler-shared" PROPERTY FOLDER "Engine/ResPack" )

    # generated cpp file used as dependency to trigger project rebuild
    set( PipelineCompiler.trigger "${CMAKE_CURRENT_SOURCE_DIR}/PipelineCompiler.trigger.cpp" CACHE INTERNAL "" FORCE )
    add_custom_command( TARGET "PipelineCompiler-shared" POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch "${PipelineCompiler.trigger}" )
    set_source_files_properties( "${PipelineCompiler.trigger}" PROPERTIES GENERATED TRUE )
endif()
