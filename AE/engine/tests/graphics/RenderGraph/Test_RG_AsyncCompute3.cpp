// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Async compute without synchronizations between frames,
    used double buffering for render targets to avoid race condition.

  .------------------------.
  |      frame 1           |
  |             .------------------------.
  |             |        frame 2         |
  |-------------|-------------.----------|
  |  graphics1  |  graphics2  |          |
  |-------------|-------------|----------|
  |             | compute1 |  | compute2 |
  '-------------'----------'--'----------'
*/

#include "Test_RenderGraph.h"
#include "graphics/RenderGraph/RenderGraphImpl.h"

namespace
{
    struct AC3_TestData
    {
        Mutex                           guard;

        RenderTechPipelinesPtr          rtech;

        // shared
        GAutorelease<ImageID>           image [2];
        GAutorelease<ImageViewID>       view  [2];
        const uint2                     imageSize   {800, 600};
        Atomic<uint>                    frameIdx    {0};

        // graphics
        GraphicsPipelineID              gppln;

        // async compute
        ComputePipelineID               cppln;
        GAutorelease<DescriptorSetID>   cpplnDS [2];
        DescSetBinding                  cpplnDSIndex;

        AsyncTask                       result [2];
        bool                            isOK   [2] = {false, false};

        ImageComparator *               imgCmp  = null;
        RC<GfxLinearMemAllocator>       gfxAlloc;
    };

    static constexpr auto&  RTech = RenderTechs::AsyncCompTestRT;


    template <typename CtxTypes>
    class AC3_GraphicsTask final : public RenderTask
    {
    public:
        AC3_TestData&   t;
        const uint      fi;

        AC3_GraphicsTask (AC3_TestData& t, uint frameIdx, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ batch, dbg },
            t{ t },
            fi{ frameIdx & 1 }
        {
            CHECK( batch->GetQueueType() == EQueueType::Graphics );
        }

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            typename CtxTypes::Graphics     ctx{ *this };

            // draw
            {
                constexpr auto&     rtech_pass = RTech.Draw_1;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = ctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.imageSize }
                                    .AddViewport( t.imageSize )
                                    .AddTarget( rtech_pass.att_Color, t.view[fi], RGBA32f{1.0f} ));

                dctx.BindPipeline( t.gppln );
                dctx.Draw( 3 );

