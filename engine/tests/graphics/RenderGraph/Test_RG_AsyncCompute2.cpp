// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Async compute without synchronizations between frames,
	used double buffering for render targets to avoid race condition.

  .------------------------.
  |      frame 1           |
  |             .----------|-------------.
  |             |        frame 2         |
  |-------------|----------|--.----------|
  |  graphics1  |  graphics2  |          |
  |-------------|----------|--|----------|
  |             | compute1 |  | compute2 |
  '-------------'----------'--'----------'
*/

#if 0
#include "Test_RenderGraph.h"

namespace
{
	struct AC2_TestData
	{
		Mutex						guard;

		// shared
		Strong<ImageID>				image [2];
		Strong<ImageViewID>			view  [2];
		const uint2					imageSize	{800, 600};
		Atomic<uint>				frameIdx	{0};

		// graphics
		GraphicsPipelineID			gppln;

		// async compute
		ComputePipelineID			cppln;
		Strong<DescriptorSetID>		cpplnDS [2];
		uint						cpplnDSIndex	= UMax;
		
		AsyncTask					result [2];
		bool						isOK   [2] = {false, false};
		
		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
		RenderTechPipelinesPtr		renderTech;

		RenderGraph					rg;
	};

	constexpr auto	img_gfx_state	= EResourceState::ShaderSample | EResourceState::FragmentShader;
	constexpr auto	img_comp_state	= EResourceState::ShaderStorage_RW | EResourceState::ComputeShader;


	template <typename CtxTypes>
	class AC2_GraphicsTask final : public RenderTask
	{
	public:
		AC2_TestData&	t;

		AC2_GraphicsTask (AC2_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{
			CHECK( batch->GetQueueType() == EQueueType::Graphics );
		}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			const uint	fi = t.frameIdx.load() & 1;

			typename CtxTypes::Graphics		ctx{ *this };
			CHECK_TE( ctx.IsValid() );

			ctx.AcquireResources();
			ctx.CommitBarriers();

			// draw
			{
				auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.imageSize }
										.AddViewport( t.imageSize )
										.AddTarget( AttachmentName{"Color"}, t.view[fi], RGBA32f{1.0f} ));
				CHECK_TE( dctx.IsValid() );

				dctx.BindPipeline( t.gppln );
				dctx.Draw( 3 );
				
				ctx.EndRenderPass( dctx );
			}
			
