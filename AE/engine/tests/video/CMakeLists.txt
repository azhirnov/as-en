# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.h" "*.cpp" )
add_executable( "Tests.Video" ${SOURCES} )

source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Tests.Video" PROPERTY FOLDER "Engine/Tests" )
target_link_libraries( "Tests.Video" PUBLIC "Video" )
target_compile_definitions( "Tests.Video" PRIVATE AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

add_test( NAME "Tests.Video" COMMAND "Tests.Video" )
