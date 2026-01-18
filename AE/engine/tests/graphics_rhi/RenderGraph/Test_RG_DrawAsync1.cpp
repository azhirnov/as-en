// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct DA1_TestData
	{
		SharedMutex					guard;

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

	static constexpr auto&	RTech = RenderTechs::DrawTestRT;

	static const ShaderTypes::Vertex_draw2	vertices[] = {
		{ float2{-1.0f, -1.0f}, HtmlColor::Red },
		{ float2{-0.5f, -1.0f}, HtmlColor::Red },
		{ float2{-1.0f,  0.5f}, HtmlColor::Red },

		{ float2{-0.5f, -1.0f}, HtmlColor::Green },
		{ float2{ 0.0f,  0.0f}, HtmlColor::Green },
		{ float2{-1.0f,  0.5f}, HtmlColor::Green },

		{ float2{ 1.0f,  1.0f}, HtmlColor::Blue },
		{ float2{ 0.5f,  1.0f}, HtmlColor::Blue },
		{ float2{ 1.0f,  0.5f}, HtmlColor::Blue },

		{ float2{ 0.5f,  1.0f}, HtmlColor::Yellow },
		{ float2{ 0.0f,  0.0f}, HtmlColor::Yellow },
		{ float2{ 1.0f,  0.5f}, HtmlColor::Yellow }
	};


	template <typename CtxTypes>
	static DrawCoro  DA1_DrawTask (DA1_TestData& t, const uint firstVertex)
	{
		DeferSharedLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Draw		dctx{ DrawCoro_Get() };

		CHECK_CE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

		dctx.BindPipeline( t.ppln );

		DrawCmd	cmd;
		cmd.vertexCount = 3;
		cmd.firstVertex	= firstVertex;
		dctx.Draw( cmd );

		DrawCoro_Execute( dctx );
	}


	template <typename CtxTypes>
	static RenderCoro  DA1_RenderPassTask (DA1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

		constexpr auto&		rtech_pass = RTech.Draw_1;
		StaticAssert( rtech_pass.attachmentsCount == 1 );

		const auto	rp_desc = RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( t.viewSize )
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::White} );

		// upload vertices
		typename CtxTypes::Transfer		copy_ctx{ RenderCoro_Get() };

		CHECK_CE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(vertices), vertices, EStagingHeapType::Static ));

		RC<DrawCommandBatch>				draw_batch;
		typename CtxTypes::CommandBuffer	cmdbuf;

		// begin render pass
		{
			typename CtxTypes::Graphics	ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer )
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			draw_batch	= ctx.BeginMtRenderPass( rp_desc, {"DrawTest.Draw_1"} );
			cmdbuf		= ctx.ReleaseCommandBuffer();
			lock.unlock();

			StaticArray< AsyncTask, 4 >	draw_tasks = {
				draw_batch->Run( DA1_DrawTask<CtxTypes>( t, 0u ), Tuple{}, {"draw cmd 1"} ),
				draw_batch->Run( DA1_DrawTask<CtxTypes>( t, 3u ), Tuple{}, {"draw cmd 2"} ),
				draw_batch->Run( DA1_DrawTask<CtxTypes>( t, 6u ), Tuple{}, {"draw cmd 3"} ),
				draw_batch->Run( DA1_DrawTask<CtxTypes>( t, 9u ), Tuple{}, {"draw cmd 4"} )
			};

			draw_batch->EndRecording();	// optional

			co_await Tuple{Threading::StrongDepArray{draw_tasks}};
		}

		CHECK_CE( lock.try_lock() );

		// continue render pass
		{
			typename CtxTypes::Graphics	ctx{ RenderCoro_Get(), *draw_batch, RVRef(cmdbuf) };

			ctx.ExecuteSecondary( *draw_batch );

			// TODO
			//draw_batch = ctx.NextMtSubpass( *draw_batch );

			ctx.EndMtRenderPass();

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );

			RenderCoro_Execute( ctx );
		}
	}


	template <typename Ctx>
	static RenderCoro  DA1_CopyTask (DA1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<DA1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  DrawAsync1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		DA1_TestData	t;

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

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.draw2 );
		CHECK_ERR( t.ppln );


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"DrawAsync1"} );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Run( DA1_RenderPassTask<CtxTypes>(t), Tuple{},					 {"Draw task"} );
		AsyncTask	task2	= t.batch->Run( DA1_CopyTask<CopyCtx>(t),		 Tuple{task1}, True{"Last"}, {"Readback task"} );

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


bool RGTest::Test_DrawAsync1 ()
{
	#ifdef AE_ENABLE_REMOTE_GRAPHICS
		return true;	// skip
	#endif

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( DrawAsync1Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
	RG_CHECK( DrawAsync1Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

	RG_CHECK( DrawAsync1Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() ));
	RG_CHECK( DrawAsync1Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
