// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{

    struct RT2_TestData
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
        GAutorelease<BufferID>          indirectBuf;

        RTShaderBindingID               sbt;
        RayTracingPipelineID            ppln;
        GAutorelease<DescriptorSetID>   ds;
        DescSetBinding                  ds_index;

        AsyncTask                       result;

        CommandBatchPtr                 batch;
        bool                            isOK        = false;

        ImageComparator *               imgCmp      = null;
        RC<GfxLinearMemAllocator>       gfxAlloc;

        RTGeometryBuild::TrianglesInfo  triangleInfo;
        RTGeometryBuild::TrianglesData  triangleData;
    };

    static const float3 buffer_vertices []  = { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
    static const uint   buffer_indices []   = { 0, 1, 2 };


    template <typename CtxTypes>
    class RT2_UploadTask final : public RenderTask
    {
    public:
        RT2_TestData&   t;

        RT2_UploadTask (RT2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            typename CtxTypes::Transfer copy_ctx{ *this };

            copy_ctx.AccumBarriers()
                .MemoryBarrier( EResourceState::Host_Write, EResourceState::CopyDst );

            RTSceneBuild    scene_build{ 1u, Default };
            scene_build.SetScratchBuffer( t.scratch );
            scene_build.SetInstanceData( t.instances );

            RTSceneBuild::Instance  inst;
            CHECK_TE( scene_build.SetGeometry( t.rtGeom, INOUT inst ));

            ASBuildIndirectCommand  ind_cmd = {};
            ind_cmd.primitiveCount = 1; // instances

            CHECK_TE( copy_ctx.UploadBuffer( t.vb,          0_b, Sizeof(buffer_vertices),   buffer_vertices, EStagingHeapType::Static ));
            CHECK_TE( copy_ctx.UploadBuffer( t.ib,          0_b, Sizeof(buffer_indices),    buffer_indices,  EStagingHeapType::Static ));
            CHECK_TE( copy_ctx.UploadBuffer( t.instances,   0_b, Sizeof(inst),              &inst,           EStagingHeapType::Static ));
            CHECK_TE( copy_ctx.UploadBuffer( t.indirectBuf, 0_b, Sizeof(ind_cmd),           &ind_cmd,        EStagingHeapType::Static ));

            typename CtxTypes::ASBuild  as_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

            as_ctx.AccumBarriers()
                .MemoryBarrier( EResourceState::CopyDst, EResourceState::BuildRTAS_Read )
                .MemoryBarrier( EResourceState::CopyDst, EResourceState::BuildRTAS_IndirectBuffer );

            as_ctx.Build(
                RTGeometryBuild{
                    ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 },
                    ArrayView<RTGeometryBuild::TrianglesData>{ &t.triangleData, 1 },
                    Default, Default,
                    Default
                }.SetScratchBuffer( t.scratch ),
                t.rtGeom );

            as_ctx.AccumBarriers()
                .MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::BuildRTAS_Read );

            as_ctx.BuildIndirect(
                scene_build,
                t.rtScene,
                t.indirectBuf );

            Execute( as_ctx );
        }
    };


    template <typename CtxTypes>
    class RT2_RayTracingTask final : public RenderTask
    {
    public:
        RT2_TestData&   t;

        RT2_RayTracingTask (RT2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
            RenderTask{ RVRef(batch), dbg },
            t{ t }
        {}

        void  Run () __Th_OV
        {
            DeferExLock lock {t.guard};
            CHECK_TE( lock.try_lock() );

            const auto  img_state   = EResourceState::ShaderStorage_Write | EResourceState::RayTracingShaders;

            typename CtxTypes::RayTracing   ctx{ *this };

            ctx.AccumBarriers()
                .MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::ShaderRTAS_Read | EResourceState::RayTracingShaders )
                //.MemoryBarrier( EResourceState::Host_Write, EResourceState::RTShaderBindingTable )    // optional
                .ImageBarrier( t.img, EResourceState::Invalidate, img_state );

            auto    bar = ctx.DeferredBarriers();
            bar.ImageBarrier( t.img, img_state, EResourceState::CopySrc );

            ctx.BindPipeline( t.ppln );
            ctx.BindDescriptorSet( t.ds_index, t.ds );
            ctx.TraceRays( t.viewSize, t.sbt );

            bar.Commit();

            Execute( ctx );
        }
    };


    template <typename Ctx>
    class RT2_CopyTask final : public RenderTask
    {
    public:
        RT2_TestData&   t;

        RT2_CopyTask (RT2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
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


    template <typename CtxTypes, typename CopyCtx>
    static bool  RayTracing2Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
    {
        auto&           rts         = RenderTaskScheduler();
        auto&           res_mngr    = rts.GetResourceManager();
        const auto      format      = EPixelFormat::RGBA8_UNorm;
        RT2_TestData    t;

        t.gfxAlloc  = MakeRC<GfxLinearMemAllocator>();
        t.imgCmp    = imageCmp;
        t.viewSize  = uint2{800, 600};

        t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
                                        .SetUsage( EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
                                      "Image", t.gfxAlloc );
        CHECK_ERR( t.img );

        t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
        CHECK_ERR( t.view );

        t.vb = res_mngr.CreateBuffer( BufferDesc{ Sizeof(buffer_vertices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
                                      "RTAS vertex buffer", t.gfxAlloc );
        CHECK_ERR( t.vb );

        t.ib = res_mngr.CreateBuffer( BufferDesc{ Sizeof(buffer_indices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
                                      "RTAS index buffer", t.gfxAlloc );
        CHECK_ERR( t.ib );

        t.instances = res_mngr.CreateBuffer( BufferDesc{ SizeOf<RTSceneBuild::Instance>, EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
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

        t.indirectBuf = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ASBuildIndirectCommand>,
                                                           EBufferUsage::Indirect | EBufferUsage::TransferDst | EBufferUsage::ShaderAddress },
                                               "indirect build", t.gfxAlloc );
        CHECK_ERR( t.indirectBuf );

        t.scratch = res_mngr.CreateBuffer( BufferDesc{ Max( geom_sizes.buildScratchSize, scene_sizes.buildScratchSize ), EBufferUsage::ASBuild_Scratch },
                                            "RTAS scratch buffer", t.gfxAlloc );
        CHECK_ERR( t.scratch );

        t.ppln = renderTech->GetRayTracingPipeline( PipelineName{"rtrace1.def"} );
        CHECK_ERR( t.ppln );

        t.sbt = renderTech->GetRTShaderBinding( RTShaderBindingName{"rtrace1.sbt0"} );
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

        AsyncTask   begin   = rts.BeginFrame();

        t.batch = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"RayTracing1"} );
        CHECK_ERR( t.batch );

        AsyncTask   task1   = t.batch->Run< RT2_UploadTask<CtxTypes>     >( Tuple{ArgRef(t)}, Tuple{begin},                 {"Upload RTAS task"} );
        AsyncTask   task2   = t.batch->Run< RT2_RayTracingTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{task1},                 {"Ray tracing task"} );
        AsyncTask   task3   = t.batch->Run< RT2_CopyTask<CopyCtx>        >( Tuple{ArgRef(t)}, Tuple{task2}, True{"Last"},   {"Readback task"} );

        AsyncTask   end     = rts.EndFrame( Tuple{task3} );

        CHECK_ERR( Scheduler().Wait({ end }));
        CHECK_ERR( end->Status() == EStatus::Completed );

        CHECK_ERR( rts.WaitAll() );

        CHECK_ERR( Scheduler().Wait({ t.result }));
        CHECK_ERR( t.result->Status() == EStatus::Completed );

        CHECK_ERR( t.isOK );
        return true;
    }

} // namespace


bool RGTest::Test_RayTracing2 ()
{
    if ( _rtPipelines == null )
        return true; // skip

    #ifdef AE_ENABLE_VULKAN
    if ( not _vulkan.GetProperties().accelerationStructureFeats.accelerationStructureIndirectBuild )
        return true; // skip
    #endif

    auto    img_cmp = _LoadReference( TEST_NAME );
    bool    result  = true;

    RG_CHECK( RayTracing2Test< DirectCtx,   DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
    RG_CHECK( RayTracing2Test< DirectCtx,   IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

    RG_CHECK( RayTracing2Test< IndirectCtx, DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
    RG_CHECK( RayTracing2Test< IndirectCtx, IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

    RG_CHECK( _CompareDumps( TEST_NAME ));

    AE_LOGI( TEST_NAME << " - passed" );
    return result;
}
