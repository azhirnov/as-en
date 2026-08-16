// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VGraphicsContext.h"

namespace AE::Graphics::_hidden_
{
namespace
{
	using ClearValues_t = StaticArray< VkClearValue, GraphicsConfig::MaxAttachments >;
	using Viewports_t	= DrawCommandBatch::Viewports_t;
	using Scissors_t	= DrawCommandBatch::Scissors_t;

/*
=================================================
	ConvertClearValue
=================================================
*/
	static void  ConvertClearValue (const RenderPassDesc::ClearValue_t &cv, OUT VkClearValue &result) __NE___
	{
		Visit( cv,
			[&result] (const RGBA32f &src)		{ MemCopy( OUT result.color.float32, src ); },
			[&result] (const RGBA32u &src)		{ MemCopy( OUT result.color.uint32, src ); },
			[&result] (const RGBA32i &src)		{ MemCopy( OUT result.color.int32, src ); },
			[&result] (const DepthStencil &src)	{ result.depthStencil = {src.depth, src.stencil}; },
			[&result] (const NullUnion &)		{ ZeroMem( OUT result ); }
		);
	}

/*
=================================================
	ConvertBeginRenderPass
=================================================
*/
	inline bool  ConvertBeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, OUT VkRenderPassBeginInfo &passBegin, OUT VkClearValue* clearValues) __NE___
	{
		passBegin.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passBegin.renderPass		= state.renderPass->Handle();
		passBegin.framebuffer		= state.framebuffer->Handle();
		passBegin.renderArea.offset	= { desc.area.left,			 desc.area.top			  };
		passBegin.renderArea.extent	= { uint(desc.area.Width()), uint(desc.area.Height()) };

		const auto&		attachments = state.renderPass->AttachmentMap();
		bool			clear_att	= false;

		for (auto [name, att] : desc.attachments)
		{
			auto	iter = attachments.find( name );
			CHECK_ERR( iter != attachments.end() );

			const bool	require_cv = iter->second.IsClear();

			if_unlikely( IsNullUnion( att.clearValue ) == require_cv )
			{
			#if not AE_OPTIMIZE_IDS
				String	str;
				str << "Clear color/depth for attachment '" << name.GetName() << "' (" << ToString(iter->second.Index()) << ") ";
				str << (require_cv ? "is required, but NOT specified" : "is NOT required, but specified");
				AE_LOGI( str );
			#endif
			}

			if ( require_cv )
			{
				ConvertClearValue( att.clearValue, OUT clearValues[ iter->second.Index() ]);
				clear_att = true;
			}
		}

		passBegin.clearValueCount	= (clear_att ? uint(desc.attachments.size()) : 0);
		passBegin.pClearValues		= (clear_att ? clearValues : null);

		ASSERT( passBegin.renderPass  != Default );
		ASSERT( passBegin.framebuffer != Default );
		return true;
	}

/*
=================================================
	_ValidateRenderPassDebugLabel
=================================================
*/
	Nd__In DebugLabel  _ValidateRenderPassDebugLabel (DebugLabel dbg)
	{
		if ( dbg.color == DebugLabel::ColorTable::Undefined )
			dbg.color = DebugLabel::ColorTable::DrawCtx;
		return dbg;
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	ConvertViewports
=================================================
*/
	void  ConvertViewports (ArrayView<Viewport> inViewports, ArrayView<RectI> inScissors, const int2 &areaSize,
							OUT DrawCommandBatch::Viewports_t &outViewports, OUT DrawCommandBatch::Scissors_t &outScissors) __NE___
	{
		ASSERT( inScissors.empty() or inScissors.size() == inViewports.size() );

		const usize		count = Min( inViewports.size(), outViewports.capacity() );
		ASSERT( inViewports.size() == count );

		// set viewports and default scissors
		for (usize i = 0; i < count; ++i)
		{
			const auto&	src = inViewports[i];

			VkViewport&	dst	= outViewports.emplace_back();
			dst.x			= src.rect.left;
			dst.y			= src.rect.top;
			dst.width		= src.rect.Width();
			dst.height		= src.rect.Height();
			dst.minDepth	= src.minDepth;
			dst.maxDepth	= src.maxDepth;

			// scissor
			VkRect2D&	rect	= outScissors.emplace_back();
			if ( inScissors.empty() )
			{
				rect.offset.x		= RoundToInt( Max( src.rect.left, 0.f ));
				rect.offset.y		= RoundToInt( Max( src.rect.top,  0.f ));
				rect.extent.width	= Min( RoundToInt( src.rect.Width()  ), areaSize.x );
				rect.extent.height	= Min( RoundToInt( src.rect.Height() ), areaSize.y );
			}
			else
			{
				const auto&	sc		= inScissors[i];
				rect.offset.x		= sc.left;
				rect.offset.y		= sc.top;
				rect.extent.width	= sc.Width();
				rect.extent.height	= sc.Height();
			}
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_BeginFirstAsyncPass
=================================================
*/
	RC<DrawCommandBatch>  _VDirectGraphicsCtx::_BeginFirstAsyncPass (const VPrimaryCmdBufState &primaryState, const RenderPassDesc &desc, DebugLabel dbg)
	{
		return RenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch( GraphicsScheduler(), primaryState, desc, dbg );
	}

	RC<DrawCommandBatch>  _VDirectGraphicsCtx::_BeginNextAsyncPass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg)
	{
		return RenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch( GraphicsScheduler(), prevPassBatch, dbg );
	}

/*
=================================================
	_BeginRenderPass
=================================================
*/
	bool  _VDirectGraphicsCtx::_BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content, DebugLabel dbg)
	{
		GFX_DBG_ONLY( _mngr.ProfilerBeginContext( _cmdbuf.Get(), _ValidateRenderPassDebugLabel( dbg ), ECtxType::RenderPass ));
		Unused( dbg );

		ClearValues_t			clear_values;
		VkRenderPassBeginInfo	pass_begin	= {};
		CHECK_ERR( ConvertBeginRenderPass( desc, state, OUT pass_begin, OUT clear_values.data() ));

		VkSubpassBeginInfo		subpass_begin = {};
		subpass_begin.sType		= VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
		subpass_begin.contents	= content;

		vkCmdBeginRenderPass2KHR( _cmdbuf.Get(), &pass_begin, &subpass_begin );
		return true;
	}

/*
=================================================
	_InitViewports
=================================================
*/
	void  _VDirectGraphicsCtx::_InitViewports (const RenderPassDesc &desc)
	{
		if ( desc.viewports.empty() )
			return;

		Viewports_t		viewports;
		Scissors_t		scissors;
		ConvertViewports( desc.viewports, Default, desc.area.Size(), OUT viewports, OUT scissors );

		vkCmdSetViewport( _cmdbuf.Get(), 0, uint(viewports.size()), viewports.data() );
		vkCmdSetScissor( _cmdbuf.Get(), 0, uint(scissors.size()), scissors.data() );
	}

/*
=================================================
	_NextSubpass
=================================================
*/
	void  _VDirectGraphicsCtx::_NextSubpass (VkCommandBuffer cmdbuf, VkSubpassContents content) const
	{
		VkSubpassBeginInfo		subpass_begin = {};
		subpass_begin.sType		= VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
		subpass_begin.contents	= content;

		VkSubpassEndInfo		subpass_end = {};
		subpass_end.sType		= VK_STRUCTURE_TYPE_SUBPASS_END_INFO;

		vkCmdNextSubpass2KHR( cmdbuf, &subpass_begin, &subpass_end );
	}

/*
=================================================
	_EndRenderPass
=================================================
*/
	void  _VDirectGraphicsCtx::_EndRenderPass (VkCommandBuffer cmdbuf) const
	{
		VkSubpassEndInfo	subpass_end = {};
		subpass_end.sType	= VK_STRUCTURE_TYPE_SUBPASS_END_INFO;

		vkCmdEndRenderPass2KHR( cmdbuf, &subpass_end );

		GFX_DBG_ONLY( _mngr.ProfilerEndContext( cmdbuf, ECtxType::RenderPass ));
	}

/*
=================================================
	_Execute
=================================================
*/
	void  _VDirectGraphicsCtx::_Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs)
	{
		vkCmdExecuteCommands( _cmdbuf.Get(), uint(secondaryCmdbufs.size()), secondaryCmdbufs.data() );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VDirectGraphicsCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseDirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VDirectGraphicsCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseDirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_BeginFirstAsyncPass
=================================================
*/
	RC<DrawCommandBatch>  _VIndirectGraphicsCtx::_BeginFirstAsyncPass (const VPrimaryCmdBufState &primaryState, const RenderPassDesc &desc, DebugLabel dbg)
	{
		return RenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch( GraphicsScheduler(), primaryState, desc, dbg );
	}

	RC<DrawCommandBatch>  _VIndirectGraphicsCtx::_BeginNextAsyncPass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg)
	{
		return RenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch( GraphicsScheduler(), prevPassBatch, dbg );
	}

/*
=================================================
	_BeginRenderPass
=================================================
*/
	bool  _VIndirectGraphicsCtx::_BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content, DebugLabel dbg)
	{
		GFX_DBG_ONLY( _mngr.ProfilerBeginContext( *_cmdbuf, _ValidateRenderPassDebugLabel( dbg ), ECtxType::RenderPass ));
		Unused( dbg );

		ClearValues_t			clear_values;
		VkRenderPassBeginInfo	pass_begin	= {};
		CHECK_ERR( ConvertBeginRenderPass( desc, state, OUT pass_begin, OUT clear_values.data() ));

		VkSubpassBeginInfo		subpass_begin = {};
		subpass_begin.sType		= VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
		subpass_begin.contents	= content;

		auto&	cmd			= _cmdbuf->CreateCmd< BeginRenderPassCmd, VkClearValue >( pass_begin.clearValueCount );	// throw
		auto*	clear_vals	= Cast<VkClearValue>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkClearValue> ));

		cmd.contents		= content;
		cmd.renderPass		= pass_begin.renderPass;
		cmd.framebuffer		= pass_begin.framebuffer;
		cmd.renderArea		= pass_begin.renderArea;
		cmd.clearValueCount	= pass_begin.clearValueCount;

		MemCopy( OUT clear_vals, clear_values.data(), SizeOf<VkClearValue> * pass_begin.clearValueCount );
		return true;
	}

