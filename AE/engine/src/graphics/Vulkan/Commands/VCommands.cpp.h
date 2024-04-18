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
	src layout:  transfer_src (base mip)
	dst layout:  transfer_src (all other mips)
=================================================
*/
	static void  GenerateMipmapsImpl (VulkanDeviceFn fn, VkCommandBuffer cmdbuf,
									  VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges) __NE___
	{
		VkImageMemoryBarrier2	img_bar	= {};
		auto&					subres	= img_bar.subresourceRange;
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

		for (auto& range : ranges)
		{
			subres.aspectMask		= VEnumCast( range.aspectMask );
			subres.baseArrayLayer	= range.baseLayer.Get();
			subres.layerCount		= range.layerCount;
			subres.baseMipLevel		= range.baseMipLevel.Get()+1;
			subres.levelCount		= range.mipmapCount-1;

			// undefined -> transfer_dst
			img_bar.oldLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
			img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			img_bar.srcAccessMask	= VK_ACCESS_2_NONE;
			img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_WRITE_BIT;
			fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );

			subres.levelCount		= 1;

			for (uint mip = range.baseMipLevel.Get()+1; mip < range.mipmapCount; ++mip)
			{
				const uint	src_mip		= mip - 1;
				const uint	dst_mip		= mip;
				const int3	src_size	= int3{Max( 1u, dimension >> src_mip )};
				const int3	dst_size	= int3{Max( 1u, dimension >> dst_mip )};

				VkImageBlit		region	= {};
				region.srcOffsets[0]	= { 0, 0, 0 };
				region.srcOffsets[1]	= { src_size.x, src_size.y, src_size.z };
				region.srcSubresource	= { subres.aspectMask, src_mip, range.baseLayer.Get(), range.layerCount };
				region.dstOffsets[0]	= { 0, 0, 0 };
				region.dstOffsets[1]	= { dst_size.x, dst_size.y, dst_size.z };
				region.dstSubresource	= { subres.aspectMask, dst_mip, range.baseLayer.Get(), range.layerCount };

				fn.vkCmdBlitImage( cmdbuf, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
									image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region, VK_FILTER_LINEAR );

				// transfer_dst -> transfer_src
				img_bar.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				img_bar.srcAccessMask	= VK_ACCESS_2_TRANSFER_WRITE_BIT;
				img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_READ_BIT;
				subres.baseMipLevel		= dst_mip;
				fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );
			}
		}
	}

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
