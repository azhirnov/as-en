// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{

	struct Db5_TestData
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
		
		ComputePipelineID				ppln;
		GAutorelease<DescriptorSetID>	ds;
		DescSetBinding					ds_index;

		AsyncTask						result;
		
		ShaderDebugger					debugger;

		CommandBatchPtr					batch;
		bool							isOK		= false;

		ImageComparator *				imgCmp		= null;
		RC<GfxLinearMemAllocator>		gfxAlloc;

		RTGeometryBuild::TrianglesInfo	triangleInfo;
		RTGeometryBuild::TrianglesData	triangleData;
	};
	
	static const float3	buffer_vertices []	= { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
	static const uint	buffer_indices []	= { 0, 1, 2 };

	
	template <typename CtxTypes>
	class Db5_UploadTask final : public RenderTask
	{
	public:
		Db5_TestData&	t;

		Db5_UploadTask (Db5_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}
		
		void  Run () __Th_OV
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
			CHECK_TE( scene_build.SetGeometry( t.rtGeom, INOUT inst ));

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
	class Db5_RayTracingTask final : public RenderTask
	{
	public:
		Db5_TestData&	t;

		Db5_RayTracingTask (Db5_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			const auto	img_state	= EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;
			
			typename CtxTypes::Transfer		copy_ctx{ *this };

			ShaderDebugger::Result	dbg;
			CHECK_TE( t.debugger.AllocForCompute( OUT dbg, copy_ctx, t.ppln, uint3{t.viewSize/2, 0} ));


			typename CtxTypes::Compute		ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::ShaderRTAS_Read | EResourceState::ComputeShader )
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			ctx.BindPipeline( t.ppln );
			ctx.BindDescriptorSet( t.ds_index, t.ds );
			ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
			ctx.Dispatch( DivCeil( t.viewSize, 8u ));

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};

	
	template <typename Ctx>
	class Db5_CopyTask final : public RenderTask
	{
	public:
		Db5_TestData&	t;

		Db5_CopyTask (Db5_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };
			
			auto	task1 = ctx.ReadbackImage( t.img, Default );
			auto	task2 = t.debugger.ReadAll( ctx );
			
			t.result = AsyncTask{ MakePromiseFrom( task1, task2 )
				.Then( [p = &t] (const Tuple<ImageMemView, Array<String>> &view_and_str)
				{
					bool	ok = p->imgCmp->Compare( view_and_str.Get<ImageMemView>() );
					ASSERT( ok );

					Array<String>	trace_str = view_and_str.Get<Array<String>>();
					ASSERT( trace_str.size() == 1 );

					if ( trace_str.size() == 1 )
					{
						const StringView	ref_str =
R"(//> gl_GlobalInvocationID: uint3 {400, 300, 0}
//> gl_LocalInvocationID: uint3 {0, 4, 0}
//> gl_WorkGroupID: uint3 {50, 37, 0}
no source

//> uv: float2 {0.500625, 0.500833}
//  gl_GlobalInvocationID: uint3 {400, 300, 0}
4. uv		 = (vec2(gl_GlobalInvocationID.xy) + 0.5) / vec2(gl_WorkGroupSize.xy * gl_NumWorkGroups.xy);

//> origin: float3 {0.500625, 0.499167, -1.000000}
//  uv: float2 {0.500625, 0.500833}
5. origin	 = vec3(uv.x, 1.0 - uv.y, -1.0f);

//> rayQueryInitialize(): void
//  origin: float3 {0.500625, 0.499167, -1.000000}
9. 	rayQueryInitializeEXT( ray_query, un_RtScene, gl_RayFlagsNoneEXT,
10. 						   /*cullMask*/0xFF, origin, /*Tmin*/0.0f, direction, /*Tmax*/10.0f );

//> rayQueryProceed(): bool {true}
12. 	while ( rayQueryProceedEXT( ray_query ))

//> rayQueryGetIntersectionType(): uint {0}
14. 		if ( rayQueryGetIntersectionTypeEXT( ray_query, false ) == gl_RayQueryCandidateIntersectionTriangleEXT )

//> (out): bool {true}
//  rayQueryGetIntersectionType(): uint {0}
14. if ( rayQueryGetIntersectionTypeEXT( ray_query, false ) == gl_RayQueryCandidateIntersectionTriangleEXT )

//> rayQueryConfirmIntersection(): void
15. 			rayQueryConfirmIntersectionEXT( ray_query );

//> rayQueryProceed(): bool {false}
12. 	while ( rayQueryProceedEXT( ray_query ))

//> rayQueryGetIntersectionType(): uint {1}
20. 	if ( rayQueryGetIntersectionTypeEXT( ray_query, true ) == gl_RayQueryCommittedIntersectionNoneEXT )

//> (out): bool {false}
//  rayQueryGetIntersectionType(): uint {1}
20. if ( rayQueryGetIntersectionTypeEXT( ray_query, true ) == gl_RayQueryCommittedIntersectionNoneEXT )

//> attribs: float2 {0.252083, 0.498333}
28. attribs		 = rayQueryGetIntersectionBarycentricsEXT( ray_query, true );

//> barycentrics: float3 {0.249583, 0.252083, 0.498333}
//  attribs: float2 {0.252083, 0.498333}
29. barycentrics = vec3( 1.0f - attribs.x - attribs.y, attribs.x, attribs.y );

//> color: float4 {0.249583, 0.252083, 0.498333, 1.000000}
//  barycentrics: float3 {0.249583, 0.252083, 0.498333}
30. color = vec4(barycentrics, 1.0);

//> imageStore(): void
//  gl_GlobalInvocationID: uint3 {400, 300, 0}
//  color: float4 {0.249583, 0.252083, 0.498333, 1.000000}
33. 	imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );

)";
						ok &= (trace_str[0] == ref_str);
						ASSERT( ok );
						p->isOK = ok;
					}
				})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			Execute( ctx );
		}
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  Debugger5Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		Db5_TestData	t;
		
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

		t.ppln = renderTech->GetComputePipeline( PipelineName{"dbg5_rquery.def"} );
		CHECK_ERR( t.ppln );

		{
			StructSet( t.ds, t.ds_index ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"dbg5_rquery.ds0"} );
			CHECK_ERR( t.ds );

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );
			updater.BindRayTracingScene( UniformName{"un_RtScene"}, t.rtScene );

			CHECK_ERR( updater.Flush() );
		}
		CHECK_ERR( t.ds );

		AsyncTask	begin	= rts.BeginFrame();

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger5"} );
		CHECK_ERR( t.batch );
		
		AsyncTask	task1	= t.batch->Run< Db5_UploadTask<CtxTypes>     >( Tuple{ArgRef(t)}, Tuple{begin},					{"Upload RTAS task"} );
		AsyncTask	task2	= t.batch->Run< Db5_RayTracingTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{task1},					{"Ray tracing task"} );
		AsyncTask	task3	= t.batch->Run< Db5_CopyTask<CopyCtx>        >( Tuple{ArgRef(t)}, Tuple{task2}, True{"Last"},	{"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task3} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_Debugger5 ()
{
	if ( _rqPipelines == null )
		return true; // skip

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Debugger5Test< DirectCtx,   DirectCtx::Transfer   >( _rqPipelines, img_cmp.get() ));
	
	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
