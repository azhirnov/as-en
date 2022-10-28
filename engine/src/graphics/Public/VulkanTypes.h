// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/IDs.h"
# include "graphics/Public/Queue.h"
# include "graphics/Vulkan/VulkanLoader.h"

namespace AE::Graphics
{

	//
	// Vulkan Image Description
	//
	struct VulkanImageDesc
	{
		VkImage						image			= Default;
		VkImageType					imageType		= VK_IMAGE_TYPE_MAX_ENUM;
		VkImageCreateFlagBits		flags			= VkImageCreateFlagBits(0);
		VkImageUsageFlagBits		usage			= VkImageUsageFlagBits(0);
		VkFormat					format			= VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits		samples			= VK_SAMPLE_COUNT_1_BIT;
		VkImageTiling				tiling			= VK_IMAGE_TILING_MAX_ENUM;
		uint3						dimension;
		uint						arrayLayers		= 0;
		uint						maxLevels		= 0;
		EQueueMask					queues			= Default;
		VkMemoryPropertyFlagBits	memFlags		= Zero;
		bool						canBeDestroyed	= true;
	};
	


	//
	// Vulkan Buffer Description
	//
	struct VulkanBufferDesc
	{
		VkBuffer					buffer			= Default;
		VkBufferUsageFlagBits		usage			= VkBufferUsageFlagBits(0);
		Bytes						size;
		EQueueMask					queues			= Default;
		VkMemoryPropertyFlagBits	memFlags		= Zero;
		bool						canBeDestroyed	= true;
	};



	//
	// Vulkan Memory Object Info
	//
	struct VulkanMemoryObjInfo
	{
		VkDeviceMemory				memory		= Default;
		VkMemoryPropertyFlagBits	flags		= Zero;
		Bytes						offset;
		Bytes						size;
		void *						mappedPtr	= null;		// include 'offset'
	};



	//
	// Vulkan Command Batch Dependency
	//
	struct VulkanCmdBatchDependency
	{
		VkSemaphore					semaphore	= Default;
		ulong						value		= 0;		// for timeline semaphore
		VkPipelineStageFlags2KHR	stages		= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;
	};


	
	enum class VDeviceAddress : VkDeviceAddress { Unknown = 0 };
	
	ND_ inline VkDeviceAddress  operator + (VDeviceAddress addr, Bytes offset) {
		return VkDeviceAddress(addr) + VkDeviceAddress(offset);
	}


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
