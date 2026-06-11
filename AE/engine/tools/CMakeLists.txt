# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

if (${AE_DESKTOP})
	add_subdirectory( "header_gen" )
	add_subdirectory( "vulkan_image_zcurve" )
	add_subdirectory( "feature_set_gen" )
	add_subdirectory( "lfas" )
	add_subdirectory( "res_updater" )
endif()

add_subdirectory( "atlas_tools" )
add_subdirectory( "geometry_tools" )
add_subdirectory( "vulkan_sync_log" )
add_subdirectory( "vulkan_ext_emulator" )
add_subdirectory( "res_loaders" )
add_subdirectory( "graphics_test" )
add_subdirectory( "cicd" )
add_subdirectory( "res_pack" )
add_subdirectory( "graphics_rhi_shared" )
