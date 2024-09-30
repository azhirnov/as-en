// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	GraphicsCtx --> DirectGraphicsCtx   --> BarrierMngr --> Vulkan device
				\-> IndirectGraphicsCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VDrawContext.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Graphics Context implementation
	//

	class _VDirectGraphicsCtx : public VBaseDirectContext
	{
	// types
	protected:
		using _DrawCtx = VDirectDrawContext;

	// methods
	public:
		ND_ VkCommandBuffer	EndCommandBuffer ()														__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()													__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VDirectGraphicsCtx (const RenderTask &task, VCommandBuffer cmdbuf, DebugLabel dbg)			__Th___	: VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Graphics } {}

		ND_ RC<VDrawCommandBatch>  _BeginFirstAsyncPass (const VPrimaryCmdBufState &,
														 const RenderPassDesc &, DebugLabel)		__Th___;
		ND_ RC<VDrawCommandBatch>  _BeginNextAsyncPass (const VDrawCommandBatch &, DebugLabel)		__Th___;

		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state,
									VkSubpassContents content, DebugLabel)							__Th___;
			void  _NextSubpass (VkCommandBuffer, VkSubpassContents content)							C_Th___;
			void  _EndRenderPass (VkCommandBuffer)													C_Th___;
			void  _InitViewports (const RenderPassDesc &desc)										__Th___;
			void  _Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs)							__Th___;

		ND_ VkCommandBuffer  _RawCmdBuf ()															C_NE___	{ return _cmdbuf.Get(); }
	};



	//
	// Vulkan Indirect Graphics Context implementation
	//

	class _VIndirectGraphicsCtx : public VBaseIndirectContext
	{
	// types
	protected:
		using _DrawCtx = VIndirectDrawContext;

	// methods
	public:
		ND_ VBakedCommands		EndCommandBuffer ()													__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()												__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VIndirectGraphicsCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)	__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Graphics } {}

		ND_ RC<VDrawCommandBatch>  _BeginFirstAsyncPass (const VPrimaryCmdBufState &,
														 const RenderPassDesc &, DebugLabel)		__Th___;
		ND_ RC<VDrawCommandBatch>  _BeginNextAsyncPass (const VDrawCommandBatch &, DebugLabel)		__Th___;

		ND_ bool  _BeginRenderPass (const RenderPassDesc &, const VPrimaryCmdBufState &,
									VkSubpassContents, DebugLabel)									__Th___;
			void  _NextSubpass (VSoftwareCmdBuf &, VkSubpassContents content)						__Th___;
			void  _EndRenderPass (VSoftwareCmdBuf &)												__Th___;
			void  _InitViewports (const RenderPassDesc &desc)										__Th___;
			void  _Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs)							__Th___;

		ND_ VSoftwareCmdBuf&  _RawCmdBuf ()															__NE___	{ return *_cmdbuf; }
	};



	//
	// Vulkan Graphics Context implementation
	//

	template <typename CtxImpl>
	class _VGraphicsContextImpl final : public CtxImpl, public IGraphicsContext
	{
	// types
	public:
		using DrawCtx		= typename CtxImpl::_DrawCtx;
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= VAccumBarriers< _VGraphicsContextImpl< CtxImpl >>;
		using DeferredBar	= VAccumDeferredBarriersForCtx< _VGraphicsContextImpl< CtxImpl >>;
		using Validator_t	= GraphicsContextValidation;
		using FinalStates_t	= VBarrierManager::RPassFinalStates_t;


	// variables
	private:
		VPrimaryCmdBufState		_primaryState;
		FinalStates_t			_finalStates;


	// methods
	public:
		explicit _VGraphicsContextImpl (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)		__Th___;

		// continue render pass
		_VGraphicsContextImpl (const RenderTask &, const VDrawCommandBatch &, CmdBuf_t)										__Th___;

		_VGraphicsContextImpl ()																							= delete;
		_VGraphicsContextImpl (const _VGraphicsContextImpl &)																= delete;


		// returns invalid state if outside of render pass
		ND_ VPrimaryCmdBufState const&  GetState ()																			C_NE___	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()																C_NE___	{ return _primaryState.IsValid(); }
		ND_ bool						IsSecondaryCmdbuf ()																C_NE___	{ return _primaryState.useSecondaryCmdbuf; }


		// synchronous rendering api
		ND_ DrawCtx	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default, void* userData = null)			__Th___;
		ND_ DrawCtx	NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg = Default, void* userData = null)						__Th___;
			void	EndRenderPass (DrawCtx& ctx)																			__Th___;


		// asynchronous rendering api
		ND_ auto	BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default, void* userData = null)			__Th___ -> RC<VDrawCommandBatch>;
		ND_ auto	NextMtSubpass (const VDrawCommandBatch &prevPassBatch, DebugLabel dbg = Default, void* userData = null)	__Th___ -> RC<VDrawCommandBatch>;
			void	EndMtRenderPass ()																						__Th___;
			void	ExecuteSecondary (VDrawCommandBatch &batch)																__Th___;

		VBARRIERMNGR_INHERIT_BARRIERS
	};


	void  ConvertViewports (ArrayView<Viewport> inViewports, ArrayView<RectI> inScissors, const int2 &areaSize,
							OUT VDrawCommandBatch::Viewports_t &outViewports, OUT VDrawCommandBatch::Scissors_t &outScissors) __NE___;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectGraphicsContext	= Graphics::_hidden_::_VGraphicsContextImpl< Graphics::_hidden_::_VDirectGraphicsCtx >;
	using VIndirectGraphicsContext	= Graphics::_hidden_::_VGraphicsContextImpl< Graphics::_hidden_::_VIndirectGraphicsCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{

/*
=================================================
	EndCommandBuffer
=================================================
*/
	inline VkCommandBuffer  _VDirectGraphicsCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseDirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	inline VCommandBuffer  _VDirectGraphicsCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseDirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	EndCommandBuffer
=================================================
*/
	inline VBakedCommands  _VIndirectGraphicsCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseIndirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	inline VSoftwareCmdBufPtr  _VIndirectGraphicsCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseIndirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) :
		RawCtx{ task, RVRef(cmdbuf), dbg }  // throw
	{
		Validator_t::CtxInit( task.GetQueueMask() );
		ZeroMem( _finalStates );
	}

	template <typename C>
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (const RenderTask &task, const VDrawCommandBatch &batch, CmdBuf_t cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf), Default },  // throw
		_primaryState{ batch.GetPrimaryCtxState() }
	{
		GCTX_CHECK( IsInsideRenderPass() );
		Validator_t::CtxInit( task.GetQueueMask() );
		ZeroMem( _finalStates );
	}

