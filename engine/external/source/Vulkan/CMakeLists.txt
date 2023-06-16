# Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#
# download Vulkan headers (Apache 2.0 license)

set( VULKAN_PATH "${AE_EXTERNAL_SHARED_PATH}/Vulkan" )

add_library( "Vulkan-lib" INTERFACE )
target_include_directories( "Vulkan-lib" INTERFACE "${VULKAN_PATH}" )
target_compile_definitions( "Vulkan-lib" INTERFACE "AE_ENABLE_VULKAN" "AE_LICENSE_APACHE_2" )
install( FILES "${VULKAN_PATH}/LICENSE.txt" DESTINATION "3party_license" RENAME "VulkanHeader-LICENSE" )
