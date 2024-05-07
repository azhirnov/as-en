// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Private/EnumToString.h"
# include "graphics/Remote/Commands/RBarrierManager.h"
# include "graphics/Remote/RRenderTaskScheduler.h"
# include "graphics/Remote/REnumCast.h"

namespace AE::Graphics::_hidden_
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RBarrierManager::RBarrierManager (const RenderTask &task) __NE___ :
		_resMngr{ GraphicsScheduler().GetResourceManager() },
		_batch{ *task.GetBatchPtr() },
		_task{ &task }
	{
		ClearBarriers();
	}

	RBarrierManager::RBarrierManager (RCommandBatch &batch) __NE___ :
		_resMngr{ GraphicsScheduler().GetResourceManager() },
		_batch{ batch },
		_task{ null }
	{
		ClearBarriers();
	}

/*
=================================================
	GetBarriers
=================================================
*/
	RDependencyInfo  RBarrierManager::GetBarriers () __NE___
	{
		RDependencyInfo		result;
		if_unlikely( _cmdCount > 0 )
		{
			_ser.reset();
			auto	data = _memStream->GetData();

			result.data		= data.data();
			result.size		= ArraySizeOf(data);
			result.count	= _cmdCount;
		}
		return result;
	}

/*
=================================================
	AllocBarriers
=================================================
*/
	Ptr<const RDependencyInfo>  RBarrierManager::AllocBarriers () __NE___
	{
		if ( auto bar = GetBarriers() )
		{
			auto&	alloc	= MemoryManager().GetGraphicsFrameAllocator().Get( GetFrameId() );
			void*	mem		= alloc.Allocate( SizeOf<RDependencyInfo> + bar.size );
			auto&	res		= *Cast<RDependencyInfo>( mem );

			CHECK_ERR( mem != null );

			MemCopy( OUT mem + SizeOf<RDependencyInfo>, bar.data, bar.size );

			res.data	= mem + SizeOf<RDependencyInfo>;
			res.size	= bar.size;
			res.count	= bar.count;

			ClearBarriers();

			return &res;
		}
		return null;
	}

/*
=================================================
	MergeBarriers
=================================================
*/
	void  RBarrierManager::MergeBarriers (INOUT RBarrierManager &mngr) __NE___
	{
		mngr._ser.reset();
		auto	data = mngr._memStream->GetData();

		_cmdCount += mngr._cmdCount;
		CHECK( _ser->stream.Write( data.data(), ArraySizeOf(data) ));

		mngr.ClearBarriers();
	}

/*
=================================================
	ClearBarriers
=================================================
*/
	void  RBarrierManager::ClearBarriers () __NE___
	{
		_memStream	= MakeRC<ArrayWStream>();
		_cmdCount	= 0;
		_ser.reset( new Serializing::Serializer{ _memStream });
		_ser->factory = &GraphicsScheduler().GetDevice().GetCommandsFactory();
	}

/*
=================================================
	BeforeBeginRenderPass
=================================================
*/
	bool  RBarrierManager::BeforeBeginRenderPass (const RenderPassDesc &desc, OUT RPrimaryCmdBufState &primaryState) __NE___
	{
		CHECK_ERR( not primaryState.IsValid() );

		auto&			res_mngr	= GetResourceManager();
		RenderPassID	rp_id		= res_mngr.GetRenderPass( desc.packId, desc.renderPassName );

		primaryState.renderPass		= res_mngr.GetResource( rp_id, False{"don't inc ref"}, True{"quiet"} );
		CHECK_ERR( primaryState.renderPass );

		primaryState.frameId		= GetFrameId();
		primaryState.subpassIndex	= 0;
		primaryState.userData		= GetBatch().GetUserData();

		// same as VFramebuffer
		primaryState.fbImages.clear();
		primaryState.fbImages.resize( desc.attachments.size() );

		for (auto [name, att] : desc.attachments)
		{
			auto	idx = primaryState.renderPass->GetAttachmentIndex( name );
			CHECK_ERR( idx < primaryState.fbImages.size() );

			auto*	view = res_mngr.GetResource( att.imageView );
			CHECK_ERR( view != null );

			primaryState.fbImages[ idx ] = view->ImageId();
		}

		return true;
	}

