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

		void  _DebugMarker (NtStringView text, RGBA8u);
		void  _PushDebugGroup (NtStringView text, RGBA8u);
		void  _PopDebugGroup ();
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
		ND_ DrawCtx		BeginRenderPass (const RenderPassDesc &desc);
		ND_ DrawCtx		NextSubpass (DrawCtx& ctx);
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

#endif // AE_ENABLE_VULKAN
