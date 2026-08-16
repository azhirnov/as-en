// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Test_RenderGraph.h"

namespace
{
	struct D1_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		GraphicsPipelineID			ppln;

		AsyncTask					result;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::DrawTest_RTech;


	template <typename CtxType>
	static RenderCoro  D1_DrawTask (D1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		typename CtxType::Graphics	ctx{ RenderCoro_Get() };

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
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  D1_CopyTask (D1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<D1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxType, typename CopyCtx>
	static bool  Draw1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		D1_TestData		t;

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

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.draw1 );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Draw1"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( D1_DrawTask<CtxType>(t), Tuple{},						{"Draw task"} );
		AsyncTask	task2	= batch->Run( D1_CopyTask<CopyCtx>(t), Tuple{task1}, True{"Last"},	{"Readback task"} );

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


RGTest::ECode  RGTest::Test_Draw1 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Draw1Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
	RG_CHECK( Draw1Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

	RG_CHECK( Draw1Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
	RG_CHECK( Draw1Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
