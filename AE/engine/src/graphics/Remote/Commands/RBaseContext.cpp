// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RBaseContext.h"

namespace AE::Graphics::_hidden_
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RSoftwareCmdBuf::RSoftwareCmdBuf () __NE___
	{
		auto&	rts = GraphicsScheduler();

		_memStream = MakeRC<ArrayWStream>();
		_ser.reset( new Serializing::Serializer{ _memStream });
		_ser->factory = &rts.GetDevice().GetCommandsFactory();

		_blockCapacity = rts.GetResourceManager().GetStagingManager().UploadBlockSize();
	}

/*
=================================================
	_Upload
=================================================
*/
	void  RSoftwareCmdBuf::_Upload (OUT Array<Msg::UploadData> &upload, OUT Array<Msg::BaseMsg*> &msgs) __NE___
	{
		Bytes	global_offset;

		for (auto& block : _blocks)
		{
			if ( block.size == 0 )
				break;

			auto&	msg = upload.emplace_back();
			msg.size	= block.size;
			msg.data	= block.data;

			global_offset += _blockCapacity;
		}

		for (auto& [dst, copy] : _hostToDev2)
		{
			ASSERT( copy.begin < copy.end );

			for (Bytes pos, size = copy.end - copy.begin; pos < size;)
			{
				auto&	msg = upload.emplace_back();
				msg.size	= Min( size - pos, _blockCapacity );
				msg.data	= copy.ptr + copy.begin + pos;

				_hostToDev.emplace_back(
					BitCast<RmDevicePtr>( BitCast<ulong>(dst) + copy.begin + pos ),
					BitCast<RmDeviceOffset>( global_offset ),
					msg.size
				);

				global_offset	+= _blockCapacity;
				pos				+= msg.size;
			}
		}

		// upload commands
		{
			auto&	msg = upload.emplace_back();
			msg.size	= _memStream->Position();
			msg.data	= _memStream->GetData().data();
		}

		for (auto& upd : upload)
			msgs.push_back( &upd );
	}

/*
=================================================
	Bake
=================================================
*/
	RmCommandBufferID  RSoftwareCmdBuf::Bake (uint exeIndex, RmCommandBatchID batchId) __NE___
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( batchId );
		CHECK_ERR( exeIndex != UMax );

		auto&	rts = GraphicsScheduler();
		auto&	sbm = rts.GetResourceManager().GetStagingManager();

		_ser.reset();

		Array<Msg::UploadData>	upload;
		Array<Msg::BaseMsg*>	msg_arr;
		_Upload( OUT upload, OUT msg_arr );

		Msg::CmdBuf_Bake				msg;
		RC<Msg::CmdBuf_Bake_Response>	res;
		msg_arr.push_back( &msg );

		msg.batchId		= batchId;
		msg.count		= _cmdCount;
		msg.exeIndex	= exeIndex;
		msg.blockSize	= _blockCapacity;
		msg.hostToDev	= RVRef(_hostToDev);
		msg.devToHost	= RVRef(_devToHost);

		CHECK_ERR( rts.GetDevice().SendAndWait( msg_arr, OUT res ));

		_Cleanup( sbm );
		return res->cmdbuf;
	}

/*
=================================================
	Bake
=================================================
*/
	RmDrawCommandBufferID  RSoftwareCmdBuf::Bake (uint drawIndex, RmDrawCommandBatchID batchId) __NE___
	{
		CHECK_ERR( IsValid() );
		CHECK_ERR( batchId );
		CHECK_ERR( drawIndex != UMax );
		CHECK_ERR( _devToHost.empty() );

		auto&	rts = GraphicsScheduler();
		auto&	sbm = rts.GetResourceManager().GetStagingManager();

		_ser.reset();

		Array<Msg::UploadData>	upload;
		Array<Msg::BaseMsg*>	msg_arr;
		_Upload( OUT upload, OUT msg_arr );

		Msg::CmdBuf_BakeDraw				msg;
		RC<Msg::CmdBuf_BakeDraw_Response>	res;
		msg_arr.push_back( &msg );

		msg.batchId		= batchId;
		msg.count		= _cmdCount;
		msg.exeIndex	= drawIndex;
		msg.blockSize	= _blockCapacity;
		msg.hostToDev	= RVRef(_hostToDev);

		CHECK_ERR( rts.GetDevice().SendAndWait( msg_arr, OUT res ));

		_Cleanup( sbm );
		return res->cmdbuf;
	}

/*
=================================================
	_Cleanup
=================================================
*/
	void  RSoftwareCmdBuf::_Cleanup (auto &sbm) __NE___
	{
		for (auto& block : _blocks) {
			sbm.Dealloc( block.data );
		}
		_blocks.clear();

		_memStream = null;
		_hostToDev.clear();
		_hostToDev2.clear();
		_devToHost.clear();
	}

