// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_TEST_SHADER_DEBUGGER
# include "Test_RenderGraph.h"

namespace
{
	struct Db2_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		GraphicsPipelineID			ppln;

		ShaderDebugger				debugger;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::DebugDraw_RTech;


	template <typename CtxType>
	static RenderCoro  Db2_DrawTask (Db2_TestData& t)
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
			constexpr auto&		rtech_pass = RTech.Draw_1;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( t.viewSize )
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} ));

			dctx.BindPipeline( t.ppln );
			dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  Db2_CopyTask (Db2_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		auto	task1 = ctx.ReadbackImage( t.img, Default );
		auto	task2 = t.debugger.ReadAll( ctx );

		t.result =
			[](auto readOp, auto dbgRes, auto& t) -> InlineCoro<>
			{
				auto	res = Coro_WaitResultOrCancel( readOp, dbgRes );
				auto& [view, trace_str] = res;

				bool	ok = t.imgCmp->Compare( view );
				ASSERT( ok );

				ASSERT( trace_str.size() == 3 );
				std::sort( trace_str.begin(), trace_str.end(), [](auto &lhs, auto &rhs) { return lhs.size() < rhs.size(); });

				if ( trace_str.size() == 3 )
				{
					const StringView	vs1_ref_str =
R"(//> gl_VertexIndex: int {1} | {0x1}
//> gl_InstanceIndex: int {0} | {0x0}
no source

//> (out): float4 {0.500000, 0.500000, 0.000000, 1.000000}
//  gl_VertexIndex: int {1} | {0x1}
23. gl_Position	= vec4( g_Positions[gl_VertexIndex], 0.0, 1.0 );

//> v_Color: float3 {0.000000, 1.000000, 0.000000}
//  gl_VertexIndex: int {1} | {0x1}
24. v_Color		= g_Colors[gl_VertexIndex];

)";
					const StringView	vs2_ref_str =
R"(//> gl_VertexIndex: int {2} | {0x2}
//> gl_InstanceIndex: int {0} | {0x0}
no source

//> (out): float4 {-0.500000, 0.500000, 0.000000, 1.000000}
//  gl_VertexIndex: int {2} | {0x2}
23. gl_Position	= vec4( g_Positions[gl_VertexIndex], 0.0, 1.0 );

//> v_Color: float3 {0.000000, 0.000000, 1.000000}
//  gl_VertexIndex: int {2} | {0x2}
24. v_Color		= g_Colors[gl_VertexIndex];

)";
					const StringView	fs_ref_str =
R"(//> gl_FragCoord: float4 {400.500000, 300.500000, 0.000000, 1.000000}
//> v_Color: float3 {0.498333, 0.252083, 0.249583}
no source

//> out_Color.xyz: float3 {0.498333, 0.252083, 0.249583}
//  v_Color: float3 {0.498333, 0.252083, 0.249583}
12. out_Color.rgb = v_Color.rgb;

//> out_Color.w: float {0.500000}
//  v_Color: float3 {0.498333, 0.252083, 0.249583}
13. out_Color.a   = fract(v_Color.r + v_Color.g + v_Color.b + 0.5f);

)";

					ok &= (trace_str[0] == vs1_ref_str);
					ok &= (trace_str[1] == vs2_ref_str);
					ok &= (trace_str[2] == fs_ref_str);
					ASSERT( ok );

					t.isOK = ok;
				}
			}( task1.readOp, task2, t );

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );

		GraphicsScheduler().AddNextCycleEndDeps( t.result );
	}


	template <typename CtxType, typename CopyCtx>
	static bool  Debugger2Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		Db2_TestData	t;

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

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.dbg2_draw );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger2"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( Db2_DrawTask<CtxType>(t), Tuple{},					{"Draw task"} );
		AsyncTask	task2	= batch->Run( Db2_CopyTask<CopyCtx>(t), Tuple{task1}, True{"Last"}, {"Readback task"} );

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


RGTest::ECode  RGTest::Test_Debugger2 ()
{
	if ( not _dbgPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Debugger2Test< DirectCtx, DirectCtx::Transfer >( _dbgPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // AE_TEST_SHADER_DEBUGGER
