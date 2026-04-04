// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Graphics::_hidden_
{
namespace
{
#	include "vulkan_loader/vkenum_to_str.h"

/*
=================================================
	DbgValidateBarrier
=================================================
*/
	template <typename T>
	static void  DbgValidateBarrier (EResourceState srcState, EResourceState dstState, const T &barrier) __NE___
	{
		Unused( srcState, dstState, barrier );

		DBG_CHECK( EResourceState_Validate( srcState ));
		DBG_CHECK( EResourceState_Validate( dstState ));

	#if 0 //def AE_DEBUG

		bool	is_r_to_r	= true;
		is_r_to_r &= (barrier.srcAccessMask & ~VPipelineScope::ReadOnlyAccessMask) == 0;
		is_r_to_r &= (barrier.dstAccessMask & ~VPipelineScope::ReadOnlyAccessMask) == 0;

		if constexpr( IsSame< T, VkImageMemoryBarrier2 > or
					  IsSame< T, VkBufferMemoryBarrier2 >)
			is_r_to_r &= (barrier.srcQueueFamilyIndex == barrier.srcQueueFamilyIndex);

		if constexpr( IsSame< T, VkImageMemoryBarrier2 >)
			is_r_to_r &= (barrier.newLayout == barrier.oldLayout);

		DBG_CHECK_MSG( not is_r_to_r,
					   "read -> read barrier ("s << ToString(srcState) << ") -> (" << ToString(dstState) << ")" );

	#endif
	}

/*
=================================================
	DbgValidateBarrier
=================================================
*/
	template <typename B>
	inline void  DbgCheckBarrier (EResourceState srcState, EResourceState dstState,
								  VkPipelineStageFlagBits2 srcSupportedStages, VkAccessFlagBits2 srcSupportedAccess,
								  VkPipelineStageFlagBits2 dstSupportedStages, VkAccessFlagBits2 dstSupportedAccess,
								  bool isAcquireRelease, const B& barrier) __NE___
	{
		ASSERT_MSG( barrier.srcStageMask == VK_PIPELINE_STAGE_2_NONE or isAcquireRelease or (barrier.srcStageMask & srcSupportedStages) != 0,
			"Incompatible srcStage: "s << VkPipelineStageFlags2ToString( barrier.srcStageMask ) << "\nfor srcState: " << ToString( srcState ) <<
			"\nsrcSupportedStages: " << VkPipelineStageFlags2ToString( srcSupportedStages ) <<
			"\nwill be used ALL_COMMANDS stage." );

		ASSERT_MSG( barrier.dstStageMask == VK_PIPELINE_STAGE_2_NONE or isAcquireRelease or (barrier.dstStageMask & dstSupportedStages) != 0,
			"Incompatible dstStage: "s << VkPipelineStageFlags2ToString( barrier.dstStageMask ) << "\nfor dstState: " << ToString( dstState ) <<
			"\ndstSupportedStages: " << VkPipelineStageFlags2ToString( dstSupportedStages ) <<
			"\nwill be used ALL_COMMANDS stage." );

		ASSERT_MSG( barrier.srcAccessMask == VK_ACCESS_2_NONE or (barrier.srcAccessMask & srcSupportedAccess) != 0,
			"Incompatible srcAccess: "s << VkAccessFlags2ToString( barrier.srcAccessMask ) << "\nfor srcState: " << ToString( srcState ) <<
			"\nsrcSupportedAccess: " << VkAccessFlags2ToString( srcSupportedAccess ) <<
			"\nwill be used MEMORY_READ | MEMORY_WRITE." );

		ASSERT_MSG( barrier.dstAccessMask == VK_ACCESS_2_NONE or (barrier.dstAccessMask & dstSupportedAccess) != 0,
			"Incompatible dstAccess: "s << VkAccessFlags2ToString( barrier.dstAccessMask ) << "\nfor dstState: " << ToString( dstState ) <<
			"\ndstSupportedAccess: " << VkAccessFlags2ToString( dstSupportedAccess ) <<
			"\nwill be used MEMORY_READ | MEMORY_WRITE." );
	}

} // namespace
} // AE::Graphics::_hidden_