/*
=================================================
	_InitViewports
=================================================
*/
	void  _VIndirectGraphicsCtx::_InitViewports (const RenderPassDesc &desc)
	{
		Viewports_t		viewports;
		Scissors_t		scissors;
		ConvertViewports( desc.viewports, Default, desc.area.Size(), OUT viewports, OUT scissors );

		// viewports
		{
			auto&	cmd		= _cmdbuf->CreateCmd< SetViewportCmd, VkViewport >( viewports.size() );	// throw
			auto*	vp_arr	= Cast<VkViewport>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkViewport> ));

			cmd.first	= 0;
			cmd.count	= ushort(viewports.size());
			MemCopy( OUT vp_arr, viewports.data(), ArraySizeOf(viewports) );
		}

		// scissors
		{
			auto&	cmd		 = _cmdbuf->CreateCmd< SetScissorCmd, VkRect2D >( scissors.size() );	// throw
			auto*	scis_arr = Cast<VkRect2D>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkRect2D> ));

			cmd.first	= 0;
			cmd.count	= ushort(scissors.size());
			MemCopy( OUT scis_arr, scissors.data(), ArraySizeOf(scissors) );
		}
	}

/*
=================================================
	_NextSubpass
=================================================
*/
	void  _VIndirectGraphicsCtx::_NextSubpass (VSoftwareCmdBuf& cmdbuf, VkSubpassContents content)
	{
		auto&	cmd = cmdbuf.CreateCmd< NextSubpassCmd >();	// throw

		cmd.contents = content;
	}

