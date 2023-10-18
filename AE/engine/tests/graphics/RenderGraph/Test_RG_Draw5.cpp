// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"
#include "graphics/RenderGraph/RenderGraphImpl.h"

namespace
{
    struct D5_TestData
    {
        RenderTechPipelinesPtr      rtech;
        uint2                       viewSize;

        GAutorelease<ImageID>       img;
        GAutorelease<ImageViewID>   view;

        GraphicsPipelineID          ppln0;
        GraphicsPipelineID          ppln1;

        AsyncTask                   result;
        bool                        isOK    = false;

        ImageComparator *           imgCmp  = null;
        GfxMemAllocatorPtr          gfxAlloc;
    };

    static constexpr auto&  RTech = RenderTechs::DrawTestRT;

    static const ShaderTypes::Vertex_draw2  vertices1[] = {
        { float2{-1.0f, -1.0f}, HtmlColor::Crimson },
        { float2{ 0.3f, -1.0f}, HtmlColor::Bisque },
        { float2{-1.0f,  1.0f}, HtmlColor::HoneyDew },
        { float2{ 0.5f,  0.8f}, HtmlColor::SteelBlue }
    };
    static const ShaderTypes::Vertex_draw2  vertices2[] = {
        { float2{ 1.0f,  1.0f}, HtmlColor::OldLace },
        { float2{-0.3f,  1.0f}, HtmlColor::Thistle },
        { float2{ 1.0f, -1.0f}, HtmlColor::Gainsboro },
        { float2{-0.5f, -0.8f}, HtmlColor::ForestGreen }
    };
    static const ShaderTypes::Vertex_draw2  vertices3[] = {
        { float2{ 0.0f, -0.5f}, HtmlColor::Red },
        { float2{ 0.5f,  0.5f}, HtmlColor::Green },
        { float2{-0.5f,  0.5f}, HtmlColor::Blue }
    };


    template <typename CtxTypes>
    class D5_DrawTask final : public RenderTask
    {
    public:
        D5_TestData&    t;
        const uint      mode;

        D5_DrawTask (D5_TestData& t, uint mode, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }, mode{ mode }
        {}

        void  Run () __Th_OV
        {
            typename CtxTypes::Graphics ctx{ *this };

            // pass 1
            if ( mode == 0 )
            {
                constexpr auto&     rtech_pass = RTech.Test4_1;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = ctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::White} ));

                VertexStream    vstream;
                CHECK_TE( dctx.AllocVStream( Sizeof(vertices1), OUT vstream ));
                MemCopy( OUT vstream.mappedPtr, vertices1, Sizeof(vertices1) );

                CHECK_TE( dctx.BindVertexBuffer( t.ppln0, VertexBufferName{"vb"}, vstream.id, vstream.offset ));

                dctx.BindPipeline( t.ppln0 );
                dctx.Draw( 4 );

                ctx.EndRenderPass( dctx );
            }

            // pass 2
            if ( mode == 0 )
            {
                constexpr auto&     rtech_pass = RTech.Test4_2;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = ctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view ));

                VertexStream    vstream;
                CHECK_TE( dctx.AllocVStream( Sizeof(vertices2), OUT vstream ));
                MemCopy( OUT vstream.mappedPtr, vertices2, Sizeof(vertices2) );

                dctx.BindVertexBuffer( 0, vstream.id, vstream.offset );

                dctx.BindPipeline( t.ppln1 );
                dctx.Draw( 4 );

                ctx.EndRenderPass( dctx );
            }

            // pass 1 v2
            if ( mode == 1 )
            {
                constexpr auto&     rtech_pass = RTech.Test4_2;
                STATIC_ASSERT( rtech_pass.attachmentsCount == 1 );

                auto    dctx = ctx.BeginRenderPass( RenderPassDesc{ t.rtech, rtech_pass, t.viewSize }
                                    .AddViewport( t.viewSize )
                                    .AddTarget( rtech_pass.att_Color, t.view ));

                VertexStream    vstream;
                CHECK_TE( dctx.AllocVStream( Sizeof(vertices3), OUT vstream ));
                MemCopy( OUT vstream.mappedPtr, vertices3, Sizeof(vertices3) );

                dctx.BindVertexBuffer( 0, vstream.id, vstream.offset );

                dctx.BindPipeline( t.ppln1 );
                dctx.Draw( 3 );

                ctx.EndRenderPass( dctx );
            }

            Execute( ctx );
        }
    };


    template <typename Ctx>
    class D5_CopyTask final : public RenderTask
    {
    public:
        D5_TestData&    t;

        D5_CopyTask (D5_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            Ctx     ctx{ *this };

            t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
                        .Then( [p = &t] (const ImageMemView &view)
                                {
                                    p->isOK = p->imgCmp->Compare( view );
                                })};

            Execute( ctx );
        }
    };


    template <typename CtxTypes, typename CopyCtx>
    static bool  Draw5Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
    {
        auto&           res_mngr    = RenderTaskScheduler().GetResourceManager();
        auto&           rg          = RenderTaskScheduler().GetRenderGraph();
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        D5_TestData     t;

        t.rtech     = renderTech;
        t.gfxAlloc  = res_mngr.CreateLinearGfxMemAllocator();
        t.imgCmp    = imageCmp;
        t.viewSize  = uint2{800, 600};

        CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

        t.img = rg.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
                                    .SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
                                "Image", t.gfxAlloc );
        CHECK_ERR( t.img );

        t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
        CHECK_ERR( t.view );

        t.ppln0 = t.rtech->GetGraphicsPipeline( RTech.Test4_1.draw4_1 );
        t.ppln1 = t.rtech->GetGraphicsPipeline( RTech.Test4_2.draw4_2 );
        CHECK_ERR( t.ppln0 and t.ppln1 );

        AsyncTask   begin   = rg.BeginFrame();

        // pass1
        auto        batch1  = rg.CmdBatch( EQueueType::Graphics, {"Draw4-pass1"} )
                                    .UseResource( t.img, EResourceState::ColorAttachment )
                                    .UploadMemory()
                                    .Begin();
        CHECK_ERR( batch1 );

        AsyncTask   task1   = batch1.template Task< D5_DrawTask<CtxTypes> >( Tuple{ArgRef(t), 0u}, {"Draw task1"} )
                                    .Run( Tuple{begin} );
        AsyncTask   task2   = batch1.template Task< D5_DrawTask<CtxTypes> >( Tuple{ArgRef(t), 1u}, {"Draw task2"} )
                                    .SubmitBatch()
                                    .Run( Tuple{begin, task1} );


        // pass2
        auto        batch2  = rg.CmdBatch( EQueueType::Graphics, {"Draw4-pass2"} )
                                    .UseResource( t.img )
                                    .ReadbackMemory()
                                    .Begin();
        CHECK_ERR( batch2 );

        AsyncTask   task3   = batch2.template Task< D5_CopyTask<CopyCtx>  >( Tuple{ArgRef(t)}, {"Readback task"} )
                                    .UseResource( t.img, EResourceState::CopySrc )
                                    .SubmitBatch().Run( Tuple{begin} );


        AsyncTask   end     = rg.EndFrame( Tuple{task1, task2, task3} );

        CHECK_ERR( Scheduler().Wait({ end }));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rg.WaitAll() );

        CHECK_ERR( Scheduler().Wait({ t.result }));
        CHECK_ERR( t.result->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK );
        return true;
    }

} // namespace


bool RGTest::Test_Draw5 ()
{
    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( Draw5Test< RG::DirectCtx, RG::DirectCtx::Transfer >( _pipelines, img_cmp.get() ));
    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}
