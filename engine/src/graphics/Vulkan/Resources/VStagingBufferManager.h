// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	hint:
		upload		- write	(on host)	- from host to device
		readback	- read  (on host)	- from device to host

	Upload staging buffer must use coherent memory and doesn't require FlushMemoryRanges.
	Readback staging buffer can use cached non-coherent memory.
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "base/Containers/RingBuffer.h"
# include "graphics/Public/CommandBufferTypes.h"
# include "graphics/Public/ResourceManager.h"
# include "graphics/Vulkan/VDevice.h"

namespace AE::Graphics
{

	//
	// Vulkan Staging Buffer Manager
	//

	class VStagingBufferManager final
	{
	// types
	public:
		struct StagingBufferResult
		{
			VkBuffer		buffer		= Default;
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
			uint			bufferImageHeight	= 0;	// in pixels, for BufferImageCopy::bufferImageHeight
		};
		using ImageRanges_t = FixedArray< StagingImageResult, ImageMemView::Count >;
		
		struct StagingImageResultRanges
		{
			ImageRanges_t	buffers;
			uint			bufferRowLength		= 0;	// in pixels, for BufferImageCopy::bufferRowLength
			Bytes			dataRowPitch;
			Bytes			dataSlicePitch;
		};

		using FrameStat_t = IResourceManager::StagingBufferStat;


	private:
		class StaticMemAllocator;
		class DynamicMemAllocator;

		struct alignas(AE_CACHE_LINE) StaticBuffer
		{
			BytesAtomic<uint>	size			{0_b};
			Bytes32u			capacity;
			Bytes32u			memOffset;
			Strong<BufferID>	buffer;
			VkBuffer			bufferHandle	= Default;
			void*				mapped			= null;
		};
		using SizePerQueue_t	= GraphicsCreateInfo::SizePerQueue_t;

		static constexpr uint	_QueueCount		= uint(EQueueType::_Count);
		static constexpr uint	_PartsCount		= 4;
		static constexpr Bytes	_BlockAlign		{16};

		struct Static
		{
			using PerFrame_t = FixedArray< StaticBuffer, GraphicsConfig::MaxFrames * uint(EQueueType::_Count) >;

			PerFrame_t					buffersForWrite;
			PerFrame_t					buffersForRead;
			VkDeviceMemory				memoryForWrite			= Default;
			VkDeviceMemory				memoryForRead			= Default;
			VkMemoryPropertyFlagBits	memoryFlagsForRead		= Zero;
			SizePerQueue_t				writeSize;
			SizePerQueue_t				readSize;
		};

		struct AvailableBuffer
		{
			Strong<BufferID>	id;
			FrameUID			lastUsage;
		};

		struct DynamicBuffer : StaticBuffer
		{
			VkDeviceMemory		memory	= Default;
		};
		
		struct DynamicBuffers
		{
			using Current_t		= FixedArray< DynamicBuffer, 8 >;
			using Available_t	= RingBuffer< AvailableBuffer >;
			
			alignas(AE_CACHE_LINE)	RWSpinLock		currentGuard;
									Current_t		current;

			alignas(AE_CACHE_LINE)	SpinLock		availableGuard;
									Available_t		available;
		};
		
		struct DynamicPerType
		{
			// mutable
			BytesAtomic<uint>			maxPerFrame		{0_b};
			mutable BytesAtomic<uint>	usedPerFrame	{0_b};
			DynamicBuffers				buffers;
			mutable BytesAtomic<ulong>	allocated		{0_b};

			// immutable
			Bytes						maxSize;
			EMemoryType					memType			= Default;
		};

		struct Dynamic
		{
			DynamicPerType			write;
			DynamicPerType			read;

			// immutable
			GfxMemAllocatorPtr		gfxAllocator;
			Bytes					blockSize;
			uint					maxFramesToRelease	= UMax;
		};

		struct StaticVStream
		{
			using PerFrame_t = StaticArray< StaticBuffer, GraphicsConfig::MaxFrames >;
			
			PerFrame_t			buffers;
			VkDeviceMemory		memory	= Default;
		};
		
		struct alignas(AE_CACHE_LINE) MappedMemRanges
		{
			using PerFrameRanges_t = FixedArray< Array<VkMappedMemoryRange>, GraphicsConfig::MaxFrames >;

			Threading::SpinLock		guard;	// only single frame is active so no need to create lock per frame
			PerFrameRanges_t		ranges;
		};


	// variables
	private:
		Static				_static;
		Dynamic				_dynamic;
		StaticVStream		_vstream;
		MappedMemRanges		_memRanges;

		VResourceManager&	_resMngr;
		
		Bytes				_maxHostMemory;
		
		DEBUG_ONLY(
			AtomicFrameUID	_frameId;
		)


	// methods
	public:
		explicit VStagingBufferManager (VResourceManager &resMngr);
		~VStagingBufferManager ();

		ND_ bool  Initialize (const GraphicsCreateInfo &info);
			void  Deinitialize ();
		
			void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg);
			void  OnEndFrame (FrameUID frameId);

			void  GetBufferRanges (OUT BufferRanges_t &result, Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign,
								   FrameUID frameId, EStagingHeapType heap, EQueueType queue, Bool upload);

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const ImageDesc &imgDesc,
								   FrameUID frameId, EQueueType queue, Bool upload);
			
			bool  AllocVStream (FrameUID frameId, Bytes size, OUT VertexStream &result);

			void  AcquireMappedMemory (FrameUID frameId, VkDeviceMemory memory, Bytes offset, Bytes size);

		ND_ FrameStat_t  GetFrameStat (FrameUID frameId) const; 

	private:
		ND_ bool  _CheckHostVisibleMemory (OUT Bytes& totalHostMem) const;
		ND_ bool  _CreateStaticBuffers (const GraphicsCreateInfo &info);
		ND_ bool  _InitDynamicBuffers (const GraphicsCreateInfo &info);
		ND_ bool  _InitVertexStream (const GraphicsCreateInfo &info);

		ND_ Bytes  _CalcBlockSize (Bytes reqSize, EStagingHeapType heap, EQueueType queue, bool upload) const;
		
		template <typename RangeType, typename BufferType>
		ND_ static bool  _AllocStatic (Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign, INOUT RangeType &result, BufferType& sb);
		
		template <bool SingleAlloc, typename RangeType>
		ND_ bool  _AllocDynamic (FrameUID frameId, INOUT Bytes &reqSize, Bytes blockSize, Bytes memOffsetAlign, bool upload, INOUT RangeType& buffers, DynamicBuffers &db) const;
		
		template <typename RangeType>
		ND_ bool  _AddToCurrent (INOUT Bytes &reqSize, Bytes blockSize, Bytes memOffsetAlign, Strong<BufferID> id, INOUT RangeType& buffers, DynamicBuffers &db) const;

		ND_ static bool  _AllocStaticImage (Bytes reqSize, Bytes rowPitch, Bytes slicePitch, Bytes memOffsetAlign, const uint2 &texelBlockSize,
											const uint3 &imageOffset, const uint3 &imageSize,
											INOUT StagingImageResultRanges &result, StaticBuffer& sb);
	
		void  _AllocDynamicImage (FrameUID frameId, Bytes reqSize, Bytes rowPitch, Bytes slicePitch, Bytes memOffsetAlign, const uint2 &texelBlockSize,
								  const uint3 &imageOffset, const uint3 &imageDataSize, bool upload,
								  INOUT StagingImageResultRanges &result, DynamicBuffers& db) const;
	};
	

}	// AE::Graphics

#endif	// AE_ENABLE_VULKAN
