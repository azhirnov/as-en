// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{

	struct RT3_TestData
	{
		Mutex							guard;

		uint2							viewSize;

		GAutorelease<ImageID>			img;
		GAutorelease<ImageViewID>		view;

		GAutorelease<BufferID>			vb;
		GAutorelease<BufferID>			ib;
		GAutorelease<BufferID>			instances;
		GAutorelease<BufferID>			scratch;

		GAutorelease<RTGeometryID>		rtGeom;
		GAutorelease<RTSceneID>			rtScene;

		RTShaderBindingID				sbt;
		RayTracingPipelineID			ppln;
		GAutorelease<DescriptorSetID>	ds;
		DescSetBinding					ds_index;

		AsyncTask						result;
		bool							isOK		= false;

		ImageComparator *				imgCmp		= null;
		GfxMemAllocatorPtr				gfxAlloc;

		RTGeometryBuild::TrianglesInfo	triangleInfo;
		RTGeometryBuild::TrianglesData	triangleData;
	};

	static constexpr auto&	RTech = RenderTechs::RayTracing_RTech;

	static const float3	c_BufferVertices []	= { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
	static const uint	c_BufferIndices []	= { 0, 1, 2 };


	template <typename CtxTypes>
	static RenderCoro  RT3_UploadTask (RT3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Transfer	copy_ctx{ RenderCoro_Get() };

		RTSceneBuild	scene_build{ 1u, Default };
		scene_build.SetScratchBuffer( t.scratch );
		scene_build.SetInstanceData( t.instances );

		CHECK_CE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(c_BufferVertices), c_BufferVertices, EStagingHeapType::Static ));
		CHECK_CE( copy_ctx.UploadBuffer( t.ib, 0_b, Sizeof(c_BufferIndices),  c_BufferIndices,  EStagingHeapType::Static ));

		switch_enum( copy_ctx.GetDevice().GetGraphicsAPI() )
		{
			case EGraphicsAPI::Vulkan :
			{
				RTSceneBuild::InstanceVk	inst;
				inst.Init();
				CHECK_CE( scene_build.SetGeometry( t.rtGeom, INOUT inst ));
				CHECK_CE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));
				break;
			}
			case EGraphicsAPI::Metal :
			{
				RTSceneBuild::InstanceMtl	inst;
				inst.Init();
				CHECK_CE( scene_build.SetGeometry( t.rtGeom, INOUT inst ));
				CHECK_CE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));
				break;
			}
		}
		switch_end

		typename CtxTypes::ASBuild	as_ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

		as_ctx.Build(
			RTGeometryBuild{
				ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 },
				ArrayView<RTGeometryBuild::TrianglesData>{ &t.triangleData, 1 },
				Default, Default,
				Default
			}.SetScratchBuffer( t.scratch ),
			t.rtGeom );

		as_ctx.Build( scene_build, t.rtScene );

		RenderCoro_Execute( as_ctx );
	}


	template <typename CtxTypes>
	static RenderCoro  RT3_RayTracingTask (RT3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::RayTracing	ctx{ RenderCoro_Get() };

		ctx.BindPipeline( t.ppln );
		ctx.BindDescriptorSet( t.ds_index, t.ds );
		ctx.TraceRays( t.viewSize, t.sbt );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  RT3_CopyTask (RT3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<RT3_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  RayTracing3Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		RT3_TestData	t;
		auto&			rg			= GraphicsScheduler().GetRenderGraph();
		auto&			res_mngr	= rg.GetResourceManager();

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( renderTech->Name() == RenderTechName{RTech} );

		t.img = rg.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
									.SetUsage( EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
								"Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.vb = rg.CreateBuffer( BufferDesc{ Sizeof(c_BufferVertices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
									  "RTAS vertex buffer", t.gfxAlloc );
		CHECK_ERR( t.vb );

		t.ib = rg.CreateBuffer( BufferDesc{ Sizeof(c_BufferIndices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
									  "RTAS index buffer", t.gfxAlloc );
		CHECK_ERR( t.ib );

		t.instances = rg.CreateBuffer( BufferDesc{ RTSceneBuild::InstanceSize, EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
											 "RTAS instance buffer", t.gfxAlloc );
		CHECK_ERR( t.instances );

		t.triangleInfo.maxPrimitives	= uint(CountOf( c_BufferIndices )) / 3;
		t.triangleInfo.maxVertex		= uint(CountOf( c_BufferVertices ));
		t.triangleInfo.vertexFormat		= EVertexType::Float3;
		t.triangleInfo.indexType		= EIndex::UInt;
		t.triangleInfo.allowTransforms	= false;

		t.triangleData.vertexData		= t.vb;
		t.triangleData.indexData		= t.ib;
		t.triangleData.vertexStride		= Sizeof(c_BufferVertices[0]);

		auto	geom_sizes = res_mngr.GetRTGeometrySizes( RTGeometryBuild{ ArrayView<RTGeometryBuild::TrianglesInfo>{ &t.triangleInfo, 1 }, Default, Default, Default, Default });
		t.rtGeom = res_mngr.CreateRTGeometry( RTGeometryDesc{ geom_sizes.rtasSize, Default }, "RT geometry", t.gfxAlloc );
		CHECK_ERR( t.rtGeom );

		auto	scene_sizes = res_mngr.GetRTSceneSizes( RTSceneBuild{ 1u, Default });
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

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );
			updater.BindRayTracingScene( UniformName{"un_RtScene"}, t.rtScene );

			CHECK_ERR( updater.Flush() );
		}
		CHECK_ERR( t.ds );


		CHECK_ERR( rg.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rg.BeginFrame() );

		auto	batch	= rg.CmdBatch( EQueueType::Graphics, {"RayTracing3"} )
							.UseResource( t.img )
							.Begin();
		CHECK_ERR( batch );

		AsyncTask	task1	= batch.Task( RT3_UploadTask<CtxTypes>(t), {"Upload RTAS task"} ).Run();

		AsyncTask	task2	= batch.Task( RT3_RayTracingTask<CtxTypes>(t), {"Ray tracing task"} )
								.UseResource( t.img, EResourceState::ShaderStorage_Write | EResourceState::RayTracingShaders )
								.Run( Tuple{task1} );

		AsyncTask	task3	= batch.Task( RT3_CopyTask<CopyCtx>(t), {"Readback task"} )
								.UseResource( t.img, EResourceState::CopySrc )
								.SubmitBatch().Run( Tuple{task2} );

		AsyncTask	end		= rg.EndFrame( Tuple{task3} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rg.WaitAll( c_MaxTimeout ));
		CHECK_ERR( t.result );

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == ETaskStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_RayTracing3 ()
{
	if ( _rtPipelines == null )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( RayTracing3Test< RG::DirectCtx,   RG::DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
	RG_CHECK( RayTracing3Test< RG::DirectCtx,   RG::IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

	RG_CHECK( RayTracing3Test< RG::IndirectCtx, RG::DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
	RG_CHECK( RayTracing3Test< RG::IndirectCtx, RG::IndirectCtx::Transfer >( _rtPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // not AE_ENABLE_METAL
