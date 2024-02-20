
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" "../../../../../temp/lib" )
target_link_libraries( "Static-lib" INTERFACE "libzstd_static" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_ZSTD" )
if (NOT TARGET "libzstd_static")
    message( FATAL_ERROR "target 'libzstd_static' is not exists" )
endif()
