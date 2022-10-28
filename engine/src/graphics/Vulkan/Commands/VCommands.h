// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{
	
/*
=================================================
	GenerateMipmapsImpl
----
	src layout:  transfer_src (level 0)
	dst layout:  transfer_src (all levels)
=================================================
*/
	inline void  GenerateMipmapsImpl (VulkanDeviceFn fn, VkCommandBuffer cmdbuf,
									  VkImage image, const uint3 &dimension, const uint levelCount, const uint layerCount, EImageAspect aspect)
	{
		ASSERT( image != Default );
		ASSERT( All( dimension > 0u ));
		ASSERT( levelCount > 0 );
		ASSERT( layerCount > 0 );

		const uint	base_layer	= 0;

		VkImageMemoryBarrier2	img_bar	= {};
		img_bar.sType					= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		img_bar.srcStageMask			= VK_PIPELINE_STAGE_2_BLIT_BIT;
		img_bar.srcAccessMask			= VK_ACCESS_2_NONE;
		img_bar.dstStageMask			= VK_PIPELINE_STAGE_2_BLIT_BIT;
		img_bar.dstAccessMask			= VK_ACCESS_2_NONE;
		img_bar.oldLayout				= VK_IMAGE_LAYOUT_UNDEFINED;
		img_bar.newLayout				= VK_IMAGE_LAYOUT_UNDEFINED;
		img_bar.srcQueueFamilyIndex		= VK_QUEUE_FAMILY_IGNORED;
		img_bar.dstQueueFamilyIndex		= VK_QUEUE_FAMILY_IGNORED;
		img_bar.image					= image;

		VkDependencyInfo		barrier	= {};
		barrier.sType					= VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		barrier.imageMemoryBarrierCount	= 1;
		barrier.pImageMemoryBarriers	= &img_bar;
		
		auto&			subres	= img_bar.subresourceRange;
		subres.aspectMask		= VEnumCast( aspect );
		subres.baseArrayLayer	= base_layer;
		subres.layerCount		= layerCount;
		subres.levelCount		= 1;

		for (uint level = 1; level < levelCount; ++level)
		{
			const uint	src_mip		= level - 1;
			const uint	dst_mip		= level;
			const int3	src_size	= int3{Max( 1u, dimension >> src_mip )};
			const int3	dst_size	= int3{Max( 1u, dimension >> dst_mip )};
			
			// undefined -> transfer_dst
			img_bar.oldLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
			img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			img_bar.srcAccessMask	= VK_ACCESS_2_NONE;
			img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_WRITE_BIT;
			subres.baseMipLevel		= dst_mip;
			fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );

			VkImageBlit		region	= {};
			region.srcOffsets[0]	= { 0, 0, 0 };
			region.srcOffsets[1]	= { src_size.x, src_size.y, src_size.z };
			region.srcSubresource	= { subres.aspectMask, src_mip, base_layer, layerCount };
			region.dstOffsets[0]	= { 0, 0, 0 };
			region.dstOffsets[1]	= { dst_size.x, dst_size.y, dst_size.z };
			region.dstSubresource	= { subres.aspectMask, dst_mip, base_layer, layerCount };

			fn.vkCmdBlitImage( cmdbuf, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
								image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_LINEAR );

			// transfer_dst -> transfer_src
			img_bar.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			img_bar.srcAccessMask	= VK_ACCESS_2_TRANSFER_WRITE_BIT;
			img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_READ_BIT;
			fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );
		}
	}

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
