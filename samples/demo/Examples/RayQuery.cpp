// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Examples/RayQuery.h"

namespace AE::Samples::Demo
{
	
	//
	// Upload Task
	//
	class RayQuerySample::UploadTask final : public RenderTask
	{
	public:
		RayQuerySample&	t;

		UploadTask (RayQuerySample* t, CommandBatchPtr batch, DebugLabel) :
			RenderTask{ batch, {"RayQuery::Upload"} },
			t{ *t }
		{}
		
		void  Run () override;
	};
	
/*
=================================================
	UploadTask::Run
=================================================
*/
	void  RayQuerySample::UploadTask::Run ()
	{
		const float3	buffer_vertices []	= { { 0.25f, 0.25f, 0.0f }, { 0.75f, 0.25f, 0.0f }, { 0.50f, 0.75f, 0.0f } };
		const uint		buffer_indices []	= { 0, 1, 2 };

		RTGeometryBuild::TrianglesInfo	triangle_info;
		RTGeometryBuild::TrianglesData	triangle_data;
		
		triangle_info.maxPrimitives		= uint(CountOf( buffer_indices )) / 3;
		triangle_info.maxVertex			= uint(CountOf( buffer_vertices ));
		triangle_info.vertexFormat		= EVertexType::Float3;
		triangle_info.indexType			= EIndex::UInt;
		triangle_info.allowTransforms	= false;
			
		{
			auto&	res_mngr = RenderTaskScheduler().GetResourceManager();
				
			t.vb = res_mngr.CreateBuffer( BufferDesc{ Sizeof(buffer_vertices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
											"RTAS vertex buffer", t.gfxAlloc );
			CHECK_TE( t.vb );
		
			t.ib = res_mngr.CreateBuffer( BufferDesc{ Sizeof(buffer_indices), EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
											"RTAS index buffer", t.gfxAlloc );
			CHECK_TE( t.ib );

			t.instances = res_mngr.CreateBuffer( BufferDesc{ SizeOf<RTSceneBuild::Instance>, EBufferUsage::ASBuild_ReadOnly | EBufferUsage::Transfer },
													"RTAS instance buffer", t.gfxAlloc );
			CHECK_TE( t.instances );

			auto	geom_sizes = res_mngr.GetRTGeometrySizes( RTGeometryBuild{ ArrayView<RTGeometryBuild::TrianglesInfo>{ &triangle_info, 1 }, Default, Default, Default, Default });
			t.rtGeom = res_mngr.CreateRTGeometry( RTGeometryDesc{ geom_sizes.rtasSize, Default }, "RT geometry", t.gfxAlloc );
			CHECK_TE( t.rtGeom );

			auto	scene_sizes = res_mngr.GetRTSceneSizes( RTSceneBuild{ 1u, Default });
			t.rtScene = res_mngr.CreateRTScene( RTSceneDesc{ scene_sizes.rtasSize, Default }, "RT scene", t.gfxAlloc );
			CHECK_TE( t.rtScene );

			t.scratch = res_mngr.CreateBuffer( BufferDesc{ Max( geom_sizes.buildScratchSize, scene_sizes.buildScratchSize ), EBufferUsage::ASBuild_Scratch },
												"RTAS scratch buffer", t.gfxAlloc );
			CHECK_TE( t.scratch );

			// update descriptors
			{
				auto [ds, idx] = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"rquery.ds0"} );
				CHECK_TE( ds and idx == t.dsIndex );
				t.descSet = RVRef(ds);

				DescriptorUpdater	updater;

				CHECK_TE( updater.Set( t.descSet, EDescUpdateMode::Partialy ));
				updater.BindImage( UniformName{"un_OutImage"}, t.rtView );
				updater.BindRayTracingScene( UniformName{"un_RtScene"}, t.rtScene );

				CHECK_TE( updater.Flush() );
			}

			CHECK_TE( t.descSet );
		}
			
		triangle_data.vertexData		= t.vb;
		triangle_data.indexData			= t.ib;
		triangle_data.vertexStride		= Sizeof(buffer_vertices[0]);

		DirectCtx::Transfer	copy_ctx{ *this };
			
		copy_ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::Host_Write, EResourceState::CopySrc );
		
		RTSceneBuild	scene_build{ 1u, Default };
		scene_build.SetScratchBuffer( t.scratch );
		scene_build.SetInstanceData( t.instances );

		RTSceneBuild::Instance	inst;
		inst.Init();
		scene_build.SetGeometry( t.rtGeom, INOUT inst );

		CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(buffer_vertices), buffer_vertices, EStagingHeapType::Static ));
		CHECK_TE( copy_ctx.UploadBuffer( t.ib, 0_b, Sizeof(buffer_indices),  buffer_indices,  EStagingHeapType::Static ));
		CHECK_TE( copy_ctx.UploadBuffer( t.instances, 0_b, Sizeof(inst), &inst, EStagingHeapType::Static ));

		DirectCtx::ASBuild	as_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
			
		as_ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::BuildRTAS_Read );

		as_ctx.Build(
			RTGeometryBuild{
				ArrayView<RTGeometryBuild::TrianglesInfo>{ &triangle_info, 1 },
				ArrayView<RTGeometryBuild::TrianglesData>{ &triangle_data, 1 },
				Default, Default,
				Default
			}.SetScratchBuffer( t.scratch ),
			t.rtGeom );
			
		as_ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::BuildRTAS_Read );
		
		as_ctx.Build( scene_build, t.rtScene );

		Execute( as_ctx );
	}
