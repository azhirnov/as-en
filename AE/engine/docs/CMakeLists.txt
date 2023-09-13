# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.*" )
add_library( "Docs" STATIC EXCLUDE_FROM_ALL ${SOURCES} "../Changelog.md" "../../../Readme.md" )
set_target_properties( "Docs" PROPERTIES LINKER_LANGUAGE CXX )
set_property( TARGET "Docs" PROPERTY FOLDER "Engine" )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
