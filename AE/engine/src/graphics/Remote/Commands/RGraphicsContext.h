// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RDrawContext.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Graphics Context implementation
	//

	class RGraphicsContext final : public Graphics::_hidden_::RBaseContext, public IGraphicsContext
	{
	// types
	public:
		using DrawCtx		= RDrawContext;
	private:
		using AccumBar		= Graphics::_hidden_::RAccumBarriers< RGraphicsContext >;
		using DeferredBar	= Graphics::_hidden_::RAccumDeferredBarriersForCtx< RGraphicsContext >;
		using Validator_t	= Graphics::_hidden_::GraphicsContextValidation;


	// variables
	private:
		RPrimaryCmdBufState		_primaryState;


	// methods
	public:
		explicit RGraphicsContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)				__Th___;

		// continue render pass
		RGraphicsContext (const RenderTask &, const RDrawCommandBatch &, CmdBuf_t)											__Th___;

		RGraphicsContext ()																									= delete;
		RGraphicsContext (const RGraphicsContext &)																			= delete;


		// returns invalid state if outside of render pass
		ND_ RPrimaryCmdBufState const&  GetState ()																			C_NE___	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()																C_NE___	{ return _primaryState.IsValid(); }
		ND_ bool						IsSecondaryCmdbuf ()																C_NE___	{ return _primaryState.useSecondaryCmdbuf; }


		// synchronous rendering api
		ND_ DrawCtx	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default, void* userData = null)			__Th___;
		ND_ DrawCtx	NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg = Default, void* userData = null)						__Th___;
			void	EndRenderPass (DrawCtx& ctx)																			__Th___;


		// asynchronous rendering api
		ND_ auto	BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default, void* userData = null)			__Th___ -> RC<RDrawCommandBatch>;
		ND_ auto	NextMtSubpass (const RDrawCommandBatch &prevPassBatch, DebugLabel dbg = Default, void* userData = null)	__Th___ -> RC<RDrawCommandBatch>;
			void	EndMtRenderPass ()																						__Th___;
			void	ExecuteSecondary (RDrawCommandBatch &batch)																__Th___;

		ND_ RmCommandBufferID	EndCommandBuffer ()																			__Th___	{ return _EndCommandBuffer( ECtxType::Graphics ); }
		ND_ CmdBuf_t			ReleaseCommandBuffer ()																		__Th___	{ return _ReleaseCommandBuffer( ECtxType::Graphics ); }

		RBARRIERMNGR_INHERIT_BARRIERS

	private:
		ND_ RC<RDrawCommandBatch>  _BeginFirstAsyncPass (const RPrimaryCmdBufState &state, const RenderPassDesc &desc, DebugLabel);
		ND_ RC<RDrawCommandBatch>  _BeginNextAsyncPass (const RDrawCommandBatch &prevPassBatch, DebugLabel);

		void  _Convert (const RenderPassDesc &srcDesc, OUT RemoteGraphics::Msg::CmdBuf_Bake::Graphics_BeginRenderPass::SerRenderPassDesc &dstDesc) __Th___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
