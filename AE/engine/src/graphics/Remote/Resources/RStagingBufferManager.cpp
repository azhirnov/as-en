// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RStagingBufferManager.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	GetBufferRanges
=================================================
*/
	void  RStagingBufferManager::GetBufferRanges (OUT BufferRanges_t &result, Bytes reqSize, Bytes blockSize, Bytes memOffsetAlign,
												  FrameUID frameId, EStagingHeapType heap, EQueueType queue, Bool upload) __NE___
	{
		ASSERT( result.empty() );
		ASSERT( _frameId.load() == frameId );

		Msg::SBM_GetBufferRanges				msg;
		RC<Msg::SBM_GetBufferRanges_Response>	res;

		msg.reqSize			= reqSize;
		msg.blockSize		= blockSize;
		msg.memOffsetAlign	= memOffsetAlign;
		msg.frameId			= frameId;
		msg.heap			= heap;
		msg.queue			= queue;
		msg.upload			= upload;

		result.clear();
		CHECK_ERRV( _resMngr.GetDevice().SendAndWait( msg, OUT res ));

		for (usize i = 0, cnt = res->ranges.size(); i < cnt; ++i)
		{
			auto&				src	= res->ranges[i];
			StagingBufferResult	dst;

			CHECK( src.buffer );

			dst.bufferHandle	= src.buffer;
			dst.bufferOffset	= src.bufferOffset;
			dst.size			= src.size;
			dst.devicePtr		= src.mapped;

			result.push_back( dst );
		}
	}

/*
=================================================
	GetImageRanges
=================================================
*/
	void  RStagingBufferManager::GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &uploadDesc, const ImageDesc &imageDesc,
												 const uint3 &imageGranularity, FrameUID frameId, EQueueType queue, Bool upload) __NE___
	{
		ASSERT( _frameId.load() == frameId );
		ASSERT( result.buffers.empty() );

		Msg::SBM_GetImageRanges					msg;
		RC<Msg::SBM_GetImageRanges_Response>	res;

		msg.uploadDesc			= uploadDesc;
		msg.imageDesc			= imageDesc;
		msg.imageGranularity	= imageGranularity;
		msg.frameId				= frameId;
		msg.queue				= queue;
		msg.upload				= upload;

		CHECK_ERRV( _resMngr.GetDevice().SendAndWait( msg, OUT res ));

		result.buffers.clear();
		for (usize i = 0, cnt = res->ranges.size(); i < cnt; ++i)
		{
			auto&				src	= res->ranges[i];
			StagingImageResult	dst;

			CHECK( src.buffer );

			dst.bufferHandle		= src.buffer;
			dst.bufferOffset		= src.bufferOffset;
			dst.size				= src.size;
			dst.devicePtr			= src.mapped;
			dst.imageOffset			= src.imageOffset;
			dst.imageDim			= src.imageDim;
			dst.bufferSlicePitch	= src.bufferSlicePitch;

			result.buffers.push_back( dst );
		}

		result.bufferRowLength	= res->bufferRowLength;
		result.planeScaleY		= res->planeScaleY;
		result.format			= res->format;
		result.dataRowPitch		= res->dataRowPitch;
		result.dataSlicePitch	= res->dataSlicePitch;
	}

/*
=================================================
	GetImageRanges
=================================================
*/
	void  RStagingBufferManager::GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &uploadDesc, const VideoImageDesc &videoDesc,
												 const uint3 &imageGranularity, FrameUID frameId, EQueueType queue, Bool upload) __NE___
	{
		ASSERT( _frameId.load() == frameId );
		ASSERT( result.buffers.empty() );

		Msg::SBM_GetImageRanges2				msg;
		RC<Msg::SBM_GetImageRanges_Response>	res;

		msg.uploadDesc			= uploadDesc;
		msg.videoDesc			= videoDesc;
		msg.imageGranularity	= imageGranularity;
		msg.frameId				= frameId;
		msg.queue				= queue;
		msg.upload				= upload;

		CHECK_ERRV( _resMngr.GetDevice().SendAndWait( msg, OUT res ));

		result.buffers.clear();
		for (usize i = 0, cnt = res->ranges.size(); i < cnt; ++i)
		{
			auto&				src	= res->ranges[i];
			StagingImageResult	dst;

			CHECK( src.buffer );

			dst.bufferHandle		= src.buffer;
			dst.bufferOffset		= src.bufferOffset;
			dst.size				= src.size;
			dst.devicePtr			= src.mapped;
			dst.imageOffset			= src.imageOffset;
			dst.imageDim			= src.imageDim;
			dst.bufferSlicePitch	= src.bufferSlicePitch;

			result.buffers.push_back( dst );
		}

		result.bufferRowLength	= res->bufferRowLength;
		result.planeScaleY		= res->planeScaleY;
		result.format			= res->format;
		result.dataRowPitch		= res->dataRowPitch;
		result.dataSlicePitch	= res->dataSlicePitch;
	}

