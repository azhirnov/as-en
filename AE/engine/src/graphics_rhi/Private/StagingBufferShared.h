// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

namespace AE::Graphics
{

	//
	// Staging Buffer Shared types
	//

	class StagingBufferShared
	{
	// types
	public:

	  #if defined(AE_ENABLE_VULKAN)
		using NativeBuffer_t		= VkBuffer;
		using NativeMemObjInfo_t	= VulkanMemoryObjInfo;

	  #elif defined(AE_ENABLE_METAL)
		using NativeBuffer_t		= MetalBuffer;
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)

	  #else
	  #	error not implemented
	  #endif


	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		struct StagingBufferResult
		{
			RmBufferID		bufferHandle;
			Bytes			bufferOffset;
			Bytes			size;
			RmDevicePtr		devicePtr	= Default;

			StagingBufferResult ()	__NE___ {}
		};
	  #else

		struct StagingBufferResult
		{
			void *			mapped			= null;
			NativeBuffer_t	bufferHandle	= Default;
			BufferID		bufferId;
			Bytes32u		bufferOffset;
			Bytes32u		size;

			StagingBufferResult ()	__NE___ {}
		};
	  #endif

		using BufferRanges_t = FixedArray< StagingBufferResult, BufferMemView::Count >;

		struct StagingImageResult : StagingBufferResult
		{
			// additional params are required to copy between staging buffer and image
			uint3			imageOffset;
			uint3			imageDim;
		  #if defined(AE_ENABLE_VULKAN)
			uint			bufferImageHeight	= 0;	// in pixels, for BufferImageCopy::bufferImageHeight
		  #elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
			Bytes			bufferSlicePitch;
		  #else
		  #	error not implemented
		  #endif
			StagingImageResult ()	__NE___ {}
		};
		using ImageRanges_t = FixedArray< StagingImageResult, ImageMemView::Count >;

		struct StagingImageResultRanges
		{
			ImageRanges_t	buffers;
			uint			bufferRowLength		= 0;		// in pixels, for BufferImageCopy::bufferRowLength
			POTValue		planeScaleY;
			EPixelFormat	format				= Default;	// used for multiplanar image, otherwise equal to image desc
			Bytes			dataRowPitch;
			Bytes			dataSlicePitch;
			uint3			regionDim;						// validated dimension of current image mip level minus offset
		};

		using FrameStat_t = IResourceManager::StagingBufferStat;
	};

} // AE::Graphics
