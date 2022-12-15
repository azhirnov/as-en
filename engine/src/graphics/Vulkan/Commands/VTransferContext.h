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
		void  ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges)					__Th___;
		void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges)__Th___;

		void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)																__Th___;
		void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)													__Th___;

		void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)											__Th___;
		void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)													__Th___;

		void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)									__Th___;
		void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)									__Th___;
		
		void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)								__Th___;
		void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)											__Th___;

		void  GenerateMipmaps (VkImage image, const uint3 &dimension, uint levelCount, uint layerCount, EImageAspect aspect)				__Th___;
		
		ND_ VkCommandBuffer	EndCommandBuffer ()																								__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																							__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectTransferCtx (const RenderTask &task)																				__Th___;
		_VDirectTransferCtx (const RenderTask &task, VCommandBuffer cmdbuf)																	__Th___;
	};



	//
	// Vulkan Indirect Transfer Context implementation
	//
	
	class _VIndirectTransferCtx : public VBaseIndirectContext
	{
	// methods
	public:
		void  ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges)					__Th___;
		void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges)__Th___;

		void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)																__Th___;
		void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)													__Th___;

		void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)											__Th___;
		void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)													__Th___;

		void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)									__Th___;
		void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)									__Th___;
		
		void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)								__Th___;
		void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)											__Th___;
		
		void  GenerateMipmaps (VkImage image, const uint3 &dimension, uint levelCount, uint layerCount, EImageAspect aspect)				__Th___;
		
		ND_ VBakedCommands		EndCommandBuffer ()																							__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																						__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectTransferCtx (const RenderTask &task)																				__Th___;
		_VIndirectTransferCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf)															__Th___;
	};


	
	//
	// Vulkan Transfer Context implementation
	//

	template <typename CtxImpl>
	class _VTransferContextImpl final : public CtxImpl, public ITransferContext
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
		explicit _VTransferContextImpl (const RenderTask &task);
		
		template <typename RawCmdBufType>
		_VTransferContextImpl (const RenderTask &task, RawCmdBufType cmdbuf);

		_VTransferContextImpl () = delete;
		_VTransferContextImpl (const _VTransferContextImpl &) = delete;
		
		using RawCtx::ClearColorImage;
		using RawCtx::ClearDepthStencilImage;
		
		void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)											__Th___	{ _ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)											__Th___	{ _ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)											__Th___	{ _ClearColorImage( image, color, ranges ); }
		void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges)							__Th___;

		using RawCtx::UpdateBuffer;
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)																__Th_OV;

		template <typename T>	void  UpdateBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)													__Th___	{ return UpdateBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	void  UpdateBuffer (BufferID buffer, Bytes offset, const Array<T> &data)												__Th___	{ return UpdateBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }

		using RawCtx::FillBuffer;
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)																			__Th_OV;

		using RawCtx::CopyBuffer;
		using RawCtx::CopyImage;

		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)															__Th_OV;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)																__Th_OV;

		using RawCtx::CopyBufferToImage;
		using RawCtx::CopyImageToBuffer;

		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)												__Th_OV;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)												__Th_OV;
		
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)												__Th_OV;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)												__Th_OV;
		
		using RawCtx::BlitImage;

		void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)											__Th___;
		
		void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)														__Th___;

		void  UploadBuffer (BufferID buffer, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)__Th_OV;
		void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)														__Th_OV;

		void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)						__Th_OV;
		void  UploadImage (ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)						__Th_OV;

		ND_ Promise<BufferMemView>	ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static)	__Th_OV;
		ND_ Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)														__Th_OV;
		
		ND_ bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped);
		ND_ bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)														__Th_OV;

		ND_ Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)														__Th_OV;
		
		using RawCtx::GenerateMipmaps;

		void  GenerateMipmaps (ImageID image)																											__Th_OV;

		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;
		
		uint3  MinImageTransferGranularity ()																											C_NE_OF;

		VBARRIERMNGR_INHERIT_BARRIERS

	private:
		template <typename ColType>
		void  _ClearColorImage (ImageID image, const ColType &color, ArrayView<ImageSubresourceRange> ranges);

		ND_ bool  _IsHostMemory (const VBuffer &buffer);
		ND_ bool  _IsHostMemory (const VImage &image);
		
		ND_ bool  _IsDeviceMemory (const VBuffer &buffer);
		ND_ bool  _IsDeviceMemory (const VImage &image);

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
	using VDirectTransferContext	= Graphics::_hidden_::_VTransferContextImpl< Graphics::_hidden_::_VDirectTransferCtx >;
	using VIndirectTransferContext	= Graphics::_hidden_::_VTransferContextImpl< Graphics::_hidden_::_VIndirectTransferCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VTransferContextImpl<C>::_VTransferContextImpl (const RenderTask &task) : RawCtx{ task }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer, task.GetQueueMask() ));
	}
		
	template <typename C>
	template <typename RawCmdBufType>
	_VTransferContextImpl<C>::_VTransferContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute | EQueueMask::AsyncTransfer, task.GetQueueMask() ));
	}

