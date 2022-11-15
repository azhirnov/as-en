// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct D1_TestData
	{
		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;
		
		GraphicsPipelineID			ppln;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};


	template <typename CtxType>
	class D1_DrawTask final : public RenderTask
	{
	public:
		D1_TestData&	t;

		D1_DrawTask (D1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
			RenderTask{ batch, dbgName, dbgColor },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

			typename CtxType::Graphics	ctx{ *this };
			
			ctx.AccumBarriers()
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			// draw
			{
				auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( AttachmentName{"Color"}, t.view, RGBA32f{HtmlColor::Black} ));

				dctx.BindPipeline( t.ppln );
				dctx.Draw( 3 );
				
				ctx.EndRenderPass( dctx );
			}

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};
	
	template <typename Ctx>
	class D1_CopyTask final : public RenderTask
	{
	public:
		D1_TestData&	t;

		D1_CopyTask (D1_TestData& t, CommandBatchPtr batch, StringView dbgName, RGBA8u dbgColor) :
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
	static bool  Draw1Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		D1_TestData		t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = renderTech->GetGraphicsPipeline( PipelineName{"draw1"} );
		CHECK_ERR( t.ppln );

		AsyncTask	begin	= rts.BeginFrame();

		auto		batch	= rts.CreateBatch( EQueueType::Graphics, 0, "Draw1" );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< D1_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin}, "Draw task" );
		AsyncTask	task2	= batch->Add< D1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, "Readback task" );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.view, t.img ));

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_Draw1 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( Draw1Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( Draw1Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));

	CHECK_ERR(( Draw1Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( Draw1Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
