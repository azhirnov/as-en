# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download glm (MIT license)

set( GLM_PATH "${AE_EXTERNAL_SHARED_PATH}/GLM" )

add_library( "GLM-lib" INTERFACE )
target_include_directories( "GLM-lib" INTERFACE "${GLM_PATH}" )
target_compile_definitions( "GLM-lib" INTERFACE "AE_ENABLE_GLM" "AE_LICENSE_MIT" )
install( FILES "${GLM_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "GLM-LICENSE.txt" )

if (${AE_ENABLE_EXTERNAL_TESTS})
    if (ANDROID)
        add_library( "Tests.GLM" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.GLM" PUBLIC "ANDROID" )
        target_include_directories( "Tests.GLM" PRIVATE "${AE_TEST_DIR}" )
    else ()
        add_executable( "Tests.GLM" "tests/main.cpp" )
        add_test( NAME "Tests.GLM" COMMAND "Tests.GLM" )
    endif ()
    set_target_properties( "Tests.GLM" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
    target_compile_features( "Tests.GLM" PUBLIC cxx_std_17 )
    set_property( TARGET "Tests.GLM" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.GLM" PUBLIC "GLM-lib" )
endif ()
