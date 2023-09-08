# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# use FFmpeg (GPLv3 or LGPLv3 license)

set( FFMPEG_PATH "${AE_DST_SHARED_PATH}/ffmpeg" )

add_custom_target( "FFmpeg-copy-dll"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${FFMPEG_PATH}/bin" "${MAIN_BINARY_DIR}/$<CONFIG>"
    COMMENT "Copying FFmpeg DLLs..." )
set_property( TARGET "FFmpeg-copy-dll" PROPERTY FOLDER "Engine/External" )

add_library( "FFmpeg-lib" INTERFACE )
target_include_directories( "FFmpeg-lib" INTERFACE "${FFMPEG_PATH}/include" )
target_compile_definitions( "FFmpeg-lib" INTERFACE "AE_ENABLE_FFMPEG" )
add_dependencies( "FFmpeg-lib" "FFmpeg-copy-dll" )

#target_compile_definitions( "FFmpeg-lib" INTERFACE "AE_LICENSE_LGPLv3_SHAREDLIB" )
target_compile_definitions( "FFmpeg-lib" INTERFACE "AE_LICENSE_GPLv3" )

#install( FILES "${FFMPEG_PATH}/LICENSE" DESTINATION "3party_license" RENAME "FFmpeg-LICENSE" )
