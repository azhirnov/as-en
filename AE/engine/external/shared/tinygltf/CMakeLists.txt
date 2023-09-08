# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download tinygltf (MIT license)

set( TINYGLTF_PATH "${AE_EXTERNAL_SHARED_PATH}/tinygltf" )

add_library( "TinyglTF-lib" INTERFACE )
target_include_directories( "TinyglTF-lib" INTERFACE "${TINYGLTF_PATH}" )
target_compile_definitions( "TinyglTF-lib" INTERFACE "AE_ENABLE_TINYGLTF" "AE_LICENSE_MIT" ) # TINYGLTF_USE_CPP14 TINYGLTF_NO_FS TINYGLTF_NO_STB_IMAGE TINYGLTF_NO_STB_IMAGE_WRITE JSON_NOEXCEPTION TINYGLTF_NOEXCEPTION
install( FILES "${TINYGLTF_PATH}/LICENSE" DESTINATION "3party_license" RENAME "TinyglTF-LICENSE" )

if (${AE_ENABLE_EXTERNAL_TESTS})
    if (ANDROID)
        add_library( "Tests.TinyglTF" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.TinyglTF" PUBLIC "ANDROID" )
        target_include_directories( "Tests.TinyglTF" PRIVATE "${AE_TEST_DIR}" )
    else ()
        add_executable( "Tests.TinyglTF" "tests/main.cpp" )
        add_test( NAME "Tests.TinyglTF" COMMAND "Tests.TinyglTF" )
    endif ()
    set_target_properties( "Tests.TinyglTF" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
    target_compile_features( "Tests.TinyglTF" PUBLIC cxx_std_17 )
    set_property( TARGET "Tests.TinyglTF" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.TinyglTF" PUBLIC "TinyglTF-lib" )
endif ()
