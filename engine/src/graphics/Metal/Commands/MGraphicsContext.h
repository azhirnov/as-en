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
	protected:
		_MDirectGraphicsCtx (Ptr<MCommandBatch> batch);
		_MDirectGraphicsCtx (Ptr<MCommandBatch> batch, MCommandBuffer cmdbuf);
		
		void  _CommitBarriers ();

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
	protected:
		_MIndirectGraphicsCtx (Ptr<MCommandBatch> batch) : MBaseIndirectContext{ batch } {}
		_MIndirectGraphicsCtx (Ptr<MCommandBatch> batch, MSoftwareCmdBufPtr cmdbuf) : MBaseIndirectContext{ batch, RVRef(cmdbuf) } {}
		
		void  _CommitBarriers ();
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
		explicit _MGraphicsContextImpl (Ptr<MCommandBatch> batch) : CtxImpl{ batch } {}
		
		template <typename RawCmdBufType>
		_MGraphicsContextImpl (Ptr<MCommandBatch> batch, RawCmdBufType cmdbuf) : CtxImpl{ batch, RVRef(cmdbuf) } {}

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


		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }

		ND_ AccumBar  AccumBarriers ()											{ return AccumBar{ *this }; }

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
		ASSERT( ctx.IsValid() );

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
		ASSERT( ctx.IsValid() );

		//RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		_primaryState = Default;
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->IsValid() );
	}
			
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)
	{
		ASSERT( _primaryState.IsValid() );
		ASSERT( ctx.IsValid() );

		//RawCtx::_EndRenderPass( ctx._RawCmdBuf() );
		
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
		ASSERT( this->IsValid() );
		
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
		ASSERT( this->IsValid() );

		//RawCtx::_EndRenderPass( this->_RawCmdBuf() );
		_primaryState = Default;
	}
	
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndMtRenderPass (const RenderPassDesc &desc)
	{
		ASSERT( this->IsValid() );
		
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
