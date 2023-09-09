# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.h" "*.cpp" "*.as" "*.glsl" "*.msl" )

if (DEFINED ANDROID)
    set( APP_NAME "SampleDemo" )
    add_library( ${APP_NAME} SHARED ${SOURCES} )
else()
    set( APP_NAME "Sample.Demo" )
    add_executable( ${APP_NAME} ${SOURCES} )
endif()

if (TARGET "OfflinePacker")
    add_custom_target( "${APP_NAME}.PackRes"
        DEPENDS  "OfflinePacker"
        COMMAND  "${MAIN_BINARY_DIR}/$<CONFIG>/OfflinePacker${CMAKE_EXECUTABLE_SUFFIX}"  "${CMAKE_CURRENT_SOURCE_DIR}/_data/main.as" "${AE_TEMP_FOLDER}/samples/demo"
        COMMENT  "pack resources for '${APP_NAME}' ..."
        VERBATIM
    )
    set_property( TARGET "${APP_NAME}.PackRes" PROPERTY FOLDER "Samples" )
    #add_dependencies( ${APP_NAME} "${APP_NAME}.PackRes" )
    add_dependencies( "PACK_RES" "${APP_NAME}.PackRes" )
endif()
target_compile_definitions( ${APP_NAME} PUBLIC AE_RES_FOLDER="${AE_TEMP_FOLDER}/samples/demo" )

if (TARGET "GraphicsHL")
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET ${APP_NAME} PROPERTY FOLDER "Samples" )
    target_link_libraries( ${APP_NAME} PUBLIC "Platform" "GraphicsHL" "Profiler" "GeometryTools" )
    target_include_directories( ${APP_NAME} PUBLIC ".." )
    target_include_directories( ${APP_NAME} PUBLIC "${AE_ENGINE_SHARED_DATA}/scripts" "${AE_ENGINE_SHARED_DATA}/shaders" )

    if (TARGET "ImGUI-lib")
        target_link_libraries( ${APP_NAME} PUBLIC "ImGUI-lib" )
    endif()

    EnablePCH2( ${APP_NAME} "Demo" )
endif()
