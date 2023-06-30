
if ((TARGET "Scripting") AND (TARGET "PipelineCompiler") AND (TARGET "ResourceLoaders") AND (TARGET "ImGUI-lib") AND AE_HAS_CXX_COROUTINE)

    file( GLOB_RECURSE LIB_SOURCES "*.h" "*.cpp" )
    file( GLOB_RECURSE DATA_FILES  "*.as" "*.glsl" "*.msl" )

    set( DATA_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/../../engine/tools/res_pack/shared_data" )
    file( GLOB_RECURSE SHARED_SHADERS "${DATA_FOLDER}/shaders/*.glsl" )

    add_executable( "ResourceEditor" ${LIB_SOURCES} ${DATA_FILES} ${SHARED_SHADERS} )
    source_group( TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${LIB_SOURCES} ${DATA_FILES} )
    source_group( "_data/shaders/shared" FILES ${SHARED_SHADERS} )
    target_link_libraries( "ResourceEditor" PUBLIC "GraphicsHL" "Platform" "Profiler" )
    set_property( TARGET "ResourceEditor" PROPERTY FOLDER "Samples" )
    target_include_directories( "ResourceEditor" PUBLIC ".." )
    target_include_directories( "ResourceEditor" PUBLIC "${DATA_FOLDER}/scripts" "${DATA_FOLDER}/shaders" )
    target_link_libraries( "ResourceEditor" PRIVATE "Scripting" "PipelineCompiler" "ResourceLoaders" "ImGUI-lib" "GeometryTools" )
    target_compile_definitions( "ResourceEditor" PRIVATE
        AE_SHARED_DATA="${DATA_FOLDER}"
        AE_LOCAL_DATA_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/_data"
    )

    if ( TARGET "Video" )
        target_link_libraries( "ResourceEditor" PRIVATE "Video" )
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
