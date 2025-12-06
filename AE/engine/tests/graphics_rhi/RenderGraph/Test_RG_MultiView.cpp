// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct MV1_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		GAutorelease<BufferID>		vb;

		GraphicsPipelineID			ppln;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK		= false;

		ImageComparator *			imgCmp0		= null;
		ImageComparator *			imgCmp1		= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::MultiView_RTech;

	static const ShaderTypes::Vertex_multiview	vertices[] = {
		{ float2{ 0.0f, -0.5f}, HtmlColor::Red },
		{ float2{ 0.5f,  0.5f}, HtmlColor::Green },
		{ float2{-0.5f,  0.5f}, HtmlColor::Blue },
	};


	template <typename CtxTypes>
	static RenderCoro  MV1_DrawTask (MV1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		// upload vertices
		typename CtxTypes::Transfer		copy_ctx{ RenderCoro_Get() };

		CHECK_CE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(vertices), vertices, EStagingHeapType::Static ));

		typename CtxTypes::Graphics		ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

		ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer )
			.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

		// draw
		{
			constexpr auto&		rtech_pass = RTech.MultiView;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( t.viewSize )
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::White} ));

			CHECK_CE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

			dctx.BindPipeline( t.ppln );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ImageBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  MV1_CopyTask (MV1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		auto	task = ctx.ReadbackImage( t.img, ReadbackImageDesc{}.ArrayLayer( 0 )).Then( t,
							[] (Promise<ImageMemView> readRes, CoSafe<MV1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp0->Compare( view );
							});

		t.result = ctx.ReadbackImage( t.img, ReadbackImageDesc{}.ArrayLayer( 1 )).Then( t,
							[] (Promise<ImageMemView> readRes, CoSafe<MV1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp1->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  MultiViewTest (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp0, ImageComparator* imageCmp1)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		MV1_TestData	t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp0	= imageCmp0;
		t.imgCmp1	= imageCmp1;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		t.vb = res_mngr.CreateBuffer( BufferDesc{ Sizeof(vertices), EBufferUsage::TransferDst | EBufferUsage::Vertex }.SetMemory( EMemoryType::DeviceLocal ),
									  "vertex buffer", t.gfxAlloc );
		CHECK_ERR( t.vb );

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetArrayLayers( 2 ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.MultiView.multiview );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"MultiView"} );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Run( MV1_DrawTask<CtxTypes>(t), Tuple{},						{"Draw task"} );
		AsyncTask	task2	= t.batch->Run( MV1_CopyTask<CopyCtx>(t),  Tuple{task1}, True{"Last"},	{"Readback task"} );

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


bool RGTest::Test_MultiView ()
{
	if ( not _mvPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return true;
	}

	auto	img_cmp0 = _LoadReference( TEST_NAME << "-0" );
	auto	img_cmp1 = _LoadReference( TEST_NAME << "-1" );
	bool	result	= true;

	RG_CHECK( MultiViewTest< DirectCtx,   DirectCtx::Transfer   >( _mvPipelines, img_cmp0.get(), img_cmp1.get() ));
	RG_CHECK( MultiViewTest< DirectCtx,   IndirectCtx::Transfer >( _mvPipelines, img_cmp0.get(), img_cmp1.get() ));

	RG_CHECK( MultiViewTest< IndirectCtx, DirectCtx::Transfer   >( _mvPipelines, img_cmp0.get(), img_cmp1.get() ));
	RG_CHECK( MultiViewTest< IndirectCtx, IndirectCtx::Transfer >( _mvPipelines, img_cmp0.get(), img_cmp1.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
