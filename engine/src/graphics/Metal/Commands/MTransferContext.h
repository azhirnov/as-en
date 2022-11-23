// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	TransferCtx --> DirectTransferCtx   --> BarrierMngr --> Metal device 
				\-> IndirectTransferCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Metal Direct Transfer Context implementation
	//
	
	class _MDirectTransferCtx : public MBaseDirectContext
	{
	// variables
	private:
		MetalBlitCommandEncoderRC	_encoder;
		
		
	// methods
	public:
		void  FillBuffer (MetalBuffer buffer, Bytes offset, Bytes size, uint data);

		void  CopyBuffer (MetalBuffer srcBuffer, MetalBuffer dstBuffer, ArrayView<BufferCopy> ranges);
		void  CopyImage (MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges);
		
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy> ranges);
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy2> ranges);

		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy> ranges);
		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy2> ranges);
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ();
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ();

	protected:
		_MDirectTransferCtx (const RenderTask &task);
		_MDirectTransferCtx (const RenderTask &task, MCommandBuffer cmdbuf);

		ND_ MetalCommandEncoder  _BaseEncoder ()					{ return MetalCommandEncoder{ _encoder.Ptr() }; }
		
		void  _GenerateMipmaps (MetalImage image);

		void  _SynchronizeResource (MetalResource);
		void  _SynchronizeResource (MetalBuffer buf)				{ _SynchronizeResource( MetalResource{ buf.Ptr() }); }
		void  _SynchronizeResource (MetalImage img)					{ _SynchronizeResource( MetalResource{ img.Ptr() }); }
		
		void  _DebugMarker (NtStringView text, RGBA8u)				{ ASSERT( _NoPendingBarriers() );  MBaseDirectContext::_DebugMarker( _BaseEncoder(), text ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u)			{ ASSERT( _NoPendingBarriers() );  MBaseDirectContext::_PushDebugGroup( _BaseEncoder(), text ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  MBaseDirectContext::_PopDebugGroup( _BaseEncoder() ); }

	private:
		void  _CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, const MPixFormatInfo &, ArrayView<BufferImageCopy> ranges);
		void  _CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, const MPixFormatInfo &, ArrayView<BufferImageCopy> ranges);
	};
	
	
	
	//
	// Metal Indirect Transfer Context implementation
	//
	
	class _MIndirectTransferCtx : public MBaseIndirectContext
	{
	// methods
	public:
		void  FillBuffer (MetalBuffer buffer, Bytes offset, Bytes size, uint data);

		void  CopyBuffer (MetalBuffer srcBuffer, MetalBuffer dstBuffer, ArrayView<BufferCopy> ranges);
		void  CopyImage (MetalImage srcImage, MetalImage dstImage, ArrayView<ImageCopy> ranges);
		
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy> ranges);
		void  CopyBufferToImage (MetalBuffer srcBuffer, MetalImage dstImage, ArrayView<BufferImageCopy2> ranges);

		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy> ranges);
		void  CopyImageToBuffer (MetalImage srcImage, MetalBuffer dstBuffer, ArrayView<BufferImageCopy2> ranges);
		
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		_MIndirectTransferCtx (const RenderTask &task);
		_MIndirectTransferCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf);
		
		void  _GenerateMipmaps (MetalImage image);
		
		void  _SynchronizeResource (MetalResource);
		void  _SynchronizeResource (MetalBuffer buf)				{ _SynchronizeResource( MetalResource{ buf.Ptr() }); }
		void  _SynchronizeResource (MetalImage img)					{ _SynchronizeResource( MetalResource{ img.Ptr() }); }
	};


	
	//
	// Metal Transfer Context implementation
	//

	template <typename CtxImpl>
	class _MTransferContextImpl : public CtxImpl, public ITransferContext
	{
	// types
	public:
		static constexpr bool	IsTransferContext		= true;
		static constexpr bool	IsMetalTransferContext	= true;
	private:
		static constexpr uint	_LocalArraySize			= 16;
		static constexpr Bytes	_StagingBufOffsetAlign	= 4_b;

		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumBarriers< _MTransferContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _MTransferContextImpl (const RenderTask &task) : RawCtx{ task } {}
		
		template <typename RawCmdBufType>
		_MTransferContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_MTransferContextImpl () = delete;
		_MTransferContextImpl (const _MTransferContextImpl &) = delete;

		using RawCtx::FillBuffer;
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)																			override;
		
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)																override;

		using RawCtx::CopyBuffer;
		using RawCtx::CopyImage;

		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)															override;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)																override;

		using RawCtx::CopyBufferToImage;
		using RawCtx::CopyImageToBuffer;

		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)												override;
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)												override;

		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)												override;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)												override;

		void  UploadBuffer (BufferID buffer, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)override;
		void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)														override;

		void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)						override;
		void  UploadImage (ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)						override;

		ND_ Promise<BufferMemView>	ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static)	override;
		ND_ Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)														override;
		
		ND_ bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)														override;

		ND_ Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)														override;
		
		void  GenerateMipmaps (ImageID srcImage)																										override;

		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;
		
		uint3  MinImageTransferGranularity ()																											C_NE_OF		{ return uint3{1}; }

		MBARRIERMNGR_INHERIT_BARRIERS
			
	private:
		ND_ static bool  _IsHostMemory (const MBuffer &buffer)		__NE___	{ return AnyBits( buffer.Description().memType, EMemoryType::HostCachedCocherent ); }
		ND_ static bool  _IsHostMemory (const MImage &image)		__NE___	{ return AnyBits( image.Description().memType, EMemoryType::HostCachedCocherent ); }

		ND_ static bool  _IsDeviceMemory (const MBuffer &buffer)	__NE___	{ return AnyBits( buffer.Description().memType, EMemoryType::DeviceLocal ); }
		ND_ static bool  _IsDeviceMemory (const MImage &image)		__NE___	{ return AnyBits( image.Description().memType, EMemoryType::DeviceLocal ); }
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectTransferContext	= _hidden_::_MTransferContextImpl< _hidden_::_MDirectTransferCtx >;
	using MIndirectTransferContext	= _hidden_::_MTransferContextImpl< _hidden_::_MIndirectTransferCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	FillBuffer
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::FillBuffer (BufferID bufferId, Bytes offset, Bytes size, uint data)
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );

		ASSERT( offset < buf.Size() );
		ASSERT( size == UMax or (offset + size) <= buf.Size() );

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
	void  _MTransferContextImpl<C>::UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)
	{
		CHECK( UploadBuffer( buffer, offset, size, data, EStagingHeapType::Static ) == size );
	}

