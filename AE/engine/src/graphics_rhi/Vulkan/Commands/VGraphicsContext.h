// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	GraphicsCtx --> DirectGraphicsCtx   --> BarrierMngr --> Vulkan device
				\-> IndirectGraphicsCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VDrawContext.h"

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
		_VDirectGraphicsCtx (RenderCoroRef task, VCommandBuffer cmdbuf, DebugLabel dbg)				__Th___	: VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Graphics } {}

		ND_ RC<DrawCommandBatch>  _BeginFirstAsyncPass (const VPrimaryCmdBufState &,
														const RenderPassDesc &, DebugLabel)			__Th___;
		ND_ RC<DrawCommandBatch>  _BeginNextAsyncPass (const DrawCommandBatch &, DebugLabel)		__Th___;

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
		_VIndirectGraphicsCtx (RenderCoroRef task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)		__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Graphics } {}

		ND_ RC<DrawCommandBatch>  _BeginFirstAsyncPass (const VPrimaryCmdBufState &,
														const RenderPassDesc &, DebugLabel)			__Th___;
		ND_ RC<DrawCommandBatch>  _BeginNextAsyncPass (const DrawCommandBatch &, DebugLabel)		__Th___;

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
	class _VGraphicsContextImpl final : public CtxImpl, public IGraphicsContextVk
	{
	// types
	public:
		using DrawCtx		= typename CtxImpl::_DrawCtx;
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
		using RenderCoroRef	= typename CtxImpl::RenderCoroRef;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= AccumBarriers< _VGraphicsContextImpl< CtxImpl >>;
		using DeferredBar	= AccumDeferredBarriersForCtx< _VGraphicsContextImpl< CtxImpl >>;
		using Validator_t	= GraphicsContextValidation;
		using FinalStates_t	= VBarrierManager::RPassFinalStates_t;


	// variables
	private:
		VPrimaryCmdBufState		_primaryState;
		FinalStates_t			_finalStates;


	// methods
	public:
		explicit _VGraphicsContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)			__Th___;

		// continue render pass
		_VGraphicsContextImpl (RenderCoroRef, const DrawCommandBatch &, CmdBuf_t)											__Th___;

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
		ND_ auto	BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default, void* userData = null)			__Th___ -> RC<DrawCommandBatch>;
		ND_ auto	NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg = Default, void* userData = null)	__Th___ -> RC<DrawCommandBatch>;
			void	EndMtRenderPass ()																						__Th___;
			void	ExecuteSecondary (DrawCommandBatch &batch)																__Th___;

		VBARRIERMNGR_INHERIT_BARRIERS
	};


	void  ConvertViewports (ArrayView<Viewport> inViewports, ArrayView<RectI> inScissors, const int2 &areaSize,
							OUT DrawCommandBatch::Viewports_t &outViewports, OUT DrawCommandBatch::Scissors_t &outScissors) __NE___;

    extern template class _VGraphicsContextImpl< _VDirectGraphicsCtx >;
    extern template class _VGraphicsContextImpl< _VIndirectGraphicsCtx >;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectGraphicsContext	= Graphics::_hidden_::_VGraphicsContextImpl< Graphics::_hidden_::_VDirectGraphicsCtx >;
	using VIndirectGraphicsContext	= Graphics::_hidden_::_VGraphicsContextImpl< Graphics::_hidden_::_VIndirectGraphicsCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