/*
=================================================
	_IsHostMemory
=================================================
*/
	template <typename C>
	bool  _VTransferContextImpl<C>::_IsHostMemory (const VBuffer &buffer)
	{
		return AnyBits( buffer.Description().memType, EMemoryType::HostCachedCocherent );
	}
	
	template <typename C>
	bool  _VTransferContextImpl<C>::_IsHostMemory (const VImage &image)
	{
		return AnyBits( image.Description().memType, EMemoryType::HostCachedCocherent );
	}
	
/*
=================================================
	_IsDeviceMemory
=================================================
*/
	template <typename C>
	bool  _VTransferContextImpl<C>::_IsDeviceMemory (const VBuffer &buffer)
	{
		return AnyBits( buffer.Description().memType, EMemoryType::DeviceLocal );
	}
	
	template <typename C>
	bool  _VTransferContextImpl<C>::_IsDeviceMemory (const VImage &image)
	{
		return AnyBits( image.Description().memType, EMemoryType::DeviceLocal );
	}

/*
=================================================
	_ClearColorImage
=================================================
*/
	template <typename C>
	template <typename ColType>
	void  _VTransferContextImpl<C>::_ClearColorImage (ImageID imageId, const ColType &color, ArrayView<ImageSubresourceRange> ranges)
	{
		auto&	img = _GetResourcesOrThrow( imageId );
		ASSERT( ranges.size() );

		VkClearColorValue										clear_value;
		FixedArray<VkImageSubresourceRange, _LocalArraySize>	vk_ranges;
		const ImageDesc &										desc	= img.Description();

		MemCopy( clear_value, color );

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			_ConvertImageSubresourceRange( OUT dst, src, desc );

			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::ClearDepthStencilImage (ImageID imageId, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges)
	{
		auto&	img = _GetResourcesOrThrow( imageId );
		ASSERT( ranges.size() );

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

			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::FillBuffer (BufferID bufferId, Bytes offset, Bytes size, uint data)
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		ASSERT( offset < buf.Size() );
		ASSERT( size == UMax or (offset + size <= buf.Size()) );

		offset	= Min( offset, buf.Size()-1 );
		size	= Min( size, buf.Size() - offset );

		RawCtx::FillBuffer( buf.Handle(), offset, size, data );
	}

/*
=================================================
	UpdateBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::UpdateBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		ASSERT( size > 0_b );
		ASSERT( data != null );

		RawCtx::UpdateBuffer( buf.Handle(), offset, size, data );
	}
	
/*
=================================================
	UploadBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::UploadBuffer (BufferID bufferId, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType)
	{
		auto&	dst_buf = _GetResourcesOrThrow( bufferId );
		ASSERT( _IsDeviceMemory( dst_buf ));
		
		ASSERT( offset < dst_buf.Size() );
		ASSERT( size == UMax or (offset + size <= dst_buf.Size()) );
		ASSERT( memView.Empty() );
		
		offset	= Min( offset, dst_buf.Size() );
		size	= Min( size, dst_buf.Size() - offset );

		VStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		VStagingBufferManager::BufferRanges_t	buffers;

		sbm.GetBufferRanges( OUT buffers, size, 0_b, _StagingBufOffsetAlign, this->_mngr.GetFrameId(), heapType, this->_mngr.GetQueueType(), True{"uload"} );

		for (auto& src_buf : buffers)
		{
			memView.PushBack( src_buf.mapped, src_buf.size );
			CopyBuffer( src_buf.buffer, dst_buf.Handle(), {VkBufferCopy{ VkDeviceSize(src_buf.bufferOffset), VkDeviceSize(offset), VkDeviceSize(src_buf.size) }});
			offset += src_buf.size;
		}
		ASSERT( buffers.size() == memView.Parts().size() );
	}
	
/*
=================================================
	UploadBuffer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::UploadBuffer (BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType)
	{
		ASSERT( not stream.IsCompleted() );

		auto&	dst_buf = _GetResourcesOrThrow( stream.Buffer() );
		ASSERT( _IsDeviceMemory( dst_buf ));
		ASSERT( stream.End() <= dst_buf.Size() );
		ASSERT( memView.Empty() );
		
		VStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		VStagingBufferManager::BufferRanges_t	buffers;

		sbm.GetBufferRanges( OUT buffers, stream.RemainSize(), 0_b, _StagingBufOffsetAlign, this->_mngr.GetFrameId(), heapType, this->_mngr.GetQueueType(), True{"uload"} );
		
		for (auto& src_buf : buffers)
		{
			memView.PushBack( src_buf.mapped, src_buf.size );
			CopyBuffer( src_buf.buffer, dst_buf.Handle(), {VkBufferCopy{ VkDeviceSize(src_buf.bufferOffset), VkDeviceSize(stream.OffsetAndPos()), VkDeviceSize(src_buf.size) }});
			stream.pos += src_buf.size;
			ASSERT( stream.pos <= stream.DataSize() );
		}
		ASSERT( buffers.size() == memView.Parts().size() );
	}

/*
=================================================
	UploadImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::UploadImage (ImageID imageId, const UploadImageDesc &uploadDesc, OUT ImageMemView &memView)
	{
		auto&	dst_img = _GetResourcesOrThrow( imageId );
		ASSERT( _IsDeviceMemory( dst_img ));

		const ImageDesc&		img_desc	= dst_img.Description();
		VStagingBufferManager&	sbm			= this->_mngr.GetStagingManager();

		VStagingBufferManager::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, uploadDesc, img_desc, this->_mngr.GetFrameId(), this->_mngr.GetQueueType(), True{"upload"} );
		
		if_unlikely( res.buffers.empty() )
			return;

		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask		= VEnumCast( uploadDesc.aspectMask );
		subres.mipLevel			= uploadDesc.mipLevel.Get();
		subres.baseArrayLayer	= uploadDesc.arrayLayer.Get();
		subres.layerCount		= 1;
		copy.bufferRowLength	= res.bufferRowLength;

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& src_buf : res.buffers)
		{
			mem_view.PushBack( src_buf.mapped, src_buf.size );
			copy.bufferOffset		= VkDeviceSize(src_buf.bufferOffset);
			copy.bufferImageHeight	= src_buf.bufferImageHeight;
			copy.imageOffset		= {int(src_buf.imageOffset.x), int(src_buf.imageOffset.y), int(src_buf.imageOffset.z)};
			copy.imageExtent		= {    src_buf.imageSize.x,        src_buf.imageSize.y,        src_buf.imageSize.z   };

			min = Min( min, src_buf.imageOffset );
			max = Max( max, src_buf.imageOffset + src_buf.imageSize );

			CopyBufferToImage( src_buf.buffer, dst_img.Handle(), {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );
		
		memView = ImageMemView{ mem_view, min, max - min, res.dataRowPitch, res.dataSlicePitch, img_desc.format, uploadDesc.aspectMask };
	}
	
/*
=================================================
	UploadImage
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::UploadImage (ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType)
	{
		ASSERT( not stream.IsCompleted() );

		auto&	dst_img = _GetResourcesOrThrow( stream.Image() );
		ASSERT( _IsDeviceMemory( dst_img ));

		const ImageDesc&		img_desc	= dst_img.Description();
		VStagingBufferManager&	sbm			= this->_mngr.GetStagingManager();

		ASSERT( All( stream.End() <= img_desc.dimension ));
		
		UploadImageDesc	upload_desc = stream.ToUploadDesc();
		upload_desc.imageOffset	+= uint3{ 0, stream.posYZ };
		upload_desc.imageSize	-= uint3{ 0, stream.posYZ };
		upload_desc.heapType	 = heapType;

		VStagingBufferManager::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, upload_desc, img_desc, this->_mngr.GetFrameId(), this->_mngr.GetQueueType(), True{"upload"} );
		
		if_unlikely( res.buffers.empty() )
			return;

		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask		= VEnumCast( upload_desc.aspectMask );
		subres.mipLevel			= upload_desc.mipLevel.Get();
		subres.baseArrayLayer	= upload_desc.arrayLayer.Get();
		subres.layerCount		= 1;
		copy.bufferRowLength	= res.bufferRowLength;

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& src_buf : res.buffers)
		{
			mem_view.PushBack( src_buf.mapped, src_buf.size );
			copy.bufferOffset		= VkDeviceSize(src_buf.bufferOffset);
			copy.bufferImageHeight	= src_buf.bufferImageHeight;
			copy.imageOffset		= {int(src_buf.imageOffset.x), int(src_buf.imageOffset.y), int(src_buf.imageOffset.z)};
			copy.imageExtent		= {    src_buf.imageSize.x,        src_buf.imageSize.y,        src_buf.imageSize.z   };

			min = Min( min, src_buf.imageOffset );
			max = Max( max, src_buf.imageOffset + src_buf.imageSize );

			ASSERT( All( min >= stream.Begin() ));
			ASSERT( All( max <= stream.End() ));

			CopyBufferToImage( src_buf.buffer, dst_img.Handle(), {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );
		
		stream.posYZ[0] = max.y - stream.Begin().y;
		stream.posYZ[1] = max.z - stream.Begin().z - 1;

		if_unlikely( stream.posYZ[0] >= stream.RegionSize().y )
		{
			stream.posYZ[0] = 0;
			stream.posYZ[1] ++;
		}

		memView = ImageMemView{ mem_view, min, max - min, res.dataRowPitch,
								((max.z - min.z > 1) ? res.dataSlicePitch : 0_b),
								img_desc.format, upload_desc.aspectMask };
	}

/*
=================================================
	ReadbackBuffer
=================================================
*/
	template <typename C>
	Promise<BufferMemView>  _VTransferContextImpl<C>::ReadbackBuffer (BufferID bufferId, Bytes offset, Bytes size, EStagingHeapType heapType)
	{
		auto&	src_buf = _GetResourcesOrThrow( bufferId );
		ASSERT( _IsDeviceMemory( src_buf ));

		ASSERT( offset < src_buf.Size() );
		ASSERT( size == UMax or (offset + size <= src_buf.Size()) );
		
		offset	= Min( offset, src_buf.Size() );
		size	= Min( size, src_buf.Size() - offset );
		
		VStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		VStagingBufferManager::BufferRanges_t	buffers;
		sbm.GetBufferRanges( OUT buffers, size, 0_b, _StagingBufOffsetAlign, this->_mngr.GetFrameId(), heapType, this->_mngr.GetQueueType(), False{"readback"} );
		
		BufferMemView	mem_view;
		for (auto& dst_buf : buffers)
		{
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
			CopyBuffer( src_buf.Handle(), dst_buf.buffer, {VkBufferCopy{ VkDeviceSize(offset), VkDeviceSize(dst_buf.bufferOffset), VkDeviceSize(dst_buf.size) }});
			offset += dst_buf.size;
		}
		ASSERT( buffers.size() == mem_view.Parts().size() );

		return Threading::MakePromiseFromValue( mem_view, Tuple{ this->_mngr.GetBatchRC() });
	}
	
/*
=================================================
	ReadHostBuffer
=================================================
*/
	template <typename C>
	Promise<ArrayView<ubyte>>  _VTransferContextImpl<C>::ReadHostBuffer (BufferID bufferId, Bytes offset, Bytes size)
	{
		VulkanMemoryObjInfo	mem_info;
		CHECK_ERR( _MapHostBuffer( bufferId, INOUT offset, INOUT size, OUT mem_info ));
		
		ArrayView<ubyte>	mem_view = ArrayView<ubyte>{ Cast<ubyte>(mem_info.mappedPtr + offset), usize(size) };

		if_unlikely( not AllBits( mem_info.flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
		{
			ASSERT( offset + size <= mem_info.size );
			this->_mngr.GetStagingManager().AcquireMappedMemory( this->_mngr.GetFrameId(), mem_info.memory, mem_info.offset, size );
		}

		return Threading::MakePromiseFromValue( mem_view, Tuple{ this->_mngr.GetBatchRC() });
	}
	
/*
=================================================
	ReadbackImage
=================================================
*/
	template <typename C>
	Promise<ImageMemView>   _VTransferContextImpl<C>::ReadbackImage (ImageID imageId, const ReadbackImageDesc &readDesc)
	{
		auto&	src_img = _GetResourcesOrThrow( imageId );
		ASSERT( _IsDeviceMemory( src_img ));
		
		const ImageDesc&		img_desc	= src_img.Description();
		VStagingBufferManager&	sbm			= this->_mngr.GetStagingManager();

		VStagingBufferManager::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, readDesc, img_desc, this->_mngr.GetFrameId(), this->_mngr.GetQueueType(), False{"readback"} );
		
		if_unlikely( res.buffers.empty() )
			return Default;

		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask		= VEnumCast( readDesc.aspectMask );
		subres.mipLevel			= readDesc.mipLevel.Get();
		subres.baseArrayLayer	= readDesc.arrayLayer.Get();
		subres.layerCount		= 1;
		copy.bufferRowLength	= res.bufferRowLength;

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& dst_buf : res.buffers)
		{
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
			copy.bufferOffset		= VkDeviceSize(dst_buf.bufferOffset);
			copy.bufferImageHeight	= dst_buf.bufferImageHeight;
			copy.imageOffset		= {int(dst_buf.imageOffset.x), int(dst_buf.imageOffset.y), int(dst_buf.imageOffset.z)};
			copy.imageExtent		= {    dst_buf.imageSize.x,        dst_buf.imageSize.y,        dst_buf.imageSize.z   };

			min = Min( min, dst_buf.imageOffset );
			max = Max( max, dst_buf.imageOffset + dst_buf.imageSize );

			CopyImageToBuffer( src_img.Handle(), dst_buf.buffer, {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );
		
		ImageMemView	img_mem_view{ mem_view, min, max - min, res.dataRowPitch, res.dataSlicePitch, img_desc.format, readDesc.aspectMask };
		return Threading::MakePromiseFromValue( img_mem_view, Tuple{ this->_mngr.GetBatchRC() });
	}
	
/*
=================================================
	MapHostBuffer
=================================================
*/
	template <typename C>
	bool  _VTransferContextImpl<C>::MapHostBuffer (BufferID bufferId, Bytes offset, INOUT Bytes &size, OUT void* &mapped)
	{
		VulkanMemoryObjInfo	mem_info;
		bool	res = _MapHostBuffer( bufferId, offset, INOUT size, OUT mem_info );
		mapped = mem_info.mappedPtr + offset;
		return res;
	}
	
	template <typename C>
	bool  _VTransferContextImpl<C>::_MapHostBuffer (BufferID bufferId, INOUT Bytes &offset, INOUT Bytes &size, OUT VulkanMemoryObjInfo &memInfo)
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		ASSERT( _IsHostMemory( buf ));

		auto&	mem = _GetResourcesOrThrow( buf.MemoryID() );
		CHECK_ERR( mem.GetMemoryInfo( OUT memInfo ));
		CHECK_ERR( memInfo.mappedPtr != null );
		
		offset	= Min( offset, buf.Size() );
		size	= Min( size, buf.Size() - offset );

		ASSERT( size > 0 );
		return true;
	}

/*
=================================================
	UpdateHostBuffer
=================================================
*/
	template <typename C>
	bool  _VTransferContextImpl<C>::UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)
	{
		VulkanMemoryObjInfo	mem_info;
		CHECK_ERR( _MapHostBuffer( bufferId, INOUT offset, INOUT size, OUT mem_info ));

		MemCopy( OUT mem_info.mappedPtr + offset, data, size );
		
		if_unlikely( not AllBits( mem_info.flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
		{
			VkMappedMemoryRange	range;
			range.sType		= VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			range.pNext		= null;
			range.memory	= mem_info.memory;
			range.offset	= VkDeviceSize(mem_info.offset + offset);
			range.size		= VkDeviceSize(size);

			ASSERT( offset + size <= mem_info.size );

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
	void  _VTransferContextImpl<C>::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)
	{
		auto  [src_buf, dst_buf] = _GetResourcesOrThrow( srcBuffer, dstBuffer );
		ASSERT( ranges.size() );

		FixedArray<VkBufferCopy, _LocalArraySize>	vk_ranges;
		const Bytes									src_size	= src_buf.Size();
		const Bytes									dst_size	= dst_buf.Size();

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			ASSERT( All( src.srcOffset < src_size ));
			ASSERT( All( src.dstOffset < dst_size ));
			ASSERT( All( (src.srcOffset + src.size) <= src_size ));
			ASSERT( All( (src.dstOffset + src.size) <= dst_size ));

			dst.srcOffset	= VkDeviceSize(Min( src.srcOffset, src_size-1 ));
			dst.dstOffset	= VkDeviceSize(Min( src.dstOffset, dst_size-1 ));
			dst.size		= VkDeviceSize(Min( src.size, src_size - src.srcOffset, dst_size - src.dstOffset ));

			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
		ASSERT( ranges.size() );

		FixedArray<VkImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &							src_desc	= src_img.Description();
		const ImageDesc &							dst_desc	= dst_img.Description();

		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();

			ASSERT( All( src.srcOffset < src_img.Dimension() ));
			ASSERT( All( (src.srcOffset + src.extent) <= src_img.Dimension() ));
			ASSERT( All( src.dstOffset < dst_img.Dimension() ));
			ASSERT( All( (src.dstOffset + src.extent) <= dst_img.Dimension() ));

			dst.srcOffset	= { int(src.srcOffset.x), int(src.srcOffset.y), int(src.srcOffset.z) };
			dst.dstOffset	= { int(src.dstOffset.x), int(src.dstOffset.y), int(src.dstOffset.z) };
			dst.extent		= { src.extent.x,         src.extent.y,         src.extent.z };
			
			_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
			_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );
			
			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)
	{
		auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );
		ASSERT( ranges.size() );

		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= dst_img.Description();
		
		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();
			
			_ConvertBufferImageCopy( OUT dst, src, img_desc );
			
			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)
	{
		auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );
		ASSERT( ranges.size() );

		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= dst_img.Description();
		const PixelFormatInfo &							fmt_info	= EPixelFormat_GetInfo( img_desc.format );
		
		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();
			
			_ConvertBufferImageCopy( OUT dst, src, img_desc, fmt_info );
			
			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)
	{
		auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );
		ASSERT( ranges.size() );
		
		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= src_img.Description();
		
		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();
			
			_ConvertBufferImageCopy( OUT dst, src, img_desc );
			
			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)
	{
		auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );
		ASSERT( ranges.size() );
		
		FixedArray<VkBufferImageCopy, _LocalArraySize>	vk_ranges;
		const ImageDesc &								img_desc	= src_img.Description();
		const PixelFormatInfo &							fmt_info	= EPixelFormat_GetInfo( img_desc.format );
		
		for (usize i = 0; i < ranges.size(); ++i)
		{
			auto&	src = ranges[i];
			auto&	dst = vk_ranges.emplace_back();
			
			_ConvertBufferImageCopy( OUT dst, src, img_desc, fmt_info );
			
			if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
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
	void  _VTransferContextImpl<C>::BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter blitFilter, ArrayView<ImageBlit> regions)
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
		ASSERT( regions.size() );
		
		FixedArray<VkImageBlit, _LocalArraySize>	vk_regions;
		const ImageDesc &							src_desc	= src_img.Description();
		const ImageDesc &							dst_desc	= dst_img.Description();
		const VkFilter								filter		= VEnumCast( blitFilter );

		for (usize i = 0; i < regions.size(); ++i)
		{
			auto&	src = regions[i];
			auto&	dst = vk_regions.emplace_back();

			ASSERT( All( src.srcOffset0 <= src_desc.dimension ));
			ASSERT( All( src.srcOffset1 <= src_desc.dimension ));
			ASSERT( All( src.dstOffset0 <= dst_desc.dimension ));
			ASSERT( All( src.dstOffset1 <= dst_desc.dimension ));

			dst.srcOffsets[0] = { int(src.srcOffset0.x), int(src.srcOffset0.y), int(src.srcOffset0.z) };
			dst.srcOffsets[1] = { int(src.srcOffset1.x), int(src.srcOffset1.y), int(src.srcOffset1.z) };
			dst.dstOffsets[0] = { int(src.dstOffset0.x), int(src.dstOffset0.y), int(src.dstOffset0.z) };
			dst.dstOffsets[1] = { int(src.dstOffset1.x), int(src.dstOffset1.y), int(src.dstOffset1.z) };
			this->_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
			this->_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

			if_unlikely( vk_regions.size() == vk_regions.capacity() )
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
	void  _VTransferContextImpl<C>::ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
		ASSERT( regions.size() );
		
		FixedArray<VkImageResolve, _LocalArraySize>	vk_regions;
		const ImageDesc &							src_desc	= src_img.Description();
		const ImageDesc &							dst_desc	= dst_img.Description();

		for (usize i = 0; i < regions.size(); ++i)
		{
			auto&	src = regions[i];
			auto&	dst = vk_regions.emplace_back();

			ASSERT( All( src.srcOffset < src_desc.dimension ));
			ASSERT( All( src.srcOffset + src.extent <= src_desc.dimension ));
			ASSERT( All( src.dstOffset < dst_desc.dimension ));
			ASSERT( All( src.dstOffset + src.extent <= dst_desc.dimension ));

			dst.srcOffset	= { int(src.srcOffset.x), int(src.srcOffset.y), int(src.srcOffset.z) };
			dst.dstOffset	= { int(src.dstOffset.x), int(src.dstOffset.y), int(src.dstOffset.z) };
			dst.extent		= { src.extent.x,		  src.extent.y,			src.extent.z		 };

			this->_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
			this->_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

			if_unlikely( vk_regions.size() == vk_regions.capacity() )
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
	void  _VTransferContextImpl<C>::GenerateMipmaps (ImageID image)
	{
		auto&				img = _GetResourcesOrThrow( image );
		ImageDesc const&	desc = img.Description();

		RawCtx::GenerateMipmaps( img.Handle(), desc.dimension,
								 desc.maxLevel.Get(), desc.arrayLayers.Get(),
								 EPixelFormat_ToImageAspect( desc.format ));
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
	void  _VTransferContextImpl<C>::_ConvertImageSubresourceRange (OUT VkImageSubresourceRange& dst, const ImageSubresourceRange& src, const ImageDesc &desc)
	{
		ASSERT( src.baseMipLevel < desc.maxLevel );
		ASSERT( src.baseArrayLayer < desc.arrayLayers );

		dst.aspectMask		= VEnumCast( src.aspectMask );
		dst.baseMipLevel	= Min( src.baseMipLevel.Get(), desc.maxLevel.Get()-1 );
		dst.levelCount		= Min( src.levelCount, desc.maxLevel.Get() - src.baseMipLevel.Get() );
		dst.baseArrayLayer	= Min( src.baseArrayLayer.Get(), desc.arrayLayers.Get()-1 );
		dst.layerCount		= Min( src.layerCount, desc.arrayLayers.Get() - src.baseArrayLayer.Get() );
	}
	
/*
=================================================
	_ConvertBufferImageCopy
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::_ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy& src, const ImageDesc &desc)
	{
		ASSERT( All( src.imageOffset < desc.dimension ));
		ASSERT( All( src.imageOffset + src.imageExtent <= desc.dimension ));
		ASSERT( src.bufferRowLength == 0 or src.bufferRowLength >= src.imageExtent.x );
		ASSERT( src.bufferImageHeight == 0 or src.bufferImageHeight >= src.imageExtent.y );

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
	void  _VTransferContextImpl<C>::_ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy2& src, const ImageDesc &desc, const PixelFormatInfo &fmtInfo)
	{
		ASSERT( All( src.imageOffset < desc.dimension ));
		ASSERT( All( src.imageOffset + src.imageExtent <= desc.dimension ));
		ASSERT( src.rowPitch != 0_b );
		ASSERT( src.slicePitch > 0_b and (src.slicePitch % src.rowPitch == 0) );

		const uint	bpp = 0;

		dst.bufferOffset		= VkDeviceSize(src.bufferOffset);
		dst.bufferRowLength		= ImageUtils::RowLength( src.rowPitch, bpp, fmtInfo.TexBlockSize() );
		dst.bufferImageHeight	= ImageUtils::ImageHeight( src.slicePitch, src.rowPitch, fmtInfo.TexBlockSize() );
		_ConvertImageSubresourceLayer( OUT dst.imageSubresource, src.imageSubres, desc );
		dst.imageOffset			= { int(src.imageOffset.x), int(src.imageOffset.y), int(src.imageOffset.z) };
		dst.imageExtent			= { src.imageExtent.x, src.imageExtent.y, src.imageExtent.z };

		ASSERT( dst.bufferRowLength == 0 or dst.bufferRowLength >= dst.imageExtent.width );
		ASSERT( dst.bufferImageHeight == 0 or dst.bufferImageHeight >= dst.imageExtent.height );
	}
	
/*
=================================================
	_ConvertImageSubresourceLayer
=================================================
*/
	template <typename C>
	void  _VTransferContextImpl<C>::_ConvertImageSubresourceLayer (OUT VkImageSubresourceLayers &dst, const ImageSubresourceLayers &src, const ImageDesc &desc)
	{
		ASSERT( src.mipLevel < desc.maxLevel );
		ASSERT( src.baseArrayLayer < desc.arrayLayers );
		ASSERT( src.aspectMask != Default );

		dst.aspectMask		= VEnumCast( src.aspectMask );
		dst.mipLevel		= Min( src.mipLevel.Get(), desc.maxLevel.Get()-1 );
		dst.baseArrayLayer	= Min( src.baseArrayLayer.Get(), desc.arrayLayers.Get()-1 );
		dst.layerCount		= Min( src.layerCount, desc.arrayLayers.Get() - src.baseArrayLayer.Get() );
	}
//-----------------------------------------------------------------------------
	


/*
=================================================
	ClearColorImage
=================================================
*/
	inline void  _VDirectTransferCtx::ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges)
	{
		ASSERT( image != Default );
		ASSERT( ranges.size() );
		ASSERT( _NoPendingBarriers() );

		vkCmdClearColorImage( _cmdbuf.Get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, uint(ranges.size()), ranges.data() );
	}
	
/*
=================================================
	ClearDepthStencilImage
=================================================
*/
	inline void  _VDirectTransferCtx::ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges)
	{
		ASSERT( image != Default );
		ASSERT( ranges.size() );
		ASSERT( _NoPendingBarriers() );

		vkCmdClearDepthStencilImage( _cmdbuf.Get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &depthStencil, uint(ranges.size()), ranges.data() );
	}
	
/*
=================================================
	FillBuffer
=================================================
*/
	inline void  _VDirectTransferCtx::FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)
	{
		ASSERT( buffer != Default );
		ASSERT( _NoPendingBarriers() );

		vkCmdFillBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
	}
	
