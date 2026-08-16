// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Allocators/VGfxMemAllocatorUtils.h"
# include "graphics_rhi/Vulkan/VRenderTaskScheduler.h"

namespace AE::Graphics
{

/*
=================================================
	GetRTASStorageMemRequirements
=================================================
*/
	bool  VGfxMemAllocatorUtils::GetRTASStorageMemRequirements (INOUT RTASMemReqAtomic &memReqAtomic) __NE___
	{
		auto&	dev = GraphicsScheduler().GetDevice();
		auto&	ext = dev.GetVExtensions();

		VkBufferCreateInfo	buf_ci = {};
		buf_ci.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_ci.flags		= 0;
		buf_ci.usage		= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		buf_ci.size			= 10 << 20;
		buf_ci.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		if ( ext.accelerationStructure )
			buf_ci.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;

		if ( ext.opacityMicromap )
			buf_ci.usage |= VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT;

		VkBuffer	buffer	= Default;
		auto		err		= dev.vkCreateBuffer( dev.GetVkDevice(), &buf_ci, null, OUT &buffer );

		if ( err != VK_SUCCESS )
			return false;

		VkMemoryRequirements	mem_req = {};
		dev.vkGetBufferMemoryRequirements( dev.GetVkDevice(), buffer, OUT &mem_req );

		mem_req.alignment = AlignUp( mem_req.alignment, 256 );  // from specs

		memReqAtomic.store( RTASMemRequirements{ mem_req.memoryTypeBits, Bytes{mem_req.alignment} });

		dev.vkDestroyBuffer( dev.GetVkDevice(), buffer, null );
		return true;
	}

/*
=================================================
	CreateStorageBuffer
=================================================
*/
	bool  VGfxMemAllocatorUtils::CreateStorageBuffer (VDevice const& dev, const Bytes size, VkDeviceMemory memory, OUT VkBuffer &buffer) __NE___
	{
		auto&	ext = dev.GetVExtensions();

		VkBufferCreateInfo	buf_ci = {};
		buf_ci.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_ci.flags		= 0;
		buf_ci.usage		= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
		buf_ci.size			= VkDeviceSize( size );
		buf_ci.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		if ( ext.accelerationStructure )
			buf_ci.usage |= VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;

		if ( ext.opacityMicromap )
			buf_ci.usage |= VK_BUFFER_USAGE_MICROMAP_STORAGE_BIT_EXT;

		VK_CHECK_ERR( dev.vkCreateBuffer( dev.GetVkDevice(), &buf_ci, null, OUT &buffer ));

		VK_CHECK_ERR( dev.vkBindBufferMemory( dev.GetVkDevice(), buffer, memory, 0 ));
		return true;
	}

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
