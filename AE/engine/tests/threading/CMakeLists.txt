# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Threading")
    file( GLOB_RECURSE SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.Threading" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.Threading" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.Threading" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.Threading" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.Threading" PUBLIC "Threading" )
    target_compile_definitions( "Tests.Threading" PUBLIC AE_TEST_THREADING )
    target_compile_definitions( "Tests.Threading" PRIVATE AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

    add_test( NAME "Tests.Threading" COMMAND "Tests.Threading" )
endif()
