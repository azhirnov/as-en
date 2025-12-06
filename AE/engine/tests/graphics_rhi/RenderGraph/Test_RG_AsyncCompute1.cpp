// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Async compute without synchronizations between frames,
	used double buffering for render targets to avoid race condition.

  .------------------------.
  |        frame 1         |
  |             .--------------------------.
  |             |          frame 2         |
  |-------------|------------.-------------|
  |  graphics1 \|  graphics2  \            |
  |-------------\--------------\-----------|
  |              \ compute1 |   \ compute2 |
  '---------------'---------'----'---------'
*/

#include "Test_RenderGraph.h"

namespace
{
	struct AC1_TestData
	{
		Mutex							guard;

		RenderTechPipelinesPtr			rtech;

		// shared
		GAutorelease<ImageID>			image [2];
		GAutorelease<ImageViewID>		view  [2];
		const uint2						imageDim	{800, 600};
		Atomic<uint>					frameIdx	{0};

		// graphics
		GraphicsPipelineID				gppln;

		// async compute
		ComputePipelineID				cppln;
		GAutorelease<DescriptorSetID>	cpplnDS [2];
		DescSetBinding					cpplnDSIndex;

		AsyncTask						result [2];
		bool							isOK   [2] = {false, false};

		ImageComparator *				imgCmp	= null;
		GfxMemAllocatorPtr				gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::AsyncCompTestRT;

	const auto	img_gfx_state	= EResourceState::ShaderSample | EResourceState::FragmentShader;
	const auto	img_comp_state	= EResourceState::ShaderStorage_RW | EResourceState::ComputeShader;


	template <typename CtxTypes>
	static RenderCoro  AC1_GraphicsTask (AC1_TestData& t, uint frameIdx)
	{
		const uint	fi = frameIdx & 1;
		CHECK( RenderCoro_Get().QueueType() == EQueueType::Graphics );

		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Graphics		ctx{ RenderCoro_Get() };

		ctx.AccumBarriers()
			.ImageBarrier( t.image[fi], EResourceState::Invalidate, img_gfx_state );

		// draw
		{
			constexpr auto&		rtech_pass = RTech.Draw_1;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.imageDim }
								.AddViewport( t.imageDim )
								.AddTarget( rtech_pass.att_Color, t.view[fi], RGBA32f{1.0f} ));

			dctx.BindPipeline( t.gppln );
			dctx.Draw( 3 );

			ctx.EndRenderPass( dctx );
		}

