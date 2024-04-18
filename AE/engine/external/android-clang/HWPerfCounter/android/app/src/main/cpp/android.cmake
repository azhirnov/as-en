
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "hwpc-main" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_HWPERFCOUNTER" )
if (NOT TARGET "hwpc-main")
	message( FATAL_ERROR "target 'hwpc-main' is not exists" )
endif()
