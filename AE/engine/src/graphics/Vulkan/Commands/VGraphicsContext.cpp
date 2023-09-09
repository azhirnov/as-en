// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VGraphicsContext.h"

namespace AE::Graphics::_hidden_
{
namespace
{
    using ClearValues_t = StaticArray< VkClearValue, GraphicsConfig::MaxAttachments >;
    using Viewports_t   = VDrawCommandBatch::Viewports_t;
    using Scissors_t    = VDrawCommandBatch::Scissors_t;

/*
=================================================
    ConvertClearValue
=================================================
*/
    static void  ConvertClearValue (const RenderPassDesc::ClearValue_t &cv, OUT VkClearValue &result) __NE___
    {
        Visit( cv,
            [&result] (const RGBA32f &src)      { MemCopy( OUT result.color.float32, src ); },
            [&result] (const RGBA32u &src)      { MemCopy( OUT result.color.uint32, src ); },
            [&result] (const RGBA32i &src)      { MemCopy( OUT result.color.int32, src ); },
            [&result] (const DepthStencil &src) { result.depthStencil = {src.depth, src.stencil}; },
            [&result] (const NullUnion &)       { ZeroMem( OUT result ); }
        );
    }

/*
=================================================
    ConvertBeginRenderPass
=================================================
*/
    inline bool  ConvertBeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, OUT VkRenderPassBeginInfo &passBegin, OUT VkClearValue* clearValues) __NE___
    {
        passBegin.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        passBegin.renderPass        = state.renderPass->Handle();
        passBegin.framebuffer       = state.framebuffer->Handle();
        passBegin.renderArea.offset = { desc.area.left,          desc.area.top            };
        passBegin.renderArea.extent = { uint(desc.area.Width()), uint(desc.area.Height()) };

        const auto&     attachments = state.renderPass->AttachmentMap();
        bool            clear_att   = false;

        for (auto [name, att] : desc.attachments)
        {
            auto    iter = attachments.find( name );
            CHECK_ERR( iter != attachments.end() );

            const bool  require_cv = iter->second.IsClear();

            if_unlikely( IsNullUnion( att.clearValue ) == require_cv )
            {
            #if not AE_OPTIMIZE_IDS
                String  str;
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

        passBegin.clearValueCount   = (clear_att ? uint(desc.attachments.size()) : 0);
        passBegin.pClearValues      = (clear_att ? clearValues : null);

        ASSERT( passBegin.renderPass  != Default );
        ASSERT( passBegin.framebuffer != Default );
        return true;
    }

/*
=================================================
    _ValidateRenderPassDebugLabel
=================================================
*/
    ND_ inline DebugLabel  _ValidateRenderPassDebugLabel (DebugLabel dbg)
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
    void  ConvertViewports (ArrayView<RenderPassDesc::Viewport> inViewports, ArrayView<RectI> inScissors,
                            OUT VDrawCommandBatch::Viewports_t &outViewports, OUT VDrawCommandBatch::Scissors_t &outScissors) __NE___
    {
        ASSERT( not inViewports.empty() );
        ASSERT( inScissors.empty() or inScissors.size() == inViewports.size() );

        // set viewports and default scissors
        for (usize i = 0; i < inViewports.size(); ++i)
        {
            const auto& src = inViewports[i];

            VkViewport& dst = outViewports.emplace_back();
            dst.x           = src.rect.left;
            dst.y           = src.rect.top;
            dst.width       = src.rect.Width();
            dst.height      = src.rect.Height();
            dst.minDepth    = src.minDepth;
            dst.maxDepth    = src.maxDepth;

            // scissor
            VkRect2D&   rect    = outScissors.emplace_back();
            if ( inScissors.empty() )
            {
                rect.offset.x       = RoundToInt( src.rect.left );
                rect.offset.y       = RoundToInt( src.rect.top );
                rect.extent.width   = RoundToInt( src.rect.Width() );
                rect.extent.height  = RoundToInt( src.rect.Height() );
            }
            else
            {
                const auto& sc      = inScissors[i];
                rect.offset.x       = sc.left;
                rect.offset.y       = sc.top;
                rect.extent.width   = sc.Width();
                rect.extent.height  = sc.Height();
            }
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _BeginFirstAsyncPass
=================================================
*/
    RC<VDrawCommandBatch>  _VDirectGraphicsCtx::_BeginFirstAsyncPass (const VPrimaryCmdBufState &primaryState, const RenderPassDesc &desc, DebugLabel dbg)
    {
        return VRenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch( RenderTaskScheduler(), primaryState, desc, dbg );
    }

    RC<VDrawCommandBatch>  _VDirectGraphicsCtx::_BeginNextAsyncPass (const VDrawCommandBatch &prevPassBatch, DebugLabel dbg)
    {
        return VRenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch( RenderTaskScheduler(), prevPassBatch, dbg );
    }

/*
=================================================
    _BeginRenderPass
=================================================
*/
    bool  _VDirectGraphicsCtx::_BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content, DebugLabel dbg)
    {
        DBG_GRAPHICS_ONLY( _mngr.ProfilerBeginContext( _cmdbuf.Get(), _ValidateRenderPassDebugLabel( dbg ), ECtxType::RenderPass ); )
        Unused( dbg );

        ClearValues_t           clear_values;
        VkRenderPassBeginInfo   pass_begin  = {};
        CHECK_ERR( ConvertBeginRenderPass( desc, state, OUT pass_begin, OUT clear_values.data() ));

        VkSubpassBeginInfo      subpass_begin = {};
        subpass_begin.sType     = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
        subpass_begin.contents  = content;

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
        Viewports_t     viewports;
        Scissors_t      scissors;
        ConvertViewports( desc.viewports, Default, OUT viewports, OUT scissors );

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
        VkSubpassBeginInfo      subpass_begin = {};
        subpass_begin.sType     = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
        subpass_begin.contents  = content;

        VkSubpassEndInfo        subpass_end = {};
        subpass_end.sType       = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;

        vkCmdNextSubpass2KHR( cmdbuf, &subpass_begin, &subpass_end );
    }

/*
=================================================
    _EndRenderPass
=================================================
*/
    void  _VDirectGraphicsCtx::_EndRenderPass (VkCommandBuffer cmdbuf) const
    {
        VkSubpassEndInfo    subpass_end = {};
        subpass_end.sType   = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;

        vkCmdEndRenderPass2KHR( cmdbuf, &subpass_end );

        DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( cmdbuf, ECtxType::RenderPass ); )
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
//-----------------------------------------------------------------------------



/*
=================================================
    _BeginFirstAsyncPass
=================================================
*/
    RC<VDrawCommandBatch>  _VIndirectGraphicsCtx::_BeginFirstAsyncPass (const VPrimaryCmdBufState &primaryState, const RenderPassDesc &desc, DebugLabel dbg)
    {
        return VRenderTaskScheduler::GraphicsContextApi::CreateFirstPassBatch( RenderTaskScheduler(), primaryState, desc, dbg );
    }

    RC<VDrawCommandBatch>  _VIndirectGraphicsCtx::_BeginNextAsyncPass (const VDrawCommandBatch &prevPassBatch, DebugLabel dbg)
    {
        return VRenderTaskScheduler::GraphicsContextApi::CreateNextPassBatch( RenderTaskScheduler(), prevPassBatch, dbg );
    }

/*
=================================================
    _BeginRenderPass
=================================================
*/
    bool  _VIndirectGraphicsCtx::_BeginRenderPass (const RenderPassDesc &desc, const VPrimaryCmdBufState &state, VkSubpassContents content, DebugLabel dbg)
    {
        DBG_GRAPHICS_ONLY( _mngr.ProfilerBeginContext( *_cmdbuf, _ValidateRenderPassDebugLabel( dbg ), ECtxType::RenderPass ); )
        Unused( dbg );

        ClearValues_t           clear_values;
        VkRenderPassBeginInfo   pass_begin  = {};
        CHECK_ERR( ConvertBeginRenderPass( desc, state, OUT pass_begin, OUT clear_values.data() ));

        VkSubpassBeginInfo      subpass_begin = {};
        subpass_begin.sType     = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
        subpass_begin.contents  = content;

        auto&   cmd         = _cmdbuf->CreateCmd< BeginRenderPassCmd, VkClearValue >( pass_begin.clearValueCount ); // throw
        auto*   clear_vals  = Cast<VkClearValue>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkClearValue> ));

        cmd.contents        = content;
        cmd.renderPass      = pass_begin.renderPass;
        cmd.framebuffer     = pass_begin.framebuffer;
        cmd.renderArea      = pass_begin.renderArea;
        cmd.clearValueCount = pass_begin.clearValueCount;

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
        Viewports_t     viewports;
        Scissors_t      scissors;
        ConvertViewports( desc.viewports, Default, OUT viewports, OUT scissors );

        // viewports
        {
            auto&   cmd     = _cmdbuf->CreateCmd< SetViewportCmd, VkViewport >( viewports.size() ); // throw
            auto*   vp_arr  = Cast<VkViewport>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkViewport> ));

            cmd.first   = 0;
            cmd.count   = ushort(viewports.size());
            MemCopy( OUT vp_arr, viewports.data(), ArraySizeOf(viewports) );
        }

        // scissors
        {
            auto&   cmd      = _cmdbuf->CreateCmd< SetScissorCmd, VkRect2D >( scissors.size() );    // throw
            auto*   scis_arr = Cast<VkRect2D>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkRect2D> ));

            cmd.first   = 0;
            cmd.count   = ushort(scissors.size());
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
        auto&   cmd = cmdbuf.CreateCmd< NextSubpassCmd >(); // throw

        cmd.contents = content;
    }

/*
=================================================
    _EndRenderPass
=================================================
*/
    void  _VIndirectGraphicsCtx::_EndRenderPass (VSoftwareCmdBuf& cmdbuf)
    {
        Unused( cmdbuf.CreateCmd< EndRenderPassCmd >() );   // throw

        DBG_GRAPHICS_ONLY(
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
        auto&   cmd     = _cmdbuf->CreateCmd< ExecuteCommandsCmd, VkCommandBuffer >( secondaryCmdbufs.size() ); // throw
        auto*   cmdbufs = Cast<VkCommandBuffer>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkCommandBuffer> ));

        cmd.count = uint(secondaryCmdbufs.size());
        MemCopy( OUT cmdbufs, secondaryCmdbufs.data(), ArraySizeOf(secondaryCmdbufs) );
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
