# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

file( GLOB_RECURSE HPP_SOURCES "*.h" )
if (NOT (DEFINED ENGINE_LIBS_PATH))
    file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
endif()
if (MSVC OR APPLE)
    file( GLOB_RECURSE OBJC_SOURCES "*.mm" )
endif()
set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} ${OBJC_SOURCES} )

add_library( "Base" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
set_property( TARGET "Base" PROPERTY FOLDER "Engine" )
target_include_directories( "Base" PUBLIC ".." )
target_link_libraries( "Base" PUBLIC "ProjectTemplate" )

if (TARGET "GLM-lib")
    target_link_libraries( "Base" PUBLIC "GLM-lib" )
else()
    message( FATAL_ERROR "GLM target is not exists" )
endif()

if (TARGET "Brotli-lib")
    target_link_libraries( "Base" PUBLIC "Brotli-lib" )
endif()
if (TARGET "zstd-lib")
    target_link_libraries( "Base" PUBLIC "zstd-lib" )
endif()
if (TARGET "lz4-lib")
    target_link_libraries( "Base" PUBLIC "lz4-lib" )
endif()

if (TARGET "Utf8Proc-lib")
    target_link_libraries( "Base" PUBLIC "Utf8Proc-lib" )
endif()

if (TARGET "Abseil-lib")
    target_link_libraries( "Base" PUBLIC "Abseil-lib" )
endif()

if (TARGET "xxHash-lib")
    target_link_libraries( "Base" PUBLIC "xxHash-lib" )
endif()

target_link_libraries( "Base" PUBLIC "FametaCounter-lib" )

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    # for sockets
    target_link_libraries( "Base" PUBLIC "ws2_32" )
endif()

if (MSVC)
    set_source_files_properties( ${OBJC_SOURCES} PROPERTIES HEADER_FILE_ONLY TRUE )
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries( "Base" PUBLIC "dl" "pthread" )
    target_compile_definitions( "Base" PUBLIC _LARGEFILE_SOURCE _LARGE_FILES _FILE_OFFSET_BITS=64 _LARGEFILE64_SOURCE )
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    target_link_libraries( "Base" PUBLIC "log" )
endif()

if (APPLE)
    find_library( COCOA_LIBRARY Cocoa REQUIRED )
    target_link_libraries( "Base" PRIVATE ${COCOA_LIBRARY} )
endif()

target_compile_definitions( "Base" PUBLIC FP_FAST_FMA FP_FAST_FMAF FP_FAST_FMAL )

EnablePCH( "Base" )
EnablePrebuild( "Base" )

install( TARGETS "Base" ARCHIVE DESTINATION "lib" )
