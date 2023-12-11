# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download Brotli (MIT license)

set( BROTLI_PATH "${AE_DST_PATH}/Brotli" )

add_library( "Brotli-lib" INTERFACE )
target_include_directories( "Brotli-lib" INTERFACE "${BROTLI_PATH}/include" )
target_compile_definitions( "Brotli-lib" INTERFACE "AE_ENABLE_BROTLI" "AE_LICENSE_MIT" )
install( FILES "${BROTLI_PATH}/LICENSE" DESTINATION "3party_license" RENAME "Brotli-LICENSE" )

if (DEFINED ANDROID)
    set_property( TARGET "Brotli-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${BROTLI_PATH}/lib/${ANDROID_ABI}/libbrotlidec-static.a"
        "${BROTLI_PATH}/lib/${ANDROID_ABI}/libbrotlienc-static.a"
        "${BROTLI_PATH}/lib/${ANDROID_ABI}/libbrotlicommon-static.a" )
else()
    set_property( TARGET "Brotli-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${BROTLI_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}brotlidec-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${BROTLI_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}brotlienc-static${CMAKE_STATIC_LIBRARY_SUFFIX}"
        "${BROTLI_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}brotlicommon-static${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
    if (DEFINED ANDROID)
        add_library( "Tests.Brotli" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.Brotli" PUBLIC "ANDROID" )
        target_include_directories( "Tests.Brotli" PRIVATE "${AE_TEST_DIR}" )
    else()
        add_executable( "Tests.Brotli" "tests/main.cpp" )
        #add_test( NAME "Tests.Brotli" COMMAND "Tests.Brotli" )
    endif()
    set_property( TARGET "Tests.Brotli" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.Brotli" PUBLIC "Brotli-lib" )
endif ()
