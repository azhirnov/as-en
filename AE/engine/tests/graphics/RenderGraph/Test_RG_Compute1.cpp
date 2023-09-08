// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
    struct C1_TestData
    {
        Mutex                               guard;

        GAutorelease<ImageID>               img0;
        GAutorelease<ImageID>               img1;
        GAutorelease<ImageID>               img2;

        GAutorelease<ImageViewID>           view0;
        GAutorelease<ImageViewID>           view1;
        GAutorelease<ImageViewID>           view2;

        GAutorelease<ComputePipelineID>     ppln0;
        GAutorelease<ComputePipelineID>     ppln1;
        GAutorelease<ComputePipelineID>     ppln2;

        GAutorelease<DescriptorSetID>       ds0;
        GAutorelease<DescriptorSetID>       ds1;
        GAutorelease<DescriptorSetID>       ds2;
        DescSetBinding                      ds_index;

        AsyncTask                           result0;
        AsyncTask                           result1;
        AsyncTask                           result2;

        bool                                isOK_0      = false;
        bool                                isOK_1      = false;
        bool                                isOK_2      = false;

        RC<GfxLinearMemAllocator>           gfxAlloc;
    };


    static bool  C1_CheckImageData (const ImageMemView &view, uint blockSize)
    {
        RWImageMemView  img_data{ view };
        bool            is_correct = true;

        for (uint y = 0; y < img_data.Dimension().y; ++y)
        {
            for (uint x = 0; x < img_data.Dimension().x; ++x)
            {
                RGBA32u     dst;
                img_data.Load( uint3(x,y,0), OUT dst );

                const uint  r   = uint(float(x % blockSize) * 255.0f / float(blockSize) + 0.5f);
                const uint  g   = uint(float(y % blockSize) * 255.0f / float(blockSize) + 0.5f);

                const bool  is_equal = (Equals( dst[0], r, 1u ) and
                                        Equals( dst[1], g, 1u ) and
                                        dst[2] == 255           and
                                        dst[3] == 0);

                //ASSERT( is_equal );
                is_correct &= is_equal;
            }
        }
        return is_correct;
    };


    template <typename Ctx>
    class C1_ComputeTask final : public RenderTask
    {
    public:
        C1_TestData&    t;

        C1_ComputeTask (C1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            const auto  img_state = EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;

            ctx.AccumBarriers()
                .ImageBarrier( t.img0, EResourceState::Invalidate, img_state )
                .ImageBarrier( t.img1, EResourceState::Invalidate, img_state )
                .ImageBarrier( t.img2, EResourceState::Invalidate, img_state );

            ctx.BindPipeline( t.ppln0 );
            ctx.BindDescriptorSet( t.ds_index, t.ds0 );
            ctx.Dispatch({ 2, 2, 1 });

            ctx.BindPipeline( t.ppln1 );
            ctx.BindDescriptorSet( t.ds_index, t.ds1 );
            ctx.Dispatch({ 4, 4, 1 });

            ctx.BindPipeline( t.ppln2 );
            ctx.BindDescriptorSet( t.ds_index, t.ds2 );
            ctx.Dispatch({ 1, 1, 1 });

            ctx.AccumBarriers()
                .ImageBarrier( t.img0, img_state, EResourceState::CopySrc )
                .ImageBarrier( t.img1, img_state, EResourceState::CopySrc )
                .ImageBarrier( t.img2, img_state, EResourceState::CopySrc );

            Execute( ctx );
        }
    };

    template <typename Ctx>
    class C1_CopyTask final : public RenderTask
    {
    public:
        C1_TestData&    t;

        C1_CopyTask (C1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            t.result0 = AsyncTask{ ctx.ReadbackImage( t.img0, Default )
                        .Then( [p = &t] (const ImageMemView &view)
                                {
                                    p->isOK_0 = C1_CheckImageData( view, 8 );
                                })};
            t.result1 = AsyncTask{ ctx.ReadbackImage( t.img1, Default )
                        .Then( [p = &t] (const ImageMemView &view)
                                {
                                    p->isOK_1 = C1_CheckImageData( view, 4 );
                                })};
            t.result2 = AsyncTask{ ctx.ReadbackImage( t.img2, Default )
                        .Then( [p = &t] (const ImageMemView &view)
                                {
                                    p->isOK_2 = C1_CheckImageData( view, 16 );
                                })};

            ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

            Execute( ctx );
        }
    };


    template <typename CompCtx, typename CopyCtx>
    static bool  Compute1Test ()
    {
        auto&           rts         = RenderTaskScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        C1_TestData     t;
        const uint2     img_dim     {16, 16};
        const auto      format      = EPixelFormat::RGBA8_UNorm;

        t.gfxAlloc = MakeRC<GfxLinearMemAllocator>();

        t.img0 = res_mngr.CreateImage( ImageDesc{}.SetDimension( img_dim ).SetFormat( format ).SetUsage( EImageUsage::Storage | EImageUsage::TransferSrc ), "Image-0", t.gfxAlloc );
        t.img1 = res_mngr.CreateImage( ImageDesc{}.SetDimension( img_dim ).SetFormat( format ).SetUsage( EImageUsage::Storage | EImageUsage::TransferSrc ), "Image-1", t.gfxAlloc );
        t.img2 = res_mngr.CreateImage( ImageDesc{}.SetDimension( img_dim ).SetFormat( format ).SetUsage( EImageUsage::Storage | EImageUsage::TransferSrc ), "Image-2", t.gfxAlloc );
        CHECK_ERR( t.img0 and t.img1 and t.img2 );

        t.view0 = res_mngr.CreateImageView( ImageViewDesc{}, t.img0, "ImageView-0" );
        t.view1 = res_mngr.CreateImageView( ImageViewDesc{}, t.img1, "ImageView-1" );
        t.view2 = res_mngr.CreateImageView( ImageViewDesc{}, t.img2, "ImageView-2" );
        CHECK_ERR( t.view0 and t.view1 and t.view2 );

        {
            ComputePipelineDesc desc;

            t.ppln0 = res_mngr.CreateComputePipeline( Default, PipelineTmplName{"compute1"}, desc );
            CHECK_ERR( t.ppln0 );

            desc.localSize = {4, 4, 1};

            t.ppln1 = res_mngr.CreateComputePipeline( Default, PipelineTmplName{"compute1"}, desc );
            CHECK_ERR( t.ppln1 );

            desc.localSize = {16, 16, 1};

            t.ppln2 = res_mngr.CreateComputePipeline( Default, PipelineTmplName{"compute1"}, desc );
            CHECK_ERR( t.ppln2 );
        }

        {
            auto [ds0, idx0] = res_mngr.CreateDescriptorSet( t.ppln0, DescriptorSetName{"compute1.ds1"} );
            auto [ds1, idx1] = res_mngr.CreateDescriptorSet( t.ppln0, DescriptorSetName{"compute1.ds1"} );
            auto [ds2, idx2] = res_mngr.CreateDescriptorSet( t.ppln0, DescriptorSetName{"compute1.ds1"} );

            t.ds_index = idx0;
            CHECK_ERR( idx0 == idx1 and idx1 == idx2 );

            t.ds0 = RVRef(ds0);
            t.ds1 = RVRef(ds1);
            t.ds2 = RVRef(ds2);
            CHECK_ERR( t.ds0 and t.ds1 and t.ds2 );

            DescriptorUpdater   updater;

            CHECK_ERR( updater.Set( t.ds0, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_OutImage"}, t.view0 );

            CHECK_ERR( updater.Set( t.ds1, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_OutImage"}, t.view1 );

            CHECK_ERR( updater.Set( t.ds2, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_OutImage"}, t.view2 );

            CHECK_ERR( updater.Flush() );
        }

        AsyncTask   begin   = rts.BeginFrame();

        auto        batch   = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Compute1"} );
        CHECK_ERR( batch );

        AsyncTask   task1   = batch->Run< C1_ComputeTask<CompCtx> >( Tuple{ArgRef(t)}, Tuple{begin},                {"Compute task"} );
        AsyncTask   task2   = batch->Run< C1_CopyTask<CopyCtx>    >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"},  {"Readback task"} );

        AsyncTask   end     = rts.EndFrame( Tuple{task2} );

        CHECK_ERR( Scheduler().Wait({ end }));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rts.WaitAll() );

        CHECK_ERR( Scheduler().Wait({ t.result0, t.result1, t.result2 }));
        CHECK_ERR( t.result0->Status() == EStatus::Completed );
        CHECK_ERR( t.result1->Status() == EStatus::Completed );
        CHECK_ERR( t.result2->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK_0 );
        CHECK_ERR( t.isOK_1 );
        CHECK_ERR( t.isOK_2 );

        return true;
    }

} // namespace


bool RGTest::Test_Compute1 ()
{
    bool    result = true;

    RG_CHECK( Compute1Test< DirectCtx::Compute,   DirectCtx::Transfer   >());
    RG_CHECK( Compute1Test< IndirectCtx::Compute, IndirectCtx::Transfer >());
    RG_CHECK( Compute1Test< DirectCtx::Compute,   IndirectCtx::Transfer >());

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}
