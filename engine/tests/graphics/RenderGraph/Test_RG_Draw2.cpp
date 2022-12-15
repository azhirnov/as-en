// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct D2_TestData
	{
		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;

		Strong<BufferID>			vb;
		
		GraphicsPipelineID			ppln;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};

	static const Vertex_draw2	vertices[] = {
		{ float2{ 0.0f, -0.5f}, HtmlColor::Red },
		{ float2{ 0.5f,  0.5f}, HtmlColor::Green },
		{ float2{-0.5f,  0.5f}, HtmlColor::Blue },
	};

	
	template <typename CtxTypes>
	class D2_DrawTask final : public RenderTask
	{
	public:
		D2_TestData&	t;

		D2_DrawTask (D2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

			// upload vertices
			typename CtxTypes::Transfer		copy_ctx{ *this };
			
			copy_ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::Host_Write, EResourceState::CopyDst );

			CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Sizeof(vertices), vertices, EStagingHeapType::Static ));

			typename CtxTypes::Graphics	ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
			
			ctx.AccumBarriers()
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
				.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer )
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			// draw
			{
				auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( AttachmentName{"Color"}, t.view, RGBA32f{HtmlColor::White} ));

				CHECK_TE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

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
	class D2_CopyTask final : public RenderTask
	{
	public:
		D2_TestData&	t;

		D2_CopyTask (D2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
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
			
			Execute( ctx );
		}
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  Draw2Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		D2_TestData		t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};
		
		t.vb = res_mngr.CreateBuffer( BufferDesc{ Sizeof(vertices), EBufferUsage::TransferDst | EBufferUsage::Vertex }.SetMemory( EMemoryType::DeviceLocal ),
									  "vertex buffer", t.gfxAlloc );
		CHECK_ERR( t.vb );

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = renderTech->GetGraphicsPipeline( PipelineName{"draw2"} );
		CHECK_ERR( t.ppln );

		AsyncTask	begin	= rts.BeginFrame();

		t.batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, ESubmitMode::Immediately, {"Draw2"} );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Add< D2_DrawTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{begin},				{"Draw task"} );
		AsyncTask	task2	= t.batch->Add< D2_CopyTask<CopyCtx>  >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"},	{"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.view, t.img, t.vb ));

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_Draw2 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( Draw2Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( Draw2Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));

	CHECK_ERR(( Draw2Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( Draw2Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
