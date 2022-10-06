// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	hint:
		upload		- write	(on host)	- from host to device
		readback	- read  (on host)	- from device to host

	Upload staging buffer must use unified (managed?) memory.
	Readback staging buffer can use unified (managed?) memory.
*/

#pragma once

#ifdef AE_ENABLE_METAL

# include "base/Containers/RingBuffer.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Metal/MDevice.h"

namespace AE::Graphics
{
	
	//
	// Metal Staging Buffer Manager
	//

	class MStagingBufferManager final
	{
	// types
	public:
		struct StagingBufferResult
		{
			MetalBuffer		buffer;
			Bytes			bufferOffset;
			Bytes			size;
			void *			mapped		= null;
		};
		using BufferRanges_t = FixedArray< StagingBufferResult, BufferMemView::Count >;

		struct StagingImageResult : StagingBufferResult
		{
			// additional params are required to copy between staging buffer and image
			uint3			imageOffset;
			uint3			imageSize;
			Bytes			bufferSlicePitch;
		};
		using ImageRanges_t = FixedArray< StagingImageResult, ImageMemView::Count >;

		struct StagingImageResultRanges
		{
			ImageRanges_t	buffers;
			Bytes			dataRowPitch;
			Bytes			dataSlicePitch;
		};

		using FrameStat_t = IResourceManager::StagingBufferStat;


	private:
		struct alignas(AE_CACHE_LINE) StaticBuffer
		{
			BytesAtomic<uint>	size			{0_b};
			Bytes32u			capacity;
			Strong<BufferID>	buffer;
			MetalBuffer			bufferHandle;
			void*				mapped			= null;
		};
		using SizePerQueue_t	= GraphicsCreateInfo::SizePerQueue_t;
		
		struct Static
		{
			using PerFrame_t = FixedArray< StaticBuffer, GraphicsConfig::MaxFrames * uint(EQueueType::_Count) >;

			PerFrame_t			buffersForWrite;
			PerFrame_t			buffersForRead;
			MetalMemory			memory;
			SizePerQueue_t		writeSize;
			SizePerQueue_t		readSize;
		};


	// variables
	private:
		Static				_static;

		MResourceManager&	_resMngr;

		DEBUG_ONLY(
			AtomicFrameUID	_frameId;
		)


	// methods
	public:
		explicit MStagingBufferManager (MResourceManager &resMngr);
		~MStagingBufferManager ();

		ND_ bool  Initialize (const GraphicsCreateInfo &info);
			void  Deinitialize ();
		
			void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg);
			void  OnEndFrame (FrameUID frameId);

			void  GetBufferRanges (OUT BufferRanges_t &result, Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign,
								   FrameUID frameId, EStagingHeapType heap, EQueueType queue, Bool upload);

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const ImageDesc &imgDesc,
								   FrameUID frameId, EQueueType queue, Bool upload);
			
			bool  AllocVStream (FrameUID frameId, Bytes size, OUT VertexStream &result);

		ND_ FrameStat_t  GetFrameStat (FrameUID frameId) const;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
