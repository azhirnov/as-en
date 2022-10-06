// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN

# include "graphics/Public/Queue.h"
# include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{
	enum class EQueueFamily : uint
	{
		_Count		= VConfig::MaxQueues,
		External	= VK_QUEUE_FAMILY_EXTERNAL,
		Foreign		= VK_QUEUE_FAMILY_FOREIGN_EXT,
		Ignored		= VK_QUEUE_FAMILY_IGNORED,
		Unknown		= Ignored,
	};

	using VQueuePtr				= Ptr< const struct VQueue >;
	using VQueueFamilyIndices_t	= FixedArray< uint, uint(EQueueType::_Count) >;



	//
	// Vulkan Queue
	//

	struct VQueue
	{
	// variables
		mutable RecursiveMutex		guard;			// use when call vkQueueSubmit, vkQueueWaitIdle, vkQueueBindSparse, vkQueuePresentKHR
		VkQueue						handle			= Default;
		EQueueType					type			= Default;
		EQueueFamily				familyIndex		= Default;
		VkQueueFlagBits				familyFlags		= Zero;
		uint						queueIndex		= UMax;
		float						priority		= 0.0f;
		VkQueueGlobalPriorityEXT	globalPriority	= Zero;
		VkPipelineStageFlagBits2	supportedStages	= Zero;		// all supported pipeline stages, except HOST and ALL
		VkAccessFlagBits2			supportedAccess	= Zero;		// all supported memory access types, except HOST and ALL
		uint3						minImageTransferGranularity;
		FixedString<64>				debugName;
		

	// methods
		VQueue () {}

		VQueue (VQueue &&other) :
			handle{other.handle}, type{other.type}, familyIndex{other.familyIndex}, familyFlags{other.familyFlags},
			queueIndex{other.queueIndex}, priority{other.priority}, globalPriority{other.globalPriority},
			supportedStages{other.supportedStages}, supportedAccess{other.supportedAccess},
			minImageTransferGranularity{other.minImageTransferGranularity},
			debugName{other.debugName}
		{}
		
		VQueue (const VQueue &other) :
			handle{other.handle}, type{other.type}, familyIndex{other.familyIndex}, familyFlags{other.familyFlags},
			queueIndex{other.queueIndex}, priority{other.priority}, globalPriority{other.globalPriority},
			supportedStages{other.supportedStages}, supportedAccess{other.supportedAccess},
			minImageTransferGranularity{other.minImageTransferGranularity},
			debugName{other.debugName}
		{}

		ND_ bool  HasUnsupportedStages (VkPipelineStageFlagBits2 stages)	const	{ return AnyBits( stages, ~supportedStages ); }
		ND_ bool  HasUnsupportedAccess (VkAccessFlagBits2 access)			const	{ return AnyBits( access, ~supportedAccess ); }
	};


}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
