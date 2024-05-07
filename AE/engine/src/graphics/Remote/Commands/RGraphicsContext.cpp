// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RGraphicsContext.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	_ValidateRenderPassDebugLabel
=================================================
*/
namespace {
	ND_ inline DebugLabel  _ValidateRenderPassDebugLabel (DebugLabel dbg)
	{
		if ( dbg.color == DebugLabel::ColorTable::Undefined )
			dbg.color = DebugLabel::ColorTable::DrawCtx;
		return dbg;
	}
}
/*
=================================================
	_Convert
=================================================
*/
	void  RGraphicsContext::_Convert (const RenderPassDesc &srcDesc, OUT Msg::CmdBuf_Bake::Graphics_BeginRenderPass::SerRenderPassDesc &dstDesc) __Th___
	{
		for (auto [name, src] : srcDesc.attachments)
		{
			auto&	dst		= dstDesc.attachments.emplace_back();
			auto&	view	= _GetResourcesOrThrow( src.imageView );

			dst.name		= name;
			dst.imageView	= view.Handle();
			dst.clearValue	= src.clearValue;
			dst.initial		= src.initial;
			dst.final		= src.final;
			dst.relaxedStateTransition	= src.relaxedStateTransition;
		}
		dstDesc.viewports		= srcDesc.viewports;
		dstDesc.area			= srcDesc.area;
		dstDesc.layerCount		= srcDesc.layerCount;
		dstDesc.renderPassName	= srcDesc.renderPassName;
		dstDesc.subpassName		= srcDesc.subpassName;

		if ( srcDesc.packId )
		{
			auto*	pack	= _mngr.GetResourceManager().GetResource( srcDesc.packId );
			dstDesc.packId	= (pack != null ? pack->Handle() : Default);
		}
	}

/*
=================================================
	constructor
=================================================
*/
	RGraphicsContext::RGraphicsContext (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), dbg, ECtxType::Graphics }
	{
		Validator_t::CtxInit( task.GetQueueMask() );
	}

	RGraphicsContext::RGraphicsContext (const RenderTask &task, const RDrawCommandBatch &batch, CmdBuf_t cmdbuf) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), Default, ECtxType::Graphics },
		_primaryState{ batch.GetPrimaryCtxState() }
	{
		GCTX_CHECK( IsInsideRenderPass() );
		Validator_t::CtxInit( task.GetQueueMask() );
	}

/*
=================================================
	_BeginFirstAsyncPass
=================================================
*/
	RC<RDrawCommandBatch>  RGraphicsContext::_BeginFirstAsyncPass (const RPrimaryCmdBufState &primaryState, const RenderPassDesc &desc, DebugLabel dbg)
	{
		return RenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch( GraphicsScheduler(), primaryState, desc, dbg );
	}

	RC<RDrawCommandBatch>  RGraphicsContext::_BeginNextAsyncPass (const RDrawCommandBatch &prevPassBatch, DebugLabel dbg)
	{
		return RenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch( GraphicsScheduler(), prevPassBatch, dbg );
	}

/*
=================================================
	BeginRenderPass
=================================================
*/
	RGraphicsContext::DrawCtx  RGraphicsContext::BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg, void* userData) __Th___
	{
		CHECK_THROW( _mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		GCTX_CHECK( not IsSecondaryCmdbuf() );

		_primaryState.userData = userData;

		CommitBarriers();
		GFX_DBG_ONLY( _mngr.ProfilerBeginContext( *_cmdbuf, _ValidateRenderPassDebugLabel( dbg ), ECtxType::RenderPass ));

		{
			Msg::CmdBuf_Bake::Graphics_BeginRenderPass  cmd;
			_Convert( desc, OUT cmd.desc );
			cmd.dbgLabel = dbg;
			_cmdbuf->AddCommand( cmd );
		}
		return DrawCtx{ _primaryState, ReleaseCommandBuffer() };	// throw
	}

/*
=================================================
	NextSubpass
=================================================
*/
	RGraphicsContext::DrawCtx  RGraphicsContext::NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg, void* userData) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( prevPassCtx._IsValid() );
		GCTX_CHECK( not IsSecondaryCmdbuf() );

		++_primaryState.subpassIndex;
		GCTX_CHECK( usize{_primaryState.subpassIndex} < _primaryState.renderPass->SubpassCount() );

		_primaryState.userData = userData;
		{
			Msg::CmdBuf_Bake::Graphics_NextSubpass  cmd;
			cmd.dbgLabel = dbg;
			_cmdbuf->AddCommand( cmd );
		}
		return DrawCtx{ _primaryState, prevPassCtx.ReleaseCommandBuffer() };	// throw
	}

