// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TransferCtx --> DirectTransferCtx   --> BarrierMngr --> Vulkan device 
				\-> IndirectTransferCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Transfer Context implementation
	//
	
	class _VDirectTransferCtx : public VBaseDirectContext
	{
	// methods
	public:
		void  ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges);
		void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges);

		void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);
		void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data);

		void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges);
		void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges);

		void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges);
		void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges);
		
		void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions);

		void  GenerateMipmaps (VkImage image, const uint3 &dimension, uint levelCount, uint layerCount, EImageAspect aspect);
		
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectTransferCtx (Ptr<VCommandBatch> batch) : VBaseDirectContext{ batch } {}
		_VDirectTransferCtx (Ptr<VCommandBatch> batch, VCommandBuffer cmdbuf) : VBaseDirectContext{ batch, RVRef(cmdbuf) } {}
	};



	//
	// Vulkan Indirect Transfer Context implementation
	//
	
	class _VIndirectTransferCtx : public VBaseIndirectContext
	{
	// methods
	public:
		void  ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges);
		void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges);

		void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);
		void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data);

		void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges);
		void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges);

		void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges);
		void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges);
		
		void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions);
		
		void  GenerateMipmaps (VkImage image, const uint3 &dimension, uint levelCount, uint layerCount, EImageAspect aspect);

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectTransferCtx (Ptr<VCommandBatch> batch) : VBaseIndirectContext{ batch } {}
		_VIndirectTransferCtx (Ptr<VCommandBatch> batch, VSoftwareCmdBufPtr cmdbuf) : VBaseIndirectContext{ batch, RVRef(cmdbuf) } {}
	};


	
	//
	// Vulkan Transfer Context implementation
	//

	template <typename CtxImpl>
	class _VTransferContextImpl : public CtxImpl, public ITransferContext
	{
	// types
	public:
		static constexpr bool	IsTransferContext		= true;
		static constexpr bool	IsVulkanTransferContext	= true;
	private:
		static constexpr uint	_LocalArraySize			= 16;
		static constexpr Bytes	_StagingBufOffsetAlign	= 4_b;

		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumBarriers< _VTransferContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _VTransferContextImpl (Ptr<VCommandBatch> batch) : RawCtx{ batch } {}
		
		template <typename RawCmdBufType>
		_VTransferContextImpl (Ptr<VCommandBatch> batch, RawCmdBufType cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_VTransferContextImpl () = delete;
		_VTransferContextImpl (const _VTransferContextImpl &) = delete;
		
		using RawCtx::ClearColorImage;
		using RawCtx::ClearDepthStencilImage;
		
		void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)	{ _ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)	{ _ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)	{ _ClearColorImage( image, color, ranges ); }
		void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges);

		using RawCtx::UpdateBuffer;
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data);

		template <typename T>	void  UpdateBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)			{ return UpdateBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	void  UpdateBuffer (BufferID buffer, Bytes offset, const Array<T> &data)		{ return UpdateBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }

		using RawCtx::FillBuffer;
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data) override final;

		using RawCtx::CopyBuffer;
		using RawCtx::CopyImage;

		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) override final;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) override final;

		using RawCtx::CopyBufferToImage;
		using RawCtx::CopyImageToBuffer;

		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) override final;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) override final;
		
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) override final;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) override final;
		
		using RawCtx::BlitImage;

		void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions);

		void  UploadBuffer (BufferID buffer, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) override final;
		void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView) override final;

		void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) override final;
		void  UploadImage (ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) override final;

		ND_ Promise<BufferMemView>	ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static) override final;
		ND_ Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc) override final;
		
		ND_ bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped) override final;
		ND_ bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) override final;

		ND_ Promise<BufferMemView>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size) override final;
		
		using RawCtx::GenerateMipmaps;

		void  GenerateMipmaps (ImageID image) override final;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;

		ND_ AccumBar  AccumBarriers ()							{ return AccumBar{ *this }; }

		VBARRIERMNGR_INHERIT_BARRIERS

	protected:
		template <typename ColType>
		void  _ClearColorImage (ImageID image, const ColType &color, ArrayView<ImageSubresourceRange> ranges);

		ND_ bool  _IsHostMemory (BufferID buffer);
		ND_ bool  _IsHostMemory (ImageID image);
		
		ND_ bool  _IsDeviceMemory (BufferID buffer);
		ND_ bool  _IsDeviceMemory (ImageID image);

		static void  _ConvertImageSubresourceRange (OUT VkImageSubresourceRange& dst, const ImageSubresourceRange& src, const ImageDesc &desc);
		static void  _ConvertImageSubresourceLayer (OUT VkImageSubresourceLayers &dst, const ImageSubresourceLayers &src, const ImageDesc &desc);
		static void  _ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy& src, const ImageDesc &desc);
		static void  _ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy2& src, const ImageDesc &desc, const PixelFormatInfo &fmtInfo);
		
		ND_ bool  _MapHostBuffer (BufferID bufferId, INOUT Bytes &offset, INOUT Bytes &size, OUT VulkanMemoryObjInfo &memInfo);
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectTransferContext	= _hidden_::_VTransferContextImpl< _hidden_::_VDirectTransferCtx >;
	using VIndirectTransferContext	= _hidden_::_VTransferContextImpl< _hidden_::_VIndirectTransferCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
