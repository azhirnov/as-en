# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (TARGET "GLSLang-lib")
    file( GLOB_RECURSE SOURCES "*.*" )
    add_library( "ShaderTrace" STATIC ${SOURCES} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    set_property( TARGET "ShaderTrace" PROPERTY FOLDER "Engine/ResPack" )

    target_include_directories( "ShaderTrace" PUBLIC "Public" )
    target_compile_definitions( "ShaderTrace" PUBLIC "AE_ENABLE_GLSL_TRACE" )
    target_link_libraries( "ShaderTrace" PUBLIC "Serializing" "GLSLang-lib" )
endif()
