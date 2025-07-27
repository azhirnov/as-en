// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Async compute without synchronizations between frames,
	used double buffering for render targets to avoid race condition.

  .------------------------.
  |      frame 1           |
  |             .------------------------.
  |             |        frame 2         |
  |-------------|-------------.----------|
  |  graphics1  |  graphics2  |          |
  |-------------|-------------|----------|
  |             | compute1 |  | compute2 |
  '-------------'----------'--'----------'
*/

#include "Test_RenderGraph.h"

namespace
{
	struct AC3_TestData
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


	template <typename CtxTypes>
	static RenderCoro  AC3_GraphicsTask (AC3_TestData& t, const uint frameIdx)
	{
		const uint	fi = frameIdx & 1;
		CHECK( RenderCoro_Get().QueueType() == Graphics::EQueueType::Graphics );

		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Graphics		ctx{ RenderCoro_Get() };

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

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes>
	static RenderCoro  AC3_ComputeTask (AC3_TestData& t, const uint frameIdx)
	{
		const uint	fi = frameIdx & 1;
		CHECK( RenderCoro_Get().QueueType() == Graphics::EQueueType::AsyncCompute );

		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Compute	ctx{ RenderCoro_Get() };

		ctx.BindPipeline( t.cppln );
		ctx.BindDescriptorSet( t.cpplnDSIndex, t.cpplnDS[fi] );
		ctx.Dispatch( DivCeil( t.imageDim, 4u ));

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  AC3_CopyTask (AC3_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		ReadbackImageDesc	readback;
		readback.heapType = EStagingHeapType::Dynamic;
		
		t.result[0] = ctx.ReadbackImage( t.image[0], readback ).Then( t,
								[] (Promise<ImageMemView> readRes, CoSafe<AC3_TestData &> t) -> InlineCoro<>
								{
									auto view = co_await readRes;
									t->isOK[0] = t->imgCmp->Compare( view );
								});
		t.result[1] = ctx.ReadbackImage( t.image[1], readback ).Then( t,
								[] (Promise<ImageMemView> readRes, CoSafe<AC3_TestData &> t) -> InlineCoro<>
								{
									auto view = co_await readRes;
									t->isOK[1] = t->imgCmp->Compare( view );
								});
		
		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static AsyncCoro  AC3_FrameTask (AC3_TestData& t)
	{
		auto&	rg = GraphicsScheduler().GetRenderGraph();

		for (; t.frameIdx.load() < 3;)
		{
			// frames [0..2]:
			const uint	fi = t.frameIdx.load() & 1;

			CHECK_CE( rg.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rg.BeginFrame() );


			// batch graph
			auto	batch_gfx = rg.CmdBatch( EQueueType::Graphics, {"graphics batch"} )
									.UseResource( t.image[fi], EResourceState::ShaderSample | EResourceState::FragmentShader )
									.Begin();
			CHECK_CE( batch_gfx );

			auto	batch_ac = rg.CmdBatch( EQueueType::AsyncCompute, {"compute batch"} )
									.UseResource( t.image[fi], EResourceState::ShaderStorage_RW | EResourceState::ComputeShader )
									.Begin();
			CHECK_CE( batch_ac );


			// add tasks to cmd batches
			auto	gfx_task	= batch_gfx.Task( AC3_GraphicsTask<CtxTypes>( t, t.frameIdx.load() ), {"graphics task"} )
										.SubmitBatch().Run();
			auto	comp_task	= batch_ac .Task( AC3_ComputeTask<CtxTypes>( t, t.frameIdx.load() ), {"async compute task"} )
										.SubmitBatch().Run( Tuple{gfx_task} );

			auto	end			= rg.EndFrame( Tuple{ gfx_task, comp_task });

			++t.frameIdx;
			Coro_Continue( end );
		}
		{
			CHECK( t.frameIdx.load() == 3 );

			// frame 3
			CHECK_CE( rg.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
			CHECK_CE( rg.BeginFrame() );

			auto	batch	= rg.CmdBatch( EQueueType::AsyncCompute, {"copy task"} )
									.UseResource( t.image[0] )
									.UseResource( t.image[1] )
									.ReadbackMemory()
									.Begin();
			CHECK_CE( batch );

			auto	read_task	= batch.Task( AC3_CopyTask<CopyCtx>(t), {"Readback task"} )
										.SubmitBatch().Run();
			auto	end			= rg.EndFrame( Tuple{read_task} );

			++t.frameIdx;
			Coro_Continue( end );
		}
		co_return;
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  AsyncCompute3Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			res_mngr	= GraphicsScheduler().GetResourceManager();
		AC3_TestData	t;
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		auto&			rg			= GraphicsScheduler().GetRenderGraph();

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		t.image[0] = rg.CreateImage( ImageDesc{}.SetDimension( t.imageDim )
											.SetFormat( format )
											.SetUsage( EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::Storage | EImageUsage::TransferSrc ),
										"Image-0", t.gfxAlloc );
		t.image[1] = rg.CreateImage( ImageDesc{}.SetDimension( t.imageDim )
											.SetFormat( format )
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
		AsyncTask	task = Scheduler().Run( AC3_FrameTask<CtxTypes, CopyCtx>( t ));

		CHECK_ERR( Scheduler().Wait( {task}, c_MaxTimeout ));
		CHECK_ERR( rg.WaitAll( c_MaxTimeout ));

		CHECK_ERR( t.frameIdx.load() == 4 );

		CHECK_ERR( Scheduler().Wait( List{ t.result[0], t.result[1] }, c_MaxTimeout ));
		CHECK_ERR( t.result[0]->Status() == ETaskStatus::Completed );
		CHECK_ERR( t.result[1]->Status() == ETaskStatus::Completed );
		CHECK_ERR( t.isOK[0] );
		CHECK_ERR( t.isOK[1] );

		return true;
	}

} // namespace


bool RGTest::Test_AsyncCompute3 ()
{
	if ( not AllBits( GraphicsScheduler().GetDevice().GetAvailableQueues(), EQueueMask::Graphics | EQueueMask::AsyncCompute ))
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return true;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( AsyncCompute3Test< RG::DirectCtx,   RG::DirectCtx::Transfer   >( _acPipelines, img_cmp.get() ));
	RG_CHECK( AsyncCompute3Test< RG::IndirectCtx, RG::IndirectCtx::Transfer >( _acPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
