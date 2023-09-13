# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ((TARGET "Scripting") AND (TARGET "PipelineCompiler") AND (TARGET "ResourceLoaders") AND (TARGET "ImGUI-lib") AND AE_HAS_CXX_COROUTINE)

    file( GLOB_RECURSE LIB_SOURCES "*.h" "*.cpp" "*.md" )
    file( GLOB_RECURSE DATA_FILES  "*.as" "*.glsl" "*.msl" )

    file( GLOB_RECURSE SHARED_SHADERS
        "${AE_ENGINE_SHARED_DATA}/shaders/*.glsl"
        "${AE_ENGINE_SHARED_DATA}/3party_shaders/*.glsl"
        "${AE_ENGINE_SHARED_DATA}/experimental/*.glsl" )

    add_executable( "ResourceEditor" ${LIB_SOURCES} ${DATA_FILES} ${SHARED_SHADERS} )

    source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LIB_SOURCES} ${DATA_FILES} )
    source_group( "_data/shaders/shared" FILES ${SHARED_SHADERS} )
    set_property( TARGET "ResourceEditor" PROPERTY FOLDER "Samples" )
    target_include_directories( "ResourceEditor" PUBLIC ".." )
    target_include_directories( "ResourceEditor" PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" "${AE_ENGINE_SHARED_DATA}/shaders" )

    EnablePCH( "ResourceEditor" )

    target_link_libraries( "ResourceEditor" PUBLIC
        # tools
        "PipelineCompiler" "ResourceLoaders" "GeometryTools"
        # engine
        "GraphicsHL" "Profiler" "Video"
    )
    target_link_libraries( "ResourceEditor" PUBLIC "ImGUI-lib" )

    target_compile_definitions( "ResourceEditor" PUBLIC
        AE_SHARED_DATA="${AE_ENGINE_SHARED_DATA}"
        AE_LOCAL_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/_data"
    )

    if (DEFINED AE_RESPACK_RELEASE)
        target_compile_definitions( "ResourceEditor" PUBLIC "AE_PUBLIC_VERSION=true" )
    else()
        target_compile_definitions( "ResourceEditor" PUBLIC "AE_PUBLIC_VERSION=false" )
    endif()
    #----------------------------------------------------------

    set( AE_TEMP_FOLDER_RESEDITOR "${AE_TEMP_FOLDER}/samples/res_editor" )

    if (TARGET "OfflinePacker")
        add_custom_target( "ResourceEditor.PackRes"
            DEPENDS  "OfflinePacker"
            COMMAND  "${MAIN_BINARY_DIR}/$<CONFIG>/OfflinePacker${CMAKE_EXECUTABLE_SUFFIX}"  "${CMAKE_CURRENT_SOURCE_DIR}/_ui_data/main.as" "${AE_TEMP_FOLDER_RESEDITOR}"
            COMMENT  "pack resources for 'ResourceEditor' ..."
            VERBATIM
        )
        set_property( TARGET "ResourceEditor.PackRes" PROPERTY FOLDER "Samples" )
        add_dependencies( "PACK_RES" "ResourceEditor.PackRes" )
    endif()

endif()
