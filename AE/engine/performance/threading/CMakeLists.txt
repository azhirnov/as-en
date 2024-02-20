# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE SOURCES "*.*" )
if (DEFINED ANDROID)
    add_library( "Perf.Threading" STATIC ${SOURCES} )
elseif (APPLE)
    add_executable( "Perf.Threading" MACOSX_BUNDLE ${SOURCES} )
else()
    add_executable( "Perf.Threading" ${SOURCES} )
endif()
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Perf.Threading" PROPERTY FOLDER "Engine/PerfTests" )
target_link_libraries( "Perf.Threading" PUBLIC "Threading" )
target_compile_definitions( "Perf.Threading" PUBLIC AE_PERFTEST_THREADING )
target_compile_definitions( "Perf.Threading" PRIVATE AE_CURRENT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" )

#add_test( NAME "Perf.Threading" COMMAND "Perf.Threading" )