/*
=================================================
	***Barrier
=================================================
*/
	void  RBarrierManager::BufferBarrier (BufferID bufferId, EResourceState srcState, EResourceState dstState) __Th___
	{
		auto&	buf = _resMngr.GetResourcesOrThrow( bufferId );

		Msg::CmdBuf_Bake::BufferBarrierCmd  cmd;
		cmd.buffer		= buf.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::BufferViewBarrier (BufferViewID viewId, EResourceState srcState, EResourceState dstState) __Th___
	{
		auto&	view = _resMngr.GetResourcesOrThrow( viewId );

		Msg::CmdBuf_Bake::BufferViewBarrierCmd  cmd;
		cmd.bufferView	= view.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ImageBarrier (ImageID imageId, EResourceState srcState, EResourceState dstState) __Th___
	{
		auto&	image = _resMngr.GetResourcesOrThrow( imageId );

		Msg::CmdBuf_Bake::ImageBarrierCmd  cmd;
		cmd.image		= image.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ImageBarrier (ImageID imageId, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes) __Th___
	{
		auto&	image = _resMngr.GetResourcesOrThrow( imageId );

		Msg::CmdBuf_Bake::ImageRangeBarrierCmd  cmd;
		cmd.image		= image.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		cmd.subRes		= subRes;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ImageViewBarrier (ImageViewID viewId, EResourceState srcState, EResourceState dstState) __Th___
	{
		auto&	view = _resMngr.GetResourcesOrThrow( viewId );

		Msg::CmdBuf_Bake::ImageViewBarrierCmd  cmd;
		cmd.imageView	= view.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::MemoryBarrier (EResourceState srcState, EResourceState dstState) __Th___
	{
		Msg::CmdBuf_Bake::MemoryBarrierCmd  cmd;
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope) __Th___
	{
		Msg::CmdBuf_Bake::MemoryBarrier2Cmd  cmd;
		cmd.srcScope	= srcScope;
		cmd.dstScope	= dstScope;
		_AddCommand( cmd );
	}

	void  RBarrierManager::MemoryBarrier () __Th___
	{
		Msg::CmdBuf_Bake::MemoryBarrier3Cmd  cmd;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope) __Th___
	{
		Msg::CmdBuf_Bake::ExecutionBarrierCmd  cmd;
		cmd.srcScope	= srcScope;
		cmd.dstScope	= dstScope;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ExecutionBarrier () __Th___
	{
		Msg::CmdBuf_Bake::ExecutionBarrier2Cmd  cmd;
		_AddCommand( cmd );
	}

	void  RBarrierManager::AcquireBufferOwnership (BufferID bufferId, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) __Th___
	{
		auto&	buf = _resMngr.GetResourcesOrThrow( bufferId );

		Msg::CmdBuf_Bake::AcquireBufferOwnershipCmd  cmd;
		cmd.buffer		= buf.Handle();
		cmd.srcQueue	= srcQueue;
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ReleaseBufferOwnership (BufferID bufferId, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) __Th___
	{
		auto&	buf = _resMngr.GetResourcesOrThrow( bufferId );

		Msg::CmdBuf_Bake::ReleaseBufferOwnershipCmd  cmd;
		cmd.buffer		= buf.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		cmd.dstQueue	= dstQueue;
		_AddCommand( cmd );
	}

	void  RBarrierManager::AcquireImageOwnership (ImageID imageId, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) __Th___
	{
		auto&	image = _resMngr.GetResourcesOrThrow( imageId );

		Msg::CmdBuf_Bake::AcquireImageOwnershipCmd  cmd;
		cmd.image		= image.Handle();
		cmd.srcQueue	= srcQueue;
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_AddCommand( cmd );
	}

	void  RBarrierManager::ReleaseImageOwnership (ImageID imageId, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) __Th___
	{
		auto&	image = _resMngr.GetResourcesOrThrow( imageId );

		Msg::CmdBuf_Bake::ReleaseImageOwnershipCmd  cmd;
		cmd.image		= image.Handle();
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		cmd.dstQueue	= dstQueue;
		_AddCommand( cmd );
	}


#if AE_DBG_GRAPHICS
/*
=================================================
	ProfilerBeginContext
=================================================
*/
	void  RBarrierManager::ProfilerBeginContext (RSoftwareCmdBuf &cmdbuf, DebugLabel dbg, IGraphicsProfiler::EContextType type) C_Th___
	{
		if ( auto prof = GetBatch().GetProfiler() )
			prof->BeginContext( &GetBatch(), &cmdbuf, dbg.label, dbg.color, type );
	}

/*
=================================================
	ProfilerEndContext
=================================================
*/
	void  RBarrierManager::ProfilerEndContext (RSoftwareCmdBuf &cmdbuf, IGraphicsProfiler::EContextType type) C_Th___
	{
		if ( auto prof = GetBatch().GetProfiler() )
			prof->EndContext( &GetBatch(), &cmdbuf, type );
	}

#endif

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_REMOTE_GRAPHICS
