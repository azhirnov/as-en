// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{

	struct Db4_TestData
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
	class Db4_UploadTask final : public RenderTask
	{
	public:
		Db4_TestData&	t;

		Db4_UploadTask (Db4_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
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
	class Db4_RayTracingTask final : public RenderTask
	{
	public:
		Db4_TestData&	t;

		Db4_RayTracingTask (Db4_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			const auto	img_state	= EResourceState::ShaderStorage_Write | EResourceState::RayTracingShaders;
			
			typename CtxTypes::Transfer		copy_ctx{ *this };

			ShaderDebugger::Result	dbg;
			CHECK_TE( t.debugger.AllocForRayTracing( OUT dbg, copy_ctx, t.ppln ));


			typename CtxTypes::RayTracing	rt_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

			rt_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::ShaderRTAS_Read | EResourceState::RayTracingShaders )
				//.MemoryBarrier( EResourceState::Host_Write, EResourceState::RTShaderBindingTable )	// optional
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			rt_ctx.BindPipeline( t.ppln );
			rt_ctx.BindDescriptorSet( t.ds_index, t.ds );
			rt_ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
			rt_ctx.TraceRays( t.viewSize, t.sbt );
			
			rt_ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( rt_ctx );
		}
	};

	
	template <typename Ctx>
	class Db4_CopyTask final : public RenderTask
	{
	public:
		Db4_TestData&	t;

		Db4_CopyTask (Db4_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
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
					ASSERT( trace_str.size() == 4 );
					std::sort( trace_str.begin(), trace_str.end(), [](auto &lhs, auto &rhs) { return lhs.size() < rhs.size(); });

					if ( trace_str.size() == 4 )
					{
						const StringView	rg1_ref_str =
R"(//> gl_LaunchIDEXT: uint3 {612, 124, 0}
no source

//> uv: float2 {0.765625, 0.207500}
//  gl_LaunchIDEXT: uint3 {612, 124, 0}
15. uv			= vec2(gl_LaunchIDEXT.xy + 0.5) / vec2(gl_LaunchSizeEXT.xy);

//> origin: float3 {0.765625, 0.792500, -1.000000}
//  uv: float2 {0.765625, 0.207500}
16. origin		= vec3(uv.x, 1.0f - uv.y, -1.0f);

//> payload: float4 {0.000000, 0.000000, 0.000000, 0.000000}
20. payload = vec4(0.0);

//> traceRays(): void
//  origin: float3 {0.765625, 0.792500, -1.000000}
24. 	traceRayEXT( /*topLevel*/un_RtScene, /*rayFlags*/gl_RayFlagsNoneEXT, /*cullMask*/0xFF,
25. 				 /*sbtRecordOffset*/ray_idx, /*sbtRecordStride*/HitGroupStride, /*missIndex*/1,
26. 				 /*origin*/origin, /*Tmin*/0.0f, /*direction*/direction, /*Tmax*/10.0f,
27. 				 /*payload*/0 );

//> payload: float4 {0.412000, 0.796000, 1.000000, 1.000000}
27. 0 );

//> color: float4 {0.412000, 0.796000, 1.000000, 1.000000}
//  payload: float4 {0.412000, 0.796000, 1.000000, 1.000000}
29. color = payload;

//> imageStore(): void
//  gl_LaunchIDEXT: uint3 {612, 124, 0}
//  color: float4 {0.412000, 0.796000, 1.000000, 1.000000}
30. 	imageStore( un_OutImage, ivec2(gl_LaunchIDEXT), color );

)";
						const StringView	rg2_ref_str =
R"(//> gl_LaunchIDEXT: uint3 {370, 326, 0}
no source

//> uv: float2 {0.463125, 0.544167}
//  gl_LaunchIDEXT: uint3 {370, 326, 0}
15. uv			= vec2(gl_LaunchIDEXT.xy + 0.5) / vec2(gl_LaunchSizeEXT.xy);

//> origin: float3 {0.463125, 0.455833, -1.000000}
//  uv: float2 {0.463125, 0.544167}
16. origin		= vec3(uv.x, 1.0f - uv.y, -1.0f);

//> payload: float4 {0.000000, 0.000000, 0.000000, 0.000000}
20. payload = vec4(0.0);

//> traceRays(): void
//  origin: float3 {0.463125, 0.455833, -1.000000}
24. 	traceRayEXT( /*topLevel*/un_RtScene, /*rayFlags*/gl_RayFlagsNoneEXT, /*cullMask*/0xFF,
25. 				 /*sbtRecordOffset*/ray_idx, /*sbtRecordStride*/HitGroupStride, /*missIndex*/1,
26. 				 /*origin*/origin, /*Tmin*/0.0f, /*direction*/direction, /*Tmax*/10.0f,
27. 				 /*payload*/0 );

//> payload: float4 {0.367917, 0.220417, 0.411667, 1.000000}
27. 0 );

//> color: float4 {0.367917, 0.220417, 0.411667, 1.000000}
//  payload: float4 {0.367917, 0.220417, 0.411667, 1.000000}
29. color = payload;

//> imageStore(): void
//  gl_LaunchIDEXT: uint3 {370, 326, 0}
//  color: float4 {0.367917, 0.220417, 0.411667, 1.000000}
30. 	imageStore( un_OutImage, ivec2(gl_LaunchIDEXT), color );

)";
						const StringView	rm_ref_str =
