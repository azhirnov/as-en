// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/Commands/VTransferContext.h"
# include "graphics_rhi/Vulkan/Commands/VCommands.cpp.h"

namespace AE::Graphics::_hidden_
{
/*
=================================================
	GenerateMipmaps
=================================================
*/
	void  _VDirectTransferCtx::GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges, EResourceState srcState) __Th___
	{
		ASSERT( _NoPendingBarriers() );

		VkPipelineStageFlags2	src_stage_mask	= VK_PIPELINE_STAGE_2_BLIT_BIT;
		VkAccessFlags2			src_access_mask	= VK_ACCESS_2_NONE;
		VkImageLayout			old_layout		= VK_IMAGE_LAYOUT_UNDEFINED;

		if ( srcState != Default )
		{
			EResourceState_ToSrcStageAccessLayout( srcState, OUT src_stage_mask, OUT src_access_mask, OUT old_layout );
			src_stage_mask	&= _mngr.GetSupportedStages();
			src_access_mask	&= _mngr.GetSupportedAccess();
		}

		GenerateMipmapsImpl( *this, _cmdbuf.Get(), image, dimension, ranges, src_stage_mask, src_access_mask, old_layout );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VDirectTransferCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::Transfer ));

		return VBaseDirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VDirectTransferCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::Transfer ));

		return VBaseDirectContext::_ReleaseCommandBuffer();
	}

/*
=================================================
	ClearColorImage
=================================================
*/
	void  _VDirectTransferCtx::ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( ClearColorImage( image, ranges ));

		vkCmdClearColorImage( _cmdbuf.Get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, uint(ranges.size()), ranges.data() );
	}

/*
=================================================
	ClearDepthStencilImage
=================================================
*/
	void  _VDirectTransferCtx::ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( ClearDepthStencilImage( image, ranges ));

		vkCmdClearDepthStencilImage( _cmdbuf.Get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &depthStencil, uint(ranges.size()), ranges.data() );
	}

/*
=================================================
	FillBuffer
=================================================
*/
	void  _VDirectTransferCtx::FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( FillBuffer( buffer, offset, size ));

		vkCmdFillBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
	}

/*
=================================================
	UpdateBuffer
=================================================
*/
	void  _VDirectTransferCtx::UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( UpdateBuffer( buffer, offset, size, data ));

		vkCmdUpdateBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
	}

/*
=================================================
	CopyBuffer
=================================================
*/
	void  _VDirectTransferCtx::CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyBuffer( srcBuffer, dstBuffer, ranges ));

		vkCmdCopyBuffer( _cmdbuf.Get(), srcBuffer, dstBuffer, uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyBuffer2KHR
	}

/*
=================================================
	CopyImage
=================================================
*/
	void  _VDirectTransferCtx::CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyImage( srcImage, dstImage, ranges ));

		vkCmdCopyImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyImage2KHR
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	void  _VDirectTransferCtx::CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyBufferToImage( srcBuffer, dstImage, ranges ));

		vkCmdCopyBufferToImage( _cmdbuf.Get(), srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyBufferToImage2KHR
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	void  _VDirectTransferCtx::CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyImageToBuffer( srcImage, dstBuffer, ranges ));

		vkCmdCopyImageToBuffer( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer, uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyImageToBuffer2KHR
	}

/*
=================================================
	BlitImage
=================================================
*/
	void  _VDirectTransferCtx::BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( BlitImage( srcImage, dstImage, filter, regions ));

		vkCmdBlitImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						uint(regions.size()), regions.data(), filter );
		// TODO vkCmdBlitImage2KHR
	}

