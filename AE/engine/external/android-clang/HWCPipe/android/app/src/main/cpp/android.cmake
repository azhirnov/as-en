
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "hwcpipe" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_HWCPIPE" )
if (NOT TARGET "hwcpipe")
	message( FATAL_ERROR "target 'hwcpipe' is not exists" )
endif()