/*
=================================================
	UpdateBuffer
=================================================
*/
	inline void  _VDirectTransferCtx::UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)
	{
		ASSERT( buffer != Default );
		ASSERT( _NoPendingBarriers() );

		vkCmdUpdateBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
	}
	
/*
=================================================
	CopyBuffer
=================================================
*/
	inline void  _VDirectTransferCtx::CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)
	{
		ASSERT( srcBuffer != Default );
		ASSERT( dstBuffer != Default );
		ASSERT( ranges.size() );
		ASSERT( _NoPendingBarriers() );

		vkCmdCopyBuffer( _cmdbuf.Get(), srcBuffer, dstBuffer, uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyBuffer2KHR
	}
	
/*
=================================================
	CopyImage
=================================================
*/
	inline void  _VDirectTransferCtx::CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)
	{
		ASSERT( srcImage != Default );
		ASSERT( dstImage != Default );
		ASSERT( ranges.size() );
		ASSERT( _NoPendingBarriers() );

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
	inline void  _VDirectTransferCtx::CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)
	{
		ASSERT( srcBuffer != Default );
		ASSERT( dstImage != Default );
		ASSERT( ranges.size() );
		ASSERT( _NoPendingBarriers() );

		vkCmdCopyBufferToImage( _cmdbuf.Get(), srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyBufferToImage2KHR
	}
	
/*
=================================================
	CopyImageToBuffer
=================================================
*/
	inline void  _VDirectTransferCtx::CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)
	{
		ASSERT( srcImage != Default );
		ASSERT( dstBuffer != Default );
		ASSERT( ranges.size() );
		ASSERT( _NoPendingBarriers() );

		vkCmdCopyImageToBuffer( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer, uint(ranges.size()), ranges.data() );
		// TODO vkCmdCopyImageToBuffer2KHR
	}
	
/*
=================================================
	BlitImage
=================================================
*/
	inline void  _VDirectTransferCtx::BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)
	{
		ASSERT( srcImage != Default );
		ASSERT( dstImage != Default );
		ASSERT( regions.size() );
		ASSERT( _NoPendingBarriers() );

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
	inline void  _VDirectTransferCtx::ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)
	{
		ASSERT( srcImage != Default );
		ASSERT( dstImage != Default );
		ASSERT( regions.size() );
		ASSERT( _NoPendingBarriers() );

		vkCmdResolveImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							uint(regions.size()), regions.data() );
		// TODO vkCmdResolveImage2KHR
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
