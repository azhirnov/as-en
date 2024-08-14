// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/REnumCast.h"
# include "graphics/Remote/Commands/RBaseContext.h"
# include "graphics/Remote/Commands/RAccumBarriers.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Transfer Context implementation
	//

	class RTransferContext final : public Graphics::_hidden_::RBaseContext, public ITransferContext
	{
	// types
	private:
		using AccumBar		= Graphics::_hidden_::RAccumBarriers< RTransferContext >;
		using DeferredBar	= Graphics::_hidden_::RAccumDeferredBarriersForCtx< RTransferContext >;
		using Validator_t	= Graphics::_hidden_::TransferContextValidation;


	// methods
	public:
		explicit RTransferContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)		__Th___;

		RTransferContext ()																							= delete;
		RTransferContext (const RTransferContext &)																	= delete;

		void  ClearColorImage (RmImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;
		void  ClearColorImage (RmImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;
		void  ClearColorImage (RmImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;
		void  ClearDepthStencilImage (RmImageID image, const DepthStencil &, ArrayView<ImageSubresourceRange>)		__Th___;

		void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;
		void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;
		void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;
		void  ClearDepthStencilImage (ImageID image, const DepthStencil &, ArrayView<ImageSubresourceRange>)		__Th___;

		void  UpdateBuffer (RmBufferID buffer, Bytes offset, Bytes size, const void* data)							__Th___;
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)							__Th_OV;

		void  FillBuffer (RmBufferID buffer, Bytes offset, Bytes size, uint data)									__Th___;
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)										__Th_OV;

		void  CopyBuffer (RmBufferID srcBuffer, RmBufferID dstBuffer, ArrayView<BufferCopy> ranges)					__Th___;
		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)						__Th_OV;

		void  CopyImage (RmImageID srcImage, RmImageID dstImage, ArrayView<ImageCopy> ranges)						__Th___;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)							__Th_OV;

		void  CopyBufferToImage (RmBufferID srcBuffer, RmImageID dstImage, ArrayView<BufferImageCopy> ranges)		__Th___;
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)			__Th_OV;

		void  CopyImageToBuffer (RmImageID srcImage, RmBufferID dstBuffer, ArrayView<BufferImageCopy> ranges)		__Th___;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)			__Th_OV;

		void  CopyBufferToImage (RmBufferID srcBuffer, RmImageID dstImage, ArrayView<BufferImageCopy2> ranges)		__Th___;
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)			__Th_OV;

		void  CopyImageToBuffer (RmImageID srcImage, RmBufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)		__Th___;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)			__Th_OV;

		void  BlitImage (RmImageID srcImage, RmImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)	__Th___;
		void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)		__Th_OV;

		void  ResolveImage (RmImageID srcImage, RmImageID dstImage, ArrayView<ImageResolve> regions)				__Th___;
		void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)					__Th___;

		void  UploadBuffer (BufferID buffer, const UploadBufferDesc &desc, OUT BufferMemView &memView)				__Th_OV;
		void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)					__Th_OV	{ _UploadImage( image, desc, OUT memView ); }
		void  UploadImage  (VideoImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)				__Th_OV	{ _UploadImage( image, desc, OUT memView ); }

		void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView)										__Th_OV;
		void  UploadImage (ImageStream &stream, OUT ImageMemView &memView)											__Th_OV	{ _UploadImage( stream, OUT memView ); }
		void  UploadImage (VideoImageStream &stream, OUT ImageMemView &memView)										__Th_OV	{ _UploadImage( stream, OUT memView ); }

		ReadbackBufferResult2	ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &desc)					__Th_OV;
		ReadbackImageResult2	ReadbackImage (ImageID image, const ReadbackImageDesc &desc)						__Th_OV	{ return _ReadbackImage( image, desc ); }
		ReadbackImageResult2	ReadbackImage (VideoImageID image, const ReadbackImageDesc &desc)					__Th_OV	{ return _ReadbackImage( image, desc ); }

		ReadbackBufferResult	ReadbackBuffer (INOUT BufferStream &stream)											__Th_OV;
		ReadbackImageResult		ReadbackImage (INOUT ImageStream &stream)											__Th_OV	{ return _ReadbackImage( stream ); }
		ReadbackImageResult		ReadbackImage (INOUT VideoImageStream &stream)										__Th_OV	{ return _ReadbackImage( stream ); }

		bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)						__Th_OV;
		bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped)					__Th_OV;

		Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)						__Th_OV;

		void  GenerateMipmaps (ImageID image, EResourceState state)													__Th_OV;
		void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state)		__Th_OV;
		void  GenerateMipmaps (RmImageID image, EResourceState state, ArrayView<ImageSubresourceRange> ranges)		__Th___;

		using ITransferContext::ClearColorImage;
		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;
		using ITransferContext::UpdateBuffer;

		uint3  MinImageTransferGranularity ()																		C_NE_OF	{ return uint3{this->_mngr.GetQueue()->minImageTransferGranularity}; }

		ND_ RmCommandBufferID	EndCommandBuffer ()																	__Th___	{ return _EndCommandBuffer( ECtxType::Transfer ); }
		ND_ CmdBuf_t			ReleaseCommandBuffer ()																__Th___	{ return _ReleaseCommandBuffer( ECtxType::Transfer ); }

		RBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ID>
		void  _UploadImage (ID image, const UploadImageDesc &desc, OUT ImageMemView &memView)						__Th___;

		template <typename StreamType>
		void  _UploadImage (INOUT StreamType &stream, OUT ImageMemView &memView)									__Th___;

		template <typename ID>
		ND_ ReadbackImageResult2  _ReadbackImage (ID image, const ReadbackImageDesc &desc)							__Th___;

		template <typename StreamType>
		ND_ ReadbackImageResult  _ReadbackImage (INOUT StreamType &stream)											__Th___;
	};


} // AE::Graphics

# include "graphics/Private/TransferContextImpl.h"

#endif // AE_ENABLE_REMOTE_GRAPHICS