/*
=================================================
	ResolveImage
=================================================
*/
	void  _VDirectTransferCtx::ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( ResolveImage( srcImage, dstImage, regions ));

		vkCmdResolveImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							uint(regions.size()), regions.data() );
		// TODO vkCmdResolveImage2KHR
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VDirectTransferCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd> inCommands) __Th___
	{
		ConvertCooperativeVectorMatrixImpl( *this, this->_cmdbuf.Get(), inCommands );
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VDirectTransferCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd2> inCommands) __Th___
	{
		FixedArray< ConvertCoopMatrixCmd, 16 >	out_cmds;

		for (auto& src : inCommands)
		{
			auto&	dst = out_cmds.emplace_back();

			auto [src_buf, dst_buf] = this->_GetResourcesOrThrow( src.srcBuffer, src.dstBuffer );

			GCTX_CHECK( src.srcOffset < src_buf.Size() );
			GCTX_CHECK( src.srcOffset + src.srcSize <= src_buf.Size() );

			GCTX_CHECK( src.dstOffset < dst_buf.Size() );
			GCTX_CHECK( src.dstOffset + src.dstSize <= dst_buf.Size() );

			dst.srcSize		= src.srcSize;
			dst.srcAddress	= src_buf.GetDeviceAddress() + src.srcOffset;
			dst.dstSize		= src.dstSize;
			dst.dstAddress	= dst_buf.GetDeviceAddress() + src.dstOffset;
			dst.numRows		= src.numRows;
			dst.numColumns	= src.numColumns;
			dst.srcStride	= src.srcStride;
			dst.dstStride	= src.dstStride;
			dst.srcType		= src.srcType;
			dst.dstType		= src.dstType;
			dst.srcLayout	= src.srcLayout;
			dst.dstLayout	= src.dstLayout;

			StaticAssert64( sizeof(ConvertCoopMatrixCmd2) == 80 );

			if_unlikely( out_cmds.IsFull() )
			{
				ConvertCooperativeVectorMatrixImpl( *this, this->_cmdbuf.Get(), out_cmds );
				out_cmds.clear();
			}
		}

		if ( not out_cmds.empty() )
			ConvertCooperativeVectorMatrixImpl( *this, this->_cmdbuf.Get(), out_cmds );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ClearColorImage
=================================================
*/
	void  _VIndirectTransferCtx::ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( ClearColorImage( image, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< ClearColorImageCmd, VkImageSubresourceRange >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<VkImageSubresourceRange>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkImageSubresourceRange> ));

		cmd.image		= image;
		cmd.layout		= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		cmd.color		= color;
		cmd.rangeCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
	}

/*
=================================================
	ClearDepthStencilImage
=================================================
*/
	void  _VIndirectTransferCtx::ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( ClearDepthStencilImage( image, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< ClearDepthStencilImageCmd, VkImageSubresourceRange >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<VkImageSubresourceRange>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkImageSubresourceRange> ));

		cmd.image			= image;
		cmd.layout			= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		cmd.depthStencil	= depthStencil;
		cmd.rangeCount		= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
	}

/*
=================================================
	FillBuffer
=================================================
*/
	void  _VIndirectTransferCtx::FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( FillBuffer( buffer, offset, size ));

		auto&	cmd	= _cmdbuf->CreateCmd< FillBufferCmd >();	// throw
		cmd.buffer	= buffer;
		cmd.offset	= offset;
		cmd.size	= size;
		cmd.data	= data;
	}

/*
=================================================
	UpdateBuffer
=================================================
*/
	void  _VIndirectTransferCtx::UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		DBG_WARNING( "use UploadBuffer instead" );
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( UpdateBuffer( buffer, offset, size, data ));

		auto&	cmd	= _cmdbuf->CreateCmd< UpdateBufferCmd, ubyte >( usize(size) );	// throw
		auto*	dst	= static_cast< void *>(&cmd + 1);
		cmd.buffer	= buffer;
		cmd.offset	= offset;
		cmd.size	= CheckCast{ size };
		MemCopy( OUT dst, data, size );
	}

/*
=================================================
	CopyBuffer
=================================================
*/
	void  _VIndirectTransferCtx::CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyBuffer( srcBuffer, dstBuffer, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< CopyBufferCmd, VkBufferCopy >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<VkBufferCopy>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkBufferCopy> ));

		cmd.srcBuffer	= srcBuffer;
		cmd.dstBuffer	= dstBuffer;
		cmd.regionCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
	}

