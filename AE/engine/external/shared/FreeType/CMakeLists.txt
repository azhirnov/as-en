# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download FreeType2 (BSD-like license)

set( FREETYPE_PATH "${AE_DST_PATH}/FreeType" )

add_library( "FreeType-lib" INTERFACE )
target_include_directories( "FreeType-lib" INTERFACE "${FREETYPE_PATH}/include/freetype2" )
target_compile_definitions( "FreeType-lib" INTERFACE "AE_ENABLE_FREETYPE" "AE_LICENSE_BSD" )
install( FILES "${FREETYPE_PATH}/LICENSE.TXT" DESTINATION "3party_license" RENAME "FreeType-LICENSE" )

if (DEFINED ANDROID)
    set_property( TARGET "FreeType-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${FREETYPE_PATH}/lib/${ANDROID_ABI}/libFreeType.a" )

elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set_property( TARGET "FreeType-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${FREETYPE_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}freetype${CMAKE_STATIC_LIBRARY_SUFFIX}" "png" "z" )

else()
    set_property( TARGET "FreeType-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${FREETYPE_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}freetype${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
    if (DEFINED ANDROID)
        add_library( "Tests.FreeType" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.FreeType" PUBLIC "ANDROID" )
        target_include_directories( "Tests.FreeType" PRIVATE "${AE_TEST_DIR}" )
    else()
        add_executable( "Tests.FreeType" "tests/main.cpp" )
        #add_test( NAME "Tests.FreeType" COMMAND "Tests.FreeType" )
    endif()
    set_property( TARGET "Tests.FreeType" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.FreeType" PUBLIC "FreeType-lib" )
endif ()
