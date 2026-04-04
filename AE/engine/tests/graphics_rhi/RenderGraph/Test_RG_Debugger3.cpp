// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_TEST_SHADER_DEBUGGER
# include "Test_RenderGraph.h"

namespace
{
	struct Db3_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		MeshPipelineID				ppln;

		ShaderDebugger				debugger;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::DrawMeshes_RTech;


	template <typename CtxType>
	static RenderCoro  Db3_DrawTask (Db3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		typename CtxType::Transfer	copy_ctx{ RenderCoro_Get() };

		ShaderDebugger::Result	dbg;
		CHECK_CE( t.debugger.AllocForGraphics( OUT dbg, copy_ctx, t.ppln ));

		typename CtxType::Graphics	ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

		ctx.AccumBarriers()
			.ResourceBarrier( t.img, EResourceState::Invalidate, img_state );

		// draw
		{
			constexpr auto&		rtech_pass = RTech.DrawMeshes_1;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( t.viewSize )
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} ));

			dctx.BindPipeline( t.ppln );
			dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
			dctx.DrawMeshTasks( uint3{1} );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  Db3_CopyTask (Db3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		auto	task1 = ctx.ReadbackImage( t.img, Default );
		auto	task2 = t.debugger.ReadAll( ctx );

		t.result =
			[] (auto readOp, auto dbgRes, auto& t) -> InlineCoro<>
			{
				auto	res = Coro_WaitResultOrCancel( readOp, dbgRes );
				auto& [view, trace_str] = res;

				bool	ok = t.imgCmp->Compare( view );
				ASSERT( ok );
				ASSERT( trace_str.size() == 1 );

				if ( trace_str.size() == 1 )
				{
					const StringView	ms_ref_str =
R"(//> gl_GlobalInvocationID: uint3 {0, 0, 0} | {0x0, 0x0, 0x0}
//> gl_LocalInvocationID: uint3 {0, 0, 0} | {0x0, 0x0, 0x0}
//> gl_WorkGroupID: uint3 {0, 0, 0} | {0x0, 0x0, 0x0}
no source

//> (out): float4 {0.000000, -0.500000, 0.000000, 1.000000}
28. gl_MeshVerticesEXT[I].gl_Position	= vec4( g_Positions[I], 0.0, 1.0 );

//> (out): float4 {1.000000, 0.000000, 0.000000, 1.000000}
29. Out[I].color						= vec4( g_Colors[I], 1.0 );

//> (out): bool {true}
31. if ( I == 0 )

//> gl_PrimitiveTriangleIndicesEXT: uint3 {0, 1, 2} | {0x0, 0x1, 0x2}
33. gl_PrimitiveTriangleIndicesEXT[0] = uvec3(0,1,2);

//> SetMeshOutputs(): void
34. 			SetMeshOutputsEXT( 3, 1 );

)";
					ok &= (trace_str[0] == ms_ref_str);
					ASSERT( ok );

					t.isOK = ok;
				}
			}( task1.readOp, task2, t );

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );

		GraphicsScheduler().AddNextCycleEndDeps( t.result );
	}


	template <typename CtxType, typename CopyCtx>
	static bool  Debugger3Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		Db3_TestData	t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = t.rtech->GetMeshPipeline( RTech.DrawMeshes_1.dbg3_draw );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger3"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( Db3_DrawTask<CtxType>(t), Tuple{},					{"Draw mesh task"} );
		AsyncTask	task2	= batch->Run( Db3_CopyTask<CopyCtx>(t), Tuple{task1}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));
		CHECK_ERR( t.result );

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == ETaskStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_Debugger3 ()
{
	if ( _dbgPipelines == null or _msPipelines == null )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Debugger3Test< DirectCtx, DirectCtx::Transfer >( _msPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // AE_TEST_SHADER_DEBUGGER
