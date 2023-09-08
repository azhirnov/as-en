# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Base")
    file( GLOB SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.Base" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.Base" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.Base" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.Base" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.Base" PUBLIC "Base" )
    target_compile_definitions( "Tests.Base" PUBLIC AE_TEST_BASE )
    target_compile_definitions( "Tests.Base" PRIVATE AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

    add_test( NAME "Tests.Base" COMMAND "Tests.Base" )
endif()
