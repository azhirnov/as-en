// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

#ifdef AE_ENABLE_VULKAN
# include "vk_types.h"
#endif
#ifdef AE_ENABLE_METAL
# include "mtl_types.h"
#endif

namespace
{
	struct DA1_TestData
	{
		SharedMutex					guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;

		Strong<BufferID>			vb;
		
		GraphicsPipelineID			ppln;

		AsyncTask					result;

		CommandBatchPtr			batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};

	static const Vertex_draw2	vertices[] = {
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
	class DA1_DrawTask final : public DrawTask
	{
	public:
		DA1_TestData&	t;
		const uint		firstVertex;
		
		DA1_DrawTask (DA1_TestData& t, uint first, RC<DrawCommandBatch> batch, StringView dbgName) :
			DrawTask{ batch, dbgName },
			t{ t },
			firstVertex{ first }
		{}

		void  Run () override
		{
			DeferSharedLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			typename CtxTypes::Draw		dctx{ *this };
			CHECK_TE( dctx.IsValid() );
			
			CHECK_TE( dctx.BindVertexBuffer( t.ppln, VertexBufferName{"vb"}, t.vb, 0_b ));

			dctx.BindPipeline( t.ppln );
			
			DrawCmd	cmd;
			cmd.vertexCount = 3;
			cmd.firstVertex	= firstVertex;
			dctx.Draw( cmd );

			Execute( dctx );
		}
	};

	
	template <typename CtxTypes>
	class DA1_RenderPassTask final : public RenderTask
	{
	public:
		DA1_TestData&			t;
		RC<DrawCommandBatch>	drawBatch;

		DA1_RenderPassTask (DA1_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			const auto	rp_desc = RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( AttachmentName{"Color"}, t.view, RGBA32f{HtmlColor::White} );

			if ( not drawBatch )
			{
				drawBatch = RenderTaskScheduler().BeginAsyncDraw( rp_desc, "DrawTest.Draw_1" );
				CHECK_TE( drawBatch );

				StaticArray< AsyncTask, 4 >	draw_tasks = {
					drawBatch->Add< DA1_DrawTask<CtxTypes> >( Tuple{ArgRef(t), 0u}, Tuple{}, "draw cmd 1" ),
					drawBatch->Add< DA1_DrawTask<CtxTypes> >( Tuple{ArgRef(t), 3u}, Tuple{}, "draw cmd 2" ),
					drawBatch->Add< DA1_DrawTask<CtxTypes> >( Tuple{ArgRef(t), 6u}, Tuple{}, "draw cmd 3" ),
					drawBatch->Add< DA1_DrawTask<CtxTypes> >( Tuple{ArgRef(t), 9u}, Tuple{}, "draw cmd 4" )
				};
				CHECK( Continue( Tuple{ Threading::StrongDepArray{draw_tasks} }));
			}
			else
			{
				const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

				DeferExLock	lock {t.guard};
				CHECK_TE( lock.try_lock() );

				// upload vertices
				typename CtxTypes::Transfer		copy_ctx{ *this };
				CHECK_TE( copy_ctx.IsValid() );
			
				copy_ctx.AccumBarriers()
					.MemoryBarrier( EResourceState::Host_Write, EResourceState::CopyDst );

				CHECK_TE( copy_ctx.UploadBuffer( t.vb, 0_b, Bytes::SizeOf(vertices), vertices, EStagingHeapType::Static ));

				// begin render pass
				typename CtxTypes::Graphics	ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
				CHECK_TE( ctx.IsValid() );
			
				ctx.AccumBarriers()
					.MemoryBarrier( EResourceState::CopyDst, EResourceState::VertexBuffer )
					.MemoryBarrier( EResourceState::CopyDst, EResourceState::IndexBuffer )
					.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

				ctx.BeginMtRenderPass( rp_desc, *drawBatch );
				ctx.ExecuteSecondary( *drawBatch );
				ctx.EndMtRenderPass();

				ctx.AccumBarriers()
					.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
				Execute( ctx );
			}
		}
	};

	
	template <typename Ctx>
	class DA1_CopyTask final : public RenderTask
	{
	public:
		DA1_TestData&	t;

		DA1_CopyTask (DA1_TestData& t, CommandBatchPtr batch, StringView dbgName) :
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

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  DrawAsync1Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		DA1_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};
		
		t.vb = res_mngr.CreateBuffer( BufferDesc{ Bytes::SizeOf(vertices), EBufferUsage::TransferDst | EBufferUsage::Vertex }.SetMemory( EMemoryType::DeviceLocal ),
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
		CHECK_ERR( begin );

		t.batch	= rts.CreateBatch( EQueueType::Graphics, 0, "DrawAsync1" );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Add< DA1_RenderPassTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{begin}, "Draw task" );
		CHECK_ERR( task1 );
		
		AsyncTask	task2	= t.batch->Add< DA1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, "Readback task" );
		CHECK_ERR( task2 );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );
		CHECK_ERR( end );

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


bool RGTest::Test_DrawAsync1 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( DrawAsync1Test< DirectCtx,   DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( DrawAsync1Test< DirectCtx,   IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));

	CHECK_ERR(( DrawAsync1Test< IndirectCtx, DirectCtx::Transfer   >( _pipelines, img_cmp.get() )));
	CHECK_ERR(( DrawAsync1Test< IndirectCtx, IndirectCtx::Transfer >( _pipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
