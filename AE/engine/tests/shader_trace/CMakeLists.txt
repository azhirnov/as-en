# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ((TARGET "ShaderTrace") AND ${AE_ENABLE_VULKAN})

    file( GLOB_RECURSE SOURCES "*.*" )
    add_executable( "Tests.ShaderTrace" ${SOURCES} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

    set_property( TARGET "Tests.ShaderTrace" PROPERTY FOLDER "Engine/Tests" )
    target_link_libraries( "Tests.ShaderTrace" PUBLIC "ResourceLoaders" "Graphics" "ShaderTrace" )

    target_include_directories( "Tests.ShaderTrace" PRIVATE "${VK_FILES_PATH}" )
    target_compile_definitions( "Tests.ShaderTrace" PUBLIC "DATA_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/ref/\"" )

    if (TARGET "SPIRV-Cross-lib")
        target_link_libraries( "Tests.ShaderTrace" PUBLIC "SPIRV-Cross-lib" )
    endif()

    add_test( NAME "Tests.ShaderTrace" COMMAND "Tests.ShaderTrace" )
endif()
