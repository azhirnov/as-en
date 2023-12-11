# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download msdfgen (MIT license)

set( MSDFGEN_PATH "${AE_DST_PATH}/msdfgen" )

add_library( "msdfgen-lib" INTERFACE )
target_include_directories( "msdfgen-lib" INTERFACE "${MSDFGEN_PATH}/include" )
target_compile_definitions( "msdfgen-lib" INTERFACE "AE_ENABLE_MSDFGEN" "AE_LICENSE_MIT" )
install( FILES "${MSDFGEN_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "msdfgen-LICENSE.txt" )

set_property( TARGET "msdfgen-lib" PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<CONFIG:Debug>:   "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-cored${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Debug>:   "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-extd${CMAKE_STATIC_LIBRARY_SUFFIX}"  >

    $<$<CONFIG:Develop>: "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-core${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Develop>: "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-ext${CMAKE_STATIC_LIBRARY_SUFFIX}"  >

    $<$<CONFIG:Profile>: "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-core${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Profile>: "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-ext${CMAKE_STATIC_LIBRARY_SUFFIX}"  >

    $<$<CONFIG:Release>: "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-core${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Release>: "${MSDFGEN_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}msdfgen-ext${CMAKE_STATIC_LIBRARY_SUFFIX}"  >
)

if (${AE_ENABLE_EXTERNAL_TESTS})
    add_executable( "Tests.msdfgen" "tests/main.cpp" )
    #add_test( NAME "Tests.msdfgen" COMMAND "Tests.msdfgen" )
    set_property( TARGET "Tests.msdfgen" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.msdfgen" PUBLIC "msdfgen-lib" )
endif ()
