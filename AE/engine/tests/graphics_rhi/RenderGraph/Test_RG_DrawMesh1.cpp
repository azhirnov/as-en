// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifndef AE_ENABLE_METAL
# include "Test_RenderGraph.h"

namespace
{
	struct DM1_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		MeshPipelineID				ppln;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::DrawMeshes_RTech;


	template <typename CtxType>
	static RenderCoro  DM1_DrawTask (DM1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		typename CtxType::Graphics	ctx{ RenderCoro_Get() };

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
			dctx.DrawMeshTasks( uint3{1} );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  DM1_CopyTask (DM1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<DM1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxType, typename CopyCtx>
	static bool  DrawMesh1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		DM1_TestData	t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = t.rtech->GetMeshPipeline( RTech.DrawMeshes_1.draw_mesh1 );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"DrawMesh1"} );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Run( DM1_DrawTask<CtxType>(t), Tuple{},						{"Draw task"} );
		AsyncTask	task2	= t.batch->Run( DM1_CopyTask<CopyCtx>(t), Tuple{task1}, True{"Last"},	{"Readback task"} );

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


RGTest::ECode  RGTest::Test_DrawMesh1 ()
{
	if ( _msPipelines == null )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( DrawMesh1Test< DirectCtx,   DirectCtx::Transfer   >( _msPipelines, img_cmp.get() ));
	RG_CHECK( DrawMesh1Test< DirectCtx,   IndirectCtx::Transfer >( _msPipelines, img_cmp.get() ));

	RG_CHECK( DrawMesh1Test< IndirectCtx, DirectCtx::Transfer   >( _msPipelines, img_cmp.get() ));
	RG_CHECK( DrawMesh1Test< IndirectCtx, IndirectCtx::Transfer >( _msPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // not AE_ENABLE_METAL
