
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "utf8proc" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_UTF8PROC" )
if (NOT TARGET "utf8proc")
	message( FATAL_ERROR "target 'utf8proc' is not exists" )
endif()
