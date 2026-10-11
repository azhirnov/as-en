
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "TracyClient" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_TRACY" )
if (NOT TARGET "TracyClient")
	message( FATAL_ERROR "target 'TracyClient' is not exists" )
endif()
