// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "DrawTestCore.h"

namespace
{
	struct CR1_TestData
	{
		Canvas&						canvas;
		RenderTechPipelinesPtr		rtech;

		Mutex						guard;

		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		GraphicsPipelineID			ppln_tristrip;
		GraphicsPipelineID			ppln_trilist;
		GraphicsPipelineID			ppln_trilist_lines;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;

		CR1_TestData (Canvas &c) : canvas{c} {}
	};

	static constexpr auto&	RTech		= RenderTechs::CanvasDrawTest;
	static constexpr auto&	DeskRTech	= RenderTechs::DesktopCanvasDrawTest;


	template <typename CtxType>
	class CR1_DrawTask final : public RenderTask
	{
	public:
		CR1_TestData&	t;

		CR1_DrawTask (CR1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ batch, dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			typename CtxType::Graphics	ctx{ *this };

			t.canvas.NextFrame( GetFrameId() );

			// draw
			{
				constexpr auto&		rtech_pass = RTech.Draw_1;
				StaticAssert( rtech_pass.attachmentsCount == 1 );
				StaticAssert( RenderTechPassName{rtech_pass} == DeskRTech.Draw_1 );

				auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
										.AddViewport( t.viewSize )
										.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black}, EResourceState::Invalidate, EResourceState::CopySrc ));
				{
					t.canvas.Draw( Rectangle2DStrip{ RectF{-0.9f, -0.9f, -0.5f, -0.5f}, HtmlColor::BlueViolet });
					t.canvas.Draw( Rectangle2DStrip{ RectF{-0.9f, -0.4f, -0.5f,  0.0f}, HtmlColor::Green });
					t.canvas.Draw( Rectangle2DStrip{ RectF{-0.9f,  0.1f, -0.5f,  0.5f}, HtmlColor::Red });

					dctx.BindPipeline( t.ppln_tristrip );
					t.canvas.Flush( dctx, EPrimitive::TriangleStrip );
				}
				{
					t.canvas.Draw( Rectangle2D{ RectF{0.5f, -0.9f, 0.9f, -0.5f}, HtmlColor::Blue });
					t.canvas.Draw( FilledCircle2D{ 16, RectF{0.5f, -0.4f, 0.9f, 0.0f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::Red });

					t.canvas.Draw( NinePatch2D{ RectF{-0.4f, -0.9f, 0.4f, -0.1f},	RectF{0.2f, 0.2f, 0.2f, 0.2f},
												RectF{0.f, 0.f, 1.f, 1.f},			RectF{0.25f, 0.25f, 0.25f, 0.25f},
												HtmlColor::Orange });

					dctx.BindPipeline( t.ppln_trilist );
					t.canvas.Flush( dctx, EPrimitive::TriangleList );
				}

				// desktop only
				if ( t.ppln_trilist_lines )
				{
					t.canvas.Draw( FilledCircle2D{ 16, RectF{0.5f, 0.5f, 0.9f, 0.9f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::Red });
					t.canvas.Draw( NinePatch2D{ RectF{-0.4f, 0.1f, 0.4f, 0.9f},		RectF{0.2f, 0.2f, 0.2f, 0.2f},
												RectF{0.f, 0.f, 1.f, 1.f},			RectF{0.25f, 0.25f, 0.25f, 0.25f},
												HtmlColor::Orange });

					dctx.BindPipeline( t.ppln_trilist_lines );
					t.canvas.Flush( dctx, EPrimitive::TriangleList );
				}

				ctx.EndRenderPass( dctx );
			}

			Execute( ctx );
		}
	};

	template <typename Ctx>
	class CR1_CopyTask final : public RenderTask
	{
	public:
		CR1_TestData&	t;

		CR1_CopyTask (CR1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ batch, dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			Ctx		ctx{ *this };

			t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
						.Then(	[p = &t] (const ImageMemView &view)
								{
									p->isOK = p->imgCmp->Compare( view );
								})};

			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			Execute( ctx );
		}
	};


	template <typename CtxType, typename CopyCtx>
	static bool  CanvasRect (Canvas* canvas, RenderTechPipelinesPtr renderTech, RenderTechPipelinesPtr desktopRenderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		CR1_TestData	t			{*canvas};

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.rtech = renderTech;

		t.ppln_tristrip = renderTech->GetGraphicsPipeline( RTech.Draw_1.rect_tristrip );
		CHECK_ERR( t.ppln_tristrip );

		t.ppln_trilist = renderTech->GetGraphicsPipeline( RTech.Draw_1.rect_trilist );
		CHECK_ERR( t.ppln_trilist );

		if ( desktopRenderTech )
			t.ppln_trilist_lines = desktopRenderTech->GetGraphicsPipeline( DeskRTech.Draw_1.rect_trilist_lines );  // optional


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Canvas batch"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run< CR1_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{},					 {"Draw task"} );
		AsyncTask	task2	= batch->Run< CR1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool DrawTestCore::Test_Canvas_Rect ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( CanvasRect< DirectCtx,   DirectCtx::Transfer   >( _canvas.get(), _canvasPpln, _canvasPplnDesk, img_cmp.get() ));
	RG_CHECK( CanvasRect< DirectCtx,   IndirectCtx::Transfer >( _canvas.get(), _canvasPpln, _canvasPplnDesk, img_cmp.get() ));

	RG_CHECK( CanvasRect< IndirectCtx, DirectCtx::Transfer   >( _canvas.get(), _canvasPpln, _canvasPplnDesk, img_cmp.get() ));
	RG_CHECK( CanvasRect< IndirectCtx, IndirectCtx::Transfer >( _canvas.get(), _canvasPpln, _canvasPplnDesk, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