/*
=================================================
	IsValid
=================================================
*/
	bool  RSoftwareCmdBuf::IsValid () C_NE___
	{
		return _memStream and _memStream->IsOpen() and _ser;
	}

/*
=================================================
	Allocate
=================================================
*/
	void*  RSoftwareCmdBuf::Allocate (const RmDevicePtr devicePtr, const Bytes size) __Th___
	{
		void*			mapped;
		RmDeviceOffset	dev_offset;

		Allocate( OUT mapped, OUT dev_offset, size );  // throw

		_hostToDev.emplace_back( devicePtr, dev_offset, size );
		return mapped;
	}

/*
=================================================
	ReadbackAlloc
=================================================
*/
	void*  RSoftwareCmdBuf::ReadbackAlloc (FrameUID frameId, RmDevicePtr devicePtr, Bytes size) __Th___
	{
		auto&	sbm			= GraphicsScheduler().GetResourceManager().GetStagingManager();
		void*	host_ptr	= sbm.AllocForReadback( frameId, size );

		CHECK_THROW( host_ptr != null );

		_devToHost.emplace_back( devicePtr, BitCast<RmHostPtr>(host_ptr), size );
		return host_ptr;
	}

/*
=================================================
	Allocate
=================================================
*/
	void  RSoftwareCmdBuf::Allocate (OUT void* &mapped, OUT RmDeviceOffset &devOffset, const Bytes size) __Th___
	{
		CHECK_THROW( size <= _blockCapacity );

		for (usize i = 0; i < _blocks.size(); ++i)
		{
			auto&		block	= _blocks[i];
			const Bytes	offset	= block.size;

			if_likely( offset + size <= _blockCapacity )
			{
				block.size	= offset + size;
				mapped		= block.data + offset;
				devOffset	= BitCast<RmDeviceOffset>( offset + i * _blockCapacity );
				return;
			}
		}

		CHECK_THROW_MSG( not _blocks.IsFull(), "overflow" );

		auto&	sbm		= GraphicsScheduler().GetResourceManager().GetStagingManager();
		auto&	block	= _blocks.emplace_back();

		block.size	= size;
		block.data	= sbm.AllocForUpload();
		CHECK_THROW( block.data != null );

		mapped		= block.data;
		devOffset	= BitCast<RmDeviceOffset>( (_blocks.size()-1) * _blockCapacity );
	}

/*
=================================================
	CopyHostToDev
=================================================
*/
	void  RSoftwareCmdBuf::CopyHostToDev (RmDevicePtr dst, void* src, Bytes offset, Bytes size) __Th___
	{
		dst = BitCast<RmDevicePtr>( BitCast<ulong>(dst) - offset );
		src	= src - offset;

		auto&	copy = _hostToDev2[ dst ];
		ASSERT( copy.ptr == null or copy.ptr == src );

		copy.ptr	= src;
		copy.begin	= Min( copy.begin, offset );
		copy.end	= Max( copy.end, offset + size );
	}

/*
=================================================
	PipelineBarrier
=================================================
*/
	void  RSoftwareCmdBuf::PipelineBarrier (const RDependencyInfo &info) __Th___
	{
		ASSERT( info.count > 0 );

		_cmdCount += info.count;
		CHECK_THROW( _ser->stream.Write( info.data, info.size ));

		AddCommand( Msg::CmdBuf_Bake::CommitBarriersCmd{} );
	}

/*
=================================================
	WriteTimestamp
=================================================
*/
	void  RSoftwareCmdBuf::WriteTimestamp (const RQueryManager::Query &q, uint index, EPipelineScope srcScope) __Th___
	{
		GCTX_CHECK( index < q.count );

		Msg::CmdBuf_Bake::WriteTimestampCmd  cmd;
		cmd.query	= q;
		cmd.index	= index;
		cmd.scope	= srcScope;
		AddCommand( cmd );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	_RBaseContext::_RBaseContext (DebugLabel dbg, RSoftwareCmdBufPtr cmdbuf) __Th___ :
		_cmdbuf{ _ReuseOrCreateCommandBuffer( RVRef(cmdbuf), dbg )}
	{
		CHECK_THROW( _IsValid() );
	}

/*
=================================================
	_EndCommandBuffer
=================================================
*/
	RmCommandBufferID  _RBaseContext::_EndCommandBuffer (uint exeIndex, RmCommandBatchID batchId) __Th___
	{
		ASSERT( _IsValid() );

		RSoftwareCmdBufPtr	tmp = RVRef(_cmdbuf);

		return tmp->Bake( exeIndex, batchId );
	}

	RmDrawCommandBufferID  _RBaseContext::_EndCommandBuffer (uint drawIndex, RmDrawCommandBatchID batchId) __Th___
	{
		ASSERT( _IsValid() );

		RSoftwareCmdBufPtr	tmp = RVRef(_cmdbuf);

		return tmp->Bake( drawIndex, batchId );
	}

/*
=================================================
	_ReleaseCommandBuffer
=================================================
*/
	RSoftwareCmdBufPtr  _RBaseContext::_ReleaseCommandBuffer () __Th___
	{
		ASSERT( _IsValid() );

		return RVRef(_cmdbuf);
	}

/*
=================================================
	_ReuseOrCreateCommandBuffer
=================================================
*/
	RSoftwareCmdBufPtr  _RBaseContext::_ReuseOrCreateCommandBuffer (RSoftwareCmdBufPtr cmdbuf, DebugLabel dbg) __Th___
	{
		if ( not (cmdbuf and cmdbuf->IsValid()) )
		{
			cmdbuf.reset( new RSoftwareCmdBuf{} );	// throw
		}
		Unused( dbg );

		return cmdbuf;
	}

/*
=================================================
	_DebugMarker
=================================================
*/
	void  _RBaseContext::_DebugMarker (DebugLabel dbg) __Th___
	{
		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::DebugMarkerCmd{ dbg });
	}

