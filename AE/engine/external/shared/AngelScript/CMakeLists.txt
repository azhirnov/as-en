# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download AngelScript SDK (ZLib license)

set( ANGELSCRIPT_PATH "${AE_DST_PATH}/AngelScript" )

add_library( "AngelScript-lib" INTERFACE )
target_include_directories( "AngelScript-lib" INTERFACE "${ANGELSCRIPT_PATH}/include" )
target_compile_definitions( "AngelScript-lib" INTERFACE "AE_ENABLE_ANGELSCRIPT" "AS_USE_NAMESPACE=1" "AS_USE_STLNAMES=1" "AE_LICENSE_ZLIB" )
install( FILES "${ANGELSCRIPT_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "AngelScript-LICENSE" )

if (DEFINED ANDROID)
    set_property( TARGET "AngelScript-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${ANGELSCRIPT_PATH}/lib/${ANDROID_ABI}/libangelscript.a"
        "${ANGELSCRIPT_PATH}/lib/${ANDROID_ABI}/libangelscript_stdstring.a"
        "${ANGELSCRIPT_PATH}/lib/${ANDROID_ABI}/libangelscript_scriptarray.a" )
else()
    set_property( TARGET "AngelScript-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<CONFIG:Release>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Release>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_stdstring${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Release>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_scriptarray${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Profile>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Profile>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_stdstring${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Profile>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_scriptarray${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Develop>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Develop>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_stdstring${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Develop>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_scriptarray${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Debug>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscriptd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Debug>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_stdstringd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Debug>: "${ANGELSCRIPT_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}angelscript_scriptarrayd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
    if (DEFINED ANDROID)
        add_library( "Tests.AngelScript" STATIC "tests/main.cpp" )
        target_compile_definitions( "Tests.AngelScript" PUBLIC "ANDROID" )
        target_include_directories( "Tests.AngelScript" PRIVATE "${AE_TEST_DIR}" )
    else()
        add_executable( "Tests.AngelScript" "tests/main.cpp" )
        add_test( NAME "Tests.AngelScript" COMMAND "Tests.AngelScript" )
    endif()
    set_property( TARGET "Tests.AngelScript" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.AngelScript" PUBLIC "AngelScript-lib" )
endif ()
