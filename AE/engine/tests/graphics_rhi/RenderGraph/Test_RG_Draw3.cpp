// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct D3_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		GraphicsPipelineID			ppln;
		PushConstantIndex			pcIdx;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::DrawTestRT;

	static const float4		vertices[] = {
		float4{ 0.0f, -0.5f, BitCast<float>(HtmlColor::Red),	1.f},
		float4{ 0.5f,  0.5f, BitCast<float>(HtmlColor::Green),	1.f},
		float4{-0.5f,  0.5f, BitCast<float>(HtmlColor::Blue),	1.f},
	};


	template <typename CtxType>
	static RenderCoro  D3_DrawTask (D3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		typename CtxType::Graphics	ctx{ RenderCoro_Get() };

		ctx.AccumBarriers()
			.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

		// draw
		{
			constexpr auto&		rtech_pass = RTech.Draw_1;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( t.viewSize )
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} ));

			dctx.BindPipeline( t.ppln );
			dctx.PushConstant( t.pcIdx, Sizeof(vertices), vertices, ShaderStructName{"PC_draw3"} );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ImageBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  D3_CopyTask (D3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<D3_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxType, typename CopyCtx>
	static bool  Draw3Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		D3_TestData		t;

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

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.draw3 );
		CHECK_ERR( t.ppln );

		t.pcIdx = res_mngr.GetPushConstantIndex< ShaderTypes::PC_draw3 >( t.ppln, PushConstantName{"pc"} );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Draw3"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( D3_DrawTask<CtxType>(t), Tuple{},						{"Draw task"} );
		AsyncTask	task2	= batch->Run( D3_CopyTask<CopyCtx>(t), Tuple{task1}, True{"Last"},	{"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == ETaskStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_Draw3 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Draw3Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
	RG_CHECK( Draw3Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

	RG_CHECK( Draw3Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
	RG_CHECK( Draw3Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
