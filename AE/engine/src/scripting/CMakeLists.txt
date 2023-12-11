# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (DEFINED ANDROID)
    set( AE_ENABLE_SCRIPTING OFF CACHE INTERNAL "" FORCE )
elseif (${AE_ENABLE_ANGELSCRIPT})
    set( AE_ENABLE_SCRIPTING ON CACHE INTERNAL "" FORCE )
else()
    set( AE_ENABLE_SCRIPTING OFF CACHE INTERNAL "" FORCE )
endif()

if (${AE_ENABLE_SCRIPTING} AND ${AE_ENABLE_EXCEPTIONS})
    file( GLOB_RECURSE HPP_SOURCES "*.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
    endif()
    set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

    add_library( "Scripting" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "Scripting" PROPERTY FOLDER "Engine" )
    target_link_libraries( "Scripting" PUBLIC "Base" )
    target_compile_definitions( "Scripting" PUBLIC "AE_ENABLE_SCRIPTING" )

    if (${AE_ENABLE_ANGELSCRIPT})
        target_link_libraries( "Scripting" PUBLIC "AngelScript-lib" )
    endif()

    EnablePCH( "Scripting" )
    EnablePrebuild( "Scripting" )

    install( TARGETS "Scripting" ARCHIVE DESTINATION "lib" )
endif()
