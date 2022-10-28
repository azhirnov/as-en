// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct DM1_TestData
	{
		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;
		
		MeshPipelineID				ppln;

		AsyncTask					result;

		CommandBatchPtr			batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};


	template <typename CtxType>
	class DM1_DrawTask final : public RenderTask
	{
	public:
		DM1_TestData&	t;

		DM1_DrawTask (DM1_TestData& t, CommandBatchPtr batch, StringView dbgName) :
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
				dctx.DrawMeshTasks( uint3{1} );
				
				ctx.EndRenderPass( dctx );
			}

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};
	
	template <typename Ctx>
	class DM1_CopyTask final : public RenderTask
	{
	public:
		DM1_TestData&	t;

		DM1_CopyTask (DM1_TestData& t, CommandBatchPtr batch, StringView dbgName) :
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
	static bool  DrawTest (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		DM1_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = renderTech->GetMeshPipeline( PipelineName{"draw_mesh1"} );
		CHECK_ERR( t.ppln );

		AsyncTask	begin	= rts.BeginFrame();
		CHECK_ERR( begin );

		t.batch	= rts.CreateBatch( EQueueType::Graphics, 0, "DrawMesh1" );
		CHECK_ERR( t.batch );

		AsyncTask	task1	= t.batch->Add< DM1_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin}, "Draw task" );
		CHECK_ERR( task1 );
		
		AsyncTask	task2	= t.batch->Add< DM1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, "Readback task" );
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


bool RGTest::Test_DrawMesh1 ()
{
	if ( _msPipelines == null )
		return true; // skip

	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( DrawTest< DirectCtx,   DirectCtx::Transfer   >( _msPipelines, img_cmp.get() )));
	CHECK_ERR(( DrawTest< DirectCtx,   IndirectCtx::Transfer >( _msPipelines, img_cmp.get() )));

	CHECK_ERR(( DrawTest< IndirectCtx, DirectCtx::Transfer   >( _msPipelines, img_cmp.get() )));
	CHECK_ERR(( DrawTest< IndirectCtx, IndirectCtx::Transfer >( _msPipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
