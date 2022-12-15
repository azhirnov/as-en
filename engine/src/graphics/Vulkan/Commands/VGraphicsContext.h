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
		ND_ VkCommandBuffer	EndCommandBuffer ()								__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()							__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectGraphicsCtx (const RenderTask &task)				__Th___	: VBaseDirectContext{ task } {}	// throw
		_VDirectGraphicsCtx (const RenderTask &task, VCommandBuffer cmdbuf)	__Th___	: VBaseDirectContext{ task, RVRef(cmdbuf) } {}	// throw

		ND_ RC<VDrawCommandBatch>  _BeginFirstAsyncPass (const VPrimaryCmdBufState &state, const RenderPassDesc &desc, DebugLabel);
		ND_ RC<VDrawCommandBatch>  _BeginNextAsyncPass (const VDrawCommandBatch &prevPassBatch, DebugLabel);

		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content, DebugLabel);
			void  _NextSubpass (VkCommandBuffer, VkSubpassContents content)	const;
			void  _EndRenderPass (VkCommandBuffer)							const;
			void  _InitViewports (const RenderPassDesc &desc);
			void  _Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs);

		ND_ VkCommandBuffer  _RawCmdBuf ()									C_NE___	{ return _cmdbuf.Get(); }
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
		ND_ VBakedCommands		EndCommandBuffer ()								__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()							__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectGraphicsCtx (const RenderTask &task)					 __Th___ : VBaseIndirectContext{ task } {}
		_VIndirectGraphicsCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf)__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf) } {}
		
		ND_ RC<VDrawCommandBatch>  _BeginFirstAsyncPass (const VPrimaryCmdBufState &state, const RenderPassDesc &desc, DebugLabel);
		ND_ RC<VDrawCommandBatch>  _BeginNextAsyncPass (const VDrawCommandBatch &prevPassBatch, DebugLabel);

		ND_ bool  _BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content, DebugLabel) __Th___;
			void  _NextSubpass (VSoftwareCmdBuf &, VkSubpassContents content)	__Th___;
			void  _EndRenderPass (VSoftwareCmdBuf &)							__Th___;
			void  _InitViewports (const RenderPassDesc &desc)					__Th___;
			void  _Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs)		__Th___;

		ND_ VSoftwareCmdBuf&  _RawCmdBuf ()										__NE___	{ return *_cmdbuf; }
	};


	
	//
	// Vulkan Graphics Context implementation
	//

	template <typename CtxImpl>
	class _VGraphicsContextImpl final : public CtxImpl, public IGraphicsContext
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
		explicit _VGraphicsContextImpl (const RenderTask &task)								__Th___;
		
		template <typename RawCmdBufType>
		_VGraphicsContextImpl (const RenderTask &task, RawCmdBufType cmdbuf)				__Th___;
		
		// continue render pass
		template <typename RawCmdBufType>
		_VGraphicsContextImpl (const RenderTask &, const VDrawCommandBatch &, RawCmdBufType) __Th___;

		_VGraphicsContextImpl ()															= delete;
		_VGraphicsContextImpl (const _VGraphicsContextImpl &)								= delete;
		
		// returns invalid state if outside of render pass
		ND_ VPrimaryCmdBufState const&  GetState ()											C_NE___	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()								C_NE___	{ return _primaryState.IsValid(); }
		ND_ bool						IsSecondaryCmdbuf ()								C_NE___	{ return _primaryState.useSecondaryCmdbuf; }


		// synchronious rendering api
		ND_ DrawCtx	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)				__Th___;
		ND_ DrawCtx	NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg = Default)						__Th___;
			void	EndRenderPass (DrawCtx& ctx)														__Th___;
			void	EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)							__Th___;

			
		// asynchronious rendering api
		ND_ auto	BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)			__Th___;
		ND_ auto	NextMtSubpass (const VDrawCommandBatch &prevPassBatch, DebugLabel dbg = Default)	__Th___;
			void	EndMtRenderPass ()																	__Th___;
			void	EndMtRenderPass (const RenderPassDesc &desc)										__Th___;
			void	ExecuteSecondary (VDrawCommandBatch &batch)											__Th___;

		VBARRIERMNGR_INHERIT_BARRIERS
	};
	

	void  ConvertViewports (ArrayView<RenderPassDesc::Viewport> inViewports, ArrayView<RectI> inScissors,
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
	inline VkCommandBuffer  _VDirectGraphicsCtx::EndCommandBuffer ()
	{
		ASSERT( _NoPendingBarriers() );
		return _VBaseDirectContext::_EndCommandBuffer();	// throw
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
		return _VBaseIndirectContext::_EndCommandBuffer();	// throw
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
	constructor
=================================================
*/
	template <typename C>
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (const RenderTask &task) : RawCtx{ task }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics, task.GetQueueMask() ));
	}
		
	template <typename C>
	template <typename RawCmdBufType>
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics, task.GetQueueMask() ));
	}
	
	template <typename C>
	template <typename RawCmdBufType>
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (const RenderTask &task, const VDrawCommandBatch &batch, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) },
		_primaryState{ batch.GetPrimaryCtxState() }
	{
		ASSERT( IsInsideRenderPass() );
		CHECK_THROW( AnyBits( EQueueMask::Graphics, task.GetQueueMask() ));
	}

