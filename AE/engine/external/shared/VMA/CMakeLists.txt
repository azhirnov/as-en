# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download Vulkan Memory Allocator headers (MIT license)

set( VMA_PATH "${AE_EXTERNAL_SHARED_PATH}/VMA" )

add_library( "VMA-lib" INTERFACE )
target_include_directories( "VMA-lib" INTERFACE "${VMA_PATH}" )
target_compile_definitions( "VMA-lib" INTERFACE "AE_ENABLE_VMA" "AE_LICENSE_MIT" )

install( FILES "${VMA_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "VMA-LICENSE.txt" )

if (${AE_ENABLE_EXTERNAL_TESTS} AND ${AE_ENABLE_VULKAN})
    if (ANDROID)
        add_library( "Tests.VMA" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.VMA" PUBLIC "ANDROID" )
        target_include_directories( "Tests.VMA" PRIVATE "${AE_TEST_DIR}" )
    else ()
        add_executable( "Tests.VMA" "tests/main.cpp" )
        add_test( NAME "Tests.VMA" COMMAND "Tests.VMA" )
    endif ()
    set_property( TARGET "Tests.VMA" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.VMA" PUBLIC "VMA-lib" )
    target_link_libraries( "Tests.VMA" PUBLIC "Vulkan-lib" )
endif ()