/*
=================================================
	BeginRenderPass
=================================================
*/
	template <typename C>
	typename _VGraphicsContextImpl<C>::DrawCtx
		_VGraphicsContextImpl<C>::BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg, void* userData)
	{
		CHECK_THROW( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState, OUT _finalStates ));
		GCTX_CHECK( not IsSecondaryCmdbuf() );

		_primaryState.userData = userData;

		CommitBarriers();	// for RG

		String	dbg_name;
		GFX_DBG_ONLY(
			if ( dbg.label.empty() )	dbg_name << this->_mngr.GetRenderTask().DbgFullName() << " - " << "RP";
			else						dbg_name = dbg.label;
		)
		CHECK_THROW( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_INLINE, DebugLabel{ dbg_name, dbg.color }));
		RawCtx::_InitViewports( desc );	// throw

		GFX_DBG_ONLY( this->PushDebugGroup( DebugLabel{ dbg_name, dbg.color });)

		return DrawCtx{ _primaryState, this->ReleaseCommandBuffer() };	// throw
	}

/*
=================================================
	NextSubpass
=================================================
*/
	template <typename C>
	typename _VGraphicsContextImpl<C>::DrawCtx
		_VGraphicsContextImpl<C>::NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg, void* userData)
	{
		ASSERT( this->_NoPendingBarriers() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( prevPassCtx._IsValid() );
		GCTX_CHECK( not IsSecondaryCmdbuf() );

		++_primaryState.subpassIndex;
		GCTX_CHECK( usize(_primaryState.subpassIndex) < _primaryState.renderPass->Subpasses().size() );

		_primaryState.userData = userData;

		GFX_DBG_ONLY( prevPassCtx.PopDebugGroup();)

		RawCtx::_NextSubpass( this->_RawCmdBuf(), VK_SUBPASS_CONTENTS_INLINE );

		GFX_DBG_ONLY(
			String	dbg_name;
			dbg_name << this->_mngr.GetRenderTask().DbgFullName() << " - ";

			if ( dbg.label.empty() )	dbg_name << "Sp-" << ToString(_primaryState.subpassIndex);
			else						dbg_name << dbg.label;

			prevPassCtx.PushDebugGroup( DebugLabel{ dbg_name, dbg.color });
		)

		return DrawCtx{ _primaryState, prevPassCtx.ReleaseCommandBuffer() };	// throw
	}

/*
=================================================
	EndRenderPass
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx)
	{
		ASSERT( this->_NoPendingBarriers() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( not IsSecondaryCmdbuf() );
		GCTX_CHECK( ctx._IsValid() );

		GFX_DBG_ONLY( ctx.PopDebugGroup();)

		RawCtx::_EndRenderPass( ctx._RawCmdBuf() );

		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		GCTX_CHECK( this->_IsValid() );

		this->_mngr.AfterEndRenderPass( _primaryState, _finalStates );
		CommitBarriers();	// for RG

		_primaryState = Default;
	}

/*
=================================================
	BeginMtRenderPass
=================================================
*/
	template <typename C>
	auto  _VGraphicsContextImpl<C>::BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg, void* userData) -> RC<VDrawCommandBatch>
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK_THROW( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState, OUT _finalStates ));
		CommitBarriers();	// for RG

		_primaryState.useSecondaryCmdbuf = true;
		_primaryState.userData			 = userData;

		GCTX_CHECK( IsSecondaryCmdbuf() );

		auto	batch = RawCtx::_BeginFirstAsyncPass( _primaryState, desc, dbg );
		CHECK_THROW( batch );

		GCTX_CHECK( _primaryState == batch->GetPrimaryCtxState() );

		CHECK_THROW( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, dbg ));
		return batch;
	}

