
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "imgui" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_IMGUI" )
if (NOT TARGET "imgui")
	message( FATAL_ERROR "target 'imgui' is not exists" )
endif()