//-----------------------------------------------------------------------------



	//
	// Ray Tracing Task
	//
	class RayQuerySample::RayTracingTask final : public RenderTask
	{
	public:
		RayQuerySample &	t;
		IOutputSurface &	surface;

		RayTracingTask (RayQuerySample* t, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) :
			RenderTask{ batch, {"RayQuery::RayTracing"} },
			t{ *t }, surface{ surf }
		{}

		void  Run () override;
	};
	
/*
=================================================
	RayTracingTask::Run
=================================================
*/
	void  RayQuerySample::RayTracingTask::Run ()
	{
		IOutputSurface::RenderTargets_t		targets;
		CHECK_TE( surface.GetTargets( OUT targets ));

		auto&		rt			= targets[0];
		const auto	img_state	= EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;
			
		DirectCtx::Compute	ctx{ *this };

		ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::BuildRTAS_Write, EResourceState::ShaderRTAS_Read | EResourceState::ComputeShader )
			.ImageBarrier( t.rtImage, EResourceState::Invalidate, img_state );

		ctx.BindPipeline( t.ppln );
		ctx.BindDescriptorSet( t.dsIndex, t.descSet );
		ctx.Dispatch( DivCeil( t.rtSize, 8u ));

		ctx.AccumBarriers()
			.ImageBarrier( t.rtImage,	img_state,										EResourceState::BlitSrc )
			.ImageBarrier( rt.imageId,	rt.initialState | EResourceState::Invalidate,	EResourceState::BlitDst );

		DirectCtx::Transfer	blit_ctx{ *this, ctx.ReleaseCommandBuffer() };
				
		ImageBlit	blit;
		blit.srcSubres	= { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
		blit.srcOffset0	= { 0u, 0u, 0u };
		blit.srcOffset1	= { t.rtSize.x, t.rtSize.y, 1u };
		blit.dstSubres	= { EImageAspect::Color, 0_mipmap, 0_layer, 1u };
		blit.dstOffset0	= { rt.region.left, rt.region.top, 0 };
		blit.dstOffset1	= { rt.region.right, rt.region.bottom, 1 };

		blit_ctx.BlitImage( t.rtImage, rt.imageId, EBlitFilter::Linear, ArrayView<ImageBlit>{ &blit, 1 });
			
		blit_ctx.AccumBarriers()
			.ImageBarrier( rt.imageId, EResourceState::BlitDst, rt.finalState );

		Execute( blit_ctx );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Init
=================================================
*/
	bool  RayQuerySample::Init (PipelinePackID pack)
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();

		gfxAlloc = MakeRC<GfxLinearMemAllocator>();
		
		rtImage = res_mngr.CreateImage( ImageDesc{}.SetDimension( rtSize ).SetFormat( EPixelFormat::RGBA8_UNorm )
											.SetUsage( EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::Transfer ),
										"RenderTarget", gfxAlloc );
		CHECK_ERR( rtImage );
	
		rtView = res_mngr.CreateImageView( ImageViewDesc{}, rtImage, "RenderTargetView" );
		CHECK_ERR( rtView );

		rqPipelines = res_mngr.LoadRenderTech( pack, RenderTechName{"RayQuery.RTech"}, Default );
		CHECK_ERR( rqPipelines );
		
		ppln = rqPipelines->GetComputePipeline( PipelineName{"rquery.def"} );
		CHECK_ERR( ppln );

		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	AsyncTask  RayQuerySample::Draw (RenderGraph &rg, ArrayView<AsyncTask> inDeps)
	{
		auto	batch = rg.Render( "RayQuery pass" );
		CHECK_ERR( batch );

		ArrayView<AsyncTask>	deps = inDeps;
		AsyncTask				upload;

		if ( not uploaded.load() )
		{
			uploaded.store( true );
		
			upload = batch->Run< UploadTask >( Tuple{this}, Tuple{deps} );

			deps = ArrayView<AsyncTask>{ upload };
		}
		
		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		return batch->Run< RayTracingTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"} );
	}
	
/*
=================================================
	destructor
=================================================
*/
	RayQuerySample::~RayQuerySample ()
	{
		auto&	res_mngr = RenderTaskScheduler().GetResourceManager();

		res_mngr.DelayedReleaseResources( rtImage, rtView, vb, ib, instances, scratch, rtGeom, rtScene, descSet );
	}


} // AE::Samples::Demo
