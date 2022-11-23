// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if 0
#include "Test_RenderGraph.h"

namespace
{

	struct RT1_TestData
	{
		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;

		Strong<BufferID>			vb;
		Strong<BufferID>			ib;
		Strong<BufferID>			instances;
		Strong<BufferID>			scratch;

		Strong<RTGeometryID>		rtGeom;
		Strong<RTSceneID>			rtScene;
		
		RayTracingPipelineID		ppln;
		Strong<DescriptorSetID>		ds;
		uint						ds_index	= UMax;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK		= false;

		ImageComparator *			imgCmp		= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;

		ShaderBindingTableManager	sbtMngr;
		Unique<ShaderBindingTable>	sbt;

		RTGeometryBuild::TrianglesInfo	triangleInfo;
		RTGeometryBuild::TrianglesData	triangleData;
	};
	
	static const float3	buffer_vertices []	= { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
	static const uint	buffer_indices []	= { 0, 1, 2 };

	
	template <typename CtxTypes>
	class RT1_UploadTask final : public RenderTask
	{
	public:
		RT1_TestData&	t;

		RT1_UploadTask (RT1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
			RenderTask{ batch, dbgName, dbgColor },
			t{ t }
		{}
		
		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			typename CtxTypes::Transfer	copy_ctx{ *this };

			RTSceneBuild::Instance	inst;
			inst.Init();

			CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(buffer_vertices), buffer_vertices, EStagingHeapType::Static ));
			CHECK_TE( copy_ctx.UploadBuffer( t.ib, 0_b, Sizeof(buffer_indices),  buffer_indices,  EStagingHeapType::Static ));
			CHECK_TE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));

			t.sbt->Upload( copy_ctx );


			typename CtxTypes::ASBuild	as_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
			
			as_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::Host_Write, EResourceState::BuildRTAS_Read );

			as_ctx.Build(
				RTGeometryBuild{
					ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 },
					ArrayView<RTGeometryBuild::TrianglesData>{ &t.triangleData, 1 },
					Default, Default,
					Default
				},
				t.rtGeom );
			
			as_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::BuildRTAS_Read );

			RTSceneBuild	build_cmd{ 1u, Default };
			build_cmd.SetGeometry( t.rtGeom, INOUT inst );

			as_ctx.Build( build_cmd, t.rtScene );
		}
	};


	template <typename CtxTypes>
	class RT1_RayTracingTask final : public RenderTask
	{
	public:
		RT1_TestData&	t;

		RT1_RayTracingTask (RT1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
			RenderTask{ batch, dbgName, dbgColor },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			const auto	img_state	= EResourceState::ShaderStorage_Write | EResourceState::RayTracingShaders;
			
			typename CtxTypes::RayTracing	ctx{ *this };

			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::BuildRTAS_Read )
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			ctx.BindPipeline( t.ppln );
			ctx.BindDescriptorSet( t.ds_index, t.ds );
			ctx.TraceRays( t.viewSize, *t.sbt );

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};

	
	template <typename Ctx>
	class RT1_CopyTask final : public RenderTask
	{
	public:
		RT1_TestData&	t;

		RT1_CopyTask (RT1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
			RenderTask{ batch, dbgName, dbgColor },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };

			t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
						.Then( [p = &t] (const ImageMemView &view)
								{
									p->isOK = p->imgCmp->Compare( view );
								})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			ExecuteAndSubmit( ctx );
		}
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  RayTracing1Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		RT1_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
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

		t.triangleInfo.maxPrimitives	= uint(CountOf( buffer_indices )) / 3;
		t.triangleInfo.maxVertex		= uint(CountOf( buffer_vertices ));
		t.triangleInfo.vertexFormat		= EVertexType::Float3;
		t.triangleInfo.indexType		= EIndex::UInt;
		t.triangleInfo.allowTransforms	= false;

		t.triangleData.vertexData		= t.vb;
		t.triangleData.indexData		= t.ib;
		t.triangleData.vertexStride		= Sizeof(buffer_vertices[0]);

		auto	geom_sizes = res_mngr.GetRTGeometrySizes( RTGeometryBuild{ ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 }, Default, Default, Default, Default });
		t.rtGeom = res_mngr.CreateRTGeometry( RTGeometryDesc{ geom_sizes.rtasSize, Default }, "RT geometry", t.gfxAlloc );
		CHECK_ERR( t.rtGeom );
		
		t.sbtMngr.Add( t.rtGeom, {RTGeometryName{"geom"}} );

		auto	scene_sizes = res_mngr.GetRTSceneSizes( RTSceneBuild{ 1u, Default });
		t.rtScene = res_mngr.CreateRTScene( RTSceneDesc{ scene_sizes.rtasSize, Default }, "RT scene", t.gfxAlloc );
		CHECK_ERR( t.rtScene );
		
		t.sbtMngr.Add( t.rtScene, {RTInstanceName{"inst"}}, {t.rtGeom} );

		t.scratch = res_mngr.CreateBuffer( BufferDesc{ Max( geom_sizes.buildScratchSize, scene_sizes.buildScratchSize ), EBufferUsage::ASBuild_Scratch },
											"RTAS scratch buffer", t.gfxAlloc );
		CHECK_ERR( t.scratch );

		t.ppln = renderTech->GetRayTracingPipeline( PipelineName{"rtrace1.def"} );
		CHECK_ERR( t.ppln );

		{
			auto [ds, idx] = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"rtrace1.ds1"} );
			t.ds		= RVRef(ds);
			t.ds_index	= idx;
		}
		CHECK_ERR( t.ds );

		AsyncTask	begin	= rts.BeginFrame();

		t.batch	= rts.CreateBatch( EQueueType::Graphics, 0, "RayTracing1" );
		CHECK_ERR( t.batch );
		
		AsyncTask	task1	= t.batch->Add< RT1_UploadTask<CtxTypes>	>( Tuple{ArgRef(t)}, Tuple{begin}, "Upload RTAS task" );
		AsyncTask	task2	= t.batch->Add< RT1_RayTracingTask<CtxTypes>>( Tuple{ArgRef(t)}, Tuple{task1}, "Ray tracing task" );
		AsyncTask	task3	= t.batch->Add< RT1_CopyTask<CopyCtx>		>( Tuple{ArgRef(t)}, Tuple{task2}, "Readback task" );

		AsyncTask	end		= rts.EndFrame( Tuple{task3} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.view, t.img,
											  t.vb, t.ib,
											  t.rtGeom, t.rtScene /*, t.ds*/ ));

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_RayTracing1 ()
{
	if ( _rtPipelines == null )
		return true; // skip

	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( RayTracing1Test< DirectCtx,   DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() )));
	//CHECK_ERR(( RayTracing1Test< DirectCtx,   IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() )));

	//CHECK_ERR(( RayTracing1Test< IndirectCtx, DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() )));
	//CHECK_ERR(( RayTracing1Test< IndirectCtx, IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
#endif
