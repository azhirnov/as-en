
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" "../../../../../temp/lib" )
target_link_libraries( "Static-lib" INTERFACE "lz4_static" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_LZ4" )
if (NOT TARGET "lz4_static")
	message( FATAL_ERROR "target 'lz4_static' is not exists" )
endif()