R"(//> gl_LaunchIDEXT: uint3 {612, 124, 0}
no source

//> payload: float4 {0.412000, 0.796000, 1.000000, 1.000000}
42. payload = vec4( 0.412f, 0.796f, 1.0f, 1.0f );

)";
						const StringView	rch_ref_str =
R"(//> gl_LaunchIDEXT: uint3 {370, 326, 0}
//> hitAttribs: float2 {0.220417, 0.411667}
no source

//> barycentrics: float3 {0.367917, 0.220417, 0.411667}
//  hitAttribs: float2 {0.220417, 0.411667}
56. barycentrics = vec3( 1.0f - hitAttribs.x - hitAttribs.y, hitAttribs.x, hitAttribs.y );

//> payload: float4 {0.367917, 0.220417, 0.411667, 1.000000}
//  barycentrics: float3 {0.367917, 0.220417, 0.411667}
57. payload = vec4(barycentrics, 1.0);

)";
						ok &= (trace_str[0] == rm_ref_str);
						ok &= (trace_str[1] == rch_ref_str);
						ok &= (trace_str[2] == rg1_ref_str);
						ok &= (trace_str[3] == rg2_ref_str);
						ASSERT( ok );

						p->isOK = ok;
					}
				})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			Execute( ctx );
		}
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  Debugger4Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		Db4_TestData	t;
		
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

		t.ppln = renderTech->GetRayTracingPipeline( PipelineName{"dbg4_rtrace1.def"} );
		CHECK_ERR( t.ppln );

		t.sbt = renderTech->GetRTShaderBinding( RTShaderBindingName{"dbg4_rtrace1.sbt0"} );
		CHECK_ERR( t.sbt );

		{
			StructSet( t.ds, t.ds_index ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"dbg4_rtrace1.ds1"} );
			CHECK_ERR( t.ds );
			
			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );
			updater.BindRayTracingScene( UniformName{"un_RtScene"}, t.rtScene );

			CHECK_ERR( updater.Flush() );
		}
		CHECK_ERR( t.ds );

		AsyncTask	begin	= rts.BeginFrame();

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger4"} );
		CHECK_ERR( t.batch );
		
		AsyncTask	task1	= t.batch->Run< Db4_UploadTask<CtxTypes>     >( Tuple{ArgRef(t)}, Tuple{begin},					{"Upload RTAS task"} );
		AsyncTask	task2	= t.batch->Run< Db4_RayTracingTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{task1},					{"Ray tracing task"} );
		AsyncTask	task3	= t.batch->Run< Db4_CopyTask<CopyCtx>        >( Tuple{ArgRef(t)}, Tuple{task2}, True{"Last"},	{"Readback task"} );

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


bool RGTest::Test_Debugger4 ()
{
	if ( _rtPipelines == null )
		return true; // skip

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Debugger4Test< DirectCtx,   DirectCtx::Transfer   >( _rtPipelines, img_cmp.get() ));
	
	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