/*
=================================================
	AllocVStream
=================================================
*/
	bool  RStagingBufferManager::AllocVStream (FrameUID frameId, const Bytes size, OUT VertexStream &result) __NE___
	{
		ASSERT( result.mappedPtr == null );
		ASSERT( _frameId.load() == frameId );

		Msg::SBM_AllocVStream				msg;
		RC<Msg::SBM_AllocVStream_Response>	res;

		msg.frameId	= frameId;
		msg.size	= size;

		CHECK_ERR( _resMngr.GetDevice().SendAndWait( msg, OUT res ));
		CHECK_ERR( res->size == size );
		CHECK_ERR( res->handle );

		auto&	vs = _vstream[ frameId.Index() ];
		CHECK_ERR( vs.mem );

		result.mappedPtr	= vs.mem->Data() + res->offset;
		result.bufferHandle	= res->handle;
		result.offset		= res->offset;
		result.size			= res->size;
		result.devicePtr	= res->mapped;

		if_unlikely( not vs.bufferId.IsValid() )
		{
			RemoteBufferDesc	desc;
			desc.bufferId	= result.bufferHandle;
			desc.desc.size	= vs.mem->Size();
			desc.desc.usage	= EBufferUsage::Vertex | EBufferUsage::Index;
			desc.devicePtr	= BitCast<RmDevicePtr>( BitCast<void*>(res->mapped) - res->offset );
			desc.hostMem	= vs.mem;
			desc.releaseRef	= false;

			auto	id = _resMngr.CreateBuffer( desc, "VStream" );
			CHECK_ERR( id );

			if_likely( vs.bufferId.SetIfEmpty( id.Get() ))
				Unused( id.Release() );
			else
				DEV_CHECK( _resMngr.ImmediatelyRelease2( id ));
		}

		result.id = vs.bufferId.Get();
		ASSERT( result.id );

		return true;
	}

/*
=================================================
	GetFrameStat
=================================================
*/
	RStagingBufferManager::FrameStat_t  RStagingBufferManager::GetFrameStat (FrameUID frameId) C_NE___
	{
		ASSERT( frameId <= _frameId.load() );

		// TODO: cache result

		Msg::SBM_GetFrameStat				msg;
		RC<Msg::SBM_GetFrameStat_Response>	res;

		msg.frameId = frameId;

		CHECK_ERR( _resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->stat;
	}

/*
=================================================
	AllocForReadback
=================================================
*/
	void*  RStagingBufferManager::AllocForReadback (FrameUID frameId, Bytes size) __NE___
	{
		ASSERT( _frameId.load() == frameId );

		auto&	alloc = _readback[ ulong(frameId.Unique()) % _readback.size() ];
		return alloc.Allocate( size );
	}

/*
=================================================
	OnBeginFrame
=================================================
*/
	void  RStagingBufferManager::OnBeginFrame (FrameUID frameId, const BeginFrameConfig &) __NE___
	{
		GFX_DBG_ONLY( _frameId.store( frameId );)

		auto&	alloc = _readback[ ulong(frameId.Unique()) % _readback.size() ];
		alloc.Discard();
	}

/*
=================================================
	OnEndFrame
=================================================
*/
	void  RStagingBufferManager::OnEndFrame (FrameUID frameId) __NE___
	{
		ASSERT( _frameId.load() == frameId );
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  RStagingBufferManager::Initialize (const GraphicsCreateInfo &info) __NE___
	{
		_uploadBlockSize = info.staging.dynamicBlockSize;
		CHECK_ERR( _uploadBlockSize > 0 );

		for (uint i = 0; i < info.maxFrames; ++i)
		{
			_vstream[i].mem = SharedMem::Create( AE::GetDefaultAllocator(), info.staging.vstreamSize );
			CHECK_ERR( _vstream[i].mem );
		}
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  RStagingBufferManager::Deinitialize () __NE___
	{
		for (auto& vs : _vstream)
		{
			auto	id = vs.bufferId.Release();
			DEV_CHECK( _resMngr.ImmediatelyRelease2( id ));
			vs.mem = null;
		}

		for (auto& alloc : _readback)
			alloc.Release();

		_upload.Release( [this](void* ptr) __NE___ { UntypedAllocator::Deallocate( ptr, _uploadBlockSize ); });
	}

/*
=================================================
	AllocForUpload
=================================================
*/
	void*  RStagingBufferManager::AllocForUpload () __NE___
	{
		void*	ptr = null;
		if_likely( _upload.Extract( OUT ptr ))
			return ptr;

		return UntypedAllocator::Allocate( _uploadBlockSize );
	}

/*
=================================================
	Dealloc
=================================================
*/
	void  RStagingBufferManager::Dealloc (void* ptr) __NE___
	{
		CHECK( ptr != null );

		if_unlikely( not _upload.Put( ptr ))
			UntypedAllocator::Deallocate( ptr, _uploadBlockSize );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
