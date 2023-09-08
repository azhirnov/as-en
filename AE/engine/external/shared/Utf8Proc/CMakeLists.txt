# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download utf8proc (MIT license)

set( U8P_PATH "${AE_DST_PATH}/Utf8Proc" )

add_library( "Utf8Proc-lib" INTERFACE )
target_include_directories( "Utf8Proc-lib" INTERFACE "${U8P_PATH}/include" )
target_compile_definitions( "Utf8Proc-lib" INTERFACE "AE_ENABLE_UTF8PROC" "UTF8PROC_STATIC" "AE_LICENSE_MIT" )
install( FILES "${U8P_PATH}/LICENSE.md" DESTINATION "3party_license" RENAME "Utf8Proc-LICENSE.md" )

if (DEFINED ANDROID)
    set_property( TARGET "Utf8Proc-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${U8P_PATH}/lib/${ANDROID_ABI}/libutf8proc.a" )
elseif(DEFINED WIN32)
    set_property( TARGET "Utf8Proc-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${U8P_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}utf8proc_static${CMAKE_STATIC_LIBRARY_SUFFIX}" )
else()
    set_property( TARGET "Utf8Proc-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${U8P_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}utf8proc${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
    if (DEFINED ANDROID)
        add_library( "Tests.Utf8Proc" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.Utf8Proc" PUBLIC "ANDROID" )
        target_include_directories( "Tests.Utf8Proc" PRIVATE "${AE_TEST_DIR}" )
    else()
        add_executable( "Tests.Utf8Proc" "tests/main.cpp" )
        add_test( NAME "Tests.Utf8Proc" COMMAND "Tests.Utf8Proc" )
    endif()
    set_property( TARGET "Tests.Utf8Proc" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.Utf8Proc" PUBLIC "Utf8Proc-lib" )
endif ()
