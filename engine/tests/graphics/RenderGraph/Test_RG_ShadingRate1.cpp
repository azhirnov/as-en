// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{
    struct SR1_TestData
    {
        Mutex                       guard;

        RenderTechPipelinesPtr      rtech;
        uint2                       viewSize;
        uint2                       texelSize;

        GAutorelease<ImageID>       img     [3];
        GAutorelease<ImageViewID>   view    [3];

        GAutorelease<ImageID>       vrsImg;
        GAutorelease<ImageViewID>   vrsView;

        GraphicsPipelineID          ppln    [3];    // per_draw, per_primitive, vrs_attachment

        AsyncTask                   result  [3];
        bool                        isOK    [3] = {false, false, false};

        CommandBatchPtr             batch;

        Array<ImageComparator*>     imgCmps;
        RC<GfxLinearMemAllocator>   gfxAlloc;
    };

    static constexpr auto&  RTech = RenderTechs::VRSTestRT;


    ND_ static int  ShadingRate (int x, int y)
    {
        return (IntLog2(x) << 2) | IntLog2(y);
    }

    static const ShaderTypes::Vertex_VRS    vertices[] = {
        {{-0.9f,  0.0f}, ShadingRate(4,4)},  {{-0.9f,  0.9f}, ShadingRate(4,4)},  {{-0.2f,  0.9f}, ShadingRate(4,4)},
        {{-1.0f, -0.3f}, ShadingRate(2,2)},  {{ 0.0f,  1.0f}, ShadingRate(2,2)},  {{ 0.0f, -0.3f}, ShadingRate(2,2)},
        {{ 0.1f, -0.1f}, ShadingRate(2,4)},  {{ 0.1f,  0.9f}, ShadingRate(2,4)},  {{ 0.9f, -0.1f}, ShadingRate(2,4)},
        {{ 0.1f,  1.0f}, ShadingRate(4,2)},  {{ 1.0f,  1.0f}, ShadingRate(4,2)},  {{ 1.0f, -0.1f}, ShadingRate(4,2)},
        {{-0.9f, -1.0f}, ShadingRate(1,1)},  {{-0.9f, -0.4f}, ShadingRate(1,1)},  {{ 0.2f, -0.4f}, ShadingRate(1,1)},
        {{ 1.0f, -0.1f}, ShadingRate(1,2)},  {{ 1.0f, -1.0f}, ShadingRate(1,2)},  {{-0.5f, -1.0f}, ShadingRate(1,2)}
    };

    ND_ static ubyte  GenTexture (uint x, uint y, uint w, uint h)
    {
        float   xdist = Abs( 0.5f - float(x) / w ) * 2.0f;
        float   ydist = Abs( 0.5f - float(y) / h ) * 2.0f;

        uint    xrate = 2 - Clamp( uint(xdist * 2 + 0.5f), 0u, 2u );
        uint    yrate = 2 - Clamp( uint(ydist * 2 + 0.5f), 0u, 2u );

        return ubyte( (xrate << 2) | yrate );
    }


    template <typename CtxType>
    class SR1_DrawTask final : public RenderTask
    {
    public:
        SR1_TestData&   t;

        SR1_DrawTask (SR1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            const auto      img_state   = EResourceState::ShaderSample | EResourceState::FragmentShader;
            uint            i           = 0;
            const uint2     vrs_size    = t.viewSize / t.texelSize;
            Array<ubyte>    vrs_data;

            vrs_data.resize( vrs_size.x * vrs_size.y );

            for (uint y = 0; y < vrs_size.y; ++y)
            {
                for (uint x = 0; x < vrs_size.x; ++x)
                {
                    ubyte   rate    = GenTexture( x, y, vrs_size.x, vrs_size.y );
                    uint    idx     = x + y * vrs_size.x;
                    vrs_data[idx] = rate;
                }
            }


            typename CtxType::Transfer  tctx{ *this };

            tctx.AccumBarriers()
                .ImageBarrier( t.vrsImg, EResourceState::Invalidate, EResourceState::CopyDst );

            UploadImageDesc upload;
            upload.aspectMask   = EImageAspect::Color;
            upload.heapType     = EStagingHeapType::Static;

            CHECK_TE( tctx.UploadImage( t.vrsImg, upload, vrs_data.data(), ArraySizeOf(vrs_data) ) == ArraySizeOf(vrs_data) );


            typename CtxType::Graphics  gctx{ *this, tctx.ReleaseCommandBuffer() };

            gctx.AccumBarriers()
                .ImageBarrier( t.img[0], EResourceState::Invalidate, img_state )
                .ImageBarrier( t.img[1], EResourceState::Invalidate, img_state )
                .ImageBarrier( t.img[2], EResourceState::Invalidate, img_state )
                .ImageBarrier( t.vrsImg, EResourceState::CopyDst, EResourceState::ShadingRateImage );

            // per draw
            {
                constexpr auto&     rtech_pass = RTech.nonVRS;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = gctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view[i], RGBA32f{HtmlColor::Black} ));

                dctx.BindPipeline( t.ppln[i] );
                dctx.SetFragmentShadingRate( EShadingRate::Size2x2, EShadingRateCombinerOp::Keep, EShadingRateCombinerOp::Keep );

                dctx.Draw( 3 );

                gctx.EndRenderPass( dctx );
            }
            ++i;

            // per primitive
            {
                constexpr auto&     rtech_pass = RTech.nonVRS;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = gctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view[i], RGBA32f{HtmlColor::Black} ));

                VertexStream    vstream;
                CHECK_TE( dctx.AllocVStream( Sizeof(vertices), OUT vstream ));
                MemCopy( OUT vstream.mappedPtr, vertices, vstream.size );

                dctx.BindPipeline( t.ppln[i] );
                dctx.BindVertexBuffer( 0, vstream.id, vstream.offset );

                dctx.SetFragmentShadingRate( EShadingRate::Size1x1, EShadingRateCombinerOp::Replace, EShadingRateCombinerOp::Keep );

                dctx.Draw( uint(CountOf(vertices)) );

                gctx.EndRenderPass( dctx );
            }
            ++i;

            // VRS attachment
            {
                constexpr auto&     rtech_pass = RTech.VRS;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 2 );

                auto    dctx = gctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color,       t.view[i], RGBA32f{HtmlColor::Black} )
                                    .AddTarget( rtech_pass.att_ShadingRate, t.vrsView ));

                dctx.BindPipeline( t.ppln[i] );
                dctx.SetFragmentShadingRate( EShadingRate::Size1x1, EShadingRateCombinerOp::Keep, EShadingRateCombinerOp::Replace );

                dctx.Draw( 3 );

                gctx.EndRenderPass( dctx );
            }
            ++i;

            // TODO: per pipeline

            gctx.AccumBarriers()
                .ImageBarrier( t.img[0], img_state, EResourceState::CopySrc )
                .ImageBarrier( t.img[1], img_state, EResourceState::CopySrc )
                .ImageBarrier( t.img[2], img_state, EResourceState::CopySrc );

            Execute( gctx );
        }
    };


    template <typename Ctx>
    class SR1_CopyTask final : public RenderTask
    {
    public:
        SR1_TestData&   t;

        SR1_CopyTask (SR1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            for (uint i = 0; i < 3; ++i)
            {
                t.result[i] = AsyncTask{ ctx.ReadbackImage( t.img[i], Default )
                            .Then( [p = &t, i] (const ImageMemView &view)
                                    {
                                        p->isOK[i] = p->imgCmps[i]->Compare( view );
                                    })};
            }

            ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

            Execute( ctx );
        }
    };


    template <typename CtxType, typename CopyCtx>
    static bool  ShadingRate1Test (RenderTechPipelinesPtr renderTech, Array<ImageComparator*> imgCmps)
    {
        auto&           rts         = RenderTaskScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        SR1_TestData    t;

        t.gfxAlloc  = MakeRC<GfxLinearMemAllocator>();
        t.imgCmps   = imgCmps;
        t.rtech     = renderTech;
        t.viewSize  = uint2{512, 512};
        t.texelSize = uint2{16};

        CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

        // create attachments
        for (uint i = 0; i < 3; ++i)
        {
            t.img[i] = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( EPixelFormat::RGBA8_UNorm )
                                                .SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
                                             "Image", t.gfxAlloc );
            CHECK_ERR( t.img[i] );

            t.view[i] = res_mngr.CreateImageView( ImageViewDesc{}, t.img[i], "ImageView" );
            CHECK_ERR( t.view[i] );
        }
        {
            t.vrsImg = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize / t.texelSize ).SetFormat( EPixelFormat::R8U )
                                            .SetUsage( EImageUsage::ShadingRate | EImageUsage::TransferDst ),
                                          "VRS-image", t.gfxAlloc );
            CHECK_ERR( t.vrsImg );

            t.vrsView = res_mngr.CreateImageView( ImageViewDesc{}, t.vrsImg, "VRS-view" );
            CHECK_ERR( t.vrsView );
        }

        t.ppln[0] = renderTech->GetGraphicsPipeline( RTech.nonVRS.per_draw );
        t.ppln[1] = renderTech->GetGraphicsPipeline( RTech.nonVRS.per_primitive );
        t.ppln[2] = renderTech->GetGraphicsPipeline( RTech.VRS.vrs_attachment );
        CHECK_ERR( t.ppln[0] and t.ppln[1] and t.ppln[2] );

        AsyncTask   begin   = rts.BeginFrame();

        auto        batch   = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Draw1"} );
        CHECK_ERR( batch );

        AsyncTask   task1   = batch->Run< SR1_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin},               {"Draw task"} );
        AsyncTask   task2   = batch->Run< SR1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

        AsyncTask   end     = rts.EndFrame( Tuple{task2} );

        CHECK_ERR( Scheduler().Wait({ end }));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rts.WaitAll() );

        CHECK_ERR( Scheduler().Wait({ t.result }));
        CHECK_ERR( t.result[0]->Status() == EStatus::Completed );
        CHECK_ERR( t.result[1]->Status() == EStatus::Completed );
        CHECK_ERR( t.result[2]->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK[0] and t.isOK[1] and t.isOK[2] );
        return true;
    }

} // namespace


bool RGTest::Test_ShadingRate1 ()
{
    auto    img_cmp1    = _LoadReference( TEST_NAME + "-1" );
    auto    img_cmp2    = _LoadReference( TEST_NAME + "-2" );
    auto    img_cmp3    = _LoadReference( TEST_NAME + "-3" );
    bool    result      = true;

    RG_CHECK( ShadingRate1Test< DirectCtx, DirectCtx::Transfer >( _vrsPipelines, {img_cmp1.get(), img_cmp2.get(), img_cmp3.get()} ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}

#endif // not AE_ENABLE_METAL
