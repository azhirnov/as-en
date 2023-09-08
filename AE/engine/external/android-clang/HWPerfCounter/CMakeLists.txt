# download google/hardware-perfcounter (Apache 2.0 license)

cmake_minimum_required( VERSION 3.10 FATAL_ERROR )

set( HWPERCOUNTER_PATH "${AE_DST_SHARED_PATH}/HWPerfCounter" )

add_library( "HWPerfCounter-lib" INTERFACE )
target_include_directories( "HWPerfCounter-lib" INTERFACE "${HWPERCOUNTER_PATH}/include" )
target_compile_definitions( "HWPerfCounter-lib" INTERFACE "AE_ENABLE_HWPERCOUNTER" "AE_LICENSE_APACHE_2" )


if (${AE_ENABLE_EXTERNAL_TESTS})
    add_library( "Tests.HWPerfCounter" SHARED "tests/main.c" )
    set_property( TARGET "Tests.HWPerfCounter" PROPERTY FOLDER "Engine/External/Tests" )
    target_include_directories( "Tests.HWPerfCounter" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../AndroidTest" )
    target_link_libraries( "Tests.HWPerfCounter" PUBLIC "HWPerfCounter-lib" )
endif ()