/*
=================================================
	EndRenderPass
=================================================
*/
	void  RGraphicsContext::EndRenderPass (DrawCtx& ctx) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( not IsSecondaryCmdbuf() );
		GCTX_CHECK( ctx._IsValid() );

		_primaryState = Default;

		_cmdbuf = ctx.ReleaseCommandBuffer();
		GCTX_CHECK( _IsValid() );

		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::Graphics_EndRenderPass{} );

		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RenderPass ));
	}

/*
=================================================
	BeginMtRenderPass
=================================================
*/
	auto  RGraphicsContext::BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg, void* userData) __Th___ -> RC<RDrawCommandBatch>
	{
		ASSERT( _NoPendingBarriers() );

		CHECK_THROW( _mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CommitBarriers();

		_primaryState.useSecondaryCmdbuf = true;
		_primaryState.userData			 = userData;

		GCTX_CHECK( IsSecondaryCmdbuf() );

		auto	batch = _BeginFirstAsyncPass( _primaryState, desc, dbg );
		CHECK_THROW( batch );

		GCTX_CHECK( _primaryState == batch->GetPrimaryCtxState() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RenderPass ));

		{
			Msg::CmdBuf_Bake::Graphics_BeginMtRenderPass  cmd;
			_Convert( desc, OUT cmd.desc );
			cmd.dbgLabel = dbg;
			cmd.batchId  = batch->Handle();
			_cmdbuf->AddCommand( cmd );
		}
		return batch;
	}

/*
=================================================
	NextMtSubpass
=================================================
*/
	auto  RGraphicsContext::NextMtSubpass (const RDrawCommandBatch &prevPassBatch, DebugLabel dbg, void* userData) __Th___ -> RC<RDrawCommandBatch>
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( IsSecondaryCmdbuf() );
		GCTX_CHECK( _IsValid() );

		++_primaryState.subpassIndex;
		GCTX_CHECK( usize{_primaryState.subpassIndex} < _primaryState.renderPass->SubpassCount() );

		_primaryState.userData = userData;

		auto	batch = _BeginNextAsyncPass( prevPassBatch, dbg );
		CHECK_THROW( batch );

		GCTX_CHECK( _primaryState == batch->GetPrimaryCtxState() );
		{
			Msg::CmdBuf_Bake::Graphics_NextMtSubpass  cmd;
			cmd.dbgLabel	= dbg;
			cmd.prevBatchId	= prevPassBatch.Handle();
			cmd.batchId		= batch->Handle();
			_cmdbuf->AddCommand( cmd );
		}
		return batch;
	}

/*
=================================================
	EndMtRenderPass
=================================================
*/
	void  RGraphicsContext::EndMtRenderPass () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _IsValid() );
		GCTX_CHECK( IsSecondaryCmdbuf() );

		_primaryState = Default;

		_cmdbuf->AddCommand( Msg::CmdBuf_Bake::Graphics_EndMtRenderPass{} );

		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RenderPass ));
	}

/*
=================================================
	ExecuteSecondary
=================================================
*/
	void  RGraphicsContext::ExecuteSecondary (RDrawCommandBatch &batch) __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( IsInsideRenderPass() );
		GCTX_CHECK( IsSecondaryCmdbuf() );
		GCTX_CHECK( _primaryState == batch.GetPrimaryCtxState() );

		uint																	count;
		StaticArray< RmDrawCommandBufferID, GraphicsConfig::MaxCmdBufPerBatch >	cmdbufs;

		CHECK( batch.GetCmdBuffers( OUT count, INOUT cmdbufs ));

		if ( count > 0 )
		{
			Msg::CmdBuf_Bake::Graphics_ExecuteSecondary  cmd;
			cmd.cmdbufs = ArrayView<RmDrawCommandBufferID>{ cmdbufs.data(), count };
			_cmdbuf->AddCommand( cmd );
		}
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
