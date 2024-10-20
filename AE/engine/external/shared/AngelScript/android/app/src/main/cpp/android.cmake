
add_subdirectory( "../../../../../temp" "temp" )
add_library( "Static-lib" INTERFACE )
target_include_directories( "Static-lib" INTERFACE "../../../../../temp" "../../../../../temp/angelscript/include" "../../../../../temp/add_on/scriptarray" "../../../../../temp/add_on/scriptstdstring" )
target_link_libraries( "Static-lib" INTERFACE "angelscript" "angelscript_stdstring" "angelscript_scriptarray" )
target_compile_definitions( "Static-lib" INTERFACE "AE_ENABLE_ANGELSCRIPT" )
if (NOT TARGET "angelscript")
	message( FATAL_ERROR "target 'angelscript' is not exists" )
endif()
if (NOT TARGET "angelscript_stdstring")
	message( FATAL_ERROR "target 'angelscript_stdstring' is not exists" )
endif()
if (NOT TARGET "angelscript_scriptarray")
	message( FATAL_ERROR "target 'angelscript_scriptarray' is not exists" )
endif()