/*
=================================================
	NextMtSubpass
=================================================
*/
	template <typename C>
	auto  _VGraphicsContextImpl<C>::NextMtSubpass (const VDrawCommandBatch &prevPassBatch, DebugLabel dbg, void* userData) -> RC<VDrawCommandBatch>
	{
		ASSERT( this->_NoPendingBarriers() );
		GCTX_CHECK( _primaryState.IsValid() );
		GCTX_CHECK( IsSecondaryCmdbuf() );
		GCTX_CHECK( this->_IsValid() );

		++_primaryState.subpassIndex;
		GCTX_CHECK( usize(_primaryState.subpassIndex) < _primaryState.renderPass->Subpasses().size() );

		_primaryState.userData = userData;

		auto	batch = RawCtx::_BeginNextAsyncPass( prevPassBatch, dbg );
		CHECK_THROW( batch );

		GCTX_CHECK( _primaryState == batch->GetPrimaryCtxState() );

		RawCtx::_NextSubpass( this->_RawCmdBuf(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
		return batch;
	}

/*
=================================================
	EndMtRenderPass
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndMtRenderPass ()
	{
		ASSERT( this->_NoPendingBarriers() );
		GCTX_CHECK( this->_IsValid() );
		GCTX_CHECK( IsSecondaryCmdbuf() );

		RawCtx::_EndRenderPass( this->_RawCmdBuf() );

		this->_mngr.AfterEndRenderPass( _primaryState, _finalStates );
		CommitBarriers();	// for RG

		_primaryState = Default;
	}

/*
=================================================
	ExecuteSecondary
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::ExecuteSecondary (VDrawCommandBatch &batch)
	{
		ASSERT( this->_NoPendingBarriers() );
		GCTX_CHECK( IsInsideRenderPass() );
		GCTX_CHECK( IsSecondaryCmdbuf() );
		GCTX_CHECK( _primaryState == batch.GetPrimaryCtxState() );

		uint																count;
		StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch >	cmdbufs;

		CHECK( batch.GetCmdBuffers( OUT count, INOUT cmdbufs ));

		if_likely( count > 0 )
			RawCtx::_Execute( ArrayView<VkCommandBuffer>{ cmdbufs.data(), count });
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
