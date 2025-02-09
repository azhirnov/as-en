// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/Resources/ResourceUploadManager.h"

namespace AE::Graphics
{
	using namespace AE::AssetPacker;

namespace {
#	include "Packer/ImagePacker.cpp.h"
}

/*
=================================================
	constructor
=================================================
*/
	ResourceUploadManager::ResourceUploadManager () __NE___
	{
		auto&	scheduler = Scheduler();
		CHECK_FATAL( scheduler.RegisterDependency< UploadResult >( GetRC() ));
		CHECK_FATAL( scheduler.RegisterDependency< WeakUploadResult >( GetRC() ));

		_uploadQueue.reserve( 1u << 8 );
		_transQueue.reserve( 1u << 8 );
	}

/*
=================================================
	destructor
=================================================
*/
	ResourceUploadManager::~ResourceUploadManager () __NE___
	{
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  ResourceUploadManager::Deinitialize () __NE___
	{
		auto&	scheduler = Scheduler();
		scheduler.UnregisterDependency< UploadResult >();
		scheduler.UnregisterDependency< WeakUploadResult >();
	}

/*
=================================================
	Resolve
=================================================
*/
	bool  ResourceUploadManager::Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE___
	{
		// only 'Background' task should depends on UploadTask
		ASSERT( task->QueueType() == ETaskQueue::Background );

		if_likely( auto* res_pp = dep.GetIf< UploadResult >() )
		{
			if_unlikely( *res_pp == null )
				return true;

			auto*	res = (*res_pp).get();	// non-null

			return res->_AddOnCompleteDependency( RVRef(task), INOUT bitIndex, True{"strong"} );
		}

		if_likely( auto* weak_res_pp = dep.GetIf< WeakUploadResult >() )
		{
			if_unlikely( weak_res_pp->_task == null )
				return true;

			auto*	res = weak_res_pp->_task.get();	// non-null

			Unused( res->_AddOnCompleteDependency( RVRef(task), INOUT bitIndex, False{"weak"} ));
			return true;	// always return 'true' because it is weak dependency
		}

		RETURN_ERR( "unsupported dependency type" );
	}

/*
=================================================
	Upload (sync)
=================================================
*/
	void  ResourceUploadManager::Upload (TransferContext_t &ctx, EStagingHeapType heapType) __NE___
	{
		_RecordFirstTransitions( ctx );
		_Upload( ctx, heapType );

		ctx.CommitBarriers();
	}

/*
=================================================
	_Upload
=================================================
*/
	template <typename TransferContext>
	void  ResourceUploadManager::_Upload (TransferContext &ctx, EStagingHeapType heapType) __NE___
	{
		// TODO: set upload limits per task/thread

		using EUploadRes = UploadTask::EUploadRes;

		for (uint cnt = 0, no_mem = 0; cnt < MaxTasksPerFrame; ++cnt)
		{
			if_unlikely( no_mem > MaxNoStagingMemory )
			{
				ASSERT( cnt > 1 );
				break;
			}

			RC<UploadTask>	task;
			{
				EXLOCK( _uploadQueueGuard );

				if_unlikely( _uploadQueue.empty() )
					break;

				task = _uploadQueue.ExtractFront();
			}

			auto	res = task->_Upload( ctx, heapType );
			switch_enum( res )
			{
				case EUploadRes::NoStagingMemory :
					++no_mem;

				case EUploadRes::Downloading :
				case EUploadRes::Uploading :
				{
					EXLOCK( _uploadQueueGuard );
					_uploadQueue.push_front( RVRef(task) );
					break;
				}

				case EUploadRes::Complete :
				case EUploadRes::Failed :
					task->_SetDependencyCompleteStatus( Bool{res == EUploadRes::Complete} );	break;
			}
			switch_end
		}
	}

/*
=================================================
	CreateTask
=================================================
*/
	RC<ResourceUploadManager::UploadTask>  ResourceUploadManager::CreateTask () __NE___
	{
		return MakeRC<UploadTask>();
	}

/*
=================================================
	EnqueueImage
=================================================
*/
	bool  ResourceUploadManager::EnqueueImage (RC<UploadTask> emptyTask, ImageID imageId, const ImagePackerHeader &header,
												RC<AsyncRDataSource> file, Bytes posInFile,
												EUploadFlags flags, EResourceState initialState, EResourceState finalState) __NE___
	{
		CHECK_ERR( emptyTask and emptyTask->_type == Default );
		CHECK_ERR( file and file->IsOpen() );
		CHECK_ERR( imageId );
		CHECK_ERR( ImagePacker_IsValid( header ));

		if_unlikely( initialState == Default )	initialState = InitialState;
		if_unlikely( finalState == Default )	finalState   = FinalState;

		emptyTask->_type		= UploadTask::EType::Image;
		emptyTask->_flags		= flags;
		emptyTask->_curState	= initialState;
		emptyTask->_finalState	= finalState;
		emptyTask->_file		= RVRef(file);
		emptyTask->_baseOffset	= posInFile;
		emptyTask->_memOffset	= 0_b;
		PlacementNew< UploadTask::ImageInfo >( OUT &emptyTask->_info.img, header, imageId );

		// start from lowest mip
		emptyTask->_info.img.currentMip.store( header.mipmaps-1 );

		emptyTask->_ReadNextImageBlock();

		emptyTask->_status.store( UploadTask::EStatus::InProgress );

		// first transition required only if image used every frame
		if ( AllBits( initialState, EResourceState::Invalidate ) and
			 AllBits( flags, EUploadFlags::UsedWhileUploading ))
		{
			emptyTask->_curState = finalState;
			EXLOCK( _transQueueGuard );
			_transQueue.emplace_back( imageId, initialState, finalState );
		}

		EXLOCK( _uploadQueueGuard );
		_uploadQueue.push_back( RVRef(emptyTask) );
		return true;
	}

/*
=================================================
	EnqueueImage
=================================================
*/
	bool  ResourceUploadManager::EnqueueImage (RC<UploadTask> emptyTask, ImageID imageId,
												RC<AsyncRDataSource> file, Bytes posInFile,
												EUploadFlags flags, EResourceState initialState, EResourceState finalState) __NE___
	{
		CHECK_ERR( imageId );

		ImagePackerHeader	header {GraphicsScheduler().GetResourceManager().GetDescription( imageId )};

		return EnqueueImage( RVRef(emptyTask), imageId, header, RVRef(file), posInFile, flags, initialState, finalState );
	}

/*
=================================================
	EnqueueBuffer
=================================================
*/
	bool  ResourceUploadManager::EnqueueBuffer (RC<UploadTask> emptyTask, BufferID bufferId, Bytes size,
												RC<AsyncRDataSource> file, Bytes posInFile,
												EUploadFlags flags, EResourceState initialState, EResourceState finalState) __NE___
	{
		CHECK_ERR( emptyTask and emptyTask->_type == Default );
		CHECK_ERR( file and file->IsOpen() );
		CHECK_ERR( bufferId );
		CHECK_ERR( size > 0 );

		if_unlikely( initialState == Default )	initialState = InitialState;
		if_unlikely( finalState == Default )	finalState   = FinalState;

		emptyTask->_type		= UploadTask::EType::Buffer;
		emptyTask->_flags		= flags;
		emptyTask->_curState	= initialState;
		emptyTask->_finalState	= finalState;
		emptyTask->_file		= RVRef(file);
		emptyTask->_baseOffset	= posInFile;
		emptyTask->_memOffset	= 0_b;
		PlacementNew< UploadTask::BufferInfo >( OUT &emptyTask->_info.buf, bufferId, size );

		emptyTask->_ReadNextBufferBlock();

		emptyTask->_status.store( UploadTask::EStatus::InProgress );

		EXLOCK( _uploadQueueGuard );
		_uploadQueue.push_back( RVRef(emptyTask) );
		return true;
	}

/*
=================================================
	FirstTransition
=================================================
*/
	bool  ResourceUploadManager::FirstTransition (ImageID imageId, EResourceState initialState, EResourceState finalState) __NE___
	{
		CHECK_ERR( imageId );
		CHECK_ERR( finalState != Default and NoBits( finalState, EResourceState::Invalidate ));

		if_unlikely( initialState == Default )	initialState = InitialState;

		EXLOCK( _transQueueGuard );
		_transQueue.emplace_back( imageId, initialState, finalState );
		return true;
	}

/*
=================================================
	_RecordFirstTransitions
=================================================
*/
	template <typename TransferContext>
	void  ResourceUploadManager::_RecordFirstTransitions (TransferContext &ctx) __NE___
	{
		TransQueue_t	queue;
		{
			EXLOCK( _transQueueGuard );
			queue = RVRef(_transQueue);
			_transQueue.clear();
		}

		for (auto& item : queue) {
			ctx.ImageBarrier( item.imageId, item.initialState, item.finalState );
		}
		queue.clear();

		// try to put back allocated memory
		{
			EXLOCK( _transQueueGuard );
			if ( _transQueue.empty() )
				_transQueue = RVRef(queue);
		}
	}

/*
=================================================
	UploadAsync
=================================================
*/
	AsyncTask  ResourceUploadManager::UploadAsync (CommandBatch &batch, uint taskCount, EStagingHeapType heapType, ArrayView<AsyncTask> deps) __NE___
	{
		FixedArray< AsyncTask, MaxUploadRTaskCount >	tasks;

		// TODO: detect memory bandwidth
		taskCount = Clamp( taskCount, 1u, MaxUploadRTaskCount );

		for (uint i = 0; i < taskCount; ++i)
		{
			tasks.push_back( batch.Run< UploadRenderTask >( Tuple{ this, heapType, i==0 }, Tuple{deps} ));
		}

		if ( taskCount == 1 )
			return tasks[0];
		else
			return Scheduler().WaitAsync( ETaskQueue::PerFrame, Tuple{ArrayView<AsyncTask>{tasks}} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	UploadRenderTask ctor
=================================================
*/
	inline ResourceUploadManager::UploadRenderTask::UploadRenderTask
		(ResourceUploadManager* p, EStagingHeapType heapType, bool isFirst, CommandBatchPtr batch, DebugLabel) __NE___ :
		RenderTask{ batch, {"UploadRenderTask"} },
		_self{ p }, _heapType{ heapType }, _isFirst{ isFirst }
	{}

/*
=================================================
	UploadRenderTask::Run
=================================================
*/
	void  ResourceUploadManager::UploadRenderTask::Run () __Th___
	{
		DirectCtx::Transfer		ctx{ *this };

		if ( _isFirst )
			_self->_RecordFirstTransitions( ctx );

		_self->_Upload( ctx, _heapType );
		_self = null;

		ctx.CommitBarriers();

		Execute( ctx );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_AddOnCompleteDependency
=================================================
*/
	bool  ResourceUploadManager::UploadTask::_AddOnCompleteDependency (AsyncTask task, INOUT uint &index, const Bool isStrong) __NE___
	{
		EXLOCK( _depsGuard );

		// skip dependency if already completed
		if_unlikely( bool ok = IsCompleted();  ok or IsFailed() )
			return ok;

		CHECK_ERR( not _deps.IsFull() );	// check for overflow

		TaskDependency	bits;
		bits.bitIndex	= index++;
		bits.isStrong	= isStrong;

		_deps.push_back( RVRef(task), bits );
		return true;
	}

/*
=================================================
	_Upload
=================================================
*/
	template <typename TransferContext>
	auto  ResourceUploadManager::UploadTask::_Upload (TransferContext &ctx, EStagingHeapType heapType) __NE___ -> EUploadRes
	{
		CHECK_ERR( _readReq, EUploadRes::Failed );

		// check download status
		using EStatus = Threading::AsyncDSRequest::Value_t::EStatus;
		switch_enum( _readReq->Status() )
		{
			case EStatus::Completed :
				break;  // upload

			case EStatus::Pending :
			case EStatus::InProgress :
				return EUploadRes::Downloading;

			case EStatus::Destroyed :
			case EStatus::_Finished :
			case EStatus::Cancelled :
			default :
				return EUploadRes::Failed;
		}
		switch_end

		const auto	read_res = _readReq->GetResult();
		ASSERT( _memOffset < read_res.dataSize );

		// upload
		switch_enum( _type )
		{
			case EType::Image :		return _UploadImage( read_res, ctx, heapType );
			case EType::Buffer :	return _UploadBuffer( read_res, ctx, heapType );

			case EType::Unknown :
			default :
				DBG_WARNING( "unknown resource type" );
				return EUploadRes::Failed;
		}
		switch_end
	}

/*
=================================================
	_UploadImage
=================================================
*/
	template <typename TransferContext>
	auto  ResourceUploadManager::UploadTask::_UploadImage (const ReadResult_t &readRes, TransferContext &ctx, EStagingHeapType heapType) __NE___ -> EUploadRes
	{
		UploadImageDesc	upload;
		ImageMemView	mem_view;

		upload.dataRowPitch		= _info.img.rowPitch;
		upload.dataSlicePitch	= _info.img.slicePitch;
		upload.imageOffset		= ImageDim_t{ ushort{0}, _info.img.offsetYZ };
		upload.imageDim			= _info.img.currentDim;
		upload.arrayLayer		= _info.img.currentLayer;
		upload.mipLevel			= MipmapLevel{ _info.img.currentMip.load() };
		upload.heapType			= heapType;

		if_unlikely( _curState != EResourceState::CopyDst )
		{
			ctx.ImageBarrier( _info.img.id, _curState, EResourceState::CopyDst );
			ctx.CommitBarriers();
			_curState = EResourceState::CopyDst;
		}

		NOTHROW_ERR(
			ctx.UploadImage( _info.img.id, upload, readRes.dataSize - _memOffset, OUT mem_view ),
			EUploadRes::Failed  // resource was destroyed
		);

		if_likely( not mem_view.Empty() )
		{
			// must upload whole row
			const ImageDim_t	block_dim = mem_view.DimensionRef();
			CHECK( block_dim.x == _info.img.currentDim.x );
			ASSERT( block_dim.y + _info.img.offsetYZ[0] <= _info.img.currentDim.y );
			ASSERT( block_dim.z + _info.img.offsetYZ[1] <= _info.img.currentDim.z );

			for (auto& part : mem_view.Parts())
			{
				Bytes	size = Min( part.size, readRes.dataSize - _memOffset );
				MemCopy( OUT part.ptr, readRes.data + _memOffset, size );
				_memOffset += size;
			}

			// update offset in image
			_info.img.offsetYZ[0] += block_dim.y;

			if ( _info.img.offsetYZ[0] >= _info.img.currentDim.y )
			{
				_info.img.offsetYZ[0] = 0;
				_info.img.offsetYZ[1] ++;
			}
			if ( _info.img.offsetYZ[1] >= _info.img.currentDim.z )
			{
				_info.img.offsetYZ[1] = 0;
				_info.img.currentLayer ++;
			}
			if ( _info.img.currentLayer.Get() >= _info.img.arrayLayers )
			{
				_info.img.currentLayer = Zero;
				upload.mipLevel --;
				_info.img.currentMip.store( ushort(upload.mipLevel.Get()) );
			}

			// upload complete
			if_unlikely( upload.mipLevel == UMax )
			{
				if ( _curState != _finalState )
					ctx.ImageBarrier( _info.img.id, _curState, _finalState );

				return EUploadRes::Complete;
			}

			if ( _memOffset >= readRes.dataSize )
			{
				ASSERT( _memOffset == readRes.dataSize );
				_ReadNextImageBlock();
			}
		}

		if ( AllBits( _flags, EUploadFlags::UsedWhileUploading ) and
			 _curState != _finalState )
		{
			ctx.ImageBarrier( _info.img.id, _curState, _finalState );
			_curState = _finalState;
		}

		return mem_view.Empty() ? EUploadRes::NoStagingMemory : EUploadRes::Uploading;
	}

/*
=================================================
	_ReadNextImageBlock
=================================================
*/
	inline void  ResourceUploadManager::UploadTask::_ReadNextImageBlock () __NE___
	{
		ASSERT( _type == EType::Image );

		_memOffset = 0_b;

		Bytes		off;
		MipmapLevel	mip {_info.img.currentMip.load()};

		ImagePacker_GetOffset( _info.img, _info.img.currentLayer, mip, ImageDim_t{ ushort{0}, _info.img.offsetYZ },
								OUT _info.img.currentDim, OUT off, OUT _info.img.rowPitch, OUT _info.img.slicePitch );

		CHECK( _info.img.rowPitch <= MaxBlockSize );

		Bytes	level_size	= _info.img.slicePitch * _info.img.currentDim.z;
		Bytes	block_size	= AlignDown( MaxBlockSize, _info.img.rowPitch );
		level_size = Min( level_size, block_size );

		CHECK( level_size != 0 );

		_readReq = _file->ReadBlock( _baseOffset + off, level_size );
	}

/*
=================================================
	_UploadBuffer
=================================================
*/
	template <typename TransferContext>
	auto  ResourceUploadManager::UploadTask::_UploadBuffer (const ReadResult_t &readRes, TransferContext &ctx, EStagingHeapType heapType) __NE___ -> EUploadRes
	{
		UploadBufferDesc	upload;
		BufferMemView		mem_view;

		upload.offset		= _info.buf.offset.load();
		upload.size			= _info.buf.size;
		upload.heapType		= heapType;

		NOTHROW_ERR(
			ctx.UploadBuffer( _info.buf.id, upload, OUT mem_view ),
			EUploadRes::Failed  // resource was destroyed
		);

		if_likely( not mem_view.Empty() )
		{
			for (auto& part : mem_view.Parts())
			{
				Bytes	size = Min( part.size, readRes.dataSize - _memOffset );
				MemCopy( OUT part.ptr, readRes.data + _memOffset, size );

				_memOffset += size;
				upload.offset += size;
			}

			_info.buf.offset.store( upload.offset );

			// upload complete
			if_unlikely( upload.offset >= _info.buf.size )
			{
				ASSERT( upload.offset == _info.buf.size );

				if ( _curState != _finalState )
					ctx.MemoryBarrier( _curState, _finalState );

				return EUploadRes::Complete;
			}

			if ( _memOffset >= readRes.dataSize )
			{
				ASSERT( _memOffset == readRes.dataSize );
				_ReadNextBufferBlock();
			}
		}

		return mem_view.Empty() ? EUploadRes::NoStagingMemory : EUploadRes::Uploading;
	}

/*
=================================================
	_ReadNextBufferBlock
=================================================
*/
	inline void  ResourceUploadManager::UploadTask::_ReadNextBufferBlock () __NE___
	{
		ASSERT( _type == EType::Buffer );

		_memOffset = 0_b;

		const Bytes		offset		= _info.buf.offset.load();
		const Bytes		remain_size = _info.buf.size - offset;

		_readReq = _file->ReadBlock( _baseOffset + offset, remain_size );
	}

/*
=================================================
	_SetDependencyCompleteStatus
----
	Allow to run tasks which waits when request is completed.
	'complete' - indicates that request is successfully complete or canceled/failed.
=================================================
*/
	inline void  ResourceUploadManager::UploadTask::_SetDependencyCompleteStatus (const Bool complete) __NE___
	{
		// cleanup
		{
			_status.store( complete ? EStatus::Complete : EStatus::Failed );
			_readReq	= null;
			_file		= null;
		}

		EXLOCK( _depsGuard );
		for (auto dep : _deps)
		{
			//	weak/strong	& complete	-> complete
			//	strong		& cancelled	-> cancelled
			//	weak		& cancelled	-> complete
			const bool	is_canceled = dep.Get<1>().isStrong and (not complete);

			Threading::IAsyncTask::Helper::SetDependencyCompletionStatus( *dep.Get<0>(), dep.Get<1>().bitIndex, Bool{is_canceled} );
		}
		_deps.clear();
	}

/*
=================================================
	CompletelyUploadedMipmap
=================================================
*/
	MipmapLevel  ResourceUploadManager::UploadTask::CompletelyUploadedMipmap () C_NE___
	{
		ASSERT( _type == EType::Image );
		ASSERT( AllBits( _flags, EUploadFlags::UsedWhileUploading ));

		uint	mip = _info.img.currentMip.load() - 1;
		ASSERT( mip <= _info.img.mipmaps );
		ASSERT( IsCompleted() == (mip == 0) );

		return MipmapLevel{mip};
	}

/*
=================================================
	CompletelyUploadedSize
=================================================
*/
	Bytes  ResourceUploadManager::UploadTask::CompletelyUploadedSize () C_NE___
	{
		ASSERT( _type == EType::Buffer );
		ASSERT( AllBits( _flags, EUploadFlags::UsedWhileUploading ));

		Bytes	off = _info.buf.offset.load();
		ASSERT( off <= _info.buf.size );

		return off;
	}


} // AE::Graphics
