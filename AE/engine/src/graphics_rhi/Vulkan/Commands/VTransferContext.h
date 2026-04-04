// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TransferCtx --> DirectTransferCtx   --> BarrierMngr --> Vulkan device
				\-> IndirectTransferCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Transfer Context implementation
	//

	class _VDirectTransferCtx : public VBaseDirectContext
	{
	private:
		using Validator_t	= TransferContextValidation;

	// methods
	public:
		void  ClearColorImage (VkImage image, const VkClearColorValue &, ArrayView<VkImageSubresourceRange>)				__Th___;
		void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &, ArrayView<VkImageSubresourceRange>)	__Th___;

		void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)												__Th___;
		void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)									__Th___;

		void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)							__Th___;
		void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)									__Th___;

		void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)					__Th___;
		void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)					__Th___;

		void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)				__Th___;
		void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)							__Th___;

		void  GenerateMipmaps (VkImage image, const uint3 &, ArrayView<ImageSubresourceRange> ranges, EResourceState)		__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()																				__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																			__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VDirectTransferCtx (RenderCoroRef task, VCommandBuffer cmdbuf, DebugLabel dbg)										__Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Transfer } {}
	};



	//
	// Vulkan Indirect Transfer Context implementation
	//

	class _VIndirectTransferCtx : public VBaseIndirectContext
	{
	private:
		using Validator_t	= TransferContextValidation;

	// methods
	public:
		void  ClearColorImage (VkImage image, const VkClearColorValue &, ArrayView<VkImageSubresourceRange>)				__Th___;
		void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &, ArrayView<VkImageSubresourceRange>)	__Th___;

		void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)												__Th___;
		void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)									__Th___;

		void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)							__Th___;
		void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)									__Th___;

		void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)					__Th___;
		void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)					__Th___;

		void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)				__Th___;
		void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)							__Th___;

		void  GenerateMipmaps (VkImage image, const uint3 &, ArrayView<ImageSubresourceRange> ranges, EResourceState)		__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()																			__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																		__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VIndirectTransferCtx (RenderCoroRef task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)								__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Transfer } {}
	};



	//
	// Vulkan Transfer Context implementation
	//

	template <typename CtxImpl>
	class _VTransferContextImpl final : public CtxImpl, public ITransferContextVk
	{
	// types
	public:
		using CmdBuf_t			= typename CtxImpl::CmdBuf_t;
		using RenderCoroRef		= typename CtxImpl::RenderCoroRef;
	private:
		static constexpr uint	_LocalArraySize			= 16;

		using RawCtx			= CtxImpl;
		using AccumBar			= AccumBarriers< _VTransferContextImpl< CtxImpl >>;
		using DeferredBar		= AccumDeferredBarriersForCtx< _VTransferContextImpl< CtxImpl >>;
		using Validator_t		= TransferContextValidation;


	// methods
	public:
		explicit _VTransferContextImpl (RenderCoroRef, CmdBuf_t = Default, DebugLabel = Default)					__Th___;

		_VTransferContextImpl ()																					= delete;
		_VTransferContextImpl (const _VTransferContextImpl &)														= delete;

		using RawCtx::ClearColorImage;
		using RawCtx::ClearDepthStencilImage;

		void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)		__Th_OV	{ _ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)		__Th_OV	{ _ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)		__Th_OV	{ _ClearColorImage( image, color, ranges ); }
		void  ClearDepthStencilImage (ImageID image, const DepthStencil &, ArrayView<ImageSubresourceRange> ranges)	__Th_OV;

		using RawCtx::UpdateBuffer;
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)							__Th_OV;

		using RawCtx::FillBuffer;
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)										__Th_OV;

		using RawCtx::CopyBuffer;
		using RawCtx::CopyImage;

		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)						__Th_OV;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)							__Th_OV;

		using RawCtx::CopyBufferToImage;
		using RawCtx::CopyImageToBuffer;

		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)			__Th_OV;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)			__Th_OV;

		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)			__Th_OV;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)			__Th_OV;

		using RawCtx::BlitImage;

		void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)		__Th_OV;

		void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)					__Th_OV;

		void  UploadBuffer (BufferID buffer, const UploadBufferDesc &desc, OUT BufferMemView &memView)				__Th_OV;
		void  UploadImage  (ImageID image, const UploadImageDesc &desc, Bytes maxSize, OUT ImageMemView &memView)	__Th_OV	{ _UploadImage( image, desc, maxSize, OUT memView ); }
		void  UploadImage  (VideoImageID id, const UploadImageDesc &desc, Bytes maxSize, OUT ImageMemView &memView)	__Th_OV	{ _UploadImage( id, desc, maxSize, OUT memView ); }

		void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView)										__Th_OV;
		void  UploadImage (ImageStream &stream, OUT ImageMemView &memView)											__Th_OV	{ _UploadImage( stream, OUT memView ); }
		void  UploadImage (VideoImageStream &stream, OUT ImageMemView &memView)										__Th_OV	{ _UploadImage( stream, OUT memView ); }

		ReadbackBufferResult2	ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &desc)					__Th_OV;
		ReadbackImageResult2	ReadbackImage (ImageID image, const ReadbackImageDesc &desc)						__Th_OV	{ return _ReadbackImage( image, desc ); }
		ReadbackImageResult2	ReadbackImage (VideoImageID image, const ReadbackImageDesc &desc)					__Th_OV	{ return _ReadbackImage( image, desc ); }

		ReadbackBufferResult	ReadbackBuffer (INOUT BufferStream &stream)											__Th_OV;
		ReadbackImageResult		ReadbackImage (INOUT ImageStream &stream)											__Th_OV	{ return _ReadbackImage( stream ); }
		ReadbackImageResult		ReadbackImage (INOUT VideoImageStream &stream)										__Th_OV	{ return _ReadbackImage( stream ); }

		bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped)					__Th_OV;
		bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)						__Th_OV;

		ReadHostBufferResult	ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)							__Th_OV;

		using RawCtx::GenerateMipmaps;

		void  GenerateMipmaps (ImageID image, EResourceState state)													__Th_OV;
		void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state)		__Th_OV;

		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;
		using ITransferContext::UpdateBuffer;

		uint3  MinImageTransferGranularity ()																		C_NE_OF;

		VBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ID>
		void  _UploadImage (ID image, const UploadImageDesc &desc, Bytes maxSize, OUT ImageMemView &memView)		__Th___;

		template <typename StreamType>
		void  _UploadImage (INOUT StreamType &stream, OUT ImageMemView &memView)									__Th___;

		template <typename ID>
		ND_ ReadbackImageResult2  _ReadbackImage (ID image, const ReadbackImageDesc &desc)							__Th___;

		template <typename StreamType>
		ND_ ReadbackImageResult  _ReadbackImage (INOUT StreamType &stream)											__Th___;

		template <typename ColType>
		void  _ClearColorImage (ImageID image, const ColType &color, ArrayView<ImageSubresourceRange> ranges)		__Th___;

		static void  _ConvertImageSubresourceRange (OUT VkImageSubresourceRange &dst, const ImageSubresourceRange& src, const ImageDesc &desc)			__NE___;
		static void  _ConvertImageSubresourceLayer (OUT VkImageSubresourceLayers &dst, const ImageSubresourceLayers &src, const ImageDesc &desc)		__NE___;
		static void  _ConvertBufferImageCopy (OUT VkBufferImageCopy &dst, const BufferImageCopy& src, const ImageDesc &desc)							__NE___;
		static void  _ConvertBufferImageCopy (OUT VkBufferImageCopy &dst, const BufferImageCopy2& src, const ImageDesc &desc, const PixelFormatInfo &)	__Th___;

		ND_ bool  _MapHostBuffer (BufferID bufferId, INOUT Bytes &offset, INOUT Bytes &size, OUT VulkanMemoryObjInfo &memInfo)							__Th___;
	};

    extern template class _VTransferContextImpl< _VDirectTransferCtx >;
    extern template class _VTransferContextImpl< _VIndirectTransferCtx >;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectTransferContext	= Graphics::_hidden_::_VTransferContextImpl< Graphics::_hidden_::_VDirectTransferCtx >;
	using VIndirectTransferContext	= Graphics::_hidden_::_VTransferContextImpl< Graphics::_hidden_::_VIndirectTransferCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
