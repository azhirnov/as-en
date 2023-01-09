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
		ND_ MetalCommandBufferRC	EndCommandBuffer ()						__Th___;
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ()					__Th___;

		MBARRIERMNGR_INHERIT_MBARRIERS

	protected:
		_MDirectGraphicsCtx (const RenderTask &task)						__Th___	: MBaseDirectContext{ task }				{ CHECK_THROW( _IsValid() ); }
		_MDirectGraphicsCtx (const RenderTask &task, MCommandBuffer cmdbuf)	__Th___	: MBaseDirectContext{ task, RVRef(cmdbuf) }	{ CHECK_THROW( _IsValid() ); }
		
		ND_ bool	_IsValid ()												C_NE___	{ return this->_cmdbuf.HasCmdBuf() and this->_cmdbuf.IsRecording(); }
		
		ND_ RC<MDrawCommandBatch>  _BeginFirstAsyncPass (const MPrimaryCmdBufState &, const RenderPassDesc &, DebugLabel dbg);
		ND_ RC<MDrawCommandBatch>  _BeginNextAsyncPass (const MDrawCommandBatch &prevPassBatch, DebugLabel dbg);

		void  _BeginRenderPass (const MPrimaryCmdBufState &primaryState, const RenderPassDesc &rpDesc, DebugLabel dbg);
		void  _NextMtSubpass () const;
		void  _EndMtRenderPass ();
		
		void  _DebugMarker (DebugLabel dbg)							{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)						{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup(); }
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
		ND_ MBakedCommands		EndCommandBuffer ()								__Th___;
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ()							__Th___;

		MBARRIERMNGR_INHERIT_MBARRIERS

	protected:
		_MIndirectGraphicsCtx (const RenderTask &task)							 __Th___ : MBaseIndirectContext{ task } {}
		_MIndirectGraphicsCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf)__Th___ : MBaseIndirectContext{ task, RVRef(cmdbuf) } {}
		
		ND_ RC<MDrawCommandBatch>  _BeginFirstAsyncPass (const MPrimaryCmdBufState &, const RenderPassDesc &, DebugLabel dbg);
		ND_ RC<MDrawCommandBatch>  _BeginNextAsyncPass (const MDrawCommandBatch &prevPassBatch, DebugLabel dbg);
		
		void  _BeginRenderPass (const MPrimaryCmdBufState &primaryState, const RenderPassDesc &rpDesc, DebugLabel dbg);
		void  _NextMtSubpass () const;
		void  _EndMtRenderPass ();
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

		using DrawCtx		= typename CtxImpl::_DrawCtx;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= MAccumBarriers< _MGraphicsContextImpl< CtxImpl >>;
		using DeferredBar	= MAccumDeferredBarriersForCtx< _MGraphicsContextImpl< CtxImpl >>;


	// variables
	private:
		MPrimaryCmdBufState		_primaryState;

		
	// methods
	public:
		explicit _MGraphicsContextImpl (const RenderTask &task)								__Th___;
		
		template <typename RawCmdBufType>
		_MGraphicsContextImpl (const RenderTask &task, RawCmdBufType cmdbuf)				__Th___;
		
		template <typename RawCmdBufType>
		_MGraphicsContextImpl (const RenderTask &, const MDrawCommandBatch &, RawCmdBufType) __Th___;

		_MGraphicsContextImpl ()															= delete;
		_MGraphicsContextImpl (const _MGraphicsContextImpl &)								= delete;
		

		// returns invalid state if outside of render pass
		ND_ MPrimaryCmdBufState const&  GetState ()											C_NE___	{ return _primaryState; }
		ND_ bool						IsInsideRenderPass ()								C_NE___	{ return _primaryState.IsValid(); }
		ND_ bool						IsSecondaryCmdbuf ()								C_NE___	{ return _primaryState.useSecondaryCmdbuf; }


		// synchronious rendering api
		ND_ DrawCtx	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)				__Th___;
		ND_ DrawCtx	NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg = Default)						__Th___;
			void	EndRenderPass (DrawCtx& ctx)														__Th___;
			void	EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)							__Th___;

			
		// asynchronious rendering api (Metal compatible)
		ND_ auto	BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)			__Th___;
		ND_ auto	NextMtSubpass (const MDrawCommandBatch &prevPassBatch, DebugLabel dbg = Default)	__Th___;
			void	EndMtRenderPass ()																	__Th___;
			void	EndMtRenderPass (const RenderPassDesc &desc)										__Th___;
			void	ExecuteSecondary (MDrawCommandBatch &batch)											__Th___;

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectGraphicsContext	= Graphics::_hidden_::_MGraphicsContextImpl< Graphics::_hidden_::_MDirectGraphicsCtx >;
	using MIndirectGraphicsContext	= Graphics::_hidden_::_MGraphicsContextImpl< Graphics::_hidden_::_MIndirectGraphicsCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_MGraphicsContextImpl<C>::_MGraphicsContextImpl (const RenderTask &task) :
		RawCtx{ task }	// throw
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics, task.GetQueueMask() ));
	}
	
	template <typename C>
	template <typename RawCmdBufType>
	_MGraphicsContextImpl<C>::_MGraphicsContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) }	// throw
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics, task.GetQueueMask() ));
	}
	
	template <typename C>
	template <typename RawCmdBufType>
	_MGraphicsContextImpl<C>::_MGraphicsContextImpl (const RenderTask &task, const MDrawCommandBatch &batch, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) },				// throw
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
	typename _MGraphicsContextImpl<C>::DrawCtx
		_MGraphicsContextImpl<C>::BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );

		CHECK_THROW( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		ASSERT( not IsSecondaryCmdbuf() );

		RawCtx::_BeginRenderPass( _primaryState, desc, dbg );
		
		CommitBarriers();	// for RG
		
		String	dbg_name;
		PROFILE_ONLY(
			dbg_name += this->_mngr.GetRenderTask().DbgFullName();
			dbg_name += " - ";
			dbg_name += (dbg.label.empty() ? StringView{"RP"} : dbg.label);
		)

		return DrawCtx{ _primaryState, this->ReleaseCommandBuffer(), desc.viewports, DebugLabel{ dbg_name, dbg.color }};	// throw
	}

