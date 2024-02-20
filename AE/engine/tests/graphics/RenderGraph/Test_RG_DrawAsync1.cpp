// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
    struct DA1_TestData
    {
        SharedMutex                 guard;

        RenderTechPipelinesPtr      rtech;
        uint2                       viewSize;

        GAutorelease<ImageID>       img;
        GAutorelease<ImageViewID>   view;

        GAutorelease<BufferID>      vb;

        GraphicsPipelineID          ppln;

        AsyncTask                   result;

        CommandBatchPtr             batch;
        bool                        isOK    = false;

        ImageComparator *           imgCmp  = null;
        GfxMemAllocatorPtr          gfxAlloc;
    };

    static constexpr auto&  RTech = RenderTechs::DrawTestRT;

    static const ShaderTypes::Vertex_draw2  vertices[] = {
        { float2{-1.0f, -1.0f}, HtmlColor::Red },
        { float2{-0.5f, -1.0f}, HtmlColor::Red },
        { float2{-1.0f,  0.5f}, HtmlColor::Red },

        { float2{-0.5f, -1.0f}, HtmlColor::Green },
        { float2{ 0.0f,  0.0f}, HtmlColor::Green },
        { float2{-1.0f,  0.5f}, HtmlColor::Green },

        { float2{ 1.0f,  1.0f}, HtmlColor::Blue },
        { float2{ 0.5f,  1.0f}, HtmlColor::Blue },
        { float2{ 1.0f,  0.5f}, HtmlColor::Blue },

        { float2{ 0.5f,  1.0f}, HtmlColor::Yellow },
        { float2{ 0.0f,  0.0f}, HtmlColor::Yellow },
        { float2{ 1.0f,  0.5f}, HtmlColor::Yellow }
    };


    template <typename CtxTypes>
    class DA1_DrawTask final : public DrawTask
    {
    public:
        DA1_TestData&   t;
        const uint      firstVertex;

        DA1_DrawTask (DA1_TestData* t, uint first, RC<DrawCommandBatch> batch, DebugLabel dbg) __NE___ :
            DrawTask{ batch, dbg },
            t{ *t },
            firstVertex{ first }
        {}

        void  Run () __Th_OV
        {
            DeferSharedLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            typename CtxTypes::Draw     dctx{ *this };

            CHECK_TE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

            dctx.BindPipeline( t.ppln );

            DrawCmd cmd;
            cmd.vertexCount = 3;
            cmd.firstVertex = firstVertex;
            dctx.Draw( cmd );

            Execute( dctx );
        }
    };


    template <typename CtxTypes>
    class DA1_RenderPassTask final : public RenderTask
    {
    public:
        DA1_TestData&                       t;
        RC<DrawCommandBatch>                drawBatch;
        typename CtxTypes::CommandBuffer    cmdbuf;

        DA1_RenderPassTask (DA1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            const auto  img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

            constexpr auto&     rtech_pass = RTech.Draw_1;
            StaticAssert( rtech_pass.attachmentsCount == 1 );

            const auto  rp_desc = RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::White} );

            if ( not drawBatch )
            {
                // upload vertices
                typename CtxTypes::Transfer     copy_ctx{ *this };

                copy_ctx.AccumBarriers()
                    .MemoryBarrier( EResourceState::Host_Write, EResourceState::CopyDst );

                CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(vertices), vertices, EStagingHeapType::Static ));

                // begin render pass
                typename CtxTypes::Graphics ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

                ctx.AccumBarriers()
                    .MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
                    .MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer )
                    .ImageBarrier( t.img, EResourceState::Invalidate, img_state );

                drawBatch = ctx.BeginMtRenderPass( rp_desc, {"DrawTest.Draw_1"} );
                CHECK_TE( drawBatch );

                cmdbuf = ctx.ReleaseCommandBuffer();
                lock.unlock();

                #ifdef AE_HAS_COROUTINE
                    const auto  CreateDrawTask = [] (DA1_TestData &t, const uint firstVertex) -> DrawTaskCoro
                    {{
                        {
                            bool    is_canceled = co_await Coro_IsCanceled;
                            CHECK( not is_canceled );
                        }{
                            auto    status = co_await Coro_Status;
                            CHECK( status == EStatus::InProgress );
                        }{
                            auto    type = co_await Coro_TaskQueue;
                            CHECK( type == ETaskQueue::Renderer );
                        }

                        DrawTask&   self = co_await DrawTask_GetRef;

                        // same as 'DA1_DrawTask'
                        DeferSharedLock lock {t.guard};
                        CHECK_CE( lock.try_lock() );

                        typename CtxTypes::Draw     dctx{ self };

                        CHECK_CE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

                        dctx.BindPipeline( t.ppln );

                        DrawCmd cmd;
                        cmd.vertexCount = 3;
                        cmd.firstVertex = firstVertex;
                        dctx.Draw( cmd );

                        co_await DrawTask_Execute( dctx );
                        co_return;
                    }};
                    StaticArray< AsyncTask, 4 > draw_tasks = {{
                        drawBatch->Run( CreateDrawTask( t, 0 ), Tuple{}, {"draw cmd 1"} ),
                        drawBatch->Run( CreateDrawTask( t, 3 ), Tuple{}, {"draw cmd 2"} ),
                        drawBatch->Run( CreateDrawTask( t, 6 ), Tuple{}, {"draw cmd 3"} ),
                        drawBatch->Run( CreateDrawTask( t, 9 ), Tuple{}, {"draw cmd 4"} )
                    }};
                #else
                    StaticArray< AsyncTask, 4 > draw_tasks = {{
                        drawBatch->Run< DA1_DrawTask<CtxTypes> >( Tuple{ &t, 0u }, Tuple{}, {"draw cmd 1"} ),
                        drawBatch->Run< DA1_DrawTask<CtxTypes> >( Tuple{ &t, 3u }, Tuple{}, {"draw cmd 2"} ),
                        drawBatch->Run< DA1_DrawTask<CtxTypes> >( Tuple{ &t, 6u }, Tuple{}, {"draw cmd 3"} ),
                        drawBatch->Run< DA1_DrawTask<CtxTypes> >( Tuple{ &t, 9u }, Tuple{}, {"draw cmd 4"} )
                    }};
                #endif
                drawBatch->EndRecording();  // optional

                return Continue( Tuple{ Threading::StrongDepArray{draw_tasks} });
            }
            else
            {
                typename CtxTypes::Graphics ctx{ *this, *drawBatch, RVRef(cmdbuf) };    // continue render pass

                ctx.ExecuteSecondary( *drawBatch );

                // TODO
                //drawBatch = ctx.NextMtSubpass( *drawBatch );

                ctx.EndMtRenderPass();

                ctx.AccumBarriers()
                    .ImageBarrier( t.img, img_state, EResourceState::CopySrc );

                Execute( ctx );
            }
        }
    };


    template <typename Ctx>
    class DA1_CopyTask final : public RenderTask
    {
    public:
        DA1_TestData&   t;

        DA1_CopyTask (DA1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            Ctx     ctx{ *this };

            t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
                        .Then( [p = &t] (const ImageMemView &view)
                                {
                                    p->isOK = p->imgCmp->Compare( view );
                                })};

            ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

            Execute( ctx );
        }
    };


    template <typename CtxTypes, typename CopyCtx>
    static bool  DrawAsync1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
    {
        auto&           rts         = GraphicsScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        DA1_TestData    t;

        t.rtech     = renderTech;
        t.gfxAlloc  = res_mngr.CreateLinearGfxMemAllocator();
        t.imgCmp    = imageCmp;
        t.viewSize  = uint2{800, 600};

        CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

        t.vb = res_mngr.CreateBuffer( BufferDesc{ Sizeof(vertices), EBufferUsage::TransferDst | EBufferUsage::Vertex }.SetMemory( EMemoryType::DeviceLocal ),
                                      "vertex buffer", t.gfxAlloc );
        CHECK_ERR( t.vb );

        t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
                                        .SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
                                      "Image", t.gfxAlloc );
        CHECK_ERR( t.img );

        t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
        CHECK_ERR( t.view );

        t.ppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.draw2 );
        CHECK_ERR( t.ppln );


        CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
        CHECK_ERR( rts.BeginFrame() );

        t.batch = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"DrawAsync1"} );
        CHECK_ERR( t.batch );

        AsyncTask   task1   = t.batch->Run< DA1_RenderPassTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{},                    {"Draw task"} );
        AsyncTask   task2   = t.batch->Run< DA1_CopyTask<CopyCtx>        >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

        AsyncTask   end     = rts.EndFrame( Tuple{task2} );


        CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

        CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
        CHECK_ERR( t.result->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK );
        return true;
    }

} // namespace


bool RGTest::Test_DrawAsync1 ()
{
    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( DrawAsync1Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
    RG_CHECK( DrawAsync1Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

    RG_CHECK( DrawAsync1Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
    RG_CHECK( DrawAsync1Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}