		ctx.AccumBarriers()
			.ImageBarrier( t.image[fi], img_gfx_state, img_comp_state );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes>
	static RenderCoro  AC1_ComputeTask (AC1_TestData& t, const uint frameIdx)
	{
		const uint fi = frameIdx & 1;
		CHECK( RenderCoro_Get().QueueType() == EQueueType::AsyncCompute );

		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Compute	ctx{ RenderCoro_Get() };

		ctx.BindPipeline( t.cppln );
		ctx.BindDescriptorSet( t.cpplnDSIndex, t.cpplnDS[fi] );
		ctx.Dispatch( DivCeil( t.imageDim, 4u ));

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  AC1_CopyTask (AC1_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		ctx.AccumBarriers()
			.ImageBarrier( t.image[0], img_comp_state, EResourceState::CopySrc )
			.ImageBarrier( t.image[1], img_comp_state, EResourceState::CopySrc );

		ReadbackImageDesc	readback;
		readback.heapType = EStagingHeapType::Dynamic;

		auto	read1_res = ctx.ReadbackImage( t.image[0], readback );
		auto	read2_res = ctx.ReadbackImage( t.image[1], readback );
		CHECK( read1_res.IsCompleted() and read2_res.IsCompleted() );

		t.result[0] = read1_res.Then( t,
							[](Promise<ImageMemView> readRes, CoSafe<AC1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK[0] = t->imgCmp->Compare( view );
							});

		t.result[1] = read2_res.Then( t,
							[](Promise<ImageMemView> readRes, CoSafe<AC1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK[1] = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	AsyncCoro  AC1_FrameTask (AC1_TestData& t)
	{
		auto&	rts = GraphicsScheduler();

		CommandBatchPtr		last_batch;

		for (; t.frameIdx.load() < 3;)
		{
			// frames [0..2]:
			CHECK_CE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rts.BeginFrame() );

			auto		batch_gfx	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"graphics batch"} );
			CHECK_CE( batch_gfx );

			// sync with previous frame
			CHECK_CE( batch_gfx->AddInputDependency( last_batch ));

			auto		batch_ac	= rts.BeginCmdBatch( EQueueType::AsyncCompute, 0, {"compute batch"} );
			CHECK_CE( batch_ac );

			// graphics to compute sync
			CHECK_CE( batch_ac->AddInputDependency( batch_gfx ));

			AsyncTask	gfx_task	= batch_gfx->Run( AC1_GraphicsTask<CtxTypes>( t, t.frameIdx.load() ),	Tuple{},		 True{"Last"}, {"graphics task"} );
			AsyncTask	comp_task	= batch_ac ->Run( AC1_ComputeTask<CtxTypes>( t, t.frameIdx.load() ),	Tuple{gfx_task}, True{"Last"}, {"async compute task"} );
			AsyncTask	end			= rts.EndFrame( Tuple{ gfx_task, comp_task });

			last_batch = batch_ac;

			++t.frameIdx;
			Coro_Continue( end );
		}

		// frame 3
		{
			CHECK( t.frameIdx.load() == 3 );

			CHECK_CE( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rts.BeginFrame() );

			auto		batch = rts.BeginCmdBatch( EQueueType::Graphics, 0, {"copy task"} );
			CHECK_CE( batch );

			CHECK_CE( batch->AddInputDependency( last_batch ));

			AsyncTask	read_task	= batch->Run( AC1_CopyTask<CopyCtx>( t ), Tuple{}, True{"Last"}, {"Readback task"} );
			AsyncTask	end			= rts.EndFrame( Tuple{read_task} );

			++t.frameIdx;
			Coro_Continue( end );
		}

		co_return;
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  AsyncCompute1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		AC1_TestData	t;
		const auto		format		= EPixelFormat::RGBA8_UNorm;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		t.image[0] = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.imageDim )
												.SetFormat( format )
												.SetQueues( EQueueMask::Graphics | EQueueMask::AsyncCompute )
												.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
										   "Image-0", t.gfxAlloc );
		t.image[1] = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.imageDim )
												.SetFormat( format )
												.SetQueues( EQueueMask::Graphics | EQueueMask::AsyncCompute )
												.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
										   "Image-1", t.gfxAlloc );
		CHECK_ERR( t.image[0] and t.image[1] );

		t.view[0] = res_mngr.CreateImageView( ImageViewDesc{}, t.image[0], "ImageView-0" );
		t.view[1] = res_mngr.CreateImageView( ImageViewDesc{}, t.image[1], "ImageView-1" );
		CHECK_ERR( t.view[0] and t.view[1] );

		t.gppln = t.rtech->GetGraphicsPipeline( RTech.Draw_1.async_comp1_graphics );
		t.cppln = t.rtech->GetComputePipeline( RTech.Compute_1.async_comp1_compute );
		CHECK_ERR( t.gppln and t.cppln );

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
		AsyncTask	task = Scheduler().Run( AC1_FrameTask<CtxTypes, CopyCtx>( t ));

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( t.frameIdx.load() == 4 );

		CHECK_ERR( Scheduler().Wait( List{t.result[0], t.result[1]}, c_MaxTimeout ));
		CHECK_ERR( t.result[0]->Status() == ETaskStatus::Completed );
		CHECK_ERR( t.result[1]->Status() == ETaskStatus::Completed );
		CHECK_ERR( t.isOK[0] );
		CHECK_ERR( t.isOK[1] );

		return true;
	}

} // namespace


bool RGTest::Test_AsyncCompute1 ()
{
	if ( not AllBits( GraphicsScheduler().GetDevice().GetAvailableQueues(), EQueueMask::Graphics | EQueueMask::AsyncCompute ))
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return true;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( AsyncCompute1Test< DirectCtx,   DirectCtx::Transfer   >( _acPipelines, img_cmp.get() ));
	RG_CHECK( AsyncCompute1Test< IndirectCtx, IndirectCtx::Transfer >( _acPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
