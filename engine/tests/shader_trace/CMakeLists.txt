if ((TARGET "ShaderTrace") AND ${AE_ENABLE_VULKAN})

	set( GRAPHICS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src/graphics" )
	set( GRAPHICS_SRC
		"${GRAPHICS_DIR}/Public/DeviceProperties.h"
		#"${GRAPHICS_DIR}/Private/DeviceProperties.cpp"
		"${GRAPHICS_DIR}/Private/EnumUtils.h"
		#"${GRAPHICS_DIR}/Private/EnumUtils.cpp"
		"${GRAPHICS_DIR}/Public/RenderState.h"
		#"${GRAPHICS_DIR}/Private/RenderState.cpp"
		"${GRAPHICS_DIR}/Public/FeatureSet.h"
		#"${GRAPHICS_DIR}/Private/FeatureSet.cpp"
	)

	file( GLOB_RECURSE SOURCES "*.*" )
	set( VK_FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../src/graphics/Vulkan" )
	set( VK_SOURCES 
		"${VK_FILES_PATH}/VDevice.h"
		#"${VK_FILES_PATH}/VDevice.cpp"
		#"${VK_FILES_PATH}/VDeviceFS.cpp"
		"${VK_FILES_PATH}/VulkanLoader.h"
		#"${VK_FILES_PATH}/VulkanLoader.cpp"
		"${VK_FILES_PATH}/VulkanCheckError.h"
		#"${VK_FILES_PATH}/VulkanCheckError.cpp"
	)

	add_executable( "Tests.ShaderTrace" ${SOURCES} ${VK_SOURCES} ${GRAPHICS_SRC} )
	source_group( TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} )
	source_group( "vulkan" FILES ${VK_SOURCES} )
	source_group( "graphics" FILES ${GRAPHICS_SRC} )

	set_property( TARGET "Tests.ShaderTrace" PROPERTY FOLDER "Engine/Tests" )
	target_link_libraries( "Tests.ShaderTrace" PUBLIC "ResourceLoaders" "Graphics" "ShaderTrace" )

	target_include_directories( "Tests.ShaderTrace" PRIVATE "${VK_FILES_PATH}" )
	target_compile_definitions( "Tests.ShaderTrace" PUBLIC "DATA_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/ref/\"" )
	
	if (TARGET "SPIRV-Cross-lib")
		target_link_libraries( "Tests.ShaderTrace" PUBLIC "SPIRV-Cross-lib" )
	endif()

	add_test( NAME "Tests.ShaderTrace" COMMAND "Tests.ShaderTrace" )
endif()
