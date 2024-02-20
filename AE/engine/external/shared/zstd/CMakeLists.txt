# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download zstd (BSD license)

set( ZSTD_PATH "${AE_DST_PATH}/zstd" )

add_library( "zstd-lib" INTERFACE )
target_include_directories( "zstd-lib" INTERFACE "${ZSTD_PATH}/include" )
target_compile_definitions( "zstd-lib" INTERFACE "AE_ENABLE_ZSTD" "AE_LICENSE_BSD_3" )
install( FILES "${ZSTD_PATH}/LICENSE" DESTINATION "3party_license" RENAME "zstd-LICENSE" )

if (DEFINED ANDROID)
    set_property( TARGET "zstd-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${ZSTD_PATH}/lib/${ANDROID_ABI}/libzstd.a")
elseif(WIN32)
    set_property( TARGET "zstd-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${ZSTD_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zstd_static${CMAKE_STATIC_LIBRARY_SUFFIX}" )
else()
    set_property( TARGET "zstd-lib" PROPERTY INTERFACE_LINK_LIBRARIES
        "${ZSTD_PATH}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zstd${CMAKE_STATIC_LIBRARY_SUFFIX}" )
endif()

if (${AE_ENABLE_EXTERNAL_TESTS})
    add_executable( "Tests.zstd" "tests/main.cpp" )
    #add_test( NAME "Tests.zstd" COMMAND "Tests.zstd" )
    set_property( TARGET "Tests.zstd" PROPERTY FOLDER "Engine/External/Tests" )
    target_link_libraries( "Tests.zstd" PUBLIC "zstd-lib" )
endif ()
