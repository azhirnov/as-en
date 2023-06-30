# find RenderDoc header

if ( (${CMAKE_SYSTEM_NAME} STREQUAL "Windows") OR (${CMAKE_SYSTEM_NAME} STREQUAL "Linux") )

    if (WIN32)
        set( AE_RENDERDOC_PATH "C:/Program Files/RenderDoc" CACHE PATH "Path to RenderDoc installation folder" )
    else ()
        set( AE_RENDERDOC_PATH "" CACHE PATH "Path to RenderDoc installation folder" )
    endif ()

    if (EXISTS "${AE_RENDERDOC_PATH}/renderdoc_app.h")
        message( STATUS "Found RenderDoc in '${AE_RENDERDOC_PATH}'" )

        add_library( "RenderDoc-lib" INTERFACE )

        target_include_directories( "RenderDoc-lib" INTERFACE "${AE_RENDERDOC_PATH}" )
        target_compile_definitions( "RenderDoc-lib" INTERFACE "AE_ENABLE_RENDERDOC" )

        if (WIN32)
            target_compile_definitions( "RenderDoc-lib" INTERFACE "AE_RENDERDOC_LIB=\"${AE_RENDERDOC_PATH}/renderdoc.dll\"" )
        else ()
            target_compile_definitions( "RenderDoc-lib" INTERFACE "AE_RENDERDOC_LIB=\"${AE_RENDERDOC_PATH}/librenderdoc.so\"" )
        endif ()
    endif ()
endif ()
