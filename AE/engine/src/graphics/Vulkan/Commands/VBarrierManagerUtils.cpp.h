// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Graphics::_hidden_
{
namespace
{
/*
=================================================
	DbgValidateBarrier
=================================================
*/
	template <typename T>
	static void  DbgValidateBarrier (EResourceState srcState, EResourceState dstState, const T &barrier) __NE___
	{
		Unused( srcState, dstState, barrier );

	#if 0 //def AE_DEBUG

		bool	is_r_to_r	= true;
		is_r_to_r &= (barrier.srcAccessMask & ~VPipelineScope::ReadOnlyAccessMask) == 0;
		is_r_to_r &= (barrier.dstAccessMask & ~VPipelineScope::ReadOnlyAccessMask) == 0;

		if constexpr( IsSameTypes< T, VkImageMemoryBarrier2 > or
					  IsSameTypes< T, VkBufferMemoryBarrier2 >)
			is_r_to_r &= (barrier.srcQueueFamilyIndex == barrier.srcQueueFamilyIndex);

		if constexpr( IsSameTypes< T, VkImageMemoryBarrier2 >)
			is_r_to_r &= (barrier.newLayout == barrier.oldLayout);

		DBG_CHECK_MSG( not is_r_to_r,
					   "read -> read barrier ("s << ToString(srcState) << ") -> (" << ToString(dstState) << ")" );

	#endif
	}

} // namespace
} // AE::Graphics::_hidden_
