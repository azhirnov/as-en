# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_ENABLE_VULKAN} OR ${AE_ENABLE_METAL} OR ${AE_ENABLE_REMOTE_GRAPHICS})

    file( GLOB_RECURSE PRIVATE_HPP_SOURCES "Private/*.h" )
    file( GLOB_RECURSE PUBLIC_HPP_SOURCES "Public/*.h" )
    file( GLOB_RECURSE RG_HPP_SOURCES "RenderGraph/*.h" )

    if (NOT (DEFINED ENGINE_LIBS_PATH))
        file( GLOB_RECURSE PRIVATE_CPP_SOURCES "Private/*.cpp" )
        file( GLOB_RECURSE PUBLIC_CPP_SOURCES "Public/*.cpp" )
        file( GLOB_RECURSE RG_CPP_SOURCES "RenderGraph/*.cpp" )
    endif()

    if (TARGET "Scripting")
        file( GLOB_RECURSE SCRIPTING_HPP_SOURCES "Scripting/*.h" )
        if (NOT (DEFINED ENGINE_LIBS_PATH))
            file( GLOB_RECURSE SCRIPTING_CPP_SOURCES "Scripting/*.cpp" )
        endif()
    endif()

    if (${AE_ENABLE_METAL} OR (MSVC AND NOT ${AE_ENABLE_REMOTE_GRAPHICS}))
        file( GLOB_RECURSE METAL_HPP_SOURCES "Metal/*.h" )
        if (NOT (DEFINED ENGINE_LIBS_PATH))
            file( GLOB_RECURSE METAL_CPP_SOURCES "Metal/*.cpp" )
            file( GLOB_RECURSE METAL_OBJC_SOURCES "Metal/*.mm" )
        endif()
    endif()

    if (MSVC OR ${AE_ENABLE_REMOTE_GRAPHICS})
        file( GLOB_RECURSE REMOTE_HPP_SOURCES "Remote/*.h" )
        if (NOT (DEFINED ENGINE_LIBS_PATH))
            file( GLOB_RECURSE REMOTE_CPP_SOURCES "Remote/*.cpp" )
        endif()
    endif()

    set( PIPELINE_COMPILER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/pipeline_compiler" )
    set( PIPELINE_COMPILER_HPP_SRC
        "${PIPELINE_COMPILER_DIR}/Packer/PipelinePack.h"
        "${PIPELINE_COMPILER_DIR}/Packer/RenderPassPack.h"
        "${PIPELINE_COMPILER_DIR}/Packer/SamplerPack.h"
        "${PIPELINE_COMPILER_DIR}/Packer/FeatureSetPack.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        set( PIPELINE_COMPILER_CPP_SRC
            "${PIPELINE_COMPILER_DIR}/Packer/PipelinePackDeserializer.cpp"
            "${PIPELINE_COMPILER_DIR}/Packer/RenderPassPack.cpp"
            "${PIPELINE_COMPILER_DIR}/Packer/SamplerPack.cpp"
            "${PIPELINE_COMPILER_DIR}/Packer/FeatureSetPack.cpp" )
    endif()

    if (${AE_ENABLE_VULKAN})
        file( GLOB_RECURSE VULKAN_HPP_SOURCES "Vulkan/*.h" )
        if (NOT (DEFINED ENGINE_LIBS_PATH))
            file( GLOB_RECURSE VULKAN_CPP_SOURCES "Vulkan/*.cpp" )
        endif()
        set( SHADER_TRACE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../tools/res_pack/shader_trace" )
        set( SHADER_TRACE_HPP_SRC
            "${SHADER_TRACE_DIR}/Public/ShaderTrace.h" )
        if (NOT (DEFINED ENGINE_LIBS_PATH))
            set( SHADER_TRACE_CPP_SRC
                "${SHADER_TRACE_DIR}/Impl/ShaderTrace.cpp"
                "${SHADER_TRACE_DIR}/Impl/ParseShaderTrace.cpp" )
        endif()
    endif()

    set( SOURCES
        ${PRIVATE_HPP_SOURCES} ${PRIVATE_CPP_SOURCES}
        ${PUBLIC_HPP_SOURCES} ${PUBLIC_CPP_SOURCES}
        ${RG_HPP_SOURCES} ${RG_CPP_SOURCES}
        ${SCRIPTING_HPP_SOURCES} ${SCRIPTING_CPP_SOURCES}
        ${VULKAN_HPP_SOURCES} ${VULKAN_CPP_SOURCES}
        ${METAL_HPP_SOURCES} ${METAL_CPP_SOURCES} ${METAL_OBJC_SOURCES}
        ${REMOTE_HPP_SOURCES} ${REMOTE_CPP_SOURCES}
    )

    add_library( "Graphics" STATIC
        ${SOURCES}
        ${PIPELINE_COMPILER_HPP_SRC} ${PIPELINE_COMPILER_CPP_SRC}
        ${SHADER_TRACE_HPP_SRC} ${SHADER_TRACE_CPP_SRC}
        ${PREBUILD_CPP_FILE} )

    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    source_group( "external/pipeline_compiler" FILES ${PIPELINE_COMPILER_HPP_SRC} ${PIPELINE_COMPILER_CPP_SRC} )

    set_property( TARGET "Graphics" PROPERTY FOLDER "Engine" )
    target_link_libraries( "Graphics" PUBLIC "Threading" "Serializing" )

    target_include_directories( "Graphics" PUBLIC "${PIPELINE_COMPILER_DIR}" )

    set( AE_GRAPHICS_STRONG_VALIDATION OFF CACHE BOOL "Safe mode: validate all arguments, throw exception in context..." )
    if (${AE_GRAPHICS_STRONG_VALIDATION})
        target_compile_definitions( "Graphics" PUBLIC AE_GRAPHICS_STRONG_VALIDATION=1 )
    else()
        target_compile_definitions( "Graphics" PUBLIC AE_GRAPHICS_STRONG_VALIDATION=0 )
    endif()

    if (${AE_ENABLE_VULKAN})
        source_group( "external/shader_trace" FILES ${SHADER_TRACE_HPP_SRC} ${SHADER_TRACE_CPP_SRC} )
        target_link_libraries( "Graphics" PUBLIC "Vulkan-lib" "VulkanExtEmulation" )
        target_include_directories( "Graphics" PUBLIC "${SHADER_TRACE_DIR}/Public" )
        target_compile_definitions( "Graphics" PUBLIC AE_ENABLE_GLSL_TRACE )

        if (${AE_ENABLE_VMA})
            target_link_libraries( "Graphics" PUBLIC "VMA-lib" )
        endif()
        if (TARGET "RenderDoc-lib")
            target_link_libraries( "Graphics" PUBLIC "RenderDoc-lib" )
        endif()
        if (WIN32 AND TARGET "NVPerf-lib")
            target_link_libraries( "Graphics" PUBLIC "NVPerf-lib" )
        endif()

        if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
            set( AE_VK_TIMELINE_SEMAPHORE OFF CACHE BOOL "Use Vulkan timeline semaphore" )
        else()
            set( AE_VK_TIMELINE_SEMAPHORE ON CACHE BOOL "Use Vulkan timeline semaphore" )
        endif()
        mark_as_advanced( AE_VK_TIMELINE_SEMAPHORE )
        if (${AE_VK_TIMELINE_SEMAPHORE})
            target_compile_definitions( "Graphics" PUBLIC AE_VK_TIMELINE_SEMAPHORE=1 )
        else()
            target_compile_definitions( "Graphics" PUBLIC AE_VK_TIMELINE_SEMAPHORE=0 )
        endif()

        if (APPLE)
            if (NOT EXISTS "/usr/local/lib/libMoltenVK.dylib")
                message( FATAL_ERROR "'/usr/local/lib/libMoltenVK.dylib' is not exists, instal VulkanSDK with MoltenVk and try again" )
            endif()
            if (NOT EXISTS "/usr/local/lib/libvulkan.dylib")
                message( FATAL_ERROR "'/usr/local/lib/libvulkan.dylib' is not exists, instal VulkanSDK with MoltenVk and try again" )
            endif()
        endif()
    endif()

    if (${AE_ENABLE_METAL} OR MSVC)
        if ( (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin") OR (${CMAKE_SYSTEM_NAME} STREQUAL "iOS") )
            find_library( METAL_FRAMEWORK Metal REQUIRED )
            target_link_libraries( "Graphics" PUBLIC ${METAL_FRAMEWORK} )
            set_target_properties( "Graphics" PROPERTIES XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO )
        else()
            set_source_files_properties( ${METAL_OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
            set_source_files_properties( ${METAL_CPP_SOURCES}  PROPERTIES HEADER_FILE_ONLY TRUE )
        endif()
    endif()

    if (${AE_ENABLE_REMOTE_GRAPHICS})
        target_link_libraries( "Graphics" PUBLIC "Networking" )
        target_compile_definitions( "Graphics" PUBLIC AE_ENABLE_REMOTE_GRAPHICS )
    else()
        set_source_files_properties( ${REMOTE_CPP_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
    endif()

    EnablePCH( "Graphics" )
    EnablePrebuild( "Graphics" )

    install( TARGETS "Graphics" ARCHIVE DESTINATION "lib" )
endif()
