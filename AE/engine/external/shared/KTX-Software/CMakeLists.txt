# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download KTX-Software (Apache 2.0)

set( KTX_PATH "${AE_DST_PATH}/KTX-Software" )

add_library( "KTX-lib" INTERFACE )
target_include_directories( "KTX-lib" INTERFACE "${KTX_PATH}/include" )
target_compile_definitions( "KTX-lib" INTERFACE "AE_ENABLE_KTX" "AE_LICENSE_APACHE_2" )
install( FILES "${KTX_PATH}/LICENSE" DESTINATION "3party_license" RENAME "KTX-LICENSE" )

set_property( TARGET "KTX-lib" PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<CONFIG:Release>: "${KTX_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ktx${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Profile>: "${KTX_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ktx${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Develop>: "${KTX_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ktx${CMAKE_STATIC_LIBRARY_SUFFIX}" >
    $<$<CONFIG:Debug>:   "${KTX_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}ktxd${CMAKE_STATIC_LIBRARY_SUFFIX}" >
)

if (${AE_ENABLE_EXTERNAL_TESTS})
    add_executable( "Tests.KTX" "tests/main.cpp" )
    add_test( NAME "Tests.KTX" COMMAND "Tests.KTX" )
    set_property( TARGET "Tests.KTX" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.KTX" PUBLIC "KTX-lib" )
endif ()
