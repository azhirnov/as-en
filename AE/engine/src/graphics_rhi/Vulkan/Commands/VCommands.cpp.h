// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VEnumCast.h"

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
	inline void  GenerateMipmapsImpl (VulkanDeviceFn fn, VkCommandBuffer cmdbuf,
									  VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges,
									  VkPipelineStageFlags2 srcStageMask, VkAccessFlags2 srcAccessMask, VkImageLayout oldLayout) __NE___
	{
		VkImageMemoryBarrier2	img_bar	= {};
		auto&					subres	= img_bar.subresourceRange;
		img_bar.sType					= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		img_bar.dstStageMask			= VK_PIPELINE_STAGE_2_BLIT_BIT;
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

			if ( oldLayout != VK_IMAGE_LAYOUT_UNDEFINED )
			{
				// src state -> transfer_src
				img_bar.srcStageMask	= srcStageMask;
				img_bar.srcAccessMask	= srcAccessMask;
				img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_READ_BIT;
				img_bar.oldLayout		= oldLayout;
				img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				subres.baseMipLevel		= range.baseMipLevel.Get();
				subres.levelCount		= 1;
				fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );
			}

			subres.baseMipLevel		= range.baseMipLevel.Get()+1;
			subres.levelCount		= range.mipmapCount-1;

			// undefined -> transfer_dst
			img_bar.srcStageMask	= srcStageMask;
			img_bar.srcAccessMask	= srcAccessMask;
			img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_WRITE_BIT;
			img_bar.oldLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
			img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
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
				img_bar.srcStageMask	= VK_PIPELINE_STAGE_2_BLIT_BIT;
				img_bar.srcAccessMask	= VK_ACCESS_2_TRANSFER_WRITE_BIT;
				img_bar.dstAccessMask	= VK_ACCESS_2_TRANSFER_READ_BIT;
				img_bar.oldLayout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				img_bar.newLayout		= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
				subres.baseMipLevel		= dst_mip;
				fn.vkCmdPipelineBarrier2KHR( cmdbuf, &barrier );
			}
		}
	}

/*
=================================================
	ConvertCooperativeVectorMatrixImpl
=================================================
*/
	inline void  ConvertCooperativeVectorMatrixImpl (VulkanDeviceFn fn, VkCommandBuffer cmdbuf, ArrayView<ConvertCoopMatrixCmd> inCommands) __NE___
	{
		CHECK_ERRV( not inCommands.empty() );

		StaticAssert( sizeof(ConvertCoopMatrixCmd::dstSize) == sizeof(RemovePointer<decltype(VkConvertCooperativeVectorMatrixInfoNV::pDstSize)>) );

		FixedArray< VkConvertCooperativeVectorMatrixInfoNV, 16 >	vk_cmds;

		for (usize i = 0; i < inCommands.size(); ++i)
		{
			const auto&	src	= inCommands[i];
			auto&		dst = vk_cmds.emplace_back();

			// TODO: move to ContextValidation ?
			CHECK_ERRV( src.srcSize > 0 and src.dstSize > 0 );
			CHECK_ERRV( src.srcAddress != Default and src.dstAddress != Default );
			CHECK_ERRV( IsMultipleOf( BitCast<ulong>(src.srcAddress), 64_b ) and IsMultipleOf( BitCast<ulong>(src.dstAddress), 64_b ));
			// TODO: check srcSize, dstSize, srcStride, dstStride

			dst.sType					= VK_STRUCTURE_TYPE_CONVERT_COOPERATIVE_VECTOR_MATRIX_INFO_NV;
			dst.pNext					= null;
			dst.srcSize					= usize{src.srcSize};
			dst.srcData.deviceAddress	= BitCast<VkDeviceAddress>( src.srcAddress );
			dst.pDstSize				= Cast<usize>( ConstCast( &src.dstSize ));		// not modified
			dst.dstData.deviceAddress	= BitCast<VkDeviceAddress>( src.dstAddress );
			dst.srcComponentType		= VEnumCast( src.srcType );
			dst.dstComponentType		= VEnumCast( src.dstType );
			dst.numRows					= src.numRows;
			dst.numColumns				= src.numColumns;
			dst.srcLayout				= VEnumCast( src.srcLayout );
			dst.srcStride				= usize{src.srcStride};
			dst.dstLayout				= VEnumCast( src.dstLayout );
			dst.dstStride				= usize{src.dstStride};

			CHECK_ERRV( dst.srcComponentType != VK_COMPONENT_TYPE_MAX_ENUM_KHR );
			CHECK_ERRV( dst.dstComponentType != VK_COMPONENT_TYPE_MAX_ENUM_KHR );
			CHECK_ERRV( dst.srcLayout != VK_COOPERATIVE_VECTOR_MATRIX_LAYOUT_MAX_ENUM_NV );
			CHECK_ERRV( dst.dstLayout != VK_COOPERATIVE_VECTOR_MATRIX_LAYOUT_MAX_ENUM_NV );

			if_unlikely( vk_cmds.IsFull() )
			{
				fn.vkCmdConvertCooperativeVectorMatrixNV( cmdbuf, uint(vk_cmds.size()), vk_cmds.data() );
				vk_cmds.clear();
			}
		}

		if ( not vk_cmds.empty() )
			fn.vkCmdConvertCooperativeVectorMatrixNV( cmdbuf, uint(vk_cmds.size()), vk_cmds.data() );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
