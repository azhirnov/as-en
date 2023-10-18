// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{
    struct Db3_TestData
    {
        Mutex                       guard;

        RenderTechPipelinesPtr      rtech;
        uint2                       viewSize;

        GAutorelease<ImageID>       img;
        GAutorelease<ImageViewID>   view;

        MeshPipelineID              ppln;

        ShaderDebugger              debugger;

        AsyncTask                   result;

        CommandBatchPtr             batch;
        bool                        isOK    = false;

        ImageComparator *           imgCmp  = null;
        GfxMemAllocatorPtr          gfxAlloc;
    };

    static constexpr auto&  RTech = RenderTechs::DrawTestRT;


    template <typename CtxType>
    class Db3_DrawTask final : public RenderTask
    {
    public:
        Db3_TestData&   t;

        Db3_DrawTask (Db3_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            const auto  img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

            typename CtxType::Transfer  copy_ctx{ *this };

            ShaderDebugger::Result  dbg;
            CHECK_TE( t.debugger.AllocForGraphics( OUT dbg, copy_ctx, t.ppln ));

            typename CtxType::Graphics  ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

            ctx.AccumBarriers()
                .ImageBarrier( t.img, EResourceState::Invalidate, img_state );

            // draw
            {
                constexpr auto&     rtech_pass = RTech.Draw_1;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = ctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} ));

                dctx.BindPipeline( t.ppln );
                dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
                dctx.DrawMeshTasks( uint3{1} );

                ctx.EndRenderPass( dctx );
            }

            ctx.AccumBarriers()
                .ImageBarrier( t.img, img_state, EResourceState::CopySrc );

            Execute( ctx );
        }
    };

    template <typename Ctx>
    class Db3_CopyTask final : public RenderTask
    {
    public:
        Db3_TestData&   t;

        Db3_CopyTask (Db3_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            auto    task1 = ctx.ReadbackImage( t.img, Default );
            auto    task2 = t.debugger.ReadAll( ctx );

            t.result = AsyncTask{ MakePromiseFrom( task1, task2 )
                .Then( [p = &t] (const Tuple<ImageMemView, Array<String>> &view_and_str)
                {
                    bool    ok = p->imgCmp->Compare( view_and_str.Get<ImageMemView>() );
                    ASSERT( ok );

                    Array<String>   trace_str = view_and_str.Get<Array<String>>();
                    ASSERT( trace_str.size() == 1 );

                    if ( trace_str.size() == 1 )
                    {
                        const StringView    ms_ref_str =
R"(//> gl_GlobalInvocationID: uint3 {0, 0, 0}
//> gl_LocalInvocationID: uint3 {0, 0, 0}
//> gl_WorkGroupID: uint3 {0, 0, 0}
no source

//> (out): float4 {0.000000, -0.500000, 0.000000, 1.000000}
29. gl_MeshVerticesEXT[I].gl_Position   = vec4( g_Positions[I], 0.0, 1.0 );

//> (out): float4 {1.000000, 0.000000, 0.000000, 1.000000}
30. Out[I].color                        = vec4( g_Colors[I], 1.0 );

//> (out): bool {true}
32. if ( I == 0 )

//> gl_PrimitiveTriangleIndicesEXT: uint3 {0, 1, 2}
34. gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0,1,2);

//> SetMeshOutputs(): void
35.             SetMeshOutputsEXT( 3, 1 );

)";
                        ok &= (trace_str[0] == ms_ref_str);
                        ASSERT( ok );

                        p->isOK = ok;
                    }
                })};

            ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

            Execute( ctx );
        }
    };


    template <typename CtxType, typename CopyCtx>
    static bool  Debugger3Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
    {
        auto&           rts         = RenderTaskScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        Db3_TestData    t;

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

        t.ppln = t.rtech->GetMeshPipeline( RTech.Draw_1.dbg3_draw );
        CHECK_ERR( t.ppln );

        AsyncTask   begin   = rts.BeginFrame();

        auto        batch   = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger3"} );
        CHECK_ERR( batch );

        AsyncTask   task1   = batch->Run< Db3_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin},               {"Draw mesh task"} );
        AsyncTask   task2   = batch->Run< Db3_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

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


bool RGTest::Test_Debugger3 ()
{
    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( Debugger3Test< DirectCtx, DirectCtx::Transfer >( _pipelines, img_cmp.get() ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}

#endif // not AE_ENABLE_METAL
