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
		ND_ VkCommandBuffer	EndCommandBuffer ();
		ND_ VCommandBuffer  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectGraphicsCtx (const VRenderTask &task) : VBaseDirectContext{ task } {}
		_VDirectGraphicsCtx (const VRenderTask &task, VCommandBuffer cmdbuf) : VBaseDirectContext{ task, RVRef(cmdbuf) } {}

		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content,
									StringView dbgName, RGBA8u dbgColor);
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
		ND_ VBakedCommands		EndCommandBuffer ();
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectGraphicsCtx (const VRenderTask &task) : VBaseIndirectContext{ task } {}
		_VIndirectGraphicsCtx (const VRenderTask &task, VSoftwareCmdBufPtr cmdbuf) : VBaseIndirectContext{ task, RVRef(cmdbuf) } {}
		
		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content,
									StringView dbgName, RGBA8u dbgColor);
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
		explicit _VGraphicsContextImpl (const VRenderTask &task) : RawCtx{ task } {}
		
		template <typename RawCmdBufType>
		_VGraphicsContextImpl (const VRenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_VGraphicsContextImpl () = delete;
		_VGraphicsContextImpl (const _VGraphicsContextImpl &) = delete;
		
		// returns invalid state if outside of render pass
		ND_ VPrimaryCmdBufState const&  GetState ()				const	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()	const	{ return _primaryState.IsValid(); }


		// synchronious rendering api
		ND_ DrawCtx		BeginRenderPass (const RenderPassDesc &desc, StringView dbgName = Default, RGBA8u dbgColor = HtmlColor::Red);
		ND_ DrawCtx		NextSubpass (DrawCtx& ctx, StringView dbgName = Default, RGBA8u dbgColor = HtmlColor::Red);
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


namespace AE::Graphics::_hidden_
{
	
/*
=================================================
	EndCommandBuffer
=================================================
*/
	inline VkCommandBuffer  _VDirectGraphicsCtx::EndCommandBuffer ()
	{
		ASSERT( _NoPendingBarriers() );
		return _VBaseDirectContext::_EndCommandBuffer();
	}
	
/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	inline VCommandBuffer  _VDirectGraphicsCtx::ReleaseCommandBuffer ()
	{
		ASSERT( _NoPendingBarriers() );
		return _VBaseDirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------

	

/*
=================================================
	EndCommandBuffer
=================================================
*/
	inline VBakedCommands  _VIndirectGraphicsCtx::EndCommandBuffer ()
	{
		ASSERT( _NoPendingBarriers() );
		return _VBaseIndirectContext::_EndCommandBuffer();
	}
	
/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	inline VSoftwareCmdBufPtr  _VIndirectGraphicsCtx::ReleaseCommandBuffer ()
	{
		ASSERT( _NoPendingBarriers() );
		return _VBaseIndirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BeginRenderPass
=================================================
*/
	template <typename C>
	typename _VGraphicsContextImpl<C>::DrawCtx
		_VGraphicsContextImpl<C>::BeginRenderPass (const RenderPassDesc &desc, StringView dbgName, RGBA8u dbgColor)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CommitBarriers();	// for RG

		String	dbg_name;
		PROFILE_ONLY(
			dbg_name += this->_mngr.GetRenderTask().DbgFullName();
			dbg_name += " - ";
			dbg_name += (dbgName.empty() ? StringView{"RP"} : dbgName);
		)

		CHECK( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_INLINE, dbg_name, dbgColor ));
		RawCtx::_InitViewports( desc );

		return DrawCtx{ _primaryState, this->ReleaseCommandBuffer(), dbg_name, dbgColor };
	}

/*
=================================================
	NextSubpass
=================================================
*/
	template <typename C>
	typename _VGraphicsContextImpl<C>::DrawCtx
		_VGraphicsContextImpl<C>::NextSubpass (DrawCtx& ctx, StringView dbgName, RGBA8u dbgColor)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( ctx.IsValid() );
		
		++_primaryState.subpassIndex;
		ASSERT( usize{_primaryState.subpassIndex} < _primaryState.renderPass->Subpasses().size() );

		RawCtx::_NextSubpass( ctx, VK_SUBPASS_CONTENTS_INLINE );
		
		String	dbg_name;
		PROFILE_ONLY(
			dbg_name += this->_mngr.GetRenderTask().DbgFullName();
			dbg_name += " - ";
			dbg_name += (dbgName.empty() ? StringView{String{"Sp-"} + ToString(_primaryState.subpassIndex)} : dbgName);
		)

		return DrawCtx{ _primaryState, ctx.ReleaseCommandBuffer(), dbg_name, dbgColor };
	}
	
/*
=================================================
	EndRenderPass
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( ctx.IsValid() );

		RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		_primaryState = Default;
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->IsValid() );
	}
			
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( ctx.IsValid() );

		RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->IsValid() );

		this->_mngr.AfterEndRenderPass( desc, _primaryState );
		CommitBarriers();	// for RG

		_primaryState = Default;
	}
	
/*
=================================================
	BeginMtRenderPass
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::BeginMtRenderPass (const RenderPassDesc &desc, const VDrawCommandBatch &batch)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CHECK( _primaryState == batch.GetPrimaryCtxState() );
		CommitBarriers();	// for RG

		CHECK( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, batch.DbgName(), batch.DbgColor() ));
	}

/*
=================================================
	NextMtSubpass
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::NextMtSubpass (const VDrawCommandBatch &batch)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( this->IsValid() );
		
		++_primaryState.subpassIndex;
		ASSERT( usize{_primaryState.subpassIndex} < _primaryState.renderPass->Subpasses().size() );
		CHECK( _primaryState == batch.GetPrimaryCtxState() );

		RawCtx::_NextSubpass( this->_RawCmdBuf(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
	}
	
/*
=================================================
	EndMtRenderPass
=================================================
*/
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndMtRenderPass ()
	{
		ASSERT( this->IsValid() );

		RawCtx::_EndRenderPass( this->_RawCmdBuf() );
		_primaryState = Default;
	}
	
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndMtRenderPass (const RenderPassDesc &desc)
	{
		ASSERT( this->IsValid() );
		
		RawCtx::_EndRenderPass( this->_RawCmdBuf() );

		this->_mngr.AfterEndRenderPass( desc, _primaryState );
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
		ASSERT( IsInsideRenderPass() );
		CHECK( _primaryState == batch.GetPrimaryCtxState() );

		uint																count;
		StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch >	cmdbufs;

		CHECK( batch.GetCmdBuffers( OUT count, INOUT cmdbufs ));

		if_likely( count > 0 )
			RawCtx::_Execute( ArrayView<VkCommandBuffer>{ cmdbufs.data(), count });
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
