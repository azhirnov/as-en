// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Examples/Canvas2D.h"

namespace AE::Samples::Demo
{
    INTERNAL_LINKAGE( constexpr auto&   RTech   = RenderTechs::Canvas_RTech );
    INTERNAL_LINKAGE( constexpr auto&   IA      = InputActions::Canvas2D );


    //
    // Upload Texture Task
    //
    class Canvas2DSample::UploadTextureTask final : public RenderTask
    {
    public:
        RC<Canvas2DSample>  t;

        UploadTextureTask (Canvas2DSample* p, CommandBatchPtr batch, DebugLabel) :
            RenderTask{ batch, {"Canvas2D::UploadTexture"} },
            t{ p }
        {}

        void  Run () __Th_OV;
    };

/*
=================================================
    UploadTextureTask::Run
=================================================
*/
    void  Canvas2DSample::UploadTextureTask::Run ()
    {
        // load texture
        {
            auto    file = GetVFS().Open<RStream>( VFS::FileName{"canvas2d.tex1"} );
            CHECK_TE( file );

            DirectCtx::Transfer     ctx{ *this };

            LoadableImage::Loader   loader;
            auto    image = loader.Load( file, ctx, t->gfxAlloc );
            CHECK_TE( image );

            t->tex = image->ReleaseImageAndView();

            ctx.AccumBarriers()
                .ImageBarrier( t->tex.image, EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );
            Execute( ctx );
        }

        // update DS
        {
            DescriptorUpdater   updater;
            CHECK_TE( updater.Set( t->ppln3_ds1, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_Texture"}, t->tex.view );
            CHECK_TE( updater.Flush() );
        }
    }
//-----------------------------------------------------------------------------



    //
    // Upload Atlas Task
    //
    class Canvas2DSample::UploadAtlasTask final : public RenderTask
    {
    public:
        RC<Canvas2DSample>  t;

        UploadAtlasTask (Canvas2DSample* p, CommandBatchPtr batch, DebugLabel) :
            RenderTask{ batch, {"Canvas2D::UploadAtlas"} },
            t{ p }
        {}

        void  Run () __Th_OV;
    };

/*
=================================================
    UploadAtlasTask::Run
=================================================
*/
    void  Canvas2DSample::UploadAtlasTask::Run ()
    {
        // load atlas
        {
            auto    file = GetVFS().Open<RStream>( VFS::FileName{"canvas2d.atlas"} );
            CHECK_TE( file );

            DirectCtx::Transfer     ctx{ *this };

            StaticImageAtlas::Loader    loader;
            t->atlas = loader.Load( file, ctx, t->gfxAlloc );
            CHECK_TE( t->atlas );

            ctx.AccumBarriers()
                .ImageBarrier( t->atlas->GetImageID(), EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );
            Execute( ctx );
        }

        // create DS
        {       
            DescriptorUpdater   updater;
            CHECK_TE( updater.Set( t->ppln3_ds2, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_Texture"}, t->atlas->GetViewID() );
            CHECK_TE( updater.Flush() );
        }
    }
//-----------------------------------------------------------------------------



    //
    // Upload Raster Font Task
    //
    class Canvas2DSample::UploadRasterFontTask final : public RenderTask
    {
    public:
        RC<RasterFont>  &   font;
        DescriptorSetID     ds;
        VFS::FileName       fname;

        UploadRasterFontTask (RC<RasterFont> *f, DescriptorSetID ds, const VFS::FileName &name, CommandBatchPtr batch, DebugLabel) :
            RenderTask{ batch, {"Canvas2D::UploadRasterFont"} },
            font{ *f }, ds{ds}, fname{name}
        {}

        void  Run () __Th_OV;
    };

/*
=================================================
    UploadRasterFontTask::Run
=================================================
*/
    void  Canvas2DSample::UploadRasterFontTask::Run ()
    {
        // load raster font
        {
            auto    file = GetVFS().Open<RStream>( fname );
            CHECK_TE( file );

            DirectCtx::Transfer     ctx{ *this };

            RasterFont::Loader  loader;
            auto    tmp = loader.Load( file, ctx, null );
            CHECK_TE( tmp );

            *font = RVRef(*tmp);

            ctx.AccumBarriers()
                .ImageBarrier( font->GetImageID(), EResourceState::CopyDst, EResourceState::ShaderSample | EResourceState::FragmentShader );
            Execute( ctx );
        }

        // create DS
        {
            DescriptorUpdater   updater;
            CHECK_TE( updater.Set( ds, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_Texture"}, font->GetViewID() );
            CHECK_TE( updater.Flush() );
        }
    }
//-----------------------------------------------------------------------------



    //
    // Process Input Task
    //
    class Canvas2DSample::ProcessInputTask final : public Threading::IAsyncTask
    {
    public:
        RC<Canvas2DSample>  t;
        ActionQueueReader   reader;

        ProcessInputTask (Canvas2DSample* p, ActionQueueReader reader) :
            IAsyncTask{ ETaskQueue::PerFrame },
            t{ p },
            reader{ RVRef(reader) }
        {}

        void  Run () __Th_OV;

        StringView  DbgName ()  C_NE_OV { return "Canvas2D::ProcessInput"; }
    };

/*
=================================================
    ProcessInputTask::Run
=================================================
*/
    void  Canvas2DSample::ProcessInputTask::Run ()
    {
        t->enter = false;

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            STATIC_ASSERT( IA.actionCount == 2 );
            switch ( uint{hdr.name} )
            {
                case IA.Cursor :
                    t->cursorPos = reader.Data<packed_float2>( hdr.offset );    break;

                case IA.Enter :
                    t->enter = true;                                            break;
            }
        }
    }
//-----------------------------------------------------------------------------



    //
    // Draw Task
    //
    class Canvas2DSample::DrawTask final : public RenderTask
    {
    public:
        RC<Canvas2DSample>  t;
        IOutputSurface &    surface;

        DrawTask (Canvas2DSample* p, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) :
            RenderTask{ batch, {"Canvas2D::Draw"} },
            t{ p }, surface{ surf }
        {}

        void  Run () __Th_OV;
    };

/*
=================================================
    DrawTask::Run
=================================================
*/
    void  Canvas2DSample::DrawTask::Run ()
    {
        IOutputSurface::RenderTargets_t     targets;
        CHECK_TE( surface.GetTargets( OUT targets ));

        auto&   rt = targets[0];

        DirectCtx::Transfer     copy_ctx{ *this };

        // update
        {
            t->angle = Wrap( t->angle + 0.005f, -float(Rad::Pi()), float(Rad::Pi()) );

            const auto  rot     = float2x2::Rotate( Rad{t->angle} );
            const auto  iden    = float2x2::Identity();

            ShaderTypes::sdf_font_ublock    ublock_data;
            ublock_data.sdfScale    = t->sdfFont->GetSDFConfig().scale;
            ublock_data.sdfBias     = t->sdfFont->GetSDFConfig().bias;
            ublock_data.pxRange     = t->sdfFont->GetSDFConfig().pixRange2D;
            ublock_data.bgColor     = float4{0.f};
            ublock_data.rotation0   = iden[0];
            ublock_data.rotation1   = iden[1];

            copy_ctx.UpdateBuffer( t->ublock, 0_b, Sizeof(ublock_data), &ublock_data );

            ublock_data.rotation0   = rot[0];
            ublock_data.rotation1   = rot[1];

            copy_ctx.UpdateBuffer( t->ublock, AlignUp( SizeOf<ShaderTypes::sdf_font_ublock>, DeviceLimits.res.minUniformBufferOffsetAlign ), Sizeof(ublock_data), &ublock_data );
        }

        DirectCtx::Graphics     gctx{ *this, copy_ctx.ReleaseCommandBuffer() };

        gctx.AccumBarriers()
            .MemoryBarrier( EResourceState::Host_Write, EResourceState::VertexBuffer )
            .MemoryBarrier( EResourceState::Host_Write, EResourceState::IndexBuffer )
            .MemoryBarrier( EResourceState::CopyDst,    EResourceState::UniformRead | EResourceState::PreRasterizationShaders | EResourceState::FragmentShader );

        auto&   canvas = *t->canvasPtr;
        canvas.SetDimensions( rt );
        canvas.NextFrame( GetFrameId() );

        // draw
        {
            constexpr auto& rtech_pass = RTech.Main;
            STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

            const auto  rp_desc = RenderPassDesc{ t->rtech, rtech_pass, rt.RegionSize() }
                                    .AddViewport( rt.RegionSize() )
                                    .AddTarget( rtech_pass.att_Color, rt.viewId, RGBA32f{HtmlColor::Black}, rt.initialState | EResourceState::Invalidate, rt.finalState );

            auto    dctx = gctx.BeginRenderPass( rp_desc );

            //{
            //  canvas.Draw( Circle2D{ 16, RectF{-0.9f, -0.5f, 0.2f, 0.2f}, HtmlColor::Blue });
            //  canvas.Flush( dctx, EPrimitive::LineList );
            //}

            // UV color
            {
                canvas.Draw( Rectangle2D{ RectF{-0.9f, -0.2f, -0.5f, 0.2f}, t->enter ? HtmlColor::Red : HtmlColor::BlueViolet });
                canvas.Draw( FilledCircle2D{ 16, RectF{0.5f, -0.2f, 0.9f, 0.2f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::Green });

                canvas.Draw( NinePatch2D{ RectF{-0.4f, -0.9f, 0.4f, -0.1f}, RectF{0.2f, 0.2f, 0.2f, 0.2f},
                                          RectF{0.f, 0.f, 1.f, 1.f},        RectF{0.25f, 0.25f, 0.25f, 0.25f},
                                          HtmlColor::Orange });


                dctx.BindPipeline( t->ppln1 );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }

            // wireframe
            if ( t->ppln2_wire )
            {
                canvas.Draw( NinePatch2D{ RectF{-0.4f, 0.1f, 0.4f, 0.9f},   RectF{0.2f, 0.2f, 0.2f, 0.2f},
                                          RectF{0.f, 0.f, 1.f, 1.f},        RectF{0.25f, 0.25f, 0.25f, 0.25f},
                                          HtmlColor::Orange });
                dctx.BindPipeline( t->ppln2_wire );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }

            // cursor (UV color)
            {
                const float2    pos = ToSNorm( t->cursorPos / float2(rt.RegionSize()) );
                canvas.Draw( Rectangle2D{ RectF{-0.05f, -0.05f, 0.05f, 0.05f} + pos, HtmlColor::White });

                dctx.BindPipeline( t->ppln1 );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }

            // textured
            {
                canvas.Draw( Rectangle2D{ RectF{float2{ 0.2f }} + float2{0.5f, -0.9f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::White });
                dctx.BindPipeline( t->ppln3 );
                dctx.BindDescriptorSet( t->dsIndex, t->ppln3_ds1 );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }{
                RectF   rect_uv;
                t->atlas->Get( ImageInAtlasName{"Blue"}, OUT rect_uv );
                canvas.Draw( Rectangle2D{ RectF{float2{ 0.2f }} + float2{0.5f, -0.6f}, rect_uv, HtmlColor::White });

                dctx.BindPipeline( t->ppln3 );
                dctx.BindDescriptorSet( t->dsIndex, t->ppln3_ds2 );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }

            // text regions
            //const RectF   txt_region{ -0.38f, -0.7f, -0.15f, -0.45f };
            const RectF     txt_region{ -0.38f, -0.7f, -0.0f, -0.3f };
            /*{
                canvas.Draw( Rectangle2D{ txt_region, t->enter ? HtmlColor::Red : HtmlColor::DarkGray });
                dctx.BindPipeline( t->ppln2 );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }*/

            // text
            Canvas::FontParams  fnt_params;
            fnt_params.heightInPx   = t->fontHeight;

            const U8StringView  text{u8"111 223\n456\ngL^.-"};

            fnt_params.color = HtmlColor::LightCoral;
            {
                canvas.DrawText( text, *t->font, fnt_params, txt_region );

                dctx.BindPipeline( t->ppln4 );
                dctx.BindDescriptorSet( t->dsIndex, t->ppln4_ds );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }


            fnt_params.color = HtmlColor::Lime;
            fnt_params.bold  = FloatToUNormShort(0.05f);
            {
                canvas.DrawText( text, *t->sdfFont, fnt_params, txt_region );

                dctx.BindPipeline( t->ppln5 );
                dctx.BindDescriptorSet( t->dsIndex, t->ppln5_ds, {0u} );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }

            fnt_params.color = HtmlColor::Lime;
            fnt_params.bold  = FloatToUNormShort(0.f);
            {
                canvas.DrawText( text, *t->sdfFont, fnt_params, RectF{0.2f, -0.5f, 1.f, 1.f} );

                dctx.BindPipeline( t->ppln5 );
                //dctx.BindDescriptorSet( t->dsIndex, t->ppln5_ds, {0u} );
                dctx.BindDescriptorSet( t->dsIndex, t->ppln5_ds, {uint(AlignUp( SizeOf<ShaderTypes::sdf_font_ublock>, DeviceLimits.res.minUniformBufferOffsetAlign ))} );
                canvas.Flush( dctx, EPrimitive::TriangleList );
            }

            t->profiler.Draw( canvas );

            gctx.EndRenderPass( dctx, rp_desc );
        }

        Execute( gctx );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Init
=================================================
*/
    bool  Canvas2DSample::Init (PipelinePackID pack)
    {
        auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();
                gfxAlloc    = res_mngr.CreateLinearGfxMemAllocator();

        rtech = res_mngr.LoadRenderTech( pack, RTech, Default );
        CHECK_ERR( rtech );

        font    = MakeRC<RasterFont>();
        sdfFont = MakeRC<RasterFont>();

        ppln1       = rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw1 );
        ppln2       = rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw2 );
    //  ppln2_wire  = rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw2_wire );
        ppln3       = rtech->GetGraphicsPipeline( RTech.Main.canvas2d_draw3 );
        ppln4       = rtech->GetGraphicsPipeline( RTech.Main.font_draw );
        ppln5       = rtech->GetGraphicsPipeline( RTech.Main.sdf_font_draw );
        CHECK_ERR( ppln1 and ppln2 and ppln3 and ppln4 and ppln5 );

        {
            auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln3, DescriptorSetName{"canvas2d.ds0"} );
            CHECK_ERR( ds and idx == dsIndex );
            ppln3_ds1 = RVRef(ds);
        }{
            auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln3, DescriptorSetName{"canvas2d.ds0"} );
            CHECK_ERR( ds and idx == dsIndex );
            ppln3_ds2 = RVRef(ds);
        }{
            auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln4, DescriptorSetName{"font.ds0"} );
            CHECK_ERR( ds and idx == dsIndex );
            ppln4_ds = RVRef(ds);
        }{
            auto [ds, idx] = res_mngr.CreateDescriptorSet( ppln5, DescriptorSetName{"sdf_font.ds0"} );
            CHECK_ERR( ds and idx == dsIndex );
            ppln5_ds = RVRef(ds);
        }


        {
            ublock = res_mngr.CreateBuffer( BufferDesc{ AlignUp( SizeOf<ShaderTypes::sdf_font_ublock>, DeviceLimits.res.minUniformBufferOffsetAlign ) * 2,
                                                        EBufferUsage::Uniform | EBufferUsage::Transfer },
                                            "Canvas2D UB", gfxAlloc );
            CHECK_ERR( ublock );

            DescriptorUpdater   updater;
            CHECK_ERR( updater.Set( ppln5_ds, EDescUpdateMode::Partialy ));
            updater.BindBuffer< ShaderTypes::sdf_font_ublock >( UniformName{"drawUB"}, ublock, 0_b );
            CHECK_ERR( updater.Flush() );
        }

        canvasPtr.reset( new Canvas{} );

        CHECK( profiler.Initialize() );

        return true;
    }

/*
=================================================
    Update
=================================================
*/
    AsyncTask  Canvas2DSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps)
    {
        return Scheduler().Run< ProcessInputTask >( Tuple{ this, RVRef(reader) }, Tuple{ deps });
    }

/*
=================================================
    Draw
=================================================
*/
    AsyncTask  Canvas2DSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> inDeps)
    {
        auto    batch = rg.Render( "Canvas2D pass" );
        CHECK_ERR( batch );

        ArrayView<AsyncTask>    deps = inDeps;
        AsyncTask               upload [4];

        if ( not uploaded.load() )
        {
            uploaded.store( true );

            upload[0] = batch->Run< UploadTextureTask    >( Tuple{this}, Tuple{deps} );
            upload[1] = batch->Run< UploadAtlasTask      >( Tuple{this}, Tuple{deps} );
            upload[2] = batch->Run< UploadRasterFontTask >( Tuple{ &font, ppln4_ds.Get(), VFS::FileName{"canvas2d.font"} }, Tuple{deps} );
            upload[3] = batch->Run< UploadRasterFontTask >( Tuple{ &sdfFont, ppln5_ds.Get(), VFS::FileName{"canvas2d.sdf_font"} }, Tuple{deps} );

            deps = ArrayView<AsyncTask>{ upload };
        }

        auto    surf_acquire = rg.BeginOnSurface( batch, deps );
        CHECK_ERR( surf_acquire );

        return batch->Run< DrawTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"}, Default );
    }

/*
=================================================
    GetInputMode
=================================================
*/
    InputModeName  Canvas2DSample::GetInputMode () const
    {
        return IA;
    }

/*
=================================================
    destructor
=================================================
*/
    Canvas2DSample::~Canvas2DSample ()
    {
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        res_mngr.DelayedReleaseResources( tex.image, tex.view, ppln3_ds1, ppln3_ds2, ppln4_ds, ppln5_ds, ublock );
        profiler.Deinitialize();
    }


} // AE::Samples::Demo
