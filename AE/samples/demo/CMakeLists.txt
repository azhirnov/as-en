# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GraphicsHL")
    file( GLOB_RECURSE CPP_SOURCES "*.h" "*.cpp" "*.as" "*.glsl" "*.msl" "*.md" )

    if (DEFINED ANDROID)
        set( APP_NAME "SampleDemo" )
        add_library( ${APP_NAME} SHARED ${CPP_SOURCES} )

    else()
        set( ANDROID_SRC_PATH "${MAIN_SOURCE_DIR}/android/demo/src/main" )
        if (NOT EXISTS "${ANDROID_SRC_PATH}")
            message( FATAL_ERROR "Android source is not exists in ${ANDROID_SRC_PATH}" )
        endif()
        file( GLOB_RECURSE JAVA_SOURCES "${ANDROID_SRC_PATH}/*.*" )

        set( APP_NAME "Sample.Demo" )
        add_executable( ${APP_NAME} ${CPP_SOURCES} ${JAVA_SOURCES} )

        source_group( "Android" FILES ${JAVA_SOURCES} )
        set_source_files_properties( ${JAVA_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
    endif()

    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_SOURCES} )
    set_property( TARGET ${APP_NAME} PROPERTY FOLDER "Samples" )
    target_link_libraries( ${APP_NAME} PUBLIC "Platform" "GraphicsHL" "Profiler" "GeometryTools" )
    target_include_directories( ${APP_NAME} PUBLIC ".." )
    target_include_directories( ${APP_NAME} PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" "${AE_ENGINE_SHARED_DATA}/shaders" )

    set( AE_TEMP_FOLDER_DEMO "${AE_TEMP_FOLDER}/samples/demo" )
    target_compile_definitions( ${APP_NAME} PUBLIC AE_RES_FOLDER="${AE_TEMP_FOLDER_DEMO}" )

    if (TARGET "ImGUI-lib")
        target_link_libraries( ${APP_NAME} PUBLIC "ImGUI-lib" )
    endif()

    EnablePCH2( ${APP_NAME} "Demo" )
    #----------------------------------------------------------

    if (TARGET "OfflinePacker")
        add_custom_target( "${APP_NAME}.PackRes"
            DEPENDS  "OfflinePacker"
            COMMAND  "${MAIN_BINARY_DIR}/$<CONFIG>/OfflinePacker${CMAKE_EXECUTABLE_SUFFIX}"  "${CMAKE_CURRENT_SOURCE_DIR}/_data/main.as" "${AE_TEMP_FOLDER_DEMO}"
            COMMENT  "pack resources for '${APP_NAME}' ..."
            VERBATIM
        )
        set_property( TARGET "${APP_NAME}.PackRes" PROPERTY FOLDER "Samples" )
        #add_dependencies( ${APP_NAME} "${APP_NAME}.PackRes" )
        add_dependencies( "PACK_RES" "${APP_NAME}.PackRes" )
    endif()
endif()
