// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{

	struct RQ1_TestData
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
		
		ComputePipelineID			ppln;
		Strong<DescriptorSetID>		ds;
		DescSetBinding				ds_index;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK		= false;

		ImageComparator *			imgCmp		= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;

		RTGeometryBuild::TrianglesInfo	triangleInfo;
		RTGeometryBuild::TrianglesData	triangleData;
	};
	
	static const float3	buffer_vertices []	= { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
	static const uint	buffer_indices []	= { 0, 1, 2 };

	
	template <typename CtxTypes>
	class RQ1_UploadTask final : public RenderTask
	{
	public:
		RQ1_TestData&	t;

		RQ1_UploadTask (RQ1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}
		
		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			typename CtxTypes::Transfer	copy_ctx{ *this };
			
			copy_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::Host_Write, EResourceState::CopyDst );

			RTSceneBuild	scene_build{ 1u, Default };
			scene_build.SetScratchBuffer( t.scratch );
			scene_build.SetInstanceData( t.instances );

			RTSceneBuild::Instance	inst;
			inst.Init();
			scene_build.SetGeometry( t.rtGeom, INOUT inst );

			CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(buffer_vertices), buffer_vertices, EStagingHeapType::Static ));
			CHECK_TE( copy_ctx.UploadBuffer( t.ib, 0_b, Sizeof(buffer_indices),  buffer_indices,  EStagingHeapType::Static ));
			CHECK_TE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));

			typename CtxTypes::ASBuild	as_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
			
			as_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::BuildRTAS_Read );

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

			as_ctx.Build( scene_build, t.rtScene );

			Execute( as_ctx );
		}
	};


	template <typename CtxTypes>
	class RQ1_RayTracingTask final : public RenderTask
	{
	public:
		RQ1_TestData&	t;

		RQ1_RayTracingTask (RQ1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			const auto	img_state	= EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;
			
			typename CtxTypes::Compute	ctx{ *this };

			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::ShaderRTAS_Read | EResourceState::ComputeShader )
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			ctx.BindPipeline( t.ppln );
			ctx.BindDescriptorSet( t.ds_index, t.ds );
			ctx.Dispatch( DivCeil( t.viewSize, 8u ));

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};

	
	template <typename Ctx>
	class RQ1_CopyTask final : public RenderTask
	{
	public:
		RQ1_TestData&	t;

		RQ1_CopyTask (RQ1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
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
			
			Execute( ctx );
		}
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  RayQuery1Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		RQ1_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

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

		auto	scene_sizes = res_mngr.GetRTSceneSizes( RTSceneBuild{ 1u, Default });
		t.rtScene = res_mngr.CreateRTScene( RTSceneDesc{ scene_sizes.rtasSize, Default }, "RT scene", t.gfxAlloc );
		CHECK_ERR( t.rtScene );

		t.scratch = res_mngr.CreateBuffer( BufferDesc{ Max( geom_sizes.buildScratchSize, scene_sizes.buildScratchSize ), EBufferUsage::ASBuild_Scratch },
											"RTAS scratch buffer", t.gfxAlloc );
		CHECK_ERR( t.scratch );

		t.ppln = renderTech->GetComputePipeline( PipelineName{"rquery1.def"} );
		CHECK_ERR( t.ppln );

		{
			StructSet( t.ds, t.ds_index ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"rquery1.ds1"} );
			CHECK_ERR( t.ds );

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );
			updater.BindRayTracingScene( UniformName{"un_RtScene"}, t.rtScene );

			CHECK_ERR( updater.Flush() );
		}
		CHECK_ERR( t.ds );

		AsyncTask	begin	= rts.BeginFrame();

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, ESubmitMode::Immediately, {"RayQuery1"} );
		CHECK_ERR( t.batch );
		
		AsyncTask	task1	= t.batch->Run< RQ1_UploadTask<CtxTypes>     >( Tuple{ArgRef(t)}, Tuple{begin},					{"Upload RTAS task"} );
		AsyncTask	task2	= t.batch->Run< RQ1_RayTracingTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{task1},					{"Ray tracing task"} );
		AsyncTask	task3	= t.batch->Run< RQ1_CopyTask<CopyCtx>        >( Tuple{ArgRef(t)}, Tuple{task2}, True{"Last"},	{"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task3} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.view, t.img,
											  t.vb, t.ib, t.instances, t.scratch,
											  t.rtGeom, t.rtScene, t.ds ));

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_RayQuery1 ()
{
	if ( _rqPipelines == null )
		return true; // skip

	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( RayQuery1Test< DirectCtx,   DirectCtx::Transfer   >( _rqPipelines, img_cmp.get() )));
	CHECK_ERR(( RayQuery1Test< DirectCtx,   IndirectCtx::Transfer >( _rqPipelines, img_cmp.get() )));

	CHECK_ERR(( RayQuery1Test< IndirectCtx, DirectCtx::Transfer   >( _rqPipelines, img_cmp.get() )));
	CHECK_ERR(( RayQuery1Test< IndirectCtx, IndirectCtx::Transfer >( _rqPipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
