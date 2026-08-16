
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "glslang_spirv_deps" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_GLSLANG" )
if (NOT TARGET "glslang_spirv_deps")
	message( FATAL_ERROR "target 'glslang_spirv_deps' is not exists" )
endif()
