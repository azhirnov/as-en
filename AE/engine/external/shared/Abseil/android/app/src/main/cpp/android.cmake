
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" )
target_link_libraries( "Static-lib" INTERFACE "Abseil" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_ABSEIL" )
if (NOT TARGET "Abseil")
    message( FATAL_ERROR "target 'Abseil' is not exists" )
endif()
