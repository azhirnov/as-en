# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "Serializing")
    file( GLOB_RECURSE SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.Serializing" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.Serializing" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.Serializing" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.Serializing" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.Serializing" PUBLIC "Serializing" )
    target_compile_definitions( "Tests.Serializing" PUBLIC AE_TEST_SERIALIZING )

    add_test( NAME "Tests.Serializing" COMMAND "Tests.Serializing" )
endif()
