# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SHARED_DATA "*.as" "*.glsl" )

add_library( "SharedData" STATIC EXCLUDE_FROM_ALL ${SHARED_DATA} )
set_target_properties( "SharedData" PROPERTIES LINKER_LANGUAGE CXX )
set_property( TARGET "SharedData" PROPERTY FOLDER "Engine" )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHARED_DATA} )
