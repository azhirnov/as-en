// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"
# include "graphics/RenderGraph/RenderGraphImpl.h"

namespace
{

    struct RT3_TestData
    {
        Mutex                           guard;

        uint2                           viewSize;

        GAutorelease<ImageID>           img;
        GAutorelease<ImageViewID>       view;

        GAutorelease<BufferID>          vb;
        GAutorelease<BufferID>          ib;
        GAutorelease<BufferID>          instances;
        GAutorelease<BufferID>          scratch;

        GAutorelease<RTGeometryID>      rtGeom;
        GAutorelease<RTSceneID>         rtScene;

        RTShaderBindingID               sbt;
        RayTracingPipelineID            ppln;
        GAutorelease<DescriptorSetID>   ds;
        DescSetBinding                  ds_index;

        AsyncTask                       result;

        RG::CommandBatchPtr             batch;
        bool                            isOK        = false;

        ImageComparator *               imgCmp      = null;
        GfxMemAllocatorPtr              gfxAlloc;

        RTGeometryBuild::TrianglesInfo  triangleInfo;
        RTGeometryBuild::TrianglesData  triangleData;
    };

    static constexpr auto&  RTech = RenderTechs::RayTracingTestRT;

    static const float3 buffer_vertices []  = { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
    static const uint   buffer_indices []   = { 0, 1, 2 };


    template <typename CtxTypes>
    class RT3_UploadTask final : public RenderTask
    {
    public:
        RT3_TestData&   t;

        RT3_UploadTask (RT3_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            typename CtxTypes::Transfer copy_ctx{ *this };

            RTSceneBuild    scene_build{ 1u, Default };
            scene_build.SetScratchBuffer( t.scratch );
            scene_build.SetInstanceData( t.instances );

            RTSceneBuild::Instance  inst;
            CHECK_TE( scene_build.SetGeometry( t.rtGeom, INOUT inst ));

            CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(buffer_vertices), buffer_vertices, EStagingHeapType::Static ));
            CHECK_TE( copy_ctx.UploadBuffer( t.ib, 0_b, Sizeof(buffer_indices),  buffer_indices,  EStagingHeapType::Static ));
            CHECK_TE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));

            typename CtxTypes::ASBuild  as_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

            as_ctx.Build(
                RTGeometryBuild{
                    ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 },
                    ArrayView<RTGeometryBuild::TrianglesData>{ &t.triangleData, 1 },
                    Default, Default,
                    Default
                }.SetScratchBuffer( t.scratch ),
                t.rtGeom );

            as_ctx.Build( scene_build, t.rtScene );

            Execute( as_ctx );
        }
    };


    template <typename CtxTypes>
    class RT3_RayTracingTask final : public RenderTask
    {
    public:
        RT3_TestData&   t;

        RT3_RayTracingTask (RT3_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            typename CtxTypes::RayTracing   ctx{ *this };

            ctx.BindPipeline( t.ppln );
            ctx.BindDescriptorSet( t.ds_index, t.ds );
            ctx.TraceRays( t.viewSize, t.sbt );

            Execute( ctx );
        }
    };


    template <typename Ctx>
    class RT3_CopyTask final : public RenderTask
    {
    public:
        RT3_TestData&   t;

        RT3_CopyTask (RT3_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
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

            Execute( ctx );
        }
    };


    template <typename CtxTypes, typename CopyCtx>
    static bool  RayTracing3Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
    {
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        RT3_TestData    t;
        auto&           rg          = RenderTaskScheduler().GetRenderGraph();
        auto&           res_mngr    = rg.GetResourceManager();

        t.gfxAlloc  = res_mngr.CreateLinearGfxMemAllocator();
        t.imgCmp    = imageCmp;
        t.viewSize  = uint2{800, 600};

        CHECK_ERR( renderTech->Name() == RenderTechName{RTech} );

        t.img = rg.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
                                    .SetUsage( EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
                                "Image", t.gfxAlloc );
        CHECK_ERR( t.img );

        t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
        CHECK_ERR( t.view );

        t.vb = rg.CreateBuffer( BufferDesc{ Sizeof(buffer_vertices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
                                      "RTAS vertex buffer", t.gfxAlloc );
        CHECK_ERR( t.vb );

        t.ib = rg.CreateBuffer( BufferDesc{ Sizeof(buffer_indices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
                                      "RTAS index buffer", t.gfxAlloc );
        CHECK_ERR( t.ib );

        t.instances = rg.CreateBuffer( BufferDesc{ SizeOf<RTSceneBuild::Instance>, EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
                                             "RTAS instance buffer", t.gfxAlloc );
        CHECK_ERR( t.instances );

        t.triangleInfo.maxPrimitives    = uint(CountOf( buffer_indices )) / 3;
        t.triangleInfo.maxVertex        = uint(CountOf( buffer_vertices ));
        t.triangleInfo.vertexFormat     = EVertexType::Float3;
        t.triangleInfo.indexType        = EIndex::UInt;
        t.triangleInfo.allowTransforms  = false;

        t.triangleData.vertexData       = t.vb;
        t.triangleData.indexData        = t.ib;
        t.triangleData.vertexStride     = Sizeof(buffer_vertices[0]);

        auto    geom_sizes = res_mngr.GetRTGeometrySizes( RTGeometryBuild{ ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 }, Default, Default, Default, Default });
        t.rtGeom = res_mngr.CreateRTGeometry( RTGeometryDesc{ geom_sizes.rtasSize, Default }, "RT geometry", t.gfxAlloc );
        CHECK_ERR( t.rtGeom );

        auto    scene_sizes = res_mngr.GetRTSceneSizes( RTSceneBuild{ 1u, Default });
        t.rtScene = res_mngr.CreateRTScene( RTSceneDesc{ scene_sizes.rtasSize, Default }, "RT scene", t.gfxAlloc );
        CHECK_ERR( t.rtScene );

        t.scratch = res_mngr.CreateBuffer( BufferDesc{ Max( geom_sizes.buildScratchSize, scene_sizes.buildScratchSize ), EBufferUsage::ASBuild_Scratch },
                                            "RTAS scratch buffer", t.gfxAlloc );
        CHECK_ERR( t.scratch );

        t.ppln = renderTech->GetRayTracingPipeline( RTech.RayTrace_1.rtrace1_def );
        CHECK_ERR( t.ppln );

        t.sbt = renderTech->GetRTShaderBinding( RTech.sbt.rtrace1_sbt0 );
        CHECK_ERR( t.sbt );

        {
            StructSet( t.ds, t.ds_index ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"rtrace1.ds0"} );
            CHECK_ERR( t.ds );

            DescriptorUpdater   updater;

            CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
            updater.BindImage( UniformName{"un_OutImage"}, t.view );
            updater.BindRayTracingScene( UniformName{"un_RtScene"}, t.rtScene );

            CHECK_ERR( updater.Flush() );
        }
        CHECK_ERR( t.ds );

        AsyncTask   begin   = rg.BeginFrame();

        t.batch = rg.CmdBatch( EQueueType::Graphics, {"RayTracing1"} )
                    .UseResource( t.img )
                    .Begin();
        CHECK_ERR( t.batch );

        AsyncTask   task1   = t.batch.template Task< RT3_UploadTask<CtxTypes> >( Tuple{ArgRef(t)}, {"Upload RTAS task"} ).Run( Tuple{begin} );

        AsyncTask   task2   = t.batch.template Task< RT3_RayTracingTask<CtxTypes> >( Tuple{ArgRef(t)}, {"Ray tracing task"} )
                                .UseResource( t.img, EResourceState::ShaderStorage_Write | EResourceState::RayTracingShaders )
                                .Run( Tuple{task1} );

        AsyncTask   task3   = t.batch.template Task< RT3_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, {"Readback task"} )
                                .UseResource( t.img, EResourceState::CopySrc )
                                .SubmitBatch().Run( Tuple{task2} );

        AsyncTask   end     = rg.EndFrame( Tuple{task3} );

        CHECK_ERR( Scheduler().Wait({ end }));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rg.WaitAll() );

        CHECK_ERR( Scheduler().Wait({ t.result }));
        CHECK_ERR( t.result->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK );
        return true;
    }

} // namespace


bool RGTest::Test_RayTracing3 ()
{
    if ( _rtPipelines == null )
        return true; // skip

    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( RayTracing3Test< RG::DirectCtx,   RG::DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
    RG_CHECK( RayTracing3Test< RG::DirectCtx,   RG::IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

    RG_CHECK( RayTracing3Test< RG::IndirectCtx, RG::DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
    RG_CHECK( RayTracing3Test< RG::IndirectCtx, RG::IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}

#endif // not AE_ENABLE_METAL
