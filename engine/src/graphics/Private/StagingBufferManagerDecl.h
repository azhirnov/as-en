// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define STBUFMNGR		VStagingBufferManager

#elif defined(AE_ENABLE_METAL)
#	define STBUFMNGR		MStagingBufferManager

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------


	// types
	public:
		
		struct StagingBufferResult
		{
			NativeBuffer_t	buffer		= Default;
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
		  #if defined(AE_ENABLE_VULKAN)
			uint			bufferImageHeight	= 0;	// in pixels, for BufferImageCopy::bufferImageHeight
		  #elif defined(AE_ENABLE_METAL)
			Bytes			bufferSlicePitch;
		  #else
		  #	error not implemented
		  #endif
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
			NativeBuffer_t		bufferHandle	= Default;
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
			
		  #if defined(AE_ENABLE_VULKAN)
			VkDeviceMemory				memoryForWrite			= Default;
			VkDeviceMemory				memoryForRead			= Default;
			VkMemoryPropertyFlagBits	memoryFlagsForRead		= Zero;
			
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
		MappedMemRanges		_memRanges;

		ResourceManager_t&	_resMngr;
		
		DEBUG_ONLY(
			AtomicFrameUID	_frameId;
		)


	// methods
	public:
		explicit STBUFMNGR (ResourceManager_t &resMngr)																			__NE___;
		~STBUFMNGR ()																											__NE___;

		ND_ bool  Initialize (const GraphicsCreateInfo &info)																	__NE___;
			void  Deinitialize ()																								__NE___;
		
			void  OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg)													__NE___;
			void  OnEndFrame (FrameUID frameId)																					__NE___;

			void  GetBufferRanges (OUT BufferRanges_t &result, Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign,
								   FrameUID frameId, EStagingHeapType heap, EQueueType queue, Bool upload)						__NE___;

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const ImageDesc &imgDesc,
								   FrameUID frameId, EQueueType queue, Bool upload)												__NE___;
			
			bool  AllocVStream (FrameUID frameId, Bytes size, OUT VertexStream &result)											__NE___;

		ND_ FrameStat_t  GetFrameStat (FrameUID frameId)																		C_NE___; 

	private:
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
//-----------------------------------------------------------------------------

#undef STBUFMNGR
