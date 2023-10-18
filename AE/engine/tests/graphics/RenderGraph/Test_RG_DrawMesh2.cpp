// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{
    struct DM2_TestData
    {
        Mutex                       guard;

        RenderTechPipelinesPtr      rtech;
        uint2                       viewSize;

        GAutorelease<ImageID>       img;
        GAutorelease<ImageViewID>   view;

        MeshPipelineID              ppln;

        AsyncTask                   result;

        CommandBatchPtr             batch;
        bool                        isOK    = false;

        ImageComparator *           imgCmp  = null;
        GfxMemAllocatorPtr          gfxAlloc;
    };

    static constexpr auto&  RTech = RenderTechs::DrawMeshesTestRT;


    template <typename CtxType>
    class DM2_DrawTask final : public RenderTask
    {
    public:
        DM2_TestData&   t;

        DM2_DrawTask (DM2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            const auto  img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

            typename CtxType::Graphics  ctx{ *this };

            ctx.AccumBarriers()
                .ImageBarrier( t.img, EResourceState::Invalidate, img_state );

            // draw
            {
                constexpr auto&     rtech_pass = RTech.DrawMeshes_1;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = ctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} ));

                dctx.BindPipeline( t.ppln );
                dctx.DrawMeshTasks( uint3{1} );

                ctx.EndRenderPass( dctx );
            }

            ctx.AccumBarriers()
                .ImageBarrier( t.img, img_state, EResourceState::CopySrc );

            Execute( ctx );
        }
    };

    template <typename Ctx>
    class DM2_CopyTask final : public RenderTask
    {
    public:
        DM2_TestData&   t;

        DM2_CopyTask (DM2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
                        .Then(  [p = &t] (const ImageMemView &view)
                                {
                                    p->isOK = p->imgCmp->Compare( view );
                                })};

            ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

            Execute( ctx );
        }
    };


    template <typename CtxType, typename CopyCtx>
    static bool  DrawMesh2Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
    {
        auto&           rts         = RenderTaskScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        DM2_TestData    t;

        t.rtech     = renderTech;
        t.gfxAlloc  = res_mngr.CreateLinearGfxMemAllocator();
        t.imgCmp    = imageCmp;
        t.viewSize  = uint2{800, 600};

        CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

        t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
                                        .SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
                                      "Image", t.gfxAlloc );
        CHECK_ERR( t.img );

        t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
        CHECK_ERR( t.view );

        t.ppln = t.rtech->GetMeshPipeline( RTech.DrawMeshes_1.draw_mesh2 );
        CHECK_ERR( t.ppln );

        AsyncTask   begin   = rts.BeginFrame();

        t.batch = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"DrawMesh1"} );
        CHECK_ERR( t.batch );

        AsyncTask   task1   = t.batch->Run< DM2_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin},                {"Draw task"} );
        AsyncTask   task2   = t.batch->Run< DM2_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"},  {"Readback task"} );

        AsyncTask   end     = rts.EndFrame( Tuple{task2} );

        CHECK_ERR( Scheduler().Wait({ end }));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rts.WaitAll() );

        CHECK_ERR( Scheduler().Wait({ t.result }));
        CHECK_ERR( t.result->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK );
        return true;
    }

} // namespace


bool RGTest::Test_DrawMesh2 ()
{
    if ( _msPipelines == null )
        return true; // skip

    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( DrawMesh2Test< DirectCtx,   DirectCtx::Transfer   >( _msPipelines, img_cmp.get() ));
    RG_CHECK( DrawMesh2Test< DirectCtx,   IndirectCtx::Transfer >( _msPipelines, img_cmp.get() ));

    RG_CHECK( DrawMesh2Test< IndirectCtx, DirectCtx::Transfer   >( _msPipelines, img_cmp.get() ));
    RG_CHECK( DrawMesh2Test< IndirectCtx, IndirectCtx::Transfer >( _msPipelines, img_cmp.get() ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}

#endif // not AE_ENABLE_METAL