/*
=================================================
	UploadBuffer
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::UploadBuffer (BufferID bufferId, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType)
	{
		auto&	dst_buf = _GetResourcesOrThrow( bufferId );
		ASSERT( _IsDeviceMemory( dst_buf ));
		ASSERT( offset + size <= dst_buf.Size() );
		ASSERT( memView.Empty() );
		
		offset	= Min( offset, dst_buf.Size() );
		size	= Min( size, dst_buf.Size() - offset );

		MStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		MStagingBufferManager::BufferRanges_t	buffers;
		
		sbm.GetBufferRanges( OUT buffers, size, 0_b, _StagingBufOffsetAlign, this->_mngr.GetFrameId(), heapType, this->_mngr.GetQueueType(), True{"uload"} );
		
		for (auto& src_buf : buffers)
		{
			memView.PushBack( src_buf.mapped, src_buf.size );
			CopyBuffer( src_buf.buffer, dst_buf.Handle(), {BufferCopy{ src_buf.bufferOffset, offset, src_buf.size }});
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
	void  _MTransferContextImpl<C>::UploadBuffer (BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType)
	{
		ASSERT( not stream.IsComplete() );

		auto&	dst_buf = _GetResourcesOrThrow( stream.Buffer() );
		ASSERT( _IsDeviceMemory( dst_buf ));
		ASSERT( stream.End() <= dst_buf.Size() );
		ASSERT( memView.Empty() );
		
		MStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		MStagingBufferManager::BufferRanges_t	buffers;

		sbm.GetBufferRanges( OUT buffers, stream.RemainSize(), 0_b, _StagingBufOffsetAlign, this->_mngr.GetFrameId(), heapType, this->_mngr.GetQueueType(), True{"uload"} );
		
		for (auto& src_buf : buffers)
		{
			memView.PushBack( src_buf.mapped, src_buf.size );
			CopyBuffer( src_buf.buffer, dst_buf.Handle(), {BufferCopy{ src_buf.bufferOffset, stream.OffsetAndPos(), src_buf.size }});
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
	void  _MTransferContextImpl<C>::UploadImage (ImageID imageId, const UploadImageDesc &uploadDesc, OUT ImageMemView &memView)
	{
		auto&	dst_img = _GetResourcesOrThrow( imageId );
		ASSERT( _IsDeviceMemory( dst_img ));

		const ImageDesc&		img_desc	= dst_img.Description();
		MStagingBufferManager&	sbm			= this->_mngr.GetStagingManager();

		MStagingBufferManager::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, uploadDesc, img_desc, this->_mngr.GetFrameId(), this->_mngr.GetQueueType(), True{"upload"} );
		
		if_unlikely( res.buffers.empty() )
			return;

		BufferImageCopy2		copy;
		ImageSubresourceLayers&	subres = copy.imageSubres;
		subres.aspectMask		= uploadDesc.aspectMask;
		subres.mipLevel			= uploadDesc.mipLevel;
		subres.baseArrayLayer	= uploadDesc.arrayLayer;
		subres.layerCount		= 1;
		copy.rowPitch			= res.dataRowPitch;

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& src_buf : res.buffers)
		{
			mem_view.PushBack( src_buf.mapped, src_buf.size );
			copy.bufferOffset	= src_buf.bufferOffset;
			copy.slicePitch		= src_buf.bufferSlicePitch;
			copy.imageOffset	= src_buf.imageOffset;
			copy.imageExtent	= src_buf.imageSize;

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
	void  _MTransferContextImpl<C>::UploadImage (ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType)
	{
		ASSERT( not stream.IsComplete() );

		auto&	dst_img = _GetResourcesOrThrow( stream.Image() );
		ASSERT( _IsDeviceMemory( dst_img ));

		const ImageDesc&		img_desc	= dst_img.Description();
		MStagingBufferManager&	sbm			= this->_mngr.GetStagingManager();

		ASSERT( All( stream.End() <= img_desc.dimension ));
		
		UploadImageDesc	upload_desc = stream.ToUploadDesc();
		upload_desc.imageOffset	+= uint3{ 0, stream.posYZ };
		upload_desc.imageSize	-= uint3{ 0, stream.posYZ };
		upload_desc.heapType	 = heapType;

		MStagingBufferManager::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, upload_desc, img_desc, this->_mngr.GetFrameId(), this->_mngr.GetQueueType(), True{"upload"} );
		
		if_unlikely( res.buffers.empty() )
			return;

		BufferImageCopy2		copy;
		ImageSubresourceLayers&	subres = copy.imageSubres;
		subres.aspectMask		= upload_desc.aspectMask;
		subres.mipLevel			= upload_desc.mipLevel;
		subres.baseArrayLayer	= upload_desc.arrayLayer;
		subres.layerCount		= 1;
		copy.rowPitch			= res.dataRowPitch;

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& src_buf : res.buffers)
		{
			mem_view.PushBack( src_buf.mapped, src_buf.size );
			copy.bufferOffset	= src_buf.bufferOffset;
			copy.slicePitch		= src_buf.bufferSlicePitch;
			copy.imageOffset	= src_buf.imageOffset;
			copy.imageExtent	= src_buf.imageSize;

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
	Promise<BufferMemView>  _MTransferContextImpl<C>::ReadbackBuffer (BufferID bufferId, Bytes offset, Bytes size, EStagingHeapType heapType)
	{
		auto&	src_buf = _GetResourcesOrThrow( bufferId );
		ASSERT( _IsDeviceMemory( src_buf ));
		ASSERT( offset + size <= src_buf.Size() );
		
		offset	= Min( offset, src_buf.Size() );
		size	= Min( size, src_buf.Size() - offset );
		
		MStagingBufferManager&					sbm	= this->_mngr.GetStagingManager();
		MStagingBufferManager::BufferRanges_t	buffers;
		sbm.GetBufferRanges( OUT buffers, size, 0_b, _StagingBufOffsetAlign, this->_mngr.GetFrameId(), heapType, this->_mngr.GetQueueType(), False{"readback"} );
		
		BufferMemView	mem_view;
		for (auto& dst_buf : buffers)
		{
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
			CopyBuffer( src_buf.Handle(), dst_buf.buffer, {BufferCopy{ offset, dst_buf.bufferOffset, dst_buf.size }});
			offset += dst_buf.size;
		}
		ASSERT( buffers.size() == mem_view.Parts().size() );

		return Threading::MakePromiseFromValue(	mem_view, Tuple{ this->_mngr.GetBatchRC() });
	}
	
/*
=================================================
	ReadHostBuffer
=================================================
*/
	template <typename C>
	Promise<ArrayView<ubyte>>  _MTransferContextImpl<C>::ReadHostBuffer (BufferID bufferId, Bytes offset, Bytes size)
	{
		auto&	src_buf = _GetResourcesOrThrow( bufferId );
		ASSERT( _IsHostMemory( src_buf ));
		
		offset	= Min( offset, src_buf.Size() );
		size	= Min( size, src_buf.Size() - offset );
		
		ArrayView<ubyte>	mem_view = ArrayView<ubyte>{ Cast<ubyte>(src_buf.MappedPtr() + offset), usize(size) };

		if_unlikely( not AllBits( src_buf.Description().memType, EMemoryType::HostCocherent ))
		{
			RawCtx::_SynchronizeResource( src_buf.Handle() );
		}

		return Threading::MakePromiseFromValue(	mem_view, Tuple{ this->_mngr.GetBatchRC() });
	}
	
