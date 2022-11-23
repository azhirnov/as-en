// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	GraphicsCtx --> DirectGraphicsCtx   --> BarrierMngr --> Metal device 
				\-> IndirectGraphicsCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MDrawContext.h"

namespace AE::Graphics::_hidden_
{

	//
	// Metal Direct Graphics Context implementation
	//
	
	class _MDirectGraphicsCtx : public MBaseDirectContext
	{
	// types
	protected:
		using _DrawCtx = MDirectDrawContext;

	// methods
	public:
		ND_ MetalCommandBufferRC	EndCommandBuffer ();
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ();

	protected:
		_MDirectGraphicsCtx (const RenderTask &task) : MBaseDirectContext{ task } {}
		_MDirectGraphicsCtx (const RenderTask &task, MCommandBuffer cmdbuf) : MBaseDirectContext{ task, RVRef(cmdbuf) } {}

		void  _DebugMarker (NtStringView text, RGBA8u)				{ ASSERT( _NoPendingBarriers() );  DBG_WARNING( "DebugMarker is not supported" ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u)			{ ASSERT( _NoPendingBarriers() );  this->_cmdbuf.PushDebugGroup( text ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  this->_cmdbuf.PopDebugGroup(); }
	};



	//
	// Metal Indirect Graphics Context implementation
	//
	
	class _MIndirectGraphicsCtx : public MBaseIndirectContext
	{
	// types
	protected:
		using _DrawCtx = MIndirectDrawContext;

	// methods
	public:
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		_MIndirectGraphicsCtx (const RenderTask &task) : MBaseIndirectContext{ task } {}
		_MIndirectGraphicsCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf) : MBaseIndirectContext{ task, RVRef(cmdbuf) } {}
	};


	
	//
	// Metal Graphics Context implementation
	//

	template <typename CtxImpl>
	class _MGraphicsContextImpl : public CtxImpl, public IGraphicsContext
	{
	// types
	public:
		static constexpr bool	IsGraphicsContext		= true;
		static constexpr bool	IsMetalGraphicsContext	= true;

		using DrawCtx	= typename CtxImpl::_DrawCtx;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumBarriers< _MGraphicsContextImpl< CtxImpl >>;


	// variables
	private:
		MPrimaryCmdBufState		_primaryState;

		
	// methods
	public:
		explicit _MGraphicsContextImpl (const RenderTask &task) : CtxImpl{ task } {}
		
		template <typename RawCmdBufType>
		_MGraphicsContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : CtxImpl{ task, RVRef(cmdbuf) } {}

		_MGraphicsContextImpl () = delete;
		_MGraphicsContextImpl (const _MGraphicsContextImpl &) = delete;
		
		// returns invalid state if outside of render pass
		ND_ MPrimaryCmdBufState const&  GetState ()				const	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()	const	{ return _primaryState.IsValid(); }


		// synchronious rendering api
		ND_ DrawCtx		BeginRenderPass (const RenderPassDesc &desc, NtStringView dbgName = Default);
		ND_ DrawCtx		NextSubpass (DrawCtx& ctx, NtStringView dbgName = Default);
			void		EndRenderPass (DrawCtx& ctx);
			void		EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc);


		// asynchronious rendering api
			void	BeginMtRenderPass (const RenderPassDesc &desc, const MDrawCommandBatch &batch);
			void	NextMtSubpass (const MDrawCommandBatch &batch);
			void	EndMtRenderPass ();
			void	EndMtRenderPass (const RenderPassDesc &desc);
			void	ExecuteSecondary (MDrawCommandBatch &batch);	// TODO: not supported in Metal

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectGraphicsContext	= _hidden_::_MGraphicsContextImpl< _hidden_::_MDirectGraphicsCtx >;
	using MIndirectGraphicsContext	= _hidden_::_MGraphicsContextImpl< _hidden_::_MIndirectGraphicsCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{
/*
=================================================
	BeginRenderPass
=================================================
*/
	template <typename C>
	typename _MGraphicsContextImpl<C>::DrawCtx
		_MGraphicsContextImpl<C>::BeginRenderPass (const RenderPassDesc &desc, NtStringView dbgName)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CommitBarriers();	// for RG

		//CHECK( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_INLINE ));
		//RawCtx::_InitViewports( desc );

		return DrawCtx{ _primaryState, this->ReleaseCommandBuffer(), (dbgName.empty() ? "RenderPass" : dbgName.c_str()) };
	}

/*
=================================================
	NextSubpass
=================================================
*/
	template <typename C>
	typename _MGraphicsContextImpl<C>::DrawCtx
		_MGraphicsContextImpl<C>::NextSubpass (DrawCtx& ctx, NtStringView dbgName)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( ctx._IsValid() );

		// TODO: dbgName
		return RVRef(ctx);
	}
	
/*
=================================================
	EndRenderPass
=================================================
*/
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx)
	{
		ASSERT( _primaryState.IsValid() );
		//ASSERT( ctx._IsValid() );

		//RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		_primaryState = Default;
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->_IsValid() );
	}
			
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( ctx._IsValid() );

		//RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		
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
	void  _MGraphicsContextImpl<C>::BeginMtRenderPass (const RenderPassDesc &desc, const MDrawCommandBatch &batch)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CHECK( _primaryState == batch.GetPrimaryCtxState() );
		CommitBarriers();	// for RG

		//CHECK( RawCtx::_BeginRenderPass( desc, _primaryState, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS ));
	}

/*
=================================================
	NextMtSubpass
=================================================
*/
	template <typename C>
	void  _MGraphicsContextImpl<C>::NextMtSubpass (const MDrawCommandBatch &batch)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( this->_IsValid() );
		
		//++_primaryState.subpassIndex;
		//ASSERT( usize{_primaryState.subpassIndex} < _primaryState.renderPass->Subpasses().size() );
		//CHECK( _primaryState == batch.GetPrimaryCtxState() );

		//RawCtx::_NextSubpass( this->_RawCmdBuf(), VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );
	}
	
/*
=================================================
	EndMtRenderPass
=================================================
*/
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndMtRenderPass ()
	{
		ASSERT( this->_IsValid() );

		//RawCtx::_EndRenderPass( this->_RawCmdBuf() );
		_primaryState = Default;
	}
	
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndMtRenderPass (const RenderPassDesc &desc)
	{
		ASSERT( this->_IsValid() );
		
		//RawCtx::_EndRenderPass( this->_RawCmdBuf() );

		//this->_mngr.AfterEndRenderPass( desc, _primaryState );
		//CommitBarriers();	// for RG

		_primaryState = Default;
	}
	
/*
=================================================
	ExecuteSecondary
=================================================
*/
	template <typename C>
	void  _MGraphicsContextImpl<C>::ExecuteSecondary (MDrawCommandBatch &batch)
	{
		ASSERT( IsInsideRenderPass() );
		CHECK( _primaryState == batch.GetPrimaryCtxState() );

		//uint																count;
		//StaticArray< VkCommandBuffer, GraphicsConfig::MaxCmdBufPerBatch >	cmdbufs;

		//CHECK( batch.GetCmdBuffers( OUT count, INOUT cmdbufs ));

		//if_likely( count > 0 )
		//	RawCtx::_Execute( ArrayView<VkCommandBuffer>{ cmdbufs.data(), count });
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
