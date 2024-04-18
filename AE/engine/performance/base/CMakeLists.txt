# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB SOURCES "*.*" )
if (DEFINED ANDROID)
	add_library( "Perf.Base" STATIC ${SOURCES} )
elseif (APPLE)
	add_executable( "Perf.Base" MACOSX_BUNDLE ${SOURCES} )
else()
	add_executable( "Perf.Base" ${SOURCES} )
endif()
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Perf.Base" PROPERTY FOLDER "Engine/PerfTests" )
target_link_libraries( "Perf.Base" PUBLIC "Base" )
target_compile_definitions( "Perf.Base" PUBLIC AE_PERFTEST_BASE )

#add_test( NAME "Perf.Base" COMMAND "Perf.Base" )
