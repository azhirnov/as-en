# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_ENABLE_VULKAN})
    file( GLOB_RECURSE HPP_SOURCES "*.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
    endif()
    set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

    add_library( "VulkanExtEmulation" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
    target_link_libraries( "VulkanExtEmulation" PUBLIC "Base" "Vulkan-lib" )
    target_include_directories( "VulkanExtEmulation" PUBLIC "." )
    set_property( TARGET "VulkanExtEmulation" PROPERTY FOLDER "Engine/Tools" )

    EnablePrebuild( "VulkanExtEmulation" )
endif()
