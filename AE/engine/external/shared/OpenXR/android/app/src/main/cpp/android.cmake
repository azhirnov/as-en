
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" "../../../../../temp/lib" )
target_link_libraries( "Static-lib" INTERFACE "libgit2" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_OPENXR" )
if (NOT TARGET "libgit2")
	message( FATAL_ERROR "target 'libgit2' is not exists" )
endif()