/*
=================================================
	NextSubpass
=================================================
*/
	template <typename C>
	typename _MGraphicsContextImpl<C>::DrawCtx
		_MGraphicsContextImpl<C>::NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( _primaryState.IsValid() );
		ASSERT( not IsSecondaryCmdbuf() );
		ASSERT( prevPassCtx._IsValid() );
		
		++_primaryState.rasterOrderGroup;
		ASSERT( uint{_primaryState.rasterOrderGroup} < _primaryState.renderPass->RasterOrderGroupCount() );

		return RVRef(prevPassCtx);
	}
	
/*
=================================================
	EndRenderPass
=================================================
*/
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( _primaryState.IsValid() );
		ASSERT( not IsSecondaryCmdbuf() );
		ASSERT( ctx._IsValid() );

		_primaryState = Default;
		
		this->_cmdbuf = ctx.ReleaseCommandBuffer();
		ASSERT( this->_IsValid() );
	}
			
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndRenderPass (DrawCtx& ctx, const RenderPassDesc &desc)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( _primaryState.IsValid() );
		ASSERT( not IsSecondaryCmdbuf() );
		ASSERT( ctx._IsValid() );
		
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
	auto  _MGraphicsContextImpl<C>::BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );
		
		CHECK_THROW( this->_mngr.BeforeBeginRenderPass( desc, OUT _primaryState ));
		CommitBarriers();	// for RG

		_primaryState.useSecondaryCmdbuf = true;
		ASSERT( IsSecondaryCmdbuf() );

		auto	batch = RawCtx::_BeginFirstAsyncPass( _primaryState, desc, dbg );
		CHECK_THROW( batch );
		
		ASSERT( _primaryState == batch->GetPrimaryCtxState() );
		return batch;
	}

/*
=================================================
	NextMtSubpass
=================================================
*/
	template <typename C>
	auto  _MGraphicsContextImpl<C>::NextMtSubpass (const MDrawCommandBatch &prevPassBatch, DebugLabel dbg)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( _primaryState.IsValid() );
		ASSERT( IsSecondaryCmdbuf() );
		ASSERT( this->_IsValid() );
		
		auto	batch = RawCtx::_BeginNextAsyncPass( prevPassBatch, dbg );
		CHECK_THROW( batch );
		
		++_primaryState.rasterOrderGroup;
		ASSERT( uint{_primaryState.rasterOrderGroup} < _primaryState.renderPass->RasterOrderGroupCount() );
		ASSERT( _primaryState == prevPassBatch.GetPrimaryCtxState() );
		
		RawCtx::_NextMtSubpass();
		return batch;
	}
	
/*
=================================================
	EndMtRenderPass
=================================================
*/
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndMtRenderPass ()
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( this->_IsValid() );
		ASSERT( IsSecondaryCmdbuf() );

		RawCtx::_EndMtRenderPass();
		_primaryState = Default;
	}
	
	template <typename C>
	void  _MGraphicsContextImpl<C>::EndMtRenderPass (const RenderPassDesc &desc)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( this->_IsValid() );
		ASSERT( IsSecondaryCmdbuf() );
		
		RawCtx::_EndMtRenderPass();

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
	void  _MGraphicsContextImpl<C>::ExecuteSecondary (MDrawCommandBatch &batch)
	{
		ASSERT( this->_NoPendingBarriers() );
		ASSERT( IsInsideRenderPass() );
		ASSERT( IsSecondaryCmdbuf() );
		CHECK( _primaryState == batch.GetPrimaryCtxState() );

		CHECK( batch.EndAllSecondary() );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
