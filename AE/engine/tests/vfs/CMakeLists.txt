# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "VFS")
    file( GLOB_RECURSE SOURCES "*.*" )
    if (DEFINED ANDROID)
        add_library( "Tests.VFS" STATIC ${SOURCES} )
    elseif (APPLE)
        add_executable( "Tests.VFS" MACOSX_BUNDLE ${SOURCES} )
    else()
        add_executable( "Tests.VFS" ${SOURCES} )
    endif()
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Tests.VFS" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.VFS" PUBLIC "VFS" )
    target_compile_definitions( "Tests.VFS" PUBLIC AE_TEST_VFS )
    target_compile_definitions( "Tests.VFS" PRIVATE AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

    add_test( NAME "Tests.VFS" COMMAND "Tests.VFS" )
endif()
