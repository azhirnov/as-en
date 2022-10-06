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
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectGraphicsCtx (Ptr<VCommandBatch> batch) : VBaseDirectContext{ batch } {}
		_VDirectGraphicsCtx (Ptr<VCommandBatch> batch, VCommandBuffer cmdbuf) : VBaseDirectContext{ batch, RVRef(cmdbuf) } {}

		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content);
			void  _NextSubpass (VkCommandBuffer, VkSubpassContents content) const;
			void  _EndRenderPass (VkCommandBuffer) const;
			void  _InitViewports (const RenderPassDesc &desc);
			void  _Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs);

		ND_ VkCommandBuffer  _RawCmdBuf ()	const	{ return _cmdbuf.Get(); }
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
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectGraphicsCtx (Ptr<VCommandBatch> batch) : VBaseIndirectContext{ batch } {}
		_VIndirectGraphicsCtx (Ptr<VCommandBatch> batch, VSoftwareCmdBufPtr cmdbuf) : VBaseIndirectContext{ batch, RVRef(cmdbuf) } {}
		
		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content);
			void  _NextSubpass (VSoftwareCmdBuf &, VkSubpassContents content) const;
			void  _EndRenderPass (VSoftwareCmdBuf &) const;
			void  _InitViewports (const RenderPassDesc &desc);
			void  _Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs);

		ND_ VSoftwareCmdBuf&  _RawCmdBuf ()		{ return *_cmdbuf; }
	};


	
	//
	// Vulkan Graphics Context implementation
	//

	template <typename CtxImpl>
	class _VGraphicsContextImpl : public CtxImpl, public IGraphicsContext
	{
	// types
	public:
		static constexpr bool	IsGraphicsContext		= true;
		static constexpr bool	IsVulkanGraphicsContext	= true;

		using DrawCtx	= typename CtxImpl::_DrawCtx;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumBarriers< _VGraphicsContextImpl< CtxImpl >>;


	// variables
	private:
		VPrimaryCmdBufState		_primaryState;

		
	// methods
	public:
		explicit _VGraphicsContextImpl (Ptr<VCommandBatch> batch) : CtxImpl{ batch } {}
		
		template <typename RawCmdBufType>
		_VGraphicsContextImpl (Ptr<VCommandBatch> batch, RawCmdBufType cmdbuf) : CtxImpl{ batch, RVRef(cmdbuf) } {}

		_VGraphicsContextImpl () = delete;
		_VGraphicsContextImpl (const _VGraphicsContextImpl &) = delete;
		
		// returns invalid state if outside of render pass
		ND_ VPrimaryCmdBufState const&  GetState ()				const	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()	const	{ return _primaryState.IsValid(); }


		// synchronious rendering api
		ND_ DrawCtx		BeginRenderPass (const RenderPassDesc &desc);
		ND_ DrawCtx		NextSubpass (DrawCtx& ctx);
			void		EndRenderPass (DrawCtx& ctx);
			void		EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc);


		// asynchronious rendering api
			void	BeginMtRenderPass (const RenderPassDesc &desc, const VDrawCommandBatch &batch);
			void	NextMtSubpass (const VDrawCommandBatch &batch);
			void	EndMtRenderPass ();
			void	EndMtRenderPass (const RenderPassDesc &desc);
			void	ExecuteSecondary (VDrawCommandBatch &batch);	// TODO: not supported in Metal


		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }

		ND_ AccumBar  AccumBarriers ()											{ return AccumBar{ *this }; }

		VBARRIERMNGR_INHERIT_BARRIERS
	};
	

	void  ConvertViewports (const RenderPassDesc &desc, OUT VDrawCommandBatch::Viewports_t &viewports, OUT VDrawCommandBatch::Scissors_t& scissors);

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectGraphicsContext	= _hidden_::_VGraphicsContextImpl< _hidden_::_VDirectGraphicsCtx >;
	using VIndirectGraphicsContext	= _hidden_::_VGraphicsContextImpl< _hidden_::_VIndirectGraphicsCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
