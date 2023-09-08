# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GraphicsHL")

    file( GLOB_RECURSE HPP_SOURCES "*.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
    endif()
    set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

    add_library( "Profiler" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

    set_property( TARGET "Profiler" PROPERTY FOLDER "Engine" )
    target_link_libraries( "Profiler" PUBLIC "GraphicsHL" )

    if (TARGET "ImGUI-lib")
        target_link_libraries( "Profiler" PUBLIC "ImGUI-lib" )
    endif()

    if (DEFINED ANDROID)
        target_link_libraries( "Profiler" PUBLIC "ARM-HWCPipe-lib" "HWPerfCounter-lib" )
    endif()

    EnablePCH( "Profiler" )
    EnablePrebuild( "Profiler" )

    install( TARGETS "Profiler" ARCHIVE DESTINATION "lib" )
endif()
