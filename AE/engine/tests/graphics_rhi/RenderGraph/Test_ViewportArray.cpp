// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Test_RenderGraph.h"

namespace
{
	struct VA1_TestData
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
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::MultiView_RTech;

	static const ShaderTypes::Vertex_viewportArray	vertices[] = {
		{ float2{ 0.0f, -0.5f}, HtmlColor::Red },
		{ float2{ 0.5f,  0.5f}, HtmlColor::Green },
		{ float2{-0.5f,  0.5f}, HtmlColor::Blue },
	};


	template <typename CtxTypes>
	static RenderCoro  VA1_DrawTask (VA1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		// upload vertices
		typename CtxTypes::Transfer		copy_ctx{ RenderCoro_Get() };

		CHECK_CE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(vertices), vertices, EStagingHeapType::Static ));

		typename CtxTypes::Graphics	ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

		ctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer )
			.ResourceBarrier( t.img, EResourceState::Invalidate, img_state );

		// draw
		{
			constexpr auto&		rtech_pass = RTech.ViewportArray;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( RectU{ uint2{0},		t.viewSize/2 })
								.AddViewport( RectU{ t.viewSize/2,	t.viewSize	})
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::White} ));

			CHECK_CE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

			DrawCmd	cmd;
			cmd.vertexCount		= 3;
			cmd.instanceCount	= 2;

			dctx.BindPipeline( t.ppln );
			dctx.Draw( cmd );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  VA1_CopyTask (VA1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<VA1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  ViewportArrayTest (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		VA1_TestData	t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		t.vb = res_mngr.CreateBuffer( BufferDesc{ Sizeof(vertices), EBufferUsage::TransferDst | EBufferUsage::Vertex }.SetMemory( EMemoryType::DeviceLocal ),
									  "vertex buffer", t.gfxAlloc );
		CHECK_ERR( t.vb );

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.ViewportArray.viewportArray );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"ViewportArray"} );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Run( VA1_DrawTask<CtxTypes>(t), Tuple{},						{"Draw task"} );
		AsyncTask	task2	= t.batch->Run( VA1_CopyTask<CopyCtx>(t),  Tuple{task1}, True{"Last"},	{"Readback task"} );

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


RGTest::ECode  RGTest::Test_ViewportArray ()
{
	if ( not _mvPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( ViewportArrayTest< DirectCtx,   DirectCtx::Transfer   >( _mvPipelines, img_cmp.get() ));
	RG_CHECK( ViewportArrayTest< DirectCtx,   IndirectCtx::Transfer >( _mvPipelines, img_cmp.get() ));

	RG_CHECK( ViewportArrayTest< IndirectCtx, DirectCtx::Transfer   >( _mvPipelines, img_cmp.get() ));
	RG_CHECK( ViewportArrayTest< IndirectCtx, IndirectCtx::Transfer >( _mvPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
