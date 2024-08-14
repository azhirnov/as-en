// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define TRANSFER_CTX( _res_, _name_ )	template <typename C>  _res_  _VTransferContextImpl<C>::_name_
#	define STAGINGBUF_MNGR					VStagingBufferManager
namespace AE::Graphics::_hidden_ {

#elif defined(AE_ENABLE_METAL)
#	define TRANSFER_CTX( _res_, _name_ )	template <typename C>  _res_  _MTransferContextImpl<C>::_name_
#	define STAGINGBUF_MNGR					MStagingBufferManager
namespace AE::Graphics::_hidden_ {

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define TRANSFER_CTX( _res_, _name_ )	_res_  RTransferContext::_name_
#	define STAGINGBUF_MNGR					RStagingBufferManager
namespace AE::Graphics {

#else
#	error not implemented
#endif


/*
=================================================
	UploadBuffer
=================================================
*/
	TRANSFER_CTX( inline void, UploadBuffer )(BufferID bufferId, const UploadBufferDesc &uploadDesc, OUT BufferMemView &memView) __Th___
	{
		auto&		dst_buf			= _GetResourcesOrThrow( bufferId );
		const Bytes	dst_buf_size	= dst_buf.Size();
		Bytes		offset			= Min( uploadDesc.offset,	dst_buf_size );
		const Bytes	data_size		= Min( uploadDesc.size,		dst_buf_size - offset );
		const auto	handle			= dst_buf.Handle();

		VALIDATE_GCTX( UploadBuffer( dst_buf.Description(), offset, data_size, memView ));

		STAGINGBUF_MNGR&				sbm	= this->_mngr.GetStagingManager();
		STAGINGBUF_MNGR::BufferRanges_t	buffers;

		sbm.GetBufferRanges( OUT buffers, data_size, uploadDesc.blockSize, GraphicsConfig::StagingBufferOffsetAlign,
							 GetFrameId(), uploadDesc.heapType, True{"upload"} );

		for (auto& src_buf : buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			memView.PushBack( _Allocate( src_buf.devicePtr, src_buf.size ), src_buf.size );
		  #else
			memView.PushBack( src_buf.mapped, src_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			CopyBuffer( src_buf.bufferHandle, handle, {VkBufferCopy{ VkDeviceSize(src_buf.bufferOffset), VkDeviceSize(offset), VkDeviceSize(src_buf.size) }});
		  #else
			CopyBuffer( src_buf.bufferHandle, handle, {BufferCopy{ src_buf.bufferOffset, offset, src_buf.size }});
		  #endif
			offset += src_buf.size;
			GCTX_CHECK( offset <= dst_buf_size );
		}
		ASSERT( buffers.size() == memView.Parts().size() );
	}

/*
=================================================
	UploadBuffer
=================================================
*/
	TRANSFER_CTX( inline void, UploadBuffer )(BufferStream &stream, OUT BufferMemView &memView) __Th___
	{
		GCTX_CHECK_MSG( stream.IsInitialized(), "Buffer stream is not initialized" );
		GCTX_CHECK_MSG( not stream.IsCompleted(), "Buffer stream is already complete" );
		GCTX_CHECK_MSG( stream.ToUploadDesc().size != UMax, "'UploadBufferDesc::size' is not defined" );

		const auto&	dst_buf = _GetResourcesOrThrow( stream.BufferId() );
		const auto	handle	= dst_buf.Handle();

		VALIDATE_GCTX( UploadBuffer( dst_buf.Description(), stream.pos, stream.RemainSize(), memView ));

		STAGINGBUF_MNGR&				sbm	= this->_mngr.GetStagingManager();
		STAGINGBUF_MNGR::BufferRanges_t	buffers;

		sbm.GetBufferRanges( OUT buffers, stream.RemainSize(), stream.BlockSize(), GraphicsConfig::StagingBufferOffsetAlign,
							 GetFrameId(), stream.HeapType(), True{"upload"} );

		for (auto& src_buf : buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			memView.PushBack( _Allocate( src_buf.devicePtr, src_buf.size ), src_buf.size );
		  #else
			memView.PushBack( src_buf.mapped, src_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			CopyBuffer( src_buf.bufferHandle, handle, {VkBufferCopy{ VkDeviceSize(src_buf.bufferOffset), VkDeviceSize(stream.OffsetAndPos()), VkDeviceSize(src_buf.size) }});
		  #else
			CopyBuffer( src_buf.bufferHandle, handle, {BufferCopy{ src_buf.bufferOffset, stream.OffsetAndPos(), src_buf.size }});
		  #endif
			stream.pos += src_buf.size;
			GCTX_CHECK( stream.pos <= stream.DataSize() );
		}
		ASSERT( buffers.size() == memView.Parts().size() );
	}

/*
=================================================
	_UploadImage
=================================================
*/
	TRANSFER_CTX( template <typename ID> void, _UploadImage )(ID imageId, const UploadImageDesc &uploadDesc, OUT ImageMemView &memView) __Th___
	{
		StaticAssert( IsSameTypes< ID, ImageID > or IsSameTypes< ID, VideoImageID >);

		auto&	dst_img = _GetResourcesOrThrow( imageId );
		VALIDATE_GCTX( UploadImage( dst_img.Description() ));

		const auto&			img_desc	= dst_img.Description();
		STAGINGBUF_MNGR&	sbm			= this->_mngr.GetStagingManager();
		const auto			handle		= [&dst_img]() {
												if constexpr( IsSameTypes< ID, ImageID >)		return dst_img.Handle();
												if constexpr( IsSameTypes< ID, VideoImageID >)	return dst_img.GetImageHandle();
											}();

		STAGINGBUF_MNGR::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, uploadDesc, img_desc, MinImageTransferGranularity(), GetFrameId(), True{"upload"} );

		if_unlikely( res.buffers.empty() )
			return;

	  #ifdef AE_ENABLE_VULKAN
		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask			= VEnumCast( uploadDesc.aspectMask );
		subres.mipLevel				= uploadDesc.mipLevel.Get();
		subres.baseArrayLayer		= uploadDesc.arrayLayer.Get();
		subres.layerCount			= 1;
		copy.bufferRowLength		= res.bufferRowLength;
	  #else
		BufferImageCopy2			copy;
		ImageSubresourceLayers&		subres = copy.imageSubres;
		subres.aspectMask			= uploadDesc.aspectMask;
		subres.mipLevel				= uploadDesc.mipLevel;
		subres.baseLayer			= uploadDesc.arrayLayer;
		subres.layerCount			= 1;
		copy.rowPitch				= res.dataRowPitch;
	  #endif

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& src_buf : res.buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			mem_view.PushBack( _Allocate( src_buf.devicePtr, src_buf.size ), src_buf.size );
		  #else
			mem_view.PushBack( src_buf.mapped, src_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			copy.bufferOffset		= VkDeviceSize(src_buf.bufferOffset);
			copy.bufferImageHeight	= src_buf.bufferImageHeight;
			copy.imageOffset		= {int(src_buf.imageOffset.x), int(src_buf.imageOffset.y), int(src_buf.imageOffset.z)};
			copy.imageExtent		= {    src_buf.imageDim.x,         src_buf.imageDim.y,         src_buf.imageDim.z   };
		  #else
			copy.bufferOffset		= src_buf.bufferOffset;
			copy.slicePitch			= src_buf.bufferSlicePitch;
			copy.imageOffset		= src_buf.imageOffset;
			copy.imageExtent		= src_buf.imageDim;
		  #endif

			min = Min( min, src_buf.imageOffset );
			max = Max( max, src_buf.imageOffset + src_buf.imageDim );

			CopyBufferToImage( src_buf.bufferHandle, handle, {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );

		memView = ImageMemView{ mem_view, min, max - min, res.dataRowPitch, res.dataSlicePitch, res.format, uploadDesc.aspectMask };
	}

/*
=================================================
	_UploadImage
=================================================
*/
	TRANSFER_CTX( template <typename StreamType> void, _UploadImage )(StreamType &stream, OUT ImageMemView &memView) __Th___
	{
		StaticAssert( IsSameTypes< StreamType, ImageStream > or
					  IsSameTypes< StreamType, VideoImageStream >);

		GCTX_CHECK_MSG( stream.IsInitialized(), "Image stream is not initialized" );
		GCTX_CHECK_MSG( not stream.IsCompleted(), "Image stream is already complete" );
		GCTX_CHECK_MSG( All( stream.ToUploadDesc().imageDim != UMax ), "'UploadImageDesc::imageDim' is not defined" );

		auto&	dst_img = _GetResourcesOrThrow( stream.ImageId() );
		VALIDATE_GCTX( UploadImage( dst_img.Description() ));

		const auto&			img_desc	= dst_img.Description();
		STAGINGBUF_MNGR&	sbm			= this->_mngr.GetStagingManager();
		const auto			handle		= [&dst_img]() {
												if constexpr( IsSameTypes< StreamType, ImageStream >)		return dst_img.Handle();
												if constexpr( IsSameTypes< StreamType, VideoImageStream >)	return dst_img.GetImageHandle();
											}();

		UploadImageDesc	upload_desc = stream.ToUploadDesc();
		upload_desc.imageOffset	+= uint3{ 0, stream.posYZ };
		upload_desc.imageDim	-= uint3{ 0, stream.posYZ };

		STAGINGBUF_MNGR::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, upload_desc, img_desc, MinImageTransferGranularity(), GetFrameId(), True{"upload"} );

		if_unlikely( res.buffers.empty() )
			return;

	  #ifdef AE_ENABLE_VULKAN
		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask			= VEnumCast( upload_desc.aspectMask );
		subres.mipLevel				= upload_desc.mipLevel.Get();
		subres.baseArrayLayer		= upload_desc.arrayLayer.Get();
		subres.layerCount			= 1;
		copy.bufferRowLength		= res.bufferRowLength;
	  #else
		BufferImageCopy2			copy;
		ImageSubresourceLayers&		subres = copy.imageSubres;
		subres.aspectMask			= upload_desc.aspectMask;
		subres.mipLevel				= upload_desc.mipLevel;
		subres.baseLayer			= upload_desc.arrayLayer;
		subres.layerCount			= 1;
		copy.rowPitch				= res.dataRowPitch;
	  #endif

		BufferMemView	mem_view;
		uint3			min {~0u};
		uint3			max {0};

		for (auto& src_buf : res.buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			mem_view.PushBack( _Allocate( src_buf.devicePtr, src_buf.size ), src_buf.size );
		  #else
			mem_view.PushBack( src_buf.mapped, src_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			copy.bufferOffset		= VkDeviceSize(src_buf.bufferOffset);
			copy.bufferImageHeight	= src_buf.bufferImageHeight;
			copy.imageOffset		= {int(src_buf.imageOffset.x), int(src_buf.imageOffset.y), int(src_buf.imageOffset.z)};
			copy.imageExtent		= {    src_buf.imageDim.x,         src_buf.imageDim.y,         src_buf.imageDim.z   };
		  #else
			copy.bufferOffset		= src_buf.bufferOffset;
			copy.slicePitch			= src_buf.bufferSlicePitch;
			copy.imageOffset		= src_buf.imageOffset;
			copy.imageExtent		= src_buf.imageDim;
		  #endif

			min = Min( min, src_buf.imageOffset );
			max = Max( max, src_buf.imageOffset + src_buf.imageDim );

			GCTX_CHECK( All( min >= stream.Begin() ));
			GCTX_CHECK( All( max <= stream.End() ));

			CopyBufferToImage( src_buf.bufferHandle, handle, {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );

		stream.posYZ[0] = max.y - stream.Begin().y;
		stream.posYZ[1] = max.z - stream.Begin().z - 1;

		if_unlikely( stream.posYZ[0] * res.planeScaleY >= stream.RegionSize().y )
		{
			stream.posYZ[0] = 0;
			stream.posYZ[1] ++;
		}

		memView = ImageMemView{ mem_view, min, max - min, res.dataRowPitch,
								((max.z - min.z > 1) ? res.dataSlicePitch : 0_b),
								res.format, upload_desc.aspectMask };
	}

/*
=================================================
	ReadbackBuffer
=================================================
*/
	TRANSFER_CTX( inline ITransferContext::ReadbackBufferResult2, ReadbackBuffer )(BufferID bufferId, const ReadbackBufferDesc &readDesc) __Th___
	{
		auto&		src_buf			= _GetResourcesOrThrow( bufferId );
		const auto	handle			= src_buf.Handle();
		const Bytes	dst_buf_size	= src_buf.Size();
		Bytes		offset			= Min( readDesc.offset, dst_buf_size );
		const Bytes	data_size		= Min( readDesc.size,   dst_buf_size - offset );

		VALIDATE_GCTX( ReadbackBuffer( src_buf.Description(), offset, data_size ));

		STAGINGBUF_MNGR&				sbm	= this->_mngr.GetStagingManager();
		STAGINGBUF_MNGR::BufferRanges_t	buffers;
		sbm.GetBufferRanges( OUT buffers, data_size, readDesc.blockSize, GraphicsConfig::StagingBufferOffsetAlign,
							 GetFrameId(), readDesc.heapType, False{"readback"} );

		BufferMemView	mem_view;
		for (auto& dst_buf : buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			mem_view.PushBack( _ReadbackAlloc( dst_buf.devicePtr, dst_buf.size ), dst_buf.size );
		  #else
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			CopyBuffer( handle, dst_buf.bufferHandle, {VkBufferCopy{ VkDeviceSize(offset), VkDeviceSize(dst_buf.bufferOffset), VkDeviceSize(dst_buf.size) }});
		  #else
			CopyBuffer( handle, dst_buf.bufferHandle, {BufferCopy{ offset, dst_buf.bufferOffset, dst_buf.size }});
		  #endif
			offset += dst_buf.size;
			GCTX_CHECK( offset <= dst_buf_size );
		}

		ASSERT( buffers.size() == mem_view.Parts().size() );
		ASSERT( offset == dst_buf_size );

		return	ReadbackBufferResult2{
					Threading::MakePromiseFromValue( mem_view,
													 Tuple{ OnFrameNextCycle{ GetFrameId() }},
													 "TransferContext::ReadbackBuffer",
													 ETaskQueue::PerFrame ),
					data_size - mem_view.DataSize()
				};
	}

/*
=================================================
	ReadbackBuffer
=================================================
*/
	TRANSFER_CTX( inline ITransferContext::ReadbackBufferResult, ReadbackBuffer )(INOUT BufferStream &stream) __Th___
	{
		GCTX_CHECK_MSG( stream.IsInitialized(), "Buffer stream is not initialized" );
		GCTX_CHECK_MSG( not stream.IsCompleted(), "Buffer stream is already complete" );
		GCTX_CHECK_MSG( stream.ToReadbackDesc().size != UMax, "'ReadbackBufferDesc::size' is not defined" );

		const auto&	src_buf			= _GetResourcesOrThrow( stream.BufferId() );
		const auto	handle			= src_buf.Handle();
		const Bytes	dst_buf_size	= src_buf.Size();
		Bytes		offset			= Min( stream.OffsetAndPos(), dst_buf_size );
		const Bytes	remain_size		= Min( stream.End(), dst_buf_size - offset );

		VALIDATE_GCTX( ReadbackBuffer( src_buf.Description(), offset, remain_size ));

		STAGINGBUF_MNGR&				sbm	= this->_mngr.GetStagingManager();
		STAGINGBUF_MNGR::BufferRanges_t	buffers;
		sbm.GetBufferRanges( OUT buffers, remain_size, stream.BlockSize(), GraphicsConfig::StagingBufferOffsetAlign,
							 GetFrameId(), stream.HeapType(), False{"readback"} );

		BufferMemView	mem_view;
		for (auto& dst_buf : buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			mem_view.PushBack( _ReadbackAlloc( dst_buf.devicePtr, dst_buf.size ), dst_buf.size );
		  #else
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			CopyBuffer( handle, dst_buf.bufferHandle, {VkBufferCopy{ VkDeviceSize(stream.pos), VkDeviceSize(dst_buf.bufferOffset), VkDeviceSize(dst_buf.size) }});
		  #else
			CopyBuffer( handle, dst_buf.bufferHandle, {BufferCopy{ stream.pos, dst_buf.bufferOffset, dst_buf.size }});
		  #endif
			stream.pos += dst_buf.size;
			GCTX_CHECK( stream.pos <= stream.DataSize() );
		}
		ASSERT( buffers.size() == mem_view.Parts().size() );

		return ReadbackBufferResult{
				Threading::MakePromiseFromValue(
					mem_view,
					Tuple{ OnFrameNextCycle{ GetFrameId() }},
					"TransferContext::ReadbackBuffer",
					ETaskQueue::PerFrame
				)};
	}

/*
=================================================
	_ReadbackImage
=================================================
*/
	TRANSFER_CTX( template <typename ID> ITransferContext::ReadbackImageResult2, _ReadbackImage )(ID imageId, const ReadbackImageDesc &readDesc) __Th___
	{
		StaticAssert( IsSameTypes< ID, ImageID > or IsSameTypes< ID, VideoImageID >);

		auto&	src_img = _GetResourcesOrThrow( imageId );
		VALIDATE_GCTX( ReadbackImage( src_img.Description() ));

		const auto&			img_desc	= src_img.Description();
		STAGINGBUF_MNGR&	sbm			= this->_mngr.GetStagingManager();
		const auto			handle		= [&src_img]() {
												if constexpr( IsSameTypes< ID, ImageID >)		return src_img.Handle();
												if constexpr( IsSameTypes< ID, VideoImageID >)	return src_img.GetImageHandle();
											}();

		STAGINGBUF_MNGR::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, readDesc, img_desc, MinImageTransferGranularity(), GetFrameId(), False{"readback"} );

		if_unlikely( res.buffers.empty() )
			return Default;

	  #ifdef AE_ENABLE_VULKAN
		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask			= VEnumCast( readDesc.aspectMask );
		subres.mipLevel				= readDesc.mipLevel.Get();
		subres.baseArrayLayer		= readDesc.arrayLayer.Get();
		subres.layerCount			= 1;
		copy.bufferRowLength		= res.bufferRowLength;
	  #else
		BufferImageCopy2			copy;
		ImageSubresourceLayers&		subres = copy.imageSubres;
		subres.aspectMask			= readDesc.aspectMask;
		subres.mipLevel				= readDesc.mipLevel;
		subres.baseLayer			= readDesc.arrayLayer;
		subres.layerCount			= 1;
		copy.rowPitch				= res.dataRowPitch;
	  #endif

		BufferMemView	mem_view;
		uint3			min		{~0u};
		uint3			max		{0};

		for (auto& dst_buf : res.buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			mem_view.PushBack( _ReadbackAlloc( dst_buf.devicePtr, dst_buf.size ), dst_buf.size );
		  #else
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			copy.bufferOffset		= VkDeviceSize(dst_buf.bufferOffset);
			copy.bufferImageHeight	= dst_buf.bufferImageHeight;
			copy.imageOffset		= {int(dst_buf.imageOffset.x), int(dst_buf.imageOffset.y), int(dst_buf.imageOffset.z)};
			copy.imageExtent		= {    dst_buf.imageDim.x,         dst_buf.imageDim.y,         dst_buf.imageDim.z   };
		  #else
			copy.bufferOffset		= dst_buf.bufferOffset;
			copy.slicePitch			= dst_buf.bufferSlicePitch;
			copy.imageOffset		= dst_buf.imageOffset;
			copy.imageExtent		= dst_buf.imageDim;
		  #endif

			min = Min( min, dst_buf.imageOffset );
			max = Max( max, dst_buf.imageOffset + dst_buf.imageDim );

			CopyImageToBuffer( handle, dst_buf.bufferHandle, {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );

		return	ReadbackImageResult2{
					Threading::MakePromiseFromValue(
						ImageMemView{ mem_view, min, max - min, res.dataRowPitch, res.dataSlicePitch, res.format, readDesc.aspectMask },
						Tuple{ OnFrameNextCycle{ GetFrameId() }},
						"TransferContext::ReadbackImage",
						ETaskQueue::PerFrame
					),
					res.regionDim - (max - min)
				};
	}


/*
=================================================
	_ReadbackImage
=================================================
*/
	TRANSFER_CTX( template <typename StreamType> ITransferContext::ReadbackImageResult, _ReadbackImage )(INOUT StreamType &stream) __Th___
	{
		StaticAssert( IsSameTypes< StreamType, ImageStream > or
					  IsSameTypes< StreamType, VideoImageStream >);

		GCTX_CHECK_MSG( stream.IsInitialized(), "Image stream is not initialized" );
		GCTX_CHECK_MSG( not stream.IsCompleted(), "Image stream is already complete" );
		GCTX_CHECK_MSG( All( stream.ToReadbackDesc().imageDim != UMax ), "'ReadbackImageDesc::imageDim' is not defined" );

		auto&	src_img = _GetResourcesOrThrow( stream.ImageId() );
		VALIDATE_GCTX( ReadbackImage( src_img.Description() ));

		const auto&			img_desc	= src_img.Description();
		STAGINGBUF_MNGR&	sbm			= this->_mngr.GetStagingManager();
		const auto			handle		= [&src_img]() {
												if constexpr( IsSameTypes< StreamType, ImageStream >)		return src_img.Handle();
												if constexpr( IsSameTypes< StreamType, VideoImageStream >)	return src_img.GetImageHandle();
											}();
		GCTX_CHECK( All( stream.End() <= img_desc.Dimension() ));

		ReadbackImageDesc	read_desc = stream.ToReadbackDesc();
		read_desc.imageOffset	+= uint3{ 0, stream.posYZ };
		read_desc.imageDim		-= uint3{ 0, stream.posYZ };

		STAGINGBUF_MNGR::StagingImageResultRanges	res;
		sbm.GetImageRanges( OUT res, read_desc, img_desc, MinImageTransferGranularity(), GetFrameId(), False{"readback"} );

		if_unlikely( res.buffers.empty() )
			return Default;

	  #ifdef AE_ENABLE_VULKAN
		VkBufferImageCopy			copy;
		VkImageSubresourceLayers&	subres = copy.imageSubresource;
		subres.aspectMask			= VEnumCast( read_desc.aspectMask );
		subres.mipLevel				= read_desc.mipLevel.Get();
		subres.baseArrayLayer		= read_desc.arrayLayer.Get();
		subres.layerCount			= 1;
		copy.bufferRowLength		= res.bufferRowLength;
	  #else
		BufferImageCopy2			copy;
		ImageSubresourceLayers&		subres = copy.imageSubres;
		subres.aspectMask			= read_desc.aspectMask;
		subres.mipLevel				= read_desc.mipLevel;
		subres.baseLayer			= read_desc.arrayLayer;
		subres.layerCount			= 1;
		copy.rowPitch				= res.dataRowPitch;
	  #endif

		BufferMemView	mem_view;
		uint3			min		{~0u};
		uint3			max		{0};

		for (auto& dst_buf : res.buffers)
		{
		  #ifdef AE_ENABLE_REMOTE_GRAPHICS
			mem_view.PushBack( _ReadbackAlloc( dst_buf.devicePtr, dst_buf.size ), dst_buf.size );
		  #else
			mem_view.PushBack( dst_buf.mapped, dst_buf.size );
		  #endif

		  #ifdef AE_ENABLE_VULKAN
			copy.bufferOffset		= VkDeviceSize(dst_buf.bufferOffset);
			copy.bufferImageHeight	= dst_buf.bufferImageHeight;
			copy.imageOffset		= {int(dst_buf.imageOffset.x), int(dst_buf.imageOffset.y), int(dst_buf.imageOffset.z)};
			copy.imageExtent		= {    dst_buf.imageDim.x,         dst_buf.imageDim.y,         dst_buf.imageDim.z   };
		  #else
			copy.bufferOffset		= dst_buf.bufferOffset;
			copy.slicePitch			= dst_buf.bufferSlicePitch;
			copy.imageOffset		= dst_buf.imageOffset;
			copy.imageExtent		= dst_buf.imageDim;
		  #endif

			min = Min( min, dst_buf.imageOffset );
			max = Max( max, dst_buf.imageOffset + dst_buf.imageDim );

			CopyImageToBuffer( handle, dst_buf.bufferHandle, {copy} );
		}
		ASSERT( res.buffers.size() == mem_view.Parts().size() );

		stream.posYZ[0] = max.y - stream.Begin().y;
		stream.posYZ[1] = max.z - stream.Begin().z - 1;

		if_unlikely( stream.posYZ[0] * res.planeScaleY >= stream.RegionSize().y )
		{
			stream.posYZ[0] = 0;
			stream.posYZ[1] ++;
		}

		return ReadbackImageResult{
					Threading::MakePromiseFromValue(
						ImageMemView{ mem_view, min, max - min, res.dataRowPitch, res.dataSlicePitch, res.format, read_desc.aspectMask },
						Tuple{ OnFrameNextCycle{ GetFrameId() }},
						"TransferContext::ReadbackImage",
						ETaskQueue::PerFrame
					)};
	}


} // AE::Graphics (_hidden_)

#undef TRANSFER_CTX
#undef STAGINGBUF_MNGR
