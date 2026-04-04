// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Public/CommandBufferTypes.h"
# include "graphics_rhi/Public/ResourceManager.h"
# include "graphics_rhi/Private/StagingBufferShared.h"
# include "graphics_rhi/Remote/RDevice.h"

namespace AE::Graphics
{

	//
	// Staging Buffer Manager
	//

	class RStagingBufferManager final : public StagingBufferShared
	{
	// types
	private:
		static constexpr Bytes	_ReadbackBlockSize = 64_MiB;

		using RBAllocator_t = Threading::LfLinearAllocator< usize{_ReadbackBlockSize}, AE_CACHE_LINE, 16 >;
		using UpAllocator_t	= Threading::LfStaticPool< void*, 32 >;

		struct VStream
		{
			StrongAtom<BufferID>	bufferId;
			RC<SharedMem>			mem;
		};

		using VStreamArr_t	= StaticArray< VStream, GraphicsConfig::MaxFrames >;
		using ReadbackArr_t	= StaticArray< RBAllocator_t, GraphicsConfig::MaxFrames+2 >;


	// variables
	private:
		ResourceManager&	_resMngr;
		Bytes				_uploadBlockSize;
		UpAllocator_t		_upload;
		VStreamArr_t		_vstream;
		ReadbackArr_t		_readback;

		GFX_DBG_ONLY(
			AtomicFrameUID	_frameId;
		)


	// methods
	public:
		explicit RStagingBufferManager (ResourceManager &resMngr)																__NE___ : _resMngr{resMngr} {}
		~RStagingBufferManager ()																								__NE___ {}

		ND_ bool  Initialize (const GraphicsCreateInfo &)																		__NE___;
			void  Deinitialize ()																								__NE___;

			void  OnBeginFrame (FrameUID, const BeginFrameConfig &)																__NE___;
			void  OnEndFrame (FrameUID)																							__NE___;

			void  GetBufferRanges (OUT BufferRanges_t &result, Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign,
								   FrameUID frameId, EStagingHeapType heap, Bool upload)										__NE___;

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const ImageDesc &,
								  Bytes maxSize, const uint3 &imageGranularity, FrameUID frameId, Bool upload)					__NE___;

			void  GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &desc, const VideoImageDesc &,
								  Bytes maxSize, const uint3 &imageGranularity, FrameUID frameId, Bool upload)					__NE___;

			bool  AllocVStream (FrameUID frameId, Bytes size, OUT VertexStream &result)											__NE___;

		ND_ void*  AllocForReadback (FrameUID frameId, Bytes size)																__NE___;

		ND_ Bytes  UploadBlockSize ()																							C_NE___	{ return _uploadBlockSize; }
		ND_ void*  AllocForUpload ()																							__NE___;
			void   Dealloc (void* ptr)																							__NE___;

		ND_ FrameStat_t  GetFrameStat (FrameUID)																				C_NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