			ctx.ReleaseResources();
			ctx.CommitBarriers();
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};


	template <typename CtxTypes>
	class AC2_ComputeTask final : public RenderTask
	{
	public:
		AC2_TestData&	t;

		AC2_ComputeTask (AC2_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{
			CHECK( batch->GetQueueType() == EQueueType::AsyncCompute );
		}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			const uint	fi = t.frameIdx.load() & 1;

			typename CtxTypes::Compute	ctx{ *this };
			CHECK_TE( ctx.IsValid() );
			
			ctx.AcquireResources();
			ctx.CommitBarriers();

			ctx.BindPipeline( t.cppln );
			ctx.BindDescriptorSet( t.cpplnDSIndex, t.cpplnDS[fi] );
			ctx.Dispatch( DivCeil( t.imageSize, 4u ));
			
			ctx.ReleaseResources();
			ctx.CommitBarriers();

			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};
	

	template <typename Ctx>
	class CopyTask final : public RenderTask
	{
	public:
		AC2_TestData&	t;

		CopyTask (AC2_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };
			CHECK_TE( ctx.IsValid() );
			
			ctx.AcquireResources();
			ctx.CommitBarriers();

			ctx.ResourceUsage( t.image[0], EResourceState::CopySrc );
			ctx.ResourceUsage( t.image[1], EResourceState::CopySrc );
			ctx.CommitBarriers();

			ReadbackImageDesc	readback;
			readback.heapType = EStagingHeapType::Dynamic;

			t.result[0] = AsyncTask{ ctx.ReadbackImage( t.image[0], readback )
							.Then( [p = &t] (const ImageMemView &view)
									{
										p->isOK[0] = p->imgCmp->Compare( view );
									})};
			t.result[1] = AsyncTask{ ctx.ReadbackImage( t.image[1], readback )
							.Then( [p = &t] (const ImageMemView &view)
									{
										p->isOK[1] = p->imgCmp->Compare( view );
									})};

			ctx.ReleaseResources();
			ctx.CommitBarriers();
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};
	
	
	template <typename CtxTypes, typename CopyCtx>
	class AC2_FrameTask final : public Threading::IAsyncTask
	{
	public:
		AC2_TestData&	t;

		AC2_FrameTask (AC2_TestData& t) :
			IAsyncTask{ EThread::Worker },
			t{ t }
		{}

		void  Run () override
		{
			if ( t.frameIdx.load() == 3 )
			{
				AsyncTask	begin = t.rg.BeginFrame();
				CHECK_TE( begin );

				auto	batch = t.rg.CreateBatch( EQueueType::Graphics, "copy task" );
				CHECK_TE( batch );

				batch->AcquireResource( t.image[0], img_comp_state );
				batch->AcquireResource( t.image[1], img_comp_state );
				//batch->ReadbackDeviceData();
				
				AsyncTask	read_task = batch->Add<CopyTask<CopyCtx>>( Tuple{ArgRef(t)}, Tuple{begin}, "Readback task" );
				CHECK_TE( read_task );

				AsyncTask	end = t.rg.EndFrame( Tuple{read_task} );
				CHECK_TE( end );
				
				++t.frameIdx;
				CHECK_TE( Continue( Tuple{end} ));
				return;
			}

			if ( t.frameIdx.load() > 3 )
				return;
			
			const uint	fi = t.frameIdx.load() & 1;

			AsyncTask	begin = t.rg.BeginFrame();
			CHECK_TE( begin );
			
			CommandBatchPtr	batch_gfx;
			CommandBatchPtr	batch_ac;

			// render graph planning stage
			{
				// init graphics batch
				batch_gfx = t.rg.CreateBatch( t.renderTech, RenderTechPassName{"Draw_1"}, EQueueType::Graphics, "graphics batch" );
				CHECK_TE( batch_gfx );

				// init compute batch
				batch_ac = t.rg.CreateBatch( t.renderTech, RenderTechPassName{"Compute_1"}, EQueueType::AsyncCompute, "compute batch" );
				CHECK_TE( batch_ac );

				batch_gfx->AcquireResource( t.image[fi], EResourceState::Invalidate | img_gfx_state );
				batch_ac->AcquireResource( t.image[fi], img_comp_state );

				t.rg.BuildBatchGraph();
			}

			AsyncTask	gfx_task = batch_gfx->Add< AC2_GraphicsTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{begin}, "graphics task" );
			CHECK_TE( gfx_task );
		
			AsyncTask	comp_task = batch_ac->Add< AC2_ComputeTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{gfx_task}, "async compute task" );
			CHECK_TE( comp_task );

			AsyncTask	end = t.rg.EndFrame( Tuple{ gfx_task, comp_task });
			CHECK_TE( end );

			++t.frameIdx;
			CHECK_TE( Continue( Tuple{end} ));
		}

		StringView  DbgName () const override { return "AC2_FrameTask"; }
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  AsyncCompute2Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		AC2_TestData	t;
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		
		t.renderTech	= renderTech;
		t.gfxAlloc		= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp		= imageCmp;

		t.image[0] = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.imageSize )
												.SetFormat( format )
												.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
										   "Image-0", t.gfxAlloc );
		t.image[1] = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.imageSize )
												.SetFormat( format )
												.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
										   "Image-1", t.gfxAlloc );
		CHECK_ERR( t.image[0] and t.image[1] );

		// track resource state by render graph
		t.rg.AddResource( t.image[0], Default );
		t.rg.AddResource( t.image[1], Default );
	
		t.view[0] = res_mngr.CreateImageView( ImageViewDesc{}, t.image[0], "ImageView-0" );
		t.view[1] = res_mngr.CreateImageView( ImageViewDesc{}, t.image[1], "ImageView-1" );
		CHECK_ERR( t.view[0] and t.view[1] );
		
		t.gppln = renderTech->GetGraphicsPipeline( PipelineName{"async_comp1.graphics"} );
		CHECK_ERR( t.gppln );
		
		t.cppln = renderTech->GetComputePipeline( PipelineName{"async_comp1.compute"} );
		CHECK_ERR( t.cppln );
		
		{
			auto [ds0, idx0] = res_mngr.CreateDescriptorSet( t.cppln, DescriptorSetName{"compute2.ds1"} );
			auto [ds1, idx1] = res_mngr.CreateDescriptorSet( t.cppln, DescriptorSetName{"compute2.ds1"} );

			t.cpplnDSIndex	= idx0;
			t.cpplnDS[0]	= RVRef(ds0);
			t.cpplnDS[1]	= RVRef(ds1);
			CHECK_ERR( t.cpplnDS[0] and t.cpplnDS[1] );

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.cpplnDS[0], EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_Image"}, t.view[0] );
			
			CHECK_ERR( updater.Set( t.cpplnDS[1], EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_Image"}, t.view[1] );

			updater.Flush();
		}
		
		// draw 3 frames
		auto	task = Scheduler().Run< AC2_FrameTask<CtxTypes, CopyCtx> >( Tuple{ArgRef(t)} );
		
		CHECK_ERR( Scheduler().Wait( {task} ));
		CHECK_ERR( rts.WaitAll() );
		CHECK_ERR( t.frameIdx.load() == 4 );
		
		CHECK_ERR( Scheduler().Wait({ t.result[0], t.result[1] }));
		CHECK_ERR( t.result[0]->Status() == EStatus::Completed );
		CHECK_ERR( t.result[1]->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources(	t.view[0], t.view[1],
												t.image[0], t.image[1],
												t.cpplnDS[0], t.cpplnDS[1] ));
		
		CHECK_ERR( t.isOK[0] );
		CHECK_ERR( t.isOK[1] );
		return true;
	}

} // namespace


bool RGTest::Test_AsyncCompute2 ()
{
	if ( not AllBits( RenderTaskScheduler().GetDevice().GetAvailableQueues(), EQueueMask::Graphics | EQueueMask::AsyncCompute ))
		return true; // skip

	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( AsyncCompute2Test< DirectCtx,   DirectCtx::Transfer   >( _acPipelines, img_cmp.get() )));
	CHECK_ERR(( AsyncCompute2Test< IndirectCtx, IndirectCtx::Transfer >( _acPipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
#endif
