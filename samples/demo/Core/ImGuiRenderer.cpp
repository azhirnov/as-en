// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "demo/Core/ImGuiRenderer.h"

# include "imgui.h"
# include "imgui_internal.h"

namespace AE::Samples::Demo
{
namespace {
    static const float  DefaultScale = 0.2f;
}

/*
=================================================
    Init
=================================================
*/
    bool  ImGuiRenderer::Init (GfxMemAllocatorPtr gfxAlloc, RenderTechPipelinesPtr rtech)
    {
        CHECK_ERR( rtech );
        _rtech = rtech;

        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        // init ImGUI context
        if ( _imguiCtx == null )
        {
            IMGUI_CHECKVERSION();

            _imguiCtx = ImGui::CreateContext();
            CHECK_ERR( _imguiCtx != null );

            ImGui::StyleColorsDark();
            //ImGui::StyleColorsClassic();
        }

        // initialize font atlas
        {
            uint8_t*    pixels;
            int         width, height;
            _imguiCtx->IO.Fonts->GetTexDataAsRGBA32( OUT &pixels, OUT &width, OUT &height );

            _font.image = res_mngr.CreateImage( ImageDesc{}.SetDimension( width, height )
                                                    .SetFormat( EPixelFormat::RGBA8_UNorm )
                                                    .SetUsage( EImageUsage::Sampled | EImageUsage::TransferDst ),
                                                "Imgui font image", gfxAlloc );
            CHECK_ERR( _font.image );

            _font.view = res_mngr.CreateImageView( ImageViewDesc{}, _font.image, "Imgui font image view" );
            CHECK_ERR( _font.view );
        }

        _ppln = _rtech->GetGraphicsPipeline( PipelineName{"imgui"} );
        CHECK_ERR( _ppln );

        {
            auto [ds, idx] = res_mngr.CreateDescriptorSet( _ppln, DescriptorSetName{"imgui.ds0"} );
            CHECK_ERR( ds and idx == _dsIndex );
            _descSet = RVRef(ds);
        }{
            DescriptorUpdater   updater;

            CHECK( updater.Set( _descSet, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_Texture"}, _font.view );
            CHECK( updater.Flush() );
        }
        return true;
    }

/*
=================================================
    SetScale
=================================================
*/
    void  ImGuiRenderer::SetScale (float scale)
    {
        _scale = scale * DefaultScale;
    }

    void  ImGuiRenderer::_UpdateScale (float2 pixToMm)
    {
        _uiToPix = _scale / Max( pixToMm.x, pixToMm.y );
        _pixToUI = 1.f / _uiToPix;
    }

/*
=================================================
    Upload
=================================================
*/
    bool  ImGuiRenderer::Upload (DirectCtx::Transfer &ctx)
    {
        if_unlikely( not _fontInitialized )
        {
            _fontInitialized = true;
            return _Upload( ctx );
        }
        return true;
    }

/*
=================================================
    Render
=================================================
*/
    bool  ImGuiRenderer::Render (DirectCtx::Draw &ctx,
                                 const IOutputSurface::RenderTarget &rt,
                                 const Function<void()> &ui)
    {
        CHECK_ERR( _fontInitialized );
        CHECK_ERR( _Update( rt, ui ));

        // same as ImGui::GetDrawData()
        auto*   viewport = _imguiCtx->Viewports[0];

        if_likely( viewport->DrawDataP.Valid )
            _DrawUI( ctx, viewport->DrawDataP );

        return true;
    }

/*
=================================================
    Draw
=================================================
*/
    bool  ImGuiRenderer::Draw (RenderTask &rtask, DirectCtx::CommandBuffer cmdbuf, const RenderTechPassName &passName, 
                               const IOutputSurface::RenderTarget &rt, const Function<void()> &ui, const Function<void(DirectCtx::Draw &)> &draw)
    {
        return Draw( rtask, RVRef(cmdbuf), passName, rt, RGBA32f{ 0.45f, 0.55f, 0.60f, 1.00f }, ui, draw );
    }

    bool  ImGuiRenderer::Draw (RenderTask &rtask, IOutputSurface &surface,
                               const Function<void()> &ui, const Function<void(DirectCtx::Draw &)> &draw)
    {
        IOutputSurface::RenderTargets_t     targets;
        CHECK_ERR( surface.GetTargets( OUT targets ));

        auto&   rt = targets[0];
        rt.initialState |= EResourceState::Invalidate;

        return Draw( rtask, DirectCtx::CommandBuffer{}, RenderTechPassName{"Main"}, rt, RGBA32f{ 0.45f, 0.55f, 0.60f, 1.00f }, ui, draw );
    }

    bool  ImGuiRenderer::Draw (RenderTask &rtask, DirectCtx::CommandBuffer cmdbuf, const RenderTechPassName &passName,
                               const IOutputSurface::RenderTarget &rt, const RenderPassDesc::ClearValue_t &clearValue,
                               const Function< void () > &ui, const Function< void (DirectCtx::Draw &) > &draw)
    {
        CHECK_ERR( _Update( rt, ui ));

        // same as ImGui::GetDrawData()
        auto*   viewport = _imguiCtx->Viewports[0];

        if_unlikely( not _fontInitialized )
        {
            DirectCtx::Transfer     copy_ctx{ rtask, RVRef(cmdbuf) };

            _fontInitialized = true;
            Unused( _Upload( copy_ctx ));

            cmdbuf = copy_ctx.ReleaseCommandBuffer();
        }

        DirectCtx::Graphics     gctx{ rtask, RVRef(cmdbuf) };

        gctx.AccumBarriers()
            .MemoryBarrier( EResourceState::Host_Write, EResourceState::VertexBuffer )
            .MemoryBarrier( EResourceState::Host_Write, EResourceState::IndexBuffer )
            .MemoryBarrier( EResourceState::CopyDst, EResourceState::ShaderUniform | EResourceState::FragmentShader | EResourceState::PreRasterizationShaders );

        const auto  rp_desc = RenderPassDesc{ _rtech, passName, rt.RegionSize() }
                                .AddViewport( rt.RegionSize() )
                                .AddTarget( AttachmentName{"Color"}, rt.viewId, clearValue, rt.initialState, rt.finalState );

        auto    dctx = gctx.BeginRenderPass( rp_desc, DebugLabel{"ImGui", HtmlColor::Yellow} );

        if ( draw )
            draw( dctx );

        if_likely( viewport->DrawDataP.Valid )
            _DrawUI( dctx, viewport->DrawDataP );

        gctx.EndRenderPass( dctx, rp_desc );

        rtask.Execute( gctx );
        return true;
    }

/*
=================================================
    _Update
=================================================
*/
    bool  ImGuiRenderer::_Update (const IOutputSurface::RenderTarget &rt, const Function<void()> &ui)
    {
        _UpdateScale( rt.pixToMm );

        ImGui::SetCurrentContext( _imguiCtx );

        const float     dt      = RenderTaskScheduler().GetFrameTimeDelta().count();
        ImGuiIO &       io      = ImGui::GetIO();
        const float2    size    = rt.RegionSizePxf() * _pixToUI;
        const float2    pos     = mousePos * _pixToUI;

        io.DisplaySize  = ImVec2{ size.x, size.y };
        io.DeltaTime    = dt;

        // update input
        {
            ZeroMem( OUT io.MouseDown );
            ZeroMem( OUT io.NavInputs );

            io.MouseDown[0] = mouseLBDown or touchActive;
            io.MousePos     = ImVec2{ pos.x, pos.y };
        }

        ImGui::NewFrame();

        if ( ui ) ui();

        ImGui::Render();

        return true;
    }

/*
=================================================
    _UploadVB
=================================================
*/
    bool  ImGuiRenderer::_UploadVB (DirectCtx::Draw &dctx, const ImDrawData &drawData)
    {
        // allocate
        Bytes   vtx_size;
        Bytes   idx_size;

        for (int i = 0; i < drawData.CmdListsCount; ++i)
        {
            ImDrawList const&   cmd_list = *drawData.CmdLists[i];

            vtx_size += cmd_list.VtxBuffer.Size * SizeOf<ImDrawVert>;
            idx_size += cmd_list.IdxBuffer.Size * SizeOf<ImDrawIdx>;
        }

        VertexStream    vstream;
        VertexStream    istream;
        CHECK_ERR( dctx.AllocVStream( vtx_size, OUT vstream ));
        CHECK_ERR( dctx.AllocVStream( idx_size, OUT istream ));


        // upload
        Bytes   vtx_offset;
        Bytes   idx_offset;

        for (int i = 0; i < drawData.CmdListsCount; ++i)
        {
            ImDrawList const&   cmd_list = *drawData.CmdLists[i];

            std::memcpy( OUT vstream.mappedPtr + vtx_offset, cmd_list.VtxBuffer.Data, cmd_list.VtxBuffer.Size * sizeof(ImDrawVert) );
            std::memcpy( OUT istream.mappedPtr + idx_offset, cmd_list.IdxBuffer.Data, cmd_list.IdxBuffer.Size * sizeof(ImDrawIdx) );

            vtx_offset += cmd_list.VtxBuffer.Size * SizeOf<ImDrawVert>;
            idx_offset += cmd_list.IdxBuffer.Size * SizeOf<ImDrawIdx>;
        }

        ASSERT( vtx_size == vtx_offset );
        ASSERT( idx_size == idx_offset );


        // bind
        dctx.BindVertexBuffer( 0, vstream.id, vstream.offset );
        dctx.BindIndexBuffer( istream.id, istream.offset, IndexDesc<ImDrawIdx>::value );

        return true;
    }

/*
=================================================
    _DrawUI
=================================================
*/
    bool  ImGuiRenderer::_DrawUI (DirectCtx::Draw &dctx, const ImDrawData &drawData)
    {
        const bool  is_minimized = (drawData.DisplaySize.x <= 0.0f or drawData.DisplaySize.y <= 0.0f);

        if_unlikely( drawData.TotalVtxCount <= 0 or is_minimized )
            return false;

        dctx.BindPipeline( _ppln );
        dctx.BindDescriptorSet( _dsIndex, _descSet );

        {
            ImVec2 const&           scale   = _imguiCtx->IO.DisplayFramebufferScale;
            ShaderTypes::imgui_ub   ub_data;

            ub_data.scale.x     = 2.0f / (drawData.DisplaySize.x * scale.x);
            ub_data.scale.y     = 2.0f / (drawData.DisplaySize.y * scale.y);

            ub_data.translate.x = -1.0f - (drawData.DisplayPos.x * ub_data.scale.x);
            ub_data.translate.y = -1.0f - (drawData.DisplayPos.y * ub_data.scale.y);

            dctx.PushConstant( _pcIndex, ub_data );
        }

        CHECK_ERR( _UploadVB( dctx, drawData ));

        uint    idx_offset  = 0;
        uint    vtx_offset  = 0;

        for (int i = 0; i < drawData.CmdListsCount; ++i)
        {
            ImDrawList const&   cmd_list = *drawData.CmdLists[i];

            for (int j = 0; j < cmd_list.CmdBuffer.Size; ++j)
            {
                ImDrawCmd const&    cmd = cmd_list.CmdBuffer[j];

                if_likely( cmd.UserCallback == null )
                {
                    RectI   scissor;
                    scissor.left    = int(cmd.ClipRect.x * _uiToPix + 0.5f);
                    scissor.top     = int(cmd.ClipRect.y * _uiToPix + 0.5f);
                    scissor.right   = int(cmd.ClipRect.z * _uiToPix + 0.5f);
                    scissor.bottom  = int(cmd.ClipRect.w * _uiToPix + 0.5f);

                    dctx.SetScissor( scissor );

                    dctx.DrawIndexed( cmd.ElemCount, 1, cmd.IdxOffset + idx_offset, cmd.VtxOffset + vtx_offset, 0 );
                }
                else
                {
                    cmd.UserCallback( &cmd_list, &cmd );
                }
            }
            idx_offset += cmd_list.IdxBuffer.Size;
            vtx_offset += cmd_list.VtxBuffer.Size;
        }
        return true;
    }

/*
=================================================
    _Upload
=================================================
*/
    bool  ImGuiRenderer::_Upload (DirectCtx::Transfer &copyCtx)
    {
        ubyte*  pixels;
        int     width, height;
        _imguiCtx->IO.Fonts->GetTexDataAsRGBA32( OUT &pixels, OUT &width, OUT &height );

        copyCtx.AccumBarriers()
            .MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc )
            .ImageBarrier( _font.image, EResourceState::Unknown, EResourceState::CopyDst );

        UploadImageDesc upload;
        upload.aspectMask   = EImageAspect::Color;
        upload.heapType     = EStagingHeapType::Dynamic;
        upload.imageSize    = uint3{width, height, 1};
        upload.dataRowPitch = Bytes{width * 4 * sizeof(ubyte)};

        const Bytes size    {width * height * 4 * sizeof(ubyte)};
        const bool  result  = (copyCtx.UploadImage( _font.image, upload, ArrayView<ubyte>{ pixels, usize(size) }) == size);

        copyCtx.AccumBarriers()
            .ImageBarrier( _font.image, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );

        return result;
    }

/*
=================================================
    constructor
=================================================
*/
    ImGuiRenderer::ImGuiRenderer (ImGuiContext* ctx) :
        _imguiCtx{ctx}, _scale{DefaultScale}
    {}

/*
=================================================
    destructor
=================================================
*/
    ImGuiRenderer::~ImGuiRenderer ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        res_mngr.DelayedReleaseResources( _descSet, _font.image, _font.view );

        if ( _imguiCtx != null )
            ImGui::DestroyContext( _imguiCtx );
    }


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI
