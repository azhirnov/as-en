# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if ( (TARGET "BASS-lib") OR (${CMAKE_SYSTEM_NAME} STREQUAL "Android") )
    set( AE_ENABLE_AUDIO TRUE )
endif()

if (${AE_ENABLE_AUDIO})
    file( GLOB_RECURSE HPP_SOURCES "*.h" )
    if (NOT (DEFINED ENGINE_LIBS_PATH))
        file( GLOB_RECURSE CPP_SOURCES "*.cpp" )
    endif()
    set( SOURCES ${HPP_SOURCES} ${CPP_SOURCES} )

    add_library( "Audio" STATIC ${SOURCES} ${PREBUILD_CPP_FILE} )
    source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )

    set_property( TARGET "Audio" PROPERTY FOLDER "Engine" )
    target_link_libraries( "Audio" PUBLIC "Threading" "Serializing" )
    target_compile_definitions( "Audio" PUBLIC AE_ENABLE_AUDIO )

    if (NOT ANDROID)
        set( ANDROID_SDK_ROOT "$ENV{ANDROID_SDK_ROOT}" )
        set( ANDROID_NDK_INCLUDE "${ANDROID_SDK_ROOT}/ndk/24.0.8215888/toolchains/llvm/prebuilt/windows-x86_64/sysroot/usr/include/SLES" )
        target_include_directories( "Audio" PRIVATE "${ANDROID_NDK_INCLUDE}" )
    endif()

    if (TARGET "BASS-lib")
        target_link_libraries( "Audio" PUBLIC "BASS-lib" )

    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        target_compile_definitions( "Audio" PUBLIC AE_ENABLE_OPENSLES )

    endif()

    EnablePCH( "Audio" )
    EnablePrebuild( "Audio" )

    install( TARGETS "Audio" ARCHIVE DESTINATION "lib" )
endif()