/*
=================================================
	ReadbackImage
=================================================
*/
	template <typename C>
	Promise<ImageMemView>   _MTransferContextImpl<C>::ReadbackImage (ImageID imageId, const ReadbackImageDesc &readDesc)
	{
		auto&	src_img = _GetResourcesOrThrow( imageId );
		ASSERT( _IsDeviceMemory( src_img ));
		
		const ImageDesc&		img_desc	= src_img.Description();
		MStagingBufferManager&	sbm			= this->_mngr.GetStagingManager();

		MStagingBufferManager::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, readDesc, img_desc, this->_mngr.GetFrameId(), this->_mngr.GetQueueType(), False{"readback"} );
		
		if_unlikely( res.buffers.empty() )
			return Default;

		BufferImageCopy2		copy;
		ImageSubresourceLayers&	subres = copy.imageSubres;
		subres.aspectMask		= readDesc.aspectMask;
		subres.mipLevel			= readDesc.mipLevel;
		subres.baseArrayLayer	= readDesc.arrayLayer;
		subres.layerCount		= 1;
		copy.rowPitch			= res.dataRowPitch;

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& dst_buf : res.buffers)
		{
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
			copy.bufferOffset	= dst_buf.bufferOffset;
			copy.slicePitch		= dst_buf.bufferSlicePitch;
			copy.imageOffset	= dst_buf.imageOffset;
			copy.imageExtent	= dst_buf.imageSize;

			min = Min( min, dst_buf.imageOffset );
			max = Max( max, dst_buf.imageOffset + dst_buf.imageSize );

			CopyImageToBuffer( src_img.Handle(), dst_buf.buffer, {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );
		
		ImageMemView	img_mem_view{ mem_view, min, max - min, res.dataRowPitch, res.dataSlicePitch, img_desc.format, readDesc.aspectMask };
		return Threading::MakePromiseFromValue(	img_mem_view, Tuple{ this->_mngr.GetBatchRC() });
	}

/*
=================================================
	UpdateHostBuffer
=================================================
*/
	template <typename C>
	bool  _MTransferContextImpl<C>::UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)
	{
		auto&	buf = _GetResourcesOrThrow( bufferId );
		
		ASSERT( offset < buf.Size() );
		ASSERT( size == UMax or (offset + size) <= buf.Size() );

		offset	= Min( offset, buf.Size()-1 );
		size	= Min( size, buf.Size() - offset );

		void*	ptr = buf.MappedPtr();
		CHECK_THROW( ptr != null );

		MemCopy( OUT ptr + offset, data, size );

		buf.DidModifyRange( offset, size );
		return true;
	}

/*
=================================================
	CopyBuffer
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)
	{
		auto  [src_buf, dst_buf] = _GetResourcesOrThrow( srcBuffer, dstBuffer );

		RawCtx::CopyBuffer( src_buf.Handle(), dst_buf.Handle(), ranges );
	}

/*
=================================================
	CopyImage
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)
	{
		auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );

		RawCtx::CopyImage( src_img.Handle(), dst_img.Handle(), ranges );
	}

/*
=================================================
	CopyBufferToImage
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)
	{
		auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );

		RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), ranges );
	}

	template <typename C>
	void  _MTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)
	{
		auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );

		RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), ranges );
	}

/*
=================================================
	CopyImageToBuffer
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)
	{
		auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );

		RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), ranges );
	}

	template <typename C>
	void  _MTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)
	{
		auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );

		RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), ranges );
	}

/*
=================================================
	GenerateMipmaps
=================================================
*/
	template <typename C>
	void  _MTransferContextImpl<C>::GenerateMipmaps (ImageID srcImage)
	{
		auto&	src_img = _GetResourcesOrThrow( srcImage );

		RawCtx::_GenerateMipmaps( src_img.Handle() );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