/*
=================================================
	BeginRenderPass
=================================================
*/
	template <typename C>
	typename _VGraphicsContextImpl<C>::DrawCtx
		_VGraphicsContextImpl<C>::BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK_THROW( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		ASSERT( not IsSecondaryCmdbuf() );

		CommitBarriers();	// for RG

		String	dbg_name;
		PROFILE_ONLY(
			dbg_name += this->_mngr.GetRenderTask().DbgFullName();
			dbg_name += " - ";
			dbg_name += (dbg.label.empty() ? StringView{"RP"} : dbg.label);
		)

		CHECK_THROW( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_INLINE, DebugLabel{ dbg_name, dbg.color }));
		RawCtx::_InitViewports( desc );	// throw

		return DrawCtx{ _primaryState, this->ReleaseCommandBuffer(), DebugLabel{ dbg_name, dbg.color }};	// throw
	}

/*
=================================================
	NextSubpass
=================================================
*/
	template <typename C>
	typename _VGraphicsContextImpl<C>::DrawCtx
		_VGraphicsContextImpl<C>::NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( _primaryState.IsValid() );
		ASSERT( prevPassCtx._IsValid() );
		ASSERT( not IsSecondaryCmdbuf() );
		
		++_primaryState.subpassIndex;
		ASSERT( usize{_primaryState.subpassIndex} < _primaryState.renderPass->Subpasses().size() );

		RawCtx::_NextSubpass( prevPassCtx, VK_SUBPASS_CONTENTS_INLINE );
		
		String	dbg_name;
		PROFILE_ONLY(
			dbg_name += this->_mngr.GetRenderTask().DbgFullName();
			dbg_name += " - ";
			dbg_name += (dbg.label.empty() ? StringView{String{"Sp-"} + ToString(_primaryState.subpassIndex)} : dbg.label);
		)

		return DrawCtx{ _primaryState, prevPassCtx.ReleaseCommandBuffer(), DebugLabel{ dbg_name, dbg.color }};	// throw
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
		ASSERT( _primaryState.IsValid() );
		ASSERT( not IsSecondaryCmdbuf() );
		ASSERT( ctx._IsValid() );

		RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		_primaryState = Default;
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->_IsValid() );
	}
			
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( not IsSecondaryCmdbuf() );
		ASSERT( ctx._IsValid() );

		RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->_IsValid() );

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
	auto  _VGraphicsContextImpl<C>::BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK_THROW( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CommitBarriers();	// for RG

		_primaryState.useSecondaryCmdbuf = true;
		ASSERT( IsSecondaryCmdbuf() );

		auto	batch = RawCtx::_BeginFirstAsyncPass( _primaryState, desc, dbg );
		CHECK_THROW( batch );

		ASSERT( _primaryState == batch->GetPrimaryCtxState() );
		
		CHECK_THROW( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS, dbg ));
		return batch;
	}

/*
=================================================
	NextMtSubpass
=================================================
*/
	template <typename C>
	auto  _VGraphicsContextImpl<C>::NextMtSubpass (const VDrawCommandBatch &prevPassBatch, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( _primaryState.IsValid() );
		ASSERT( IsSecondaryCmdbuf() );
		ASSERT( this->_IsValid() );
		
		++_primaryState.subpassIndex;
		ASSERT( usize{_primaryState.subpassIndex} < _primaryState.renderPass->Subpasses().size() );

		auto	batch = RawCtx::_BeginNextAsyncPass( prevPassBatch, dbg );
		CHECK_THROW( batch );
		
		ASSERT( _primaryState == batch->GetPrimaryCtxState() );

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
		ASSERT( this->_IsValid() );
		ASSERT( IsSecondaryCmdbuf() );

		RawCtx::_EndRenderPass( this->_RawCmdBuf() );
		_primaryState = Default;
	}
	
	template <typename C>
	void  _VGraphicsContextImpl<C>::EndMtRenderPass (const RenderPassDesc &desc)
	{
		ASSERT( this->_IsValid() );
		ASSERT( IsSecondaryCmdbuf() );
		
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
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( IsInsideRenderPass() );
		ASSERT( IsSecondaryCmdbuf() );
		CHECK( _primaryState == batch.GetPrimaryCtxState() );

		uint																count;
		StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch >	cmdbufs;

		CHECK( batch.GetCmdBuffers( OUT count, INOUT cmdbufs ));

		if_likely( count > 0 )
			RawCtx::_Execute( ArrayView<VkCommandBuffer>{ cmdbufs.data(), count });
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