/*
=================================================
	_PushDebugGroup
=================================================
*/
	void  _RBaseContext::_PushDebugGroup (DebugLabel dbg) __Th___
	{
		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::PushDebugGroupCmd{ dbg });
	}

/*
=================================================
	_PopDebugGroup
=================================================
*/
	void  _RBaseContext::_PopDebugGroup () __Th___
	{
		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::PopDebugGroupCmd{} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RBaseContext::RBaseContext (const RenderTask &task, RSoftwareCmdBufPtr cmdbuf, DebugLabel dbg, ECtxType ctxType) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), (dbg ? dbg : DebugLabel( task.DbgFullName(), task.DbgColor() )), ctxType, 0 }
	{}

	inline RBaseContext::RBaseContext (const RenderTask &task, RSoftwareCmdBufPtr cmdbuf, DebugLabel dbg, ECtxType ctxType, int) __Th___ :
		_RBaseContext{ dbg, RVRef(cmdbuf) },
		_mngr{ task }
	{
		switch_enum( ctxType )
		{
			case ECtxType::Transfer :		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::BeginTransfer{ dbg });	break;
			case ECtxType::Compute :		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::BeginCompute{ dbg });	break;
			case ECtxType::Graphics :		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::BeginGraphics{ dbg });	break;
			case ECtxType::ASBuild :		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::BeginASBuild{ dbg });	break;
			case ECtxType::RayTracing :		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::BeginRayTracing{ dbg });	break;
			case ECtxType::RenderPass :
			case ECtxType::VideoDecode :
			case ECtxType::VideoEncode :
			case ECtxType::Unknown :
			default :						CHECK_THROW_MSG( false, "unsupported context type" );
		}
		switch_end

		GFX_DBG_ONLY( _mngr.ProfilerBeginContext( *_cmdbuf, dbg, ctxType );)

		if ( auto bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
		{
			_cmdbuf->PipelineBarrier( *bar );
			GRAPHICS_DBG_SYNC( _DebugMarker({"Task.InitialBarriers"});)
		}
	}

/*
=================================================
	_EndCommandBuffer
=================================================
*/
	RmCommandBufferID  RBaseContext::_EndCommandBuffer (ECtxType ctxType) __Th___
	{
		ASSERT( _NoPendingBarriers() );

		if ( auto bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
		{
			GRAPHICS_DBG_SYNC( _DebugMarker({"Task.FinalBarriers"});)
			_cmdbuf->PipelineBarrier( *bar );
		}
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ctxType ));

		return _RBaseContext::_EndCommandBuffer( _mngr.GetRenderTask().GetExecutionIndex(), _mngr.GetBatch().Handle() );
	}

/*
=================================================
	_ReleaseCommandBuffer
=================================================
*/
	RSoftwareCmdBufPtr  RBaseContext::_ReleaseCommandBuffer (ECtxType ctxType) __Th___
	{
		ASSERT( _IsValid() );
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ctxType ));
		return RVRef(_cmdbuf);
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	void  RBaseContext::_CommitBarriers () __Th___
	{
		auto	bar = _mngr.GetBarriers();
		if_unlikely( bar )
		{
			_cmdbuf->PipelineBarrier( bar );
			_mngr.ClearBarriers();
		}
	}

/*
=================================================
	_WriteTimestamp
=================================================
*/
	void  RBaseContext::_WriteTimestamp (const RQueryManager::Query &q, uint index, EPipelineScope srcScope) __Th___
	{
		GCTX_CHECK( _mngr.GetDevice().HasFeature( RDevice::EFeature::WriteTimestamp ));

		_cmdbuf->WriteTimestamp( q, index, srcScope );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_REMOTE_GRAPHICS