/*
=================================================
	_EndRenderPass
=================================================
*/
	void  _VIndirectGraphicsCtx::_EndRenderPass (VSoftwareCmdBuf& cmdbuf)
	{
		Unused( cmdbuf.CreateCmd< EndRenderPassCmd >() );	// throw

		GFX_DBG_ONLY(
			if ( auto prof = _mngr.GetBatch().GetProfiler() )
				cmdbuf.ProfilerEndContext( prof.get(), &_mngr.GetBatch(), ECtxType::RenderPass );
		)
	}

/*
=================================================
	_Execute
=================================================
*/
	void  _VIndirectGraphicsCtx::_Execute (ArrayView<VkCommandBuffer> secondaryCmdbufs)
	{
		auto&	cmd		= _cmdbuf->CreateCmd< ExecuteCommandsCmd, VkCommandBuffer >( secondaryCmdbufs.size() );	// throw
		auto*	cmdbufs	= Cast<VkCommandBuffer>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkCommandBuffer> ));

		cmd.count = uint(secondaryCmdbufs.size());
		MemCopy( OUT cmdbufs, secondaryCmdbufs.data(), ArraySizeOf(secondaryCmdbufs) );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VBakedCommands  _VIndirectGraphicsCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		return VBaseIndirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VIndirectGraphicsCtx::ReleaseCommandBuffer () __Th___
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
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf, DebugLabel dbg) :
		RawCtx{ task, RVRef(cmdbuf), dbg }  // throw
	{
		Validator_t::CtxInit( task.QueueMask() );
		ZeroMem( _finalStates );
	}

	template <typename C>
	_VGraphicsContextImpl<C>::_VGraphicsContextImpl (RenderCoroRef task, const DrawCommandBatch &batch, CmdBuf_t cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf), Default },  // throw
		_primaryState{ batch.GetPrimaryCtxState() }
	{
		GCTX_CHECK( IsInsideRenderPass() );
		Validator_t::CtxInit( task.QueueMask() );
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

		RawCtx::_NextSubpass( prevPassCtx._RawCmdBuf(), VK_SUBPASS_CONTENTS_INLINE );

		GFX_DBG_ONLY(
			String	dbg_name;
			dbg_name << this->_mngr.GetRenderTask().DbgFullName() << " - ";

			if ( dbg.label.empty() )	dbg_name << "Sp-" << ToString(_primaryState.subpassIndex);
			else						dbg_name << dbg.label;

			prevPassCtx.PushDebugGroup( DebugLabel{ dbg_name, dbg.color });
		)
		Unused( dbg );

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
	auto  _VGraphicsContextImpl<C>::BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg, void* userData) -> RC<DrawCommandBatch>
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
	auto  _VGraphicsContextImpl<C>::NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg, void* userData) -> RC<DrawCommandBatch>
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
	void  _VGraphicsContextImpl<C>::ExecuteSecondary (DrawCommandBatch &batch)
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
//-----------------------------------------------------------------------------

    template class _VGraphicsContextImpl< _VDirectGraphicsCtx >;
    template class _VGraphicsContextImpl< _VIndirectGraphicsCtx >;

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
