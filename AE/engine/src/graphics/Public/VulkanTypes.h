// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/IDs.h"
# include "graphics/Public/Queue.h"
# include "graphics/Public/ResourceEnums.h"
# include "graphics/Vulkan/VulkanLoader.h"

namespace AE::Graphics
{

	//
	// Vulkan Image description
	//
	struct VulkanImageDesc
	{
		VkImage						image			= Default;
		VkImageType					imageType		= VK_IMAGE_TYPE_MAX_ENUM;
		VkImageCreateFlagBits		flags			= Zero;
		EImageOpt					options			= Default;					// some options are not defined in 'flags'
		VkImageUsageFlagBits		usage			= Zero;
		VkFormat					format			= VK_FORMAT_UNDEFINED;
		VkSampleCountFlagBits		samples			= VK_SAMPLE_COUNT_1_BIT;
		VkImageTiling				tiling			= VK_IMAGE_TILING_MAX_ENUM;
		uint3						dimension;
		uint						arrayLayers		= 0;
		uint						maxLevels		= 0;
		EQueueMask					queues			= Default;
		VkMemoryPropertyFlagBits	memFlags		= Zero;
		VkImageAspectFlagBits		aspectMask		= Zero;
		bool						canBeDestroyed	= false;
		bool						allocMemory		= false;
	};


	//
	// Vulkan Image View description
	//
	struct VulkanImageViewDesc
	{
		VkImageView					view			 = Default;
		VkImageViewCreateFlagBits	flags			 = Zero;
		VkImageViewType				viewType		 = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
		VkFormat					format			 = VK_FORMAT_UNDEFINED;
		VkComponentMapping			components		 {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
		VkImageSubresourceRange		subresourceRange { 0, 0, 0, 0, 0 };
		bool						canBeDestroyed	 = false;
	};


	//
	// Vulkan Buffer description
	//
	struct VulkanBufferDesc
	{
		VkBuffer					buffer			= Default;
		VkBufferUsageFlagBits		usage			= VkBufferUsageFlagBits(0);
		Bytes						size;
		EQueueMask					queues			= Default;
		VkMemoryPropertyFlagBits	memFlags		= Zero;
		bool						canBeDestroyed	= false;
		bool						allocMemory		= false;
	};


	//
	// Vulkan Buffer View description
	//
	struct VulkanBufferViewDesc
	{
		VkBufferView				view			= Default;
		VkFormat					format			= VK_FORMAT_UNDEFINED;
		Bytes						offset;
		Bytes						range;
		bool						canBeDestroyed	= false;
	};


	//
	// Vulkan Memory Object Info
	//
	struct VulkanMemoryObjInfo
	{
		VkDeviceMemory				memory			= Default;
		VkMemoryPropertyFlagBits	flags			= Zero;
		Bytes						offset;
		Bytes						size;
		void *						mappedPtr		= null;		// include 'offset'
	};


	//
	// Vulkan Command Batch Dependency
	//
	struct VulkanCmdBatchDependency
	{
		VkSemaphore					semaphore		= Default;
		ulong						value			= 0;		// for timeline semaphore
	//	VkPipelineStageFlags2KHR	stages			= VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT_KHR;

		ND_ explicit operator bool ()	C_NE___	{ return semaphore != Default; }
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