                ctx.EndRenderPass( dctx );
            }

            Execute( ctx );
        }
    };


    template <typename CtxTypes>
    class AC3_ComputeTask final : public RenderTask
    {
    public:
        AC3_TestData&   t;
        const uint      fi;

        AC3_ComputeTask (AC3_TestData& t, uint frameIdx, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ batch, dbg },
            t{ t },
            fi{ frameIdx & 1 }
        {
            CHECK( batch->GetQueueType() == EQueueType::AsyncCompute );
        }

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            typename CtxTypes::Compute  ctx{ *this };

            ctx.BindPipeline( t.cppln );
            ctx.BindDescriptorSet( t.cpplnDSIndex, t.cpplnDS[fi] );
            ctx.Dispatch( DivCeil( t.imageSize, 4u ));

            Execute( ctx );
        }
    };


    template <typename Ctx>
    class AC3_CopyTask final : public RenderTask
    {
    public:
        AC3_TestData&   t;

        AC3_CopyTask (AC3_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            ReadbackImageDesc   readback;
            readback.heapType = EStagingHeapType::Dynamic;

            t.result[0] = AsyncTask{ ctx.ReadbackImage( t.image[0], readback )
                            .Then( [p = &t] (const ImageMemView &view)
                                    {
                                        p->isOK[0] = p->imgCmp->Compare( view );
                                    })};
            t.result[1] = AsyncTask{ ctx.ReadbackImage( t.image[1], readback )
                            .Then( [p = &t] (const ImageMemView &view)
                                    {
                                        p->isOK[1] = p->imgCmp->Compare( view );
                                    })};

            Execute( ctx );
        }
    };


    template <typename CtxTypes, typename CopyCtx>
    class AC3_FrameTask final : public Threading::IAsyncTask
    {
    public:
        AC3_TestData&       t;

        AC3_FrameTask (AC3_TestData& t) :
            IAsyncTask{ ETaskQueue::PerFrame },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            auto&   rg = RenderTaskScheduler().GetRenderGraph();

            if ( t.frameIdx.load() == 3 )
            {
                // frame 3
                AsyncTask   begin       = rg.BeginFrame();
                auto        batch       = rg.CmdBatch( EQueueType::AsyncCompute, {"copy task"} )
                                                .UseResource( t.image[0] )
                                                .UseResource( t.image[1] )
                                                .ReadbackMemory()
                                                .Begin();
                CHECK_TE( batch );

                AsyncTask   read_task   = batch.template Task< AC3_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, {"Readback task"} )
                                                .SubmitBatch().Run( Tuple{begin} );
                AsyncTask   end         = rg.EndFrame( Tuple{read_task} );

                ++t.frameIdx;
                return Continue( Tuple{end} );
            }

            if ( t.frameIdx.load() > 3 )
                return; // frame 4+

            // frames [0..2]:
            const uint  fi      = t.frameIdx.load() & 1;
            AsyncTask   begin   = rg.BeginFrame();


            // batch graph
            auto        batch_gfx = rg.CmdBatch( EQueueType::Graphics, {"graphics batch"} )
                                        .UseResource( t.image[fi], EResourceState::ShaderSample | EResourceState::FragmentShader )
                                        .Begin();
            CHECK_TE( batch_gfx );

            auto        batch_ac = rg.CmdBatch( EQueueType::AsyncCompute, {"compute batch"} )
                                        .UseResource( t.image[fi], EResourceState::ShaderStorage_RW | EResourceState::ComputeShader )
                                        .Begin();
            CHECK_TE( batch_ac );


            // add tasks to cmd batches
            AsyncTask   gfx_task    = batch_gfx.template Task< AC3_GraphicsTask<CtxTypes> >( Tuple{ ArgRef(t), t.frameIdx.load() }, {"graphics task"}      ).SubmitBatch().Run( Tuple{begin} );
            AsyncTask   comp_task   = batch_ac .template Task< AC3_ComputeTask<CtxTypes>  >( Tuple{ ArgRef(t), t.frameIdx.load() }, {"async compute task"} ).SubmitBatch().Run( Tuple{gfx_task} );

            AsyncTask   end         = rg.EndFrame( Tuple{ gfx_task, comp_task });

            ++t.frameIdx;
            return Continue( Tuple{end} );
        }

        StringView  DbgName ()  C_NE_OV { return "AC3_FrameTask"; }
    };


    template <typename CtxTypes, typename CopyCtx>
    static bool  AsyncCompute3Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
    {
        auto&           res_mngr    = RenderTaskScheduler().GetResourceManager();
        AC3_TestData    t;
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        auto&           rg          = RenderTaskScheduler().GetRenderGraph();

        t.rtech     = renderTech;
        t.gfxAlloc  = MakeRC<GfxLinearMemAllocator>();
        t.imgCmp    = imageCmp;

        CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

        t.image[0] = rg.CreateImage( ImageDesc{}.SetDimension( t.imageSize )
                                            .SetFormat( format )
                                            .SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
                                        "Image-0", t.gfxAlloc );
        t.image[1] = rg.CreateImage( ImageDesc{}.SetDimension( t.imageSize )
                                            .SetFormat( format )
                                            .SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
                                        "Image-1", t.gfxAlloc );
        CHECK_ERR( t.image[0] and t.image[1] );

        t.view[0] = res_mngr.CreateImageView( ImageViewDesc{}, t.image[0], "ImageView-0" );
        t.view[1] = res_mngr.CreateImageView( ImageViewDesc{}, t.image[1], "ImageView-1" );
        CHECK_ERR( t.view[0] and t.view[1] );

        t.gppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.async_comp1_graphics );
        t.cppln = t.rtech->GetComputePipeline( RTech.Compute_1.async_comp1_compute );
        CHECK_ERR( t.gppln and t.cppln );

        {
            auto [ds0, idx0] = res_mngr.CreateDescriptorSet( t.cppln, DescriptorSetName{"compute2.ds1"} );
            auto [ds1, idx1] = res_mngr.CreateDescriptorSet( t.cppln, DescriptorSetName{"compute2.ds1"} );

            t.cpplnDSIndex  = idx0;
            t.cpplnDS[0]    = RVRef(ds0);
            t.cpplnDS[1]    = RVRef(ds1);
            CHECK_ERR( t.cpplnDS[0] and t.cpplnDS[1] );

            DescriptorUpdater   updater;

            CHECK_ERR( updater.Set( t.cpplnDS[0], EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_Image"}, t.view[0] );

            CHECK_ERR( updater.Set( t.cpplnDS[1], EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_Image"}, t.view[1] );

            updater.Flush();
        }

        // draw 3 frames
        auto    task = Scheduler().Run< AC3_FrameTask<CtxTypes, CopyCtx> >( Tuple{ArgRef(t)} );

        CHECK_ERR( Scheduler().Wait( {task} ));
        CHECK_ERR( rg.WaitAll() );

        CHECK_ERR( t.frameIdx.load() == 4 );

        CHECK_ERR( Scheduler().Wait({ t.result[0], t.result[1] }));
        CHECK_ERR( t.result[0]->Status() == EStatus::Completed );
        CHECK_ERR( t.result[1]->Status() == EStatus::Completed );
        CHECK_ERR( t.isOK[0] );
        CHECK_ERR( t.isOK[1] );

        return true;
    }

} // namespace


bool RGTest::Test_AsyncCompute3 ()
{
    if ( not AllBits( RenderTaskScheduler().GetDevice().GetAvailableQueues(), EQueueMask::Graphics | EQueueMask::AsyncCompute ))
        return true; // skip

    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( AsyncCompute3Test< RG::DirectCtx,   RG::DirectCtx::Transfer   >( _acPipelines, img_cmp.get() ));
    RG_CHECK( AsyncCompute3Test< RG::IndirectCtx, RG::IndirectCtx::Transfer >( _acPipelines, img_cmp.get() ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}