/*
=================================================
	CopyImage
=================================================
*/
	void  _VIndirectTransferCtx::CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyImage( srcImage, dstImage, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< CopyImageCmd, VkImageCopy >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<VkImageCopy>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkImageCopy> ));

		cmd.srcImage	= srcImage;
		cmd.srcLayout	= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		cmd.dstImage	= dstImage;
		cmd.dstLayout	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		cmd.regionCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	void  _VIndirectTransferCtx::CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyBufferToImage( srcBuffer, dstImage, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< CopyBufferToImageCmd, VkBufferImageCopy >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<VkBufferImageCopy>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkBufferImageCopy> ));

		cmd.srcBuffer	= srcBuffer;
		cmd.dstImage	= dstImage;
		cmd.dstLayout	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		cmd.regionCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	void  _VIndirectTransferCtx::CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( CopyImageToBuffer( srcImage, dstBuffer, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< CopyImageToBufferCmd, VkBufferImageCopy >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<VkBufferImageCopy>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkBufferImageCopy> ));

		cmd.srcImage	= srcImage;
		cmd.srcLayout	= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		cmd.dstBuffer	= dstBuffer;
		cmd.regionCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
	}

/*
=================================================
	BlitImage
=================================================
*/
	void  _VIndirectTransferCtx::BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( BlitImage( srcImage, dstImage, filter, regions ));

		auto&	cmd			= _cmdbuf->CreateCmd< BlitImageCmd, VkImageBlit >( regions.size() );	// throw
		auto*	dst_regions	= Cast<VkImageBlit>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkImageBlit> ));

		cmd.srcImage	= srcImage;
		cmd.srcLayout	= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		cmd.dstImage	= dstImage;
		cmd.dstLayout	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		cmd.filter		= filter;
		cmd.regionCount	= uint(regions.size());
		MemCopy( OUT dst_regions, regions.data(), ArraySizeOf(regions) );
	}

/*
=================================================
	ResolveImage
=================================================
*/
	void  _VIndirectTransferCtx::ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( ResolveImage( srcImage, dstImage, regions ));

		auto&	cmd			= _cmdbuf->CreateCmd< ResolveImageCmd, VkImageResolve >( regions.size() );	// throw
		auto*	dst_regions	= Cast<VkImageResolve>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<VkImageResolve> ));

		cmd.srcImage	= srcImage;
		cmd.srcLayout	= VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		cmd.dstImage	= dstImage;
		cmd.dstLayout	= VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		cmd.regionCount	= uint(regions.size());
		MemCopy( OUT dst_regions, regions.data(), ArraySizeOf(regions) );
	}

/*
=================================================
	GenerateMipmaps
=================================================
*/
	void  _VIndirectTransferCtx::GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges, EResourceState srcState) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( GenerateMipmaps( image, dimension, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< GenerateMipmapsCmd, ImageSubresourceRange >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<ImageSubresourceRange>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<ImageSubresourceRange> ));

		cmd.image		= image;
		cmd.dimension	= dimension;
		cmd.rangeCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );

		if ( srcState != Default )
		{
			EResourceState_ToSrcStageAccessLayout( srcState, OUT cmd.srcStageMask, OUT cmd.srcAccessMask, OUT cmd.oldLayout );
			cmd.srcStageMask	&= _mngr.GetSupportedStages();
			cmd.srcAccessMask	&= _mngr.GetSupportedAccess();
		}else{
			cmd.srcStageMask	= VK_PIPELINE_STAGE_2_BLIT_BIT;
			cmd.srcAccessMask	= VK_ACCESS_2_NONE;
			cmd.oldLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		}
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VBakedCommands  _VIndirectTransferCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::Transfer ));

		return VBaseIndirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VIndirectTransferCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::Transfer ));

		return VBaseIndirectContext::_ReleaseCommandBuffer();
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VIndirectTransferCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd> inCommands) __Th___
	{
		auto&	cmd		 = _cmdbuf->CreateCmd< ConvertCooperativeVectorMatrixCmd, ConvertCoopMatrixCmd >( inCommands.size() );	// throw
		auto*	dst_cmds = Cast<ConvertCoopMatrixCmd>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<ConvertCoopMatrixCmd> ));

		cmd.count = uint(inCommands.size());
		MemCopy( OUT dst_cmds, inCommands.data(), ArraySizeOf(inCommands) );
	}

