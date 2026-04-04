// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{

	struct OM1_TestData
	{
		Mutex							guard;

		uint2							viewSize;

		GAutorelease<ImageID>			img;
		GAutorelease<ImageViewID>		view;

		GAutorelease<BufferID>			vb;
		GAutorelease<BufferID>			ib;
		GAutorelease<BufferID>			instances;
		GAutorelease<BufferID>			scratch;

		GAutorelease<BufferID>			opacityBuffer;
		GAutorelease<BufferID>			triangleArray;

		GAutorelease<RTGeometryID>		rtGeom;
		GAutorelease<RTSceneID>			rtScene;
		GAutorelease<RTMicromapID>		rtMicromap;

		RTShaderBindingID				sbt;
		RayTracingPipelineID			ppln;
		GAutorelease<DescriptorSetID>	ds;
		DescSetBinding					ds_index;

		AsyncTask						result;

		RG::CommandBatchPtr				batch;
		bool							isOK		= false;

		ImageComparator *				imgCmp		= null;
		GfxMemAllocatorPtr				gfxAlloc;

		RTGeometryBuild::TrianglesInfo	triangleInfo;
		RTGeometryBuild::TrianglesData	triangleData;
		RTGeometryBuild::MicromapInfo	geomMmInfo;

		RTMicromapInfo					mmInfo;
		RTMicromapInfo::Usage			mmUsage;
	};

	static constexpr auto&	RTech = RenderTechs::OpacityMicromap_RTech;

	static const float3	c_BufferVertices []	= {
		{0.1f,  0.1f, 0.f},  {0.5f,  0.98f, 0.f},  {0.9f,  0.1f, 0.f},		// 0, 1, 2
		{0.5f,  0.1f, 0.f},  {0.35f, 0.6f,  0.f},  {0.65f, 0.6f, 0.f}		// 3, 4, 5
	};
	static const uint	c_BufferIndices []	= {
		0, 4, 3,	// left
		4, 1, 5,	// up
		3, 5, 2,	// right
		4, 5, 3		// center
	};

	static const auto	c_OMM_Format		= EOpacityMicromapFormat::TwoState;
	static const uint	c_BitsPerMM			= c_OMM_Format == EOpacityMicromapFormat::TwoState  ? 1 :
											  c_OMM_Format == EOpacityMicromapFormat::FourState ? 2 : 0;
	static const uint	c_SubdivisionLevel	= 2;		// 16 micro tris per triangle = 2 bytes

	static const ushort	c_OpacityData [] = {
		0b1010'1010'1010'1010,
		0b0101'0101'0101'0101,
		0b0011'0011'0011'0011,
		0b0010'0010'0010'0010
	};
	static const RTMicromapBuild::Triangle	c_MicroTriangles [] = {
		{ 0_b,  c_SubdivisionLevel,  c_OMM_Format },
		{ 2_b,  c_SubdivisionLevel,  c_OMM_Format },
		{ 4_b,  c_SubdivisionLevel,  c_OMM_Format },
		{ 6_b,  c_SubdivisionLevel,  c_OMM_Format }
	};
	StaticAssert( CountOf(c_MicroTriangles) * IPow( 4u, c_SubdivisionLevel ) * c_BitsPerMM == sizeof(c_OpacityData) * 8 );


	template <typename CtxTypes>
	static RenderCoro  OM1_UploadTask (OM1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		RTSceneBuild	scene_build{ 1u, Default };
		scene_build.SetScratchBuffer( t.scratch );
		scene_build.SetInstanceData( t.instances );

		typename CtxTypes::Transfer	copy_ctx{ RenderCoro_Get() };
		{
			CHECK_CE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(c_BufferVertices), c_BufferVertices, EStagingHeapType::Static ));
			CHECK_CE( copy_ctx.UploadBuffer( t.ib, 0_b, Sizeof(c_BufferIndices),  c_BufferIndices,  EStagingHeapType::Static ));

			{
				RTSceneBuild::InstanceVk	inst;
				inst.Init();
				CHECK_CE( scene_build.SetGeometry( t.rtGeom, INOUT inst ));
				CHECK_CE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));
			}

			CHECK_CE( copy_ctx.UploadBuffer( t.opacityBuffer, 0_b, Sizeof(c_OpacityData),    c_OpacityData,    EStagingHeapType::Static ));
			CHECK_CE( copy_ctx.UploadBuffer( t.triangleArray, 0_b, Sizeof(c_MicroTriangles), c_MicroTriangles, EStagingHeapType::Static ));
		}

		typename CtxTypes::ASBuild	as_ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };
		{
			as_ctx.Build(
				RTMicromapBuild{ t.mmInfo }
					.SetData( t.opacityBuffer )
					.SetScratchBuffer( t.scratch )
					.SetTriangleArray( t.triangleArray ),
				t.rtMicromap );

			as_ctx.Build(
				RTGeometryBuild{}
					.SetTriangles({ &t.triangleInfo, 1 }, { &t.triangleData, 1 })
					.SetMicromaps({ &t.geomMmInfo,   1 })
					.SetScratchBuffer( t.scratch ),
				t.rtGeom );

			as_ctx.Build( scene_build, t.rtScene );
		}
		RenderCoro_Execute( as_ctx );
	}


	template <typename CtxTypes>
	static RenderCoro  OM1_RayTracingTask (OM1_TestData& t)
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
	static RenderCoro  OM1_CopyTask (OM1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<OM1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  OpacityMicromap1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		OM1_TestData	t;
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


		t.mmUsage.triangleCount			= uint(CountOf( c_MicroTriangles ));
		t.mmUsage.subdivisionLevel		= c_SubdivisionLevel;
		t.mmUsage.format.opacity		= c_OMM_Format;

		t.mmInfo.type					= EMicromapType::Opacity;
		t.mmInfo.buildFlags				= EBuildMicromapFlags::PreferFastBuild;
		t.mmInfo.usage					= { &t.mmUsage, 1u };

		auto	mm_sizes = res_mngr.GetRTMicromapSizes( t.mmInfo );
		t.rtMicromap = rg.CreateRTMicromap( RTMicromapDesc{ mm_sizes.micromapSize, t.mmInfo.type }, "RT micromap", t.gfxAlloc );
		CHECK_ERR( t.rtMicromap );


		t.triangleInfo.maxPrimitives	= uint(CountOf( c_BufferIndices )) / 3;
		t.triangleInfo.maxVertex		= uint(CountOf( c_BufferVertices ));
		t.triangleInfo.vertexFormat		= EVertexType::Float3;
		t.triangleInfo.indexType		= EIndex::UInt;
		t.triangleInfo.allowTransforms	= false;
		t.triangleInfo.micromapIndex	= 0;

		t.triangleData.vertexData		= t.vb;
		t.triangleData.indexData		= t.ib;
		t.triangleData.vertexStride		= Sizeof(c_BufferVertices[0]);

		t.geomMmInfo.baseTriangle		= 0;
		t.geomMmInfo.usage				= { &t.mmUsage, 1u };

		t.geomMmInfo.micromapId			= t.rtMicromap;

		auto	geom_sizes = res_mngr.GetRTGeometrySizes( RTGeometryBuild{}
															.SetTriangles({ &t.triangleInfo, 1 })
															.SetMicromaps({ &t.geomMmInfo,   1 })
														);
		t.rtGeom = res_mngr.CreateRTGeometry( RTGeometryDesc{ geom_sizes.rtasSize, Default }, "RT geometry", t.gfxAlloc );
		CHECK_ERR( t.rtGeom );

		auto	scene_sizes = res_mngr.GetRTSceneSizes( RTSceneBuild{ 1u, Default });
		t.rtScene = res_mngr.CreateRTScene( RTSceneDesc{ scene_sizes.rtasSize, Default }, "RT scene", t.gfxAlloc );
		CHECK_ERR( t.rtScene );

		t.scratch = rg.CreateBuffer( BufferDesc{ Max( geom_sizes.buildScratchSize, scene_sizes.buildScratchSize, mm_sizes.buildScratchSize ),
												 EBufferUsage::ASBuild_Scratch },
									 "RTAS scratch buffer", t.gfxAlloc );
		CHECK_ERR( t.scratch );

		t.opacityBuffer = rg.CreateBuffer( BufferDesc{ Sizeof(c_OpacityData), EBufferUsage::MMBuild_ReadOnly | EBufferUsage::Transfer }, "Opacity data", t.gfxAlloc );
		CHECK_ERR( t.opacityBuffer );

		t.triangleArray = rg.CreateBuffer( BufferDesc{ Sizeof(c_MicroTriangles), EBufferUsage::MMBuild_ReadOnly | EBufferUsage::Transfer }, "MicroTriangle array", t.gfxAlloc );
		CHECK_ERR( t.triangleArray );

		t.ppln = renderTech->GetRayTracingPipeline( RTech.RayTrace_1.mmtrace1_def );
		CHECK_ERR( t.ppln );

		t.sbt = renderTech->GetRTShaderBinding( RTech.sbt.mmtrace1_sbt0 );
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

		t.batch	= rg.CmdBatch( EQueueType::Graphics, {"RayTracing3"} )
					.UseResource( t.img )
					.Begin();
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch.Task( OM1_UploadTask<CtxTypes>(t), {"Upload RTAS task"} ).Run();

		AsyncTask	task2	= t.batch.Task( OM1_RayTracingTask<CtxTypes>(t), {"Ray tracing task"} )
								.UseResource( t.img, EResourceState::ShaderStorage_Write | EResourceState::RayTracingShaders )
								.Run( Tuple{task1} );

		AsyncTask	task3	= t.batch.Task( OM1_CopyTask<CopyCtx>(t), {"Readback task"} )
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


RGTest::ECode  RGTest::Test_OpacityMicromap1 ()
{
	if ( _ommPipelines == null )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( OpacityMicromap1Test< RG::DirectCtx,   RG::DirectCtx::Transfer   >( _ommPipelines, img_cmp.get() ));
	RG_CHECK( OpacityMicromap1Test< RG::DirectCtx,   RG::IndirectCtx::Transfer >( _ommPipelines, img_cmp.get() ));

	RG_CHECK( OpacityMicromap1Test< RG::IndirectCtx, RG::DirectCtx::Transfer   >( _ommPipelines, img_cmp.get() ));
	RG_CHECK( OpacityMicromap1Test< RG::IndirectCtx, RG::IndirectCtx::Transfer >( _ommPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // not AE_ENABLE_METAL
