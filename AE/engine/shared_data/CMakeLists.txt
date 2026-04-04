# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SHARED_DATA "*.as" "*.glsl" "*.h" )

add_library( "SharedData" STATIC EXCLUDE_FROM_ALL ${SHARED_DATA} )
set_target_properties( "SharedData" PROPERTIES LINKER_LANGUAGE CXX )
target_compile_definitions( "SharedData" PUBLIC "SCRIPT" )

set_property( TARGET "SharedData" PROPERTY FOLDER "Engine" )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHARED_DATA} )
