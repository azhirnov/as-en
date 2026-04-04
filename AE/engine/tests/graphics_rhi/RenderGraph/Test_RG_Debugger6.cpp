// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_TEST_SHADER_DEBUGGER
# include "Test_RenderGraph.h"

namespace
{
	struct Db6_TestData
	{
		Mutex							guard;

		GAutorelease<ImageID>			img;
		GAutorelease<ImageViewID>		view;

		GAutorelease<ComputePipelineID>	ppln;

		GAutorelease<DescriptorSetID>	ds;
		DescSetBinding					ds_index;

		ShaderDebugger					debugger;

		AsyncTask						result;
		bool							isOK		= false;

		GfxMemAllocatorPtr				gfxAlloc;
	};


	template <typename CtxTypes>
	static RenderCoro  Db6_ComputeTask (Db6_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		typename CtxTypes::Transfer	copy_ctx{ RenderCoro_Get() };

		ShaderDebugger::Result	dbg;
		CHECK_CE( t.debugger.AllocForAsserts( OUT dbg, copy_ctx, t.ppln.Get() ));

		typename CtxTypes::Compute	comp_ctx{ RenderCoro_Get(), copy_ctx.ReleaseCommandBuffer() };

		const auto	img_state = EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;

		comp_ctx.AccumBarriers()
			.ResourceBarrier( t.img, EResourceState::Invalidate, img_state );

		comp_ctx.BindPipeline( t.ppln );
		comp_ctx.BindDescriptorSet( t.ds_index, t.ds );
		comp_ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
		comp_ctx.Dispatch({ 2, 2, 1 });

		comp_ctx.AccumBarriers()
			.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );

		RenderCoro_Execute( comp_ctx );
	}


	template <typename Ctx>
	static RenderCoro  Db6_CopyTask (Db6_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		auto	task1 = ctx.ReadbackImage( t.img, Default );
		auto	task2 = t.debugger.ReadAll( ctx );

		t.result =
			[] (auto readOp, auto dbgRes, auto& t) -> InlineCoro<>
			{
				auto	res = Coro_WaitResultOrCancel( readOp, dbgRes );
				auto& [view, trace_str] = res;

				ASSERT( trace_str.size() == 2 );
				if ( trace_str.size() == 2 )
				{
					bool	ok = true;
					ok &= (trace_str[0] == "	ASSERT( color.a < 0.5 );");
					ok &= (trace_str[1] == "	ASSERT( false );");
					ASSERT( ok );

					t.isOK = ok;
				}
			}
			( task1.readOp, task2, t );

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );

		GraphicsScheduler().AddNextCycleEndDeps( t.result );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  Debugger6Test ()
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		Db6_TestData	t;
		const uint2		img_dim		{16, 16};
		const auto		format		= EPixelFormat::RGBA8_UNorm;

		t.gfxAlloc = res_mngr.CreateLinearGfxMemAllocator();

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( img_dim ).SetFormat( format ).SetUsage( EImageUsage::Storage | EImageUsage::TransferSrc ), "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		{
			ComputePipelineDesc	desc;

			t.ppln = res_mngr.CreateComputePipeline( Default, PipelineTmplName{"dbg6_compute"}, desc );
			CHECK_ERR( t.ppln );
		}

		{
			StructSet( t.ds, t.ds_index ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"dbg6_compute.ds1"} );
			CHECK_ERR( t.ds );

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );

			CHECK_ERR( updater.Flush() );
		}


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger6"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( Db6_ComputeTask<CtxTypes>(t), Tuple{},					{"Compute task"} );
		AsyncTask	task2	= batch->Run( Db6_CopyTask<CopyCtx>(t),		Tuple{task1}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));
		CHECK_ERR( t.result );

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == ETaskStatus::Completed );

		CHECK_ERR( t.isOK );

		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_Debugger6 ()
{
	if ( not _dbgPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	bool	result = true;

	RG_CHECK( Debugger6Test< DirectCtx, DirectCtx::Transfer >());

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // AE_TEST_SHADER_DEBUGGER
