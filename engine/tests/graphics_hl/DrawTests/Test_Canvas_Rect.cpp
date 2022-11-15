// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "DrawTestCore.h"

namespace
{
	struct CR1_TestData
	{
		Canvas&						canvas;

		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;
		
		GraphicsPipelineID			ppln_tristrip;
		GraphicsPipelineID			ppln_trilist;
		GraphicsPipelineID			ppln_trilist_lines;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;

		CR1_TestData (Canvas &c) : canvas{c} {}
	};


	template <typename CtxType>
	class CR1_DrawTask final : public RenderTask
	{
	public:
		CR1_TestData&	t;

		CR1_DrawTask (CR1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
			RenderTask{ batch, dbgName, dbgColor },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			typename CtxType::Graphics	ctx{ *this };
			
			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::Host_Write, EResourceState::VertexBuffer )
				.MemoryBarrier( EResourceState::Host_Write, EResourceState::IndexBuffer );

			t.canvas.NextFrame( GetFrameId() );

			// draw
			{
				const RenderPassDesc	rp_desc = RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.viewSize }
													.AddViewport( t.viewSize )
													.AddTarget( AttachmentName{"Color"}, t.view, RGBA32f{HtmlColor::Black}, EResourceState::Invalidate, EResourceState::CopySrc );

				auto	dctx = ctx.BeginRenderPass( rp_desc );

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
				{
					t.canvas.Draw( FilledCircle2D{ 16, RectF{0.5f, 0.5f, 0.9f, 0.9f}, RectF{0.f, 0.f, 1.f, 1.f}, HtmlColor::Red });
					t.canvas.Draw( NinePatch2D{ RectF{-0.4f, 0.1f, 0.4f, 0.9f},		RectF{0.2f, 0.2f, 0.2f, 0.2f},
												RectF{0.f, 0.f, 1.f, 1.f},			RectF{0.25f, 0.25f, 0.25f, 0.25f},
												HtmlColor::Orange });

					dctx.BindPipeline( t.ppln_trilist_lines );
					t.canvas.Flush( dctx, EPrimitive::TriangleList );
				}
				ctx.EndRenderPass( dctx, rp_desc );
			}
			
			Execute( ctx );
		}
	};
	
	template <typename Ctx>
	class CR1_CopyTask final : public RenderTask
	{
	public:
		CR1_TestData&	t;

		CR1_CopyTask (CR1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
			RenderTask{ batch, dbgName, dbgColor },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };

			t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
						.Then( [p = &t] (const ImageMemView &view)
								{
									p->isOK = p->imgCmp->Compare( view );
								})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			ExecuteAndSubmit( ctx );
		}
	};

	
	template <typename CtxType, typename CopyCtx>
	static bool  CanvasRect (Canvas* canvas, RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		CR1_TestData	t			{*canvas};
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln_tristrip = renderTech->GetGraphicsPipeline( PipelineName{"rect_tristrip"} );
		CHECK_ERR( t.ppln_tristrip );

		t.ppln_trilist = renderTech->GetGraphicsPipeline( PipelineName{"rect_trilist"} );
		CHECK_ERR( t.ppln_trilist );

		t.ppln_trilist_lines = renderTech->GetGraphicsPipeline( PipelineName{"rect_trilist_lines"} );
		CHECK_ERR( t.ppln_trilist_lines );

		AsyncTask	begin	= rts.BeginFrame();

		auto		batch	= rts.CreateBatch( EQueueType::Graphics, 0, "Canvas batch" );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< CR1_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin}, "Draw task" );
		AsyncTask	task2	= batch->Add< CR1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, "Readback task" );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );

		CHECK_ERR( res_mngr.ReleaseResources( t.view, t.img ));
		return true;
	}

} // namespace


bool DrawTestCore::Test_Canvas_Rect ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( CanvasRect< DirectCtx,   DirectCtx::Transfer   >( _canvas.get(), _canvasPpln, img_cmp.get() )));
	CHECK_ERR(( CanvasRect< DirectCtx,   IndirectCtx::Transfer >( _canvas.get(), _canvasPpln, img_cmp.get() )));

	CHECK_ERR(( CanvasRect< IndirectCtx, DirectCtx::Transfer   >( _canvas.get(), _canvasPpln, img_cmp.get() )));
	CHECK_ERR(( CanvasRect< IndirectCtx, IndirectCtx::Transfer >( _canvas.get(), _canvasPpln, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