/*
=================================================
	_ConvertCooperativeVectorMatrix
=================================================
*/
	void  _VIndirectTransferCtx::_ConvertCooperativeVectorMatrix (ArrayView<ConvertCoopMatrixCmd2> inCommands) __Th___
	{
		FixedArray< ConvertCoopMatrixCmd, 16 >	out_cmds;

		for (auto& src : inCommands)
		{
			auto&	dst = out_cmds.emplace_back();

			auto [src_buf, dst_buf] = this->_GetResourcesOrThrow( src.srcBuffer, src.dstBuffer );

			GCTX_CHECK( src.srcOffset < src_buf.Size() );
			GCTX_CHECK( src.srcOffset + src.srcSize <= src_buf.Size() );

			GCTX_CHECK( src.dstOffset < dst_buf.Size() );
			GCTX_CHECK( src.dstOffset + src.dstSize <= dst_buf.Size() );

			dst.srcSize		= src.srcSize;
			dst.srcAddress	= src_buf.GetDeviceAddress() + src.srcOffset;
			dst.dstSize		= src.dstSize;
			dst.dstAddress	= dst_buf.GetDeviceAddress() + src.dstOffset;
			dst.numRows		= src.numRows;
			dst.numColumns	= src.numColumns;
			dst.srcStride	= src.srcStride;
			dst.dstStride	= src.dstStride;
			dst.srcType		= src.srcType;
			dst.dstType		= src.dstType;
			dst.srcLayout	= src.srcLayout;
			dst.dstLayout	= src.dstLayout;

			StaticAssert64( sizeof(ConvertCoopMatrixCmd2) == 80 );

			if_unlikely( out_cmds.IsFull() )
			{
				_ConvertCooperativeVectorMatrix( out_cmds );
				out_cmds.clear();
			}
		}

		if ( not out_cmds.empty() )
			_ConvertCooperativeVectorMatrix( out_cmds );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VTransferContextImpl<C>::_VTransferContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RawCtx{ task, RVRef(cmdbuf), dbg }
	{
		Validator_t::CtxInit( task.QueueMask() );
	}

/*
=================================================
	_ClearColorImage
=================================================
*/
	template <typename C>
	template <typename ColType>
	void  _VTransferContextImpl<C>::_ClearColorImage (ImageID imageId, const ColType &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		auto&	img = _GetResourcesOrThrow( imageId );
		VALIDATE_GCTX( ClearColorImage( img.Description(), ranges ));

		VkClearColorValue										clear_value;
		FixedArray<VkImageSubresourceRange, _LocalArraySize>	vk_ranges;
		const ImageDesc &										desc	= img.Description();

		MemCopy( OUT clear_value, color );

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertImageSubresourceRange( OUT dst, src, desc );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::ClearColorImage( img.Handle(), clear_value, vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::ClearColorImage( img.Handle(), clear_value, vk_ranges );
	}

/*
=================================================
	ClearDepthStencilImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::ClearDepthStencilImage (ImageID imageId, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		auto&	img = _GetResourcesOrThrow( imageId );
		VALIDATE_GCTX( ClearDepthStencilImage( img.Description(), ranges ));

		VkClearDepthStencilValue								clear_value;
		FixedArray<VkImageSubresourceRange, _LocalArraySize>	vk_ranges;
		const ImageDesc &										desc	= img.Description();

		clear_value.depth	= depthStencil.depth;
		clear_value.stencil	= depthStencil.stencil;

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertImageSubresourceRange( OUT dst, src, desc );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::ClearDepthStencilImage( img.Handle(), clear_value, vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::ClearDepthStencilImage( img.Handle(), clear_value, vk_ranges );
	}

/*
=================================================
	FillBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::FillBuffer (BufferID bufferId, Bytes offset, Bytes size, uint data) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		VALIDATE_GCTX( FillBuffer( buf.Description(), offset, size ));

		const Bytes	buf_size = buf.Size();
		offset	= Min( offset, buf_size );
		size	= Min( size, buf_size - offset );

		RawCtx::FillBuffer( buf.Handle(), offset, size, data );
	}

/*
=================================================
	UpdateBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::UpdateBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		VALIDATE_GCTX( UpdateBuffer( buf.Description(), offset, size, data ));

		RawCtx::UpdateBuffer( buf.Handle(), offset, size, data );
	}

/*
=================================================
	ReadHostBuffer
=================================================
*/
	template <typename C>
	ITransferContext::ReadHostBufferResult  _VTransferContextImpl<C>::ReadHostBuffer (BufferID bufferId, Bytes offset, Bytes size) __Th___
	{
		VulkanMemoryObjInfo	mem_info;
		CHECK_ERR( _MapHostBuffer( bufferId, INOUT offset, INOUT size, OUT mem_info ));

		ArrayView<ubyte>	mem_view = ArrayView<ubyte>{ Cast<ubyte>(mem_info.mappedPtr + offset), usize(size) };

		if_unlikely( NoBits( mem_info.flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
		{
			GCTX_CHECK( offset + size <= mem_info.size );
			this->_mngr.GetStagingManager().AcquireMappedMemory( GetFrameId(), mem_info.memory, mem_info.offset + offset, size );
		}

		return	ReadHostBufferResult{
					Scheduler().Run(
						ETaskQueue::PerFrame,
						DeferResult<ArrayView<ubyte>>( mem_view ),
						Tuple{ OnFrameNextCycle{ GetFrameId() }},
						"VTransferContext::ReadHostBuffer"
					)};
	}

/*
=================================================
	MapHostBuffer
=================================================
*/
	template <typename C>
	bool  _VTransferContextImpl<C>::MapHostBuffer (BufferID bufferId, Bytes offset, INOUT Bytes &size, OUT void* &mapped) __Th___
	{
		VulkanMemoryObjInfo	mem_info;
		bool	res = _MapHostBuffer( bufferId, offset, INOUT size, OUT mem_info );
		mapped = mem_info.mappedPtr + offset;
		return res;
	}

	template <typename C>
	bool  _VTransferContextImpl<C>::_MapHostBuffer (BufferID bufferId, INOUT Bytes &offset, INOUT Bytes &size, OUT VulkanMemoryObjInfo &memInfo) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		VALIDATE_GCTX( MapHostBuffer( buf.Description(), offset, size ));

		auto&	mem = _GetResourcesOrThrow( buf.MemoryId() );
		CHECK_ERR( mem.GetMemoryInfo( OUT memInfo ));
		CHECK_ERR( memInfo.mappedPtr != null );

		const Bytes	buf_size = buf.Size();
		offset	= Min( offset, buf_size );
		size	= Min( size, buf_size - offset );

		ASSERT( size > 0 );
		return true;
	}

/*
=================================================
	UpdateHostBuffer
=================================================
*/
	template <typename C>
	bool  _VTransferContextImpl<C>::UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) __Th___
	{
		VulkanMemoryObjInfo	mem_info;
		CHECK_ERR( _MapHostBuffer( bufferId, INOUT offset, INOUT size, OUT mem_info ));

		MemCopy( OUT mem_info.mappedPtr + offset, data, size );

		if_unlikely( NoBits( mem_info.flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
		{
			VkMappedMemoryRange	range;
			range.sType		= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.pNext		= null;
			range.memory	= mem_info.memory;
			range.offset	= VkDeviceSize(mem_info.offset + offset);
			range.size		= VkDeviceSize(size);

			GCTX_CHECK( offset + size <= mem_info.size );

			auto&	dev = this->_mngr.GetDevice();
			VK_CHECK( dev.vkFlushMappedMemoryRanges( dev.GetVkDevice(), 1, &range ));
		}
		return true;
	}

/*
=================================================
	CopyBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) __Th___
	{
		auto  [src_buf, dst_buf] = _GetResourcesOrThrow( srcBuffer, dstBuffer );
		VALIDATE_GCTX( CopyBuffer( src_buf.Description(), dst_buf.Description(), ranges ));

		FixedArray<VkBufferCopy, _LocalArraySize>	vk_ranges;
		const Bytes									src_size	= src_buf.Size();
		const Bytes									dst_size	= dst_buf.Size();

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			dst.srcOffset	= VkDeviceSize(Min( src.srcOffset, src_size ));
			dst.dstOffset	= VkDeviceSize(Min( src.dstOffset, dst_size ));
			dst.size		= VkDeviceSize(Min( src.size, src_size - src.srcOffset, dst_size - src.dstOffset ));

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::CopyBuffer( src_buf.Handle(), dst_buf.Handle(), vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::CopyBuffer( src_buf.Handle(), dst_buf.Handle(), vk_ranges );
	}

/*
=================================================
	CopyImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) __Th___
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
		VALIDATE_GCTX( CopyImage( src_img.Description(), dst_img.Description(), ranges ));

		FixedArray<VkImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &							src_desc	= src_img.Description();
		const ImageDesc &							dst_desc	= dst_img.Description();

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			dst.srcOffset	= { int(src.srcOffset.x), int(src.srcOffset.y), int(src.srcOffset.z) };
			dst.dstOffset	= { int(src.dstOffset.x), int(src.dstOffset.y), int(src.dstOffset.z) };
			dst.extent		= { src.extent.x,         src.extent.y,         src.extent.z };

			_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
			_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::CopyImage( src_img.Handle(), dst_img.Handle(), vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::CopyImage( src_img.Handle(), dst_img.Handle(), vk_ranges );
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) __Th___
	{
		auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );
		VALIDATE_GCTX( CopyBufferToImage( src_buf.Description(), dst_img.Description(), ranges ));

		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= dst_img.Description();

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertBufferImageCopy( OUT dst, src, img_desc );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );
		VALIDATE_GCTX( CopyBufferToImage( src_buf.Description(), dst_img.Description(), ranges ));

		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= dst_img.Description();
		const PixelFormatInfo &							fmt_info	= EPixelFormat_GetInfo( img_desc.format );

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertBufferImageCopy( OUT dst, src, img_desc, fmt_info );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) __Th___
	{
		auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );
		VALIDATE_GCTX( CopyImageToBuffer( src_img.Description(), dst_buf.Description(), ranges ));

		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= src_img.Description();

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertBufferImageCopy( OUT dst, src, img_desc );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );
		VALIDATE_GCTX( CopyImageToBuffer( src_img.Description(), dst_buf.Description(), ranges ));

		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= src_img.Description();
		const PixelFormatInfo &							fmt_info	= EPixelFormat_GetInfo( img_desc.format );

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertBufferImageCopy( OUT dst, src, img_desc, fmt_info );

			if_unlikely( vk_ranges.IsFull() )
			{
				RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
				vk_ranges.clear();
			}
		}

		if ( vk_ranges.size() )
			RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
	}

/*
=================================================
	BlitImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter blitFilter, ArrayView<ImageBlit> regions) __Th___
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
		VALIDATE_GCTX( BlitImage( src_img.Description(), dst_img.Description(), blitFilter, regions ));

		FixedArray<VkImageBlit, _LocalArraySize>	vk_regions;
		const ImageDesc &							src_desc	= src_img.Description();
		const ImageDesc &							dst_desc	= dst_img.Description();
		const VkFilter								filter		= VEnumCast( blitFilter );

		for (usize i = 0; i < regions.size(); ++i)
		{
			auto&	src = regions[i];
			auto&	dst = vk_regions.emplace_back();

			dst.srcOffsets[0] = { int(src.srcOffset0.x), int(src.srcOffset0.y), int(src.srcOffset0.z) };
			dst.srcOffsets[1] = { int(src.srcOffset1.x), int(src.srcOffset1.y), int(src.srcOffset1.z) };
			dst.dstOffsets[0] = { int(src.dstOffset0.x), int(src.dstOffset0.y), int(src.dstOffset0.z) };
			dst.dstOffsets[1] = { int(src.dstOffset1.x), int(src.dstOffset1.y), int(src.dstOffset1.z) };
			this->_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
			this->_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

			if_unlikely( vk_regions.IsFull() )
			{
				RawCtx::BlitImage( src_img.Handle(), dst_img.Handle(), filter, vk_regions );
				vk_regions.clear();
			}
		}

		if ( vk_regions.size() )
			RawCtx::BlitImage( src_img.Handle(), dst_img.Handle(), filter, vk_regions );
	}

/*
=================================================
	ResolveImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions) __Th___
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
		VALIDATE_GCTX( ResolveImage( src_img.Description(), dst_img.Description(), regions ));

		FixedArray<VkImageResolve, _LocalArraySize>	vk_regions;
		const ImageDesc &							src_desc	= src_img.Description();
		const ImageDesc &							dst_desc	= dst_img.Description();

		for (usize i = 0; i < regions.size(); ++i)
		{
			auto&	src = regions[i];
			auto&	dst = vk_regions.emplace_back();

			dst.srcOffset	= { int(src.srcOffset.x), int(src.srcOffset.y), int(src.srcOffset.z) };
			dst.dstOffset	= { int(src.dstOffset.x), int(src.dstOffset.y), int(src.dstOffset.z) };
			dst.extent		= { src.extent.x,		  src.extent.y,			src.extent.z		 };

			this->_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
			this->_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

			if_unlikely( vk_regions.IsFull() )
			{
				RawCtx::ResolveImage( src_img.Handle(), dst_img.Handle(), vk_regions );
				vk_regions.clear();
			}
		}

		if ( vk_regions.size() )
			RawCtx::ResolveImage( src_img.Handle(), dst_img.Handle(), vk_regions );
	}

/*
=================================================
	GenerateMipmaps
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::GenerateMipmaps (ImageID image, EResourceState srcState) __Th___
	{
		auto&					img = _GetResourcesOrThrow( image );
		ImageDesc const&		desc = img.Description();
		ImageSubresourceRange	range;

		range.aspectMask	= EPixelFormat_ToImageAspect( desc.format );
		range.baseMipLevel	= 0_mipmap;
		range.baseLayer		= 0_layer;
		range.layerCount	= LayerCount_t(desc.arrayLayers.Get());
		range.mipmapCount	= MipmapCount_t(desc.mipLevels.Get());

		VALIDATE_GCTX( GenerateMipmaps( img.Description(), {range} ));
		RawCtx::GenerateMipmaps( img.Handle(), desc.Dimension(), {range}, srcState );
	}

	template <typename C>
	void  _VTransferContextImpl<C>::GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState srcState) __Th___
	{
		auto&				img = _GetResourcesOrThrow( image );
		ImageDesc const&	desc = img.Description();

		VALIDATE_GCTX( GenerateMipmaps( img.Description(), ranges ));
		RawCtx::GenerateMipmaps( img.Handle(), desc.Dimension(), ranges, srcState );
	}

/*
=================================================
	MinImageTransferGranularity
=================================================
*/
	template <typename C>
	uint3  _VTransferContextImpl<C>::MinImageTransferGranularity () C_NE___
	{
		return uint3{this->_mngr.GetQueue()->minImageTransferGranularity};
	}

/*
=================================================
	_ConvertImageSubresourceRange
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::_ConvertImageSubresourceRange (OUT VkImageSubresourceRange& dst, const ImageSubresourceRange& src, const ImageDesc &desc) __NE___
	{
		dst.aspectMask		= VEnumCast( src.aspectMask );
		dst.baseMipLevel	= Min( src.baseMipLevel.Get(), desc.mipLevels.Get()-1 );
		dst.levelCount		= Min( src.mipmapCount, desc.mipLevels.Get() - src.baseMipLevel.Get() );
		dst.baseArrayLayer	= Min( src.baseLayer.Get(), desc.arrayLayers.Get()-1 );
		dst.layerCount		= Min( src.layerCount, desc.arrayLayers.Get() - src.baseLayer.Get() );
	}

/*
=================================================
	_ConvertBufferImageCopy
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::_ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy& src, const ImageDesc &desc) __NE___
	{
		dst.bufferOffset		= VkDeviceSize(src.bufferOffset);
		dst.bufferRowLength		= src.bufferRowLength;
		dst.bufferImageHeight	= src.bufferImageHeight;
		_ConvertImageSubresourceLayer( OUT dst.imageSubresource, src.imageSubres, desc );
		dst.imageOffset			= { int(src.imageOffset.x), int(src.imageOffset.y), int(src.imageOffset.z) };
		dst.imageExtent			= { src.imageExtent.x, src.imageExtent.y, src.imageExtent.z };
	}

/*
=================================================
	_ConvertBufferImageCopy
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::_ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy2& src, const ImageDesc &desc, const PixelFormatInfo &fmtInfo) __Th___
	{
		const uint	bits_per_block = AllBits( src.imageSubres.aspectMask, EImageAspect::Stencil ) ?
										fmtInfo.bitsPerBlock2 : fmtInfo.bitsPerBlock;

		dst.bufferOffset		= VkDeviceSize(src.bufferOffset);
		dst.bufferRowLength		= ImageUtils::RowLength( src.rowPitch, bits_per_block, fmtInfo.TexBlockDim() );
		dst.bufferImageHeight	= ImageUtils::ImageHeight( src.slicePitch, src.rowPitch, fmtInfo.TexBlockDim() );
		_ConvertImageSubresourceLayer( OUT dst.imageSubresource, src.imageSubres, desc );
		dst.imageOffset			= { int(src.imageOffset.x), int(src.imageOffset.y), int(src.imageOffset.z) };
		dst.imageExtent			= { src.imageExtent.x, src.imageExtent.y, src.imageExtent.z };

		GCTX_CHECK( dst.bufferRowLength == 0 or dst.bufferRowLength >= dst.imageExtent.width );
		GCTX_CHECK( dst.bufferImageHeight == 0 or dst.bufferImageHeight >= dst.imageExtent.height );
	}

/*
=================================================
	_ConvertImageSubresourceLayer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::_ConvertImageSubresourceLayer (OUT VkImageSubresourceLayers &dst, const ImageSubresourceLayers &src, const ImageDesc &desc) __NE___
	{
		dst.aspectMask		= VEnumCast( src.aspectMask );
		dst.mipLevel		= Min( src.mipLevel.Get(), desc.mipLevels.Get()-1 );
		dst.baseArrayLayer	= Min( src.baseLayer.Get(), desc.arrayLayers.Get()-1 );
		dst.layerCount		= Min( src.layerCount, desc.arrayLayers.Get() - src.baseLayer.Get() );
	}

} // AE::Graphics::_hidden_

# include "graphics_rhi/Private/TransferContextImpl.cpp.h"
//-----------------------------------------------------------------------------

namespace AE::Graphics::_hidden_
{
    template class _VTransferContextImpl< _VDirectTransferCtx >;
    template class _VTransferContextImpl< _VIndirectTransferCtx >;

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
