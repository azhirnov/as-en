# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.*" )
add_executable( "LockFreeAlgSandbox" ${SOURCES} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "LockFreeAlgSandbox" PROPERTY FOLDER "Engine/ToolApps" )
target_link_libraries( "LockFreeAlgSandbox" PUBLIC "Base" )
target_include_directories( "LockFreeAlgSandbox" PRIVATE "." "../../tests" )
target_compile_definitions( "LockFreeAlgSandbox" PUBLIC "AE_LFAS_ENABLED" )

add_test( NAME "LockFreeAlgSandbox" COMMAND "LockFreeAlgSandbox" )
