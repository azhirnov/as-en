// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VTransferContext.h"
# include "graphics/Vulkan/Commands/VCommands.cpp.h"

namespace AE::Graphics::_hidden_
{
/*
=================================================
	GenerateMipmaps
=================================================
*/
	void  _VDirectTransferCtx::GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );

		GenerateMipmapsImpl( *this, _cmdbuf.Get(), image, dimension, ranges );
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
		cmd.size	= CheckCast< Bytes32u >( size );
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
	void  _VIndirectTransferCtx::GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		VALIDATE_GCTX( GenerateMipmaps( image, dimension, ranges ));

		auto&	cmd			= _cmdbuf->CreateCmd< GenerateMipmapsCmd, ImageSubresourceRange >( ranges.size() );	// throw
		auto*	dst_ranges	= Cast<ImageSubresourceRange>( AlignUp( static_cast< void *>(&cmd + 1), AlignOf<ImageSubresourceRange> ));

		cmd.image		= image;
		cmd.dimension	= dimension;
		cmd.rangeCount	= uint(ranges.size());
		MemCopy( OUT dst_ranges, ranges.data(), ArraySizeOf(ranges) );
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


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
