# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB SOURCES "*.*" )

if (APPLE)
    add_executable( "Tests.NetworkStressTest" MACOSX_BUNDLE ${SOURCES} )
else()
    add_executable( "Tests.NetworkStressTest" ${SOURCES} )
endif()
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Tests.NetworkStressTest" PROPERTY FOLDER "Engine/Tests" )
target_link_libraries( "Tests.NetworkStressTest" PUBLIC "Networking" )
