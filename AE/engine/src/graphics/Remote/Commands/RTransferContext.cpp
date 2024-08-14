// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RTransferContext.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RTransferContext::RTransferContext (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), dbg, ECtxType::Transfer }
	{
		Validator_t::CtxInit( task.GetQueueMask() );
	}

/*
=================================================
	ClearColorImage
=================================================
*/
	void  RTransferContext::ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		auto&	img = _GetResourcesOrThrow( image );
		VALIDATE_GCTX( ClearColorImage( img.Description(), ranges ));
		ClearColorImage( img.Handle(), color, ranges );
	}

	void  RTransferContext::ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		auto&	img = _GetResourcesOrThrow( image );
		VALIDATE_GCTX( ClearColorImage( img.Description(), ranges ));
		ClearColorImage( img.Handle(), color, ranges );
	}

	void  RTransferContext::ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		auto&	img = _GetResourcesOrThrow( image );
		VALIDATE_GCTX( ClearColorImage( img.Description(), ranges ));
		ClearColorImage( img.Handle(), color, ranges );
	}

	void  RTransferContext::ClearColorImage (RmImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::ClearColorImage ));

		Msg::CmdBuf_Bake::ClearColorImageCmd	cmd;
		cmd.image	= image;
		cmd.color	= color;
		cmd.ranges	= ranges;
		_cmdbuf->AddCommand( cmd );
	}

	void  RTransferContext::ClearColorImage (RmImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::ClearColorImage ));

		Msg::CmdBuf_Bake::ClearColorImageCmd	cmd;
		cmd.image	= image;
		cmd.color	= color;
		cmd.ranges	= ranges;
		_cmdbuf->AddCommand( cmd );
	}

	void  RTransferContext::ClearColorImage (RmImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::ClearColorImage ));

		Msg::CmdBuf_Bake::ClearColorImageCmd	cmd;
		cmd.image	= image;
		cmd.color	= color;
		cmd.ranges	= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	ClearDepthStencilImage
=================================================
*/
	void  RTransferContext::ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		auto&	img = _GetResourcesOrThrow( image );
		VALIDATE_GCTX( ClearDepthStencilImage( img.Description(), ranges ));
		ClearDepthStencilImage( img.Handle(), depthStencil, ranges );
	}

	void  RTransferContext::ClearDepthStencilImage (RmImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::ClearDepthStencilImage ));

		Msg::CmdBuf_Bake::ClearDepthStencilImageCmd	cmd;
		cmd.image			= image;
		cmd.depthStencil	= depthStencil;
		cmd.ranges			= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	UpdateBuffer
