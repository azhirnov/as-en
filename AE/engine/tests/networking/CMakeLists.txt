# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Networking")
    file( GLOB_RECURSE SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.Networking" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.Networking" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.Networking" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.Networking" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.Networking" PUBLIC "Networking" )
    target_compile_definitions( "Tests.Networking" PUBLIC AE_TEST_NETWORKING )

    add_test( NAME "Tests.Networking" COMMAND "Tests.Networking" )
endif()
