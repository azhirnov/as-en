// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	hint:
		upload		- write	(on host)	- from host to device
		readback	- read  (on host)	- from device to host

	Upload staging buffer must use coherent memory and doesn't require FlushMemoryRanges.
	Readback staging buffer can use cached non-coherent memory.
	Vertex buffer use unified memory and doesn't require FlushMemoryRanges.
*/

#if defined(AE_ENABLE_VULKAN)
#	define STBUFMNGR		VStagingBufferManager

#elif defined(AE_ENABLE_METAL)
#	define STBUFMNGR		MStagingBufferManager

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------

namespace AE::Graphics
{

	//
	// Staging Buffer Manager
	//

	class STBUFMNGR final
	{
	// types
	public:

	  #if defined(AE_ENABLE_VULKAN)
		using NativeBuffer_t		= VkBuffer;
		using NativeMemObjInfo_t	= VulkanMemoryObjInfo;

	  #elif defined(AE_ENABLE_METAL)
		using NativeBuffer_t		= MetalBuffer;
		using NativeMemObjInfo_t	= MetalMemoryObjInfo;

	  #else
	  #	error not implemented
	  #endif


		struct StagingBufferResult
		{
			void *			mapped			= null;
			NativeBuffer_t	bufferHandle	= Default;
			BufferID		bufferId;
			Bytes32u		bufferOffset;
			Bytes32u		size;

			StagingBufferResult ()	__NE___ {}
		};
		using BufferRanges_t = FixedArray< StagingBufferResult, BufferMemView::Count >;

		struct StagingImageResult : StagingBufferResult
		{
			// additional params are required to copy between staging buffer and image
			uint3			imageOffset;
			uint3			imageDim;
		  #if defined(AE_ENABLE_VULKAN)
			uint			bufferImageHeight	= 0;	// in pixels, for BufferImageCopy::bufferImageHeight
		  #elif defined(AE_ENABLE_METAL)
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
			uint			planeScaleY			= 0;
			EPixelFormat	format				= Default;	// used for multiplanar image, otherwise equal to image desc
			Bytes			dataRowPitch;
			Bytes			dataSlicePitch;
		};

		using FrameStat_t = IResourceManager::StagingBufferStat;


	private:
		class StaticMemAllocator;
		class DynamicMemAllocator;

		struct alignas(AE_CACHE_LINE) StaticBuffer
		{
			AtomicByte<uint>	size			{0_b};
			Bytes32u			capacity;
		  #ifdef AE_ENABLE_VULKAN
			Bytes32u			memOffset;
		  #endif
			Strong<BufferID>	bufferId;
			NativeBuffer_t		bufferHandle	= Default;
			void*				mapped			= null;
		};
		using SizePerQueue_t	= GraphicsCreateInfo::SizePerQueue_t;

		static constexpr uint	_QueueCount		= uint(EQueueType::_Count);
		static constexpr Bytes	_BlockAlign		{16};

		struct Static
		{
			using PerFrame_t = FixedArray< StaticBuffer, GraphicsConfig::MaxFrames * uint(EQueueType::_Count) >;

			PerFrame_t					buffersForWrite;
			PerFrame_t					buffersForRead;

		  #if defined(AE_ENABLE_VULKAN)
			VkDeviceMemory				memoryForWrite			= Default;
			VkDeviceMemory				memoryForRead			= Default;
			bool						isReadCoherent			= false;

		  #elif defined(AE_ENABLE_METAL)
			MetalMemoryRC				memory;

		  #else
		  #	error not implemented
		  #endif

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
		  #if defined(AE_ENABLE_VULKAN)
			VkDeviceMemory		memory	= Default;

		  #elif defined(AE_ENABLE_METAL)
		  #else
		  #	error not implemented
		  #endif
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
			AtomicByte<uint>			maxPerFrame		{0_b};
			mutable AtomicByte<uint>	usedPerFrame	[2];
			mutable AtomicByte<ulong>	allocated		{0_b};
			DynamicBuffers				buffers;

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

		  #if defined(AE_ENABLE_VULKAN)
			VkDeviceMemory		memory	= Default;

		  #elif defined(AE_ENABLE_METAL)
			MetalMemoryRC		memory;

		  #else
		  #	error not implemented
		  #endif
		};