=================================================
*/
	void  RTransferContext::UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );
		VALIDATE_GCTX( UpdateBuffer( buf.Description(), offset, size, data ));
		UpdateBuffer( buf.Handle(), offset, size, data );
	}

	void  RTransferContext::UpdateBuffer (RmBufferID buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		Msg::CmdBuf_Bake::UpdateBufferCmd	cmd;
		cmd.buffer	= buffer;
		cmd.offset	= offset;
		cmd.size	= size;

		void*	dst;
		_cmdbuf->Allocate( OUT dst, OUT cmd.data, size );  // throw
		MemCopy( OUT dst, data, size );

		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	FillBuffer
=================================================
*/
	void  RTransferContext::FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );
		VALIDATE_GCTX( FillBuffer( buf.Description(), offset, size ));
		FillBuffer( buf.Handle(), offset, size, data );
	}

	void  RTransferContext::FillBuffer (RmBufferID buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		Msg::CmdBuf_Bake::FillBufferCmd	cmd;
		cmd.buffer	= buffer;
		cmd.offset	= offset;
		cmd.size	= size;
		cmd.data	= data;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	CopyBuffer
=================================================
*/
	void  RTransferContext::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcBuffer );
		auto&	dst = _GetResourcesOrThrow( dstBuffer );

		VALIDATE_GCTX( CopyBuffer( src.Description(), dst.Description(), ranges ));
		CopyBuffer( src.Handle(), dst.Handle(), ranges );
	}

	void  RTransferContext::CopyBuffer (RmBufferID srcBuffer, RmBufferID dstBuffer, ArrayView<BufferCopy> ranges) __Th___
	{
		Msg::CmdBuf_Bake::CopyBufferCmd	cmd;
		cmd.srcBuffer	= srcBuffer;
		cmd.dstBuffer	= dstBuffer;
		cmd.ranges		= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	CopyImage
=================================================
*/
	void  RTransferContext::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcImage );
		auto&	dst = _GetResourcesOrThrow( dstImage );

		VALIDATE_GCTX( CopyImage( src.Description(), dst.Description(), ranges ));
		CopyImage( src.Handle(), dst.Handle(), ranges );
	}

	void  RTransferContext::CopyImage (RmImageID srcImage, RmImageID dstImage, ArrayView<ImageCopy> ranges) __Th___
	{
		Msg::CmdBuf_Bake::CopyImageCmd	cmd;
		cmd.srcImage	= srcImage;
		cmd.dstImage	= dstImage;
		cmd.ranges		= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	void  RTransferContext::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcBuffer );
		auto&	dst = _GetResourcesOrThrow( dstImage );

		VALIDATE_GCTX( CopyBufferToImage( src.Description(), dst.Description(), ranges ));
		CopyBufferToImage( src.Handle(), dst.Handle(), ranges );
	}

	void  RTransferContext::CopyBufferToImage (RmBufferID srcBuffer, RmImageID dstImage, ArrayView<BufferImageCopy> ranges) __Th___
	{
		Msg::CmdBuf_Bake::CopyBufferToImageCmd	cmd;
		cmd.srcBuffer	= srcBuffer;
		cmd.dstImage	= dstImage;
		cmd.ranges		= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	void  RTransferContext::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcImage );
		auto&	dst = _GetResourcesOrThrow( dstBuffer );

		VALIDATE_GCTX( CopyImageToBuffer( src.Description(), dst.Description(), ranges ));
		CopyImageToBuffer( src.Handle(), dst.Handle(), ranges );
	}

	void  RTransferContext::CopyImageToBuffer (RmImageID srcImage, RmBufferID dstBuffer, ArrayView<BufferImageCopy> ranges) __Th___
	{
		Msg::CmdBuf_Bake::CopyImageToBufferCmd	cmd;
		cmd.srcImage	= srcImage;
		cmd.dstBuffer	= dstBuffer;
		cmd.ranges		= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	void  RTransferContext::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcBuffer );
		auto&	dst = _GetResourcesOrThrow( dstImage );

		VALIDATE_GCTX( CopyBufferToImage( src.Description(), dst.Description(), ranges ));
		CopyBufferToImage( src.Handle(), dst.Handle(), ranges );
	}

	void  RTransferContext::CopyBufferToImage (RmBufferID srcBuffer, RmImageID dstImage, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		Msg::CmdBuf_Bake::CopyBufferToImage2Cmd	cmd;
		cmd.srcBuffer	= srcBuffer;
		cmd.dstImage	= dstImage;
		cmd.ranges		= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	void  RTransferContext::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcImage );
		auto&	dst = _GetResourcesOrThrow( dstBuffer );

		VALIDATE_GCTX( CopyImageToBuffer( src.Description(), dst.Description(), ranges ));
		CopyImageToBuffer( src.Handle(), dst.Handle(), ranges );
	}

	void  RTransferContext::CopyImageToBuffer (RmImageID srcImage, RmBufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		Msg::CmdBuf_Bake::CopyImageToBuffer2Cmd	cmd;
		cmd.srcImage	= srcImage;
		cmd.dstBuffer	= dstBuffer;
		cmd.ranges		= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BlitImage
=================================================
*/
	void  RTransferContext::BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcImage );
		auto&	dst = _GetResourcesOrThrow( dstImage );

		VALIDATE_GCTX( BlitImage( src.Description(), dst.Description(), filter, regions ));
		BlitImage( src.Handle(), dst.Handle(), filter, regions );
	}

	void  RTransferContext::BlitImage (RmImageID srcImage, RmImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions) __Th___
	{
		Msg::CmdBuf_Bake::BlitImageCmd	cmd;
		cmd.srcImage	= srcImage;
		cmd.dstImage	= dstImage;
		cmd.filter		= filter;
		cmd.regions		= regions;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	ResolveImage
=================================================
*/
	void  RTransferContext::ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions) __Th___
	{
		auto&	src = _GetResourcesOrThrow( srcImage );
		auto&	dst = _GetResourcesOrThrow( dstImage );

		VALIDATE_GCTX( ResolveImage( src.Description(), dst.Description(), regions ));
		ResolveImage( src.Handle(), dst.Handle(), regions );
	}

	void  RTransferContext::ResolveImage (RmImageID srcImage, RmImageID dstImage, ArrayView<ImageResolve> regions) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::ResolveImage ));

		Msg::CmdBuf_Bake::ResolveImageCmd	cmd;
		cmd.srcImage	= srcImage;
		cmd.dstImage	= dstImage;
		cmd.regions		= regions;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	GenerateMipmaps
