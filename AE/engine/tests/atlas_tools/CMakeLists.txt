# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "AtlasTools")
    file( GLOB SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.AtlasTools" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.AtlasTools" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.AtlasTools" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.AtlasTools" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.AtlasTools" PUBLIC "AtlasTools" )
    target_compile_definitions( "Tests.AtlasTools" PUBLIC AE_TEST_ATLAS_TOOLS )

    add_test( NAME "Tests.AtlasTools" COMMAND "Tests.AtlasTools" )
endif()
