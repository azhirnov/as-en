# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Scripting")
    file( GLOB_RECURSE SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.Scripting" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.Scripting" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.Scripting" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.Scripting" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.Scripting" PUBLIC "Scripting" )
    target_compile_definitions( "Tests.Scripting" PUBLIC AE_TEST_SCRIPTING )

    add_test( NAME "Tests.Scripting" COMMAND "Tests.Scripting" )
endif()