=================================================
*/
	void  RTransferContext::GenerateMipmaps (ImageID image, EResourceState state) __Th___
	{
		auto&	img		= _GetResourcesOrThrow( image );
		auto&	desc	= img.Description();

		ImageSubresourceRange	range;
		range.aspectMask	= EPixelFormat_ToImageAspect( desc.format );
		range.baseMipLevel	= 0_mipmap;
		range.baseLayer		= 0_layer;
		range.layerCount	= ushort(desc.arrayLayers.Get());
		range.mipmapCount	= ushort(desc.mipLevels.Get());

		VALIDATE_GCTX( GenerateMipmaps( desc, ArrayView<ImageSubresourceRange>{ &range, 1 }));
		GenerateMipmaps( img.Handle(), state, ArrayView<ImageSubresourceRange>{ &range, 1 });
	}

	void  RTransferContext::GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state) __Th___
	{
		auto&	img = _GetResourcesOrThrow( image );
		VALIDATE_GCTX( GenerateMipmaps( img.Description(), ranges ));
		GenerateMipmaps( img.Handle(), state, ranges );
	}

	void  RTransferContext::GenerateMipmaps (RmImageID image, EResourceState state, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		Msg::CmdBuf_Bake::GenerateMipmapsCmd	cmd;
		cmd.image	= image;
		cmd.state	= state;
		cmd.ranges	= ranges;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	UpdateHostBuffer
=================================================
*/
	bool  RTransferContext::UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		CHECK_ERR( buf.IsMapped() );

		Msg::CmdBuf_Bake::UpdateHostBufferCmd  cmd;
		cmd.buffer	= buf.Handle();
		cmd.offset	= offset;
		cmd.size	= size;

		void*	dst;
		_cmdbuf->Allocate( OUT dst, OUT cmd.memOffset, size );  // throw
		MemCopy( OUT dst, data, size );

		_cmdbuf->AddCommand( cmd );
		return true;
	}

/*
=================================================
	MapHostBuffer
=================================================
*/
	bool  RTransferContext::MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped) __Th___
	{
		UNTESTED
		return false;
	}

/*
=================================================
	ReadHostBuffer
=================================================
*/
	Promise<ArrayView<ubyte>>  RTransferContext::ReadHostBuffer (BufferID bufferId, Bytes offset, Bytes size) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		CHECK_ERR( buf.IsMapped() );

		void*	ptr = buf.GetHostPtr();

		Msg::CmdBuf_Bake::ReadHostBufferCmd  cmd;
		cmd.buffer	= buf.Handle();
		cmd.offset	= offset;
		cmd.size	= size;
		cmd.hostPtr	= BitCast<RmHostPtr>( ptr );
		_cmdbuf->AddCommand( cmd );

		ArrayView<ubyte>	mem_view = ArrayView<ubyte>{ Cast<ubyte>(ptr + offset), usize(size) };

		return Threading::MakePromiseFromValue(	mem_view,
												Tuple{ this->_mngr.GetBatchRC() },
												"RTransferContext::ReadHostBuffer",
												ETaskQueue::PerFrame
											   );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
