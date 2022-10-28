// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if 0
#include "Test_RenderGraph.h"

namespace
{
	struct D3_TestData
	{
		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;
		
		GraphicsPipelineID			ppln;

		AsyncTask					result;

		CommandBatchPtr			batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};
	
	static const float4		vertices[] = {
		float4{ 0.0f, -0.5f, 0.f, 1.f},
		float4{ 0.5f,  0.5f, 0.f, 1.f},
		float4{-0.5f,  0.5f, 0.f, 1.f},
	};


	template <typename CtxType>
	class D3_DrawTask final : public RenderTask
	{
	public:
		D3_TestData&	t;

		D3_DrawTask (D3_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

			typename CtxType::Graphics	ctx{ *this };
			CHECK_TE( ctx.IsValid() );
			
			ctx.AccumBarriers()
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			// draw
			{
				auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( AttachmentName{"Color"}, t.view, RGBA32f{HtmlColor::Black} ));
				
				CHECK_TE( dctx.IsValid() );

				dctx.BindPipeline( t.ppln );
				dctx.PushConstant( 0_b, Bytes::SizeOf(vertices), vertices, EShaderStages::Vertex );
				dctx.Draw( 3 );
				
				ctx.EndRenderPass( dctx );
			}

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};
	
	template <typename Ctx>
	class D3_CopyTask final : public RenderTask
	{
	public:
		D3_TestData&	t;

		D3_CopyTask (D3_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };
			CHECK_TE( ctx.IsValid() );

			t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
						.Then( [p = &t] (const ImageMemView &view)
								{
									p->isOK = p->imgCmp->Compare( view );
								})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};

	
	template <typename CtxType, typename CopyCtx>
	static bool  Draw3Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		D3_TestData		t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = renderTech->GetGraphicsPipeline( PipelineName{"draw3"} );
		CHECK_ERR( t.ppln );

		AsyncTask	begin	= rts.BeginFrame();
		CHECK_ERR( begin );

		auto	batch = rts.CreateBatch( EQueueType::Graphics, 0, "Draw3" );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< D3_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin}, "Draw task" );
		CHECK_ERR( task1 );
		
		AsyncTask	task2	= batch->Add< D3_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, "Readback task" );
		CHECK_ERR( task2 );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );
		CHECK_ERR( end );

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


bool RGTest::Test_Draw3 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( Draw3Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( Draw3Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));

	CHECK_ERR(( Draw3Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( Draw3Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}

#endif