	  #if defined(AE_ENABLE_VULKAN)
		struct alignas(AE_CACHE_LINE) MappedMemRanges
		{
			using PerFrameRanges_t = FixedArray< Array<VkMappedMemoryRange>, GraphicsConfig::MaxFrames >;

			Threading::SpinLock		guard;	// only single frame is active so no need to create lock per frame
			PerFrameRanges_t		ranges;
		};

	  #elif defined(AE_ENABLE_METAL)
		struct alignas(AE_CACHE_LINE) MappedMemRanges
		{
			// TODO
		};

	  #else
	  #	error not implemented
	  #endif


	// variables
	private:
		Static				_static;
		Dynamic				_dynamic;
		StaticVStream		_vstream;

		Bytes16u			_memSizeAlign	{1};
		MappedMemRanges		_memRanges;

		ResourceManager&	_resMngr;

		GFX_DBG_ONLY(
			AtomicFrameUID	_frameId;
		)


	// methods
	public:
		explicit STBUFMNGR (ResourceManager &resMngr)																			__NE___;
		~STBUFMNGR ()																											__NE___;

		ND_ bool  Initialize (const GraphicsCreateInfo &info)																	__NE___;
			void  Deinitialize ()																								__NE___;

			void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg)													__NE___;
			void  OnEndFrame (FrameUID frameId)																					__NE___;

			void  GetBufferRanges (OUT BufferRanges_t &result, Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign,
								   FrameUID frameId, EStagingHeapType heap, EQueueType queue, Bool upload)						__NE___;

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const ImageDesc &,
								  const uint3 &imageGranularity, FrameUID frameId, EQueueType queue, Bool upload)				__NE___;

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const VideoImageDesc &,
								  const uint3 &imageGranularity, FrameUID frameId, EQueueType queue, Bool upload)				__NE___;

			bool  AllocVStream (FrameUID frameId, Bytes size, OUT VertexStream &result)											__NE___;

		ND_ FrameStat_t  GetFrameStat (FrameUID frameId)																		C_NE___;

		  #ifdef AE_ENABLE_VULKAN
			void  AcquireMappedMemory (FrameUID frameId, VkDeviceMemory memory, Bytes offset, Bytes size)						__NE___;
		  #endif


	private:
		ND_ bool  _CreateStaticBuffers (const GraphicsCreateInfo &info)															__NE___;
		ND_ bool  _InitDynamicBuffers (const GraphicsCreateInfo &info)															__NE___;
		ND_ bool  _InitVertexStream (const GraphicsCreateInfo &info)															__NE___;

		ND_ Bytes  _CalcBlockSize (Bytes reqSize, EStagingHeapType heap, EQueueType queue, bool upload)							C_NE___;

		template <typename RangeType, typename BufferType>
		ND_ static bool  _AllocStatic (Bytes32u reqSize, Bytes32u blockSize, Bytes32u memOffsetAlign,
										INOUT RangeType &result, BufferType& sb)												__NE___;

		template <bool SingleAlloc, typename RangeType>
		ND_ bool  _AllocDynamic (FrameUID frameId, INOUT Bytes &reqSize, Bytes blockSize, Bytes memOffsetAlign, bool upload,
								 INOUT RangeType& buffers, DynamicBuffers &db)													C_NE___;

		template <typename RangeType>
		ND_ bool  _AddToCurrent (INOUT Bytes &reqSize, Bytes blockSize, Bytes memOffsetAlign, Strong<BufferID> id,
								 INOUT RangeType& buffers, DynamicBuffers &db)													C_NE___;

		ND_ static bool  _AllocStaticImage (Bytes reqSize, Bytes rowPitch, Bytes slicePitch, Bytes memOffsetAlign,
											const uint2 &texelBlockDim, const uint3 &imageOffset, const uint3 &imageDim,
											INOUT StagingImageResultRanges &result, StaticBuffer& sb)							__NE___;

		void  _AllocDynamicImage (FrameUID frameId, Bytes reqSize, Bytes rowPitch, Bytes slicePitch, Bytes memOffsetAlign,
								  const uint2 &texelBlockDim, const uint3 &imageOffset, const uint3 &imageDataDim, bool upload,
								  INOUT StagingImageResultRanges &result, DynamicBuffers& db)									C_NE___;
	};


} // AE::Graphics
//-----------------------------------------------------------------------------

#undef STBUFMNGR
