# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# Assimp (BSD-like)

set( ASSIMP_PATH "${AE_DST_PATH}/Assimp" )

add_library( "Assimp-lib" INTERFACE )
target_include_directories( "Assimp-lib" INTERFACE "${ASSIMP_PATH}/include" )
target_compile_definitions( "Assimp-lib" INTERFACE "AE_ENABLE_ASSIMP" "AE_LICENSE_BSD" )
install( FILES "${ASSIMP_PATH}/LICENSE" DESTINATION "3party_license" RENAME "Assimp-LICENSE" )

if (MSVC)
    set( ASSIMP_LIB_NAME "assimp-vc143-mt" )
    set_property( TARGET "Assimp-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<CONFIG:Release>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${ASSIMP_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Release>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Profile>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${ASSIMP_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Profile>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Develop>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${ASSIMP_LIB_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Develop>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Debug>:   "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${ASSIMP_LIB_NAME}d${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Debug>:   "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstaticd${CMAKE_STATIC_LIBRARY_SUFFIX}" > )
else()
    set_property( TARGET "Assimp-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        $<$<CONFIG:Release>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Release>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Profile>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Profile>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Develop>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimp${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Develop>: "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstatic${CMAKE_STATIC_LIBRARY_SUFFIX}" >

        $<$<CONFIG:Debug>:   "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}assimpd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
        $<$<CONFIG:Debug>:   "${ASSIMP_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zlibstaticd${CMAKE_STATIC_LIBRARY_SUFFIX}" > )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
    add_executable( "Tests.Assimp" "tests/main.cpp" )
    add_test( NAME "Tests.Assimp" COMMAND "Tests.Assimp" )
    set_target_properties( "Tests.Assimp" PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES )
    target_compile_features( "Tests.Assimp" PUBLIC cxx_std_17 )
    set_property( TARGET "Tests.Assimp" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.Assimp" PUBLIC "Assimp-lib" )
endif ()
