# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE DOCS "*.*" )
set( DOCS2 "../engine/Changelog.md" )
add_library( "Docs" STATIC EXCLUDE_FROM_ALL ${DOCS} ${DOCS2} )
set_target_properties( "Docs" PROPERTIES LINKER_LANGUAGE CXX )
set_property( TARGET "Docs" PROPERTY FOLDER "Docs" )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DOCS} )
source_group( "engine" FILES "../engine/Changelog.md" )
