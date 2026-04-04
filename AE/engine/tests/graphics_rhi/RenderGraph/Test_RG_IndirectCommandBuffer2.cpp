// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	static constexpr auto&	RTech				= RenderTechs::IndirectCmds_RTech;
	static constexpr uint	c_MaxSequenceCount	= 2;
	static constexpr uint	c_MaxDrawCount		= 1;


	struct ICB2_TestData
	{
		Mutex						guard;

		RenderTechPipelinesPtr		rtech;
		uint2						viewSize;

		GAutorelease<ImageID>		img;
		GAutorelease<ImageViewID>	view;

		IndirectExecutionSetID		execSet;

		RC<IPreprocessingStateCommandPool>		stateCmdPool;
		GAutorelease<IndirectCommandsLayoutID>	cmdLayout;
		RC<IGraphicsStateCommandBuffer>			stateCmdbuf;

		struct SequenceType {
			uint						exec;
			DrawIndirectCommand			draw;
		};
		StaticArray<SequenceType, c_MaxSequenceCount>	cmdData;

		GAutorelease<BufferID>		inputBuf;
		GAutorelease<BufferID>		preprocessBuf;

		AsyncTask					result;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		GfxMemAllocatorPtr			gfxAlloc;
	};


	template <typename CtxType>
	static RenderCoro  ICB2_DrawTask (ICB2_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		constexpr auto&		rtech_pass = RTech.Main;
		StaticAssert( rtech_pass.attachmentsCount == 1 );

		const auto	rp_desc = RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} );

		const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;


		// setup preprocess / execute command
		ExecuteGeneratedCommands2Cmd	cmd;

		cmd.indirectExecutionSet	= t.execSet;
		cmd.indirectCommandsLayout	= t.cmdLayout;

		cmd.preprocessBuffer		= t.preprocessBuf;
		cmd.indirectBuffer			= t.inputBuf;
		cmd.sequenceCountBuffer		= Default;		// unused

		cmd.maxSequenceCount		= c_MaxSequenceCount;
		cmd.maxDrawCount			= c_MaxDrawCount;
		cmd.shaderStages			= EShaderStages::Vertex | EShaderStages::Fragment;


		// update
		typename CtxType::Transfer	tctx{ RenderCoro_Get() };
		{
			tctx.AccumBarriers()
				.ResourceBarrier( t.inputBuf, EResourceState::Invalidate, EResourceState::CopyDst );

			CHECK_CE( tctx.UploadBuffer( t.inputBuf, 0_b, Sizeof(t.cmdData), &t.cmdData ));
		}


		// preprocess
		t.stateCmdbuf = t.stateCmdPool->BeginGraphics();
		CHECK_CE( t.stateCmdbuf );

		typename CtxType::Compute	comp_ctx{ RenderCoro_Get(), tctx.ReleaseCommandBuffer() };
		{
			comp_ctx.AccumBarriers()
				.ResourceBarrier( t.inputBuf, EResourceState::CopyDst, EResourceState::ICB_Preprocess_Read )
				.ResourceBarrier( t.preprocessBuf, EResourceState::Invalidate, EResourceState::ICB_Preprocess_Write );

			t.stateCmdbuf->SetViewports( rp_desc.viewports );

			// any pipeline from execution set must be bound.
			// also all descriptor sets must be bound.
			t.stateCmdbuf->BindInitialPipeline( t.execSet );

			CHECK_CE( t.stateCmdbuf->End() );

			cmd.preprocessStates = t.stateCmdbuf.get();

			comp_ctx.PreprocessGeneratedCommands( cmd );
		}


		// draw
		typename CtxType::Graphics	gctx{ RenderCoro_Get(), comp_ctx.ReleaseCommandBuffer() };
		{
			gctx.AccumBarriers()
				.ResourceBarrier( t.img, EResourceState::Invalidate, img_state )
				.ResourceBarrier( t.inputBuf, EResourceState::ICB_Preprocess_Read, EResourceState::IndirectBuffer )
				.ResourceBarrier( t.preprocessBuf, EResourceState::ICB_Preprocess_Write, EResourceState::IndirectBuffer );

			auto	dctx = gctx.BeginRenderPass( rp_desc );

			t.stateCmdbuf->CopyStates( dctx );

			dctx.ExecuteGeneratedCommands( cmd );

			gctx.EndRenderPass( dctx );

			gctx.AccumBarriers()
				.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );
		}

		RenderCoro_Execute( gctx );
	}


	template <typename Ctx>
	static RenderCoro  ICB2_CopyTask (ICB2_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<ICB2_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxType, typename CopyCtx>
	static bool  ICB2Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		ICB2_TestData	t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		// create image & view
		{
			t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
											.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
										  "Image", t.gfxAlloc );
			CHECK_ERR( t.img );

			t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
			CHECK_ERR( t.view );
		}

		t.execSet = t.rtech->GetIndirectExecutionSet( RTech.Main.execSet.icb_set1 );
		CHECK_ERR( t.execSet );

		// create command layout
		{
			IndirectCommandsLayoutDescBuilder	builder;

			builder.Usage( EIndirectCommandsLayoutUsage::ExplicitPreprocess );

			builder.Stages( EShaderStages::Vertex | EShaderStages::Fragment );

			// must be first token
			builder.ExecutionSet( EShaderStages::Vertex | EShaderStages::Fragment );
			builder.Advance( Sizeof(t.cmdData[0].exec) );

			// must be last token
			builder.Draw();
			builder.Advance( Sizeof(t.cmdData[0].draw) );

			CHECK_ERR( Sizeof(t.cmdData) == builder.Size( c_MaxSequenceCount ));

			t.cmdLayout = res_mngr.CreateIndirectCommandsLayout( builder.Build() );
			CHECK_ERR( t.cmdLayout );
		}

		// write command data
		{
			const uint	ppln_count = res_mngr.GetResource( t.execSet )->PipelineCount();

			t.cmdData[0].exec				= 1;	// green
			t.cmdData[0].draw.vertexCount	= 3;
			t.cmdData[0].draw.instanceCount	= 1;
			t.cmdData[0].draw.firstInstance	= 1;	// left
			t.cmdData[0].draw.firstVertex	= 0;
			CHECK_ERR( t.cmdData[0].exec < ppln_count );

			t.cmdData[1].exec				= 2;	// blue
			t.cmdData[1].draw.vertexCount	= 3;
			t.cmdData[1].draw.instanceCount	= 1;
			t.cmdData[1].draw.firstInstance	= 2;	// right
			t.cmdData[1].draw.firstVertex	= 0;
			CHECK_ERR( t.cmdData[1].exec < ppln_count );
		}

		// create buffers
		{
			GeneratedCommandsMemoryRequirementsDesc	desc;
			desc.indirectExecutionSet	= t.execSet;
			desc.indirectCommandsLayout	= t.cmdLayout;
			desc.maxSequenceCount		= c_MaxSequenceCount;
			desc.maxDrawCount			= c_MaxDrawCount;

			SizeAndAlign	pp_size = res_mngr.GetPreprocessingBufferSize( desc );
			pp_size.size = Max( pp_size.size, 1_KiB );	// 'GetPreprocessingBufferSize()' may return 0

			t.inputBuf		= res_mngr.CreateBuffer( BufferDesc{ Sizeof(t.cmdData), EBufferUsage::ShaderAddress | EBufferUsage::Transfer }, "input buffer" );
			CHECK_ERR( t.inputBuf );

			t.preprocessBuf	= res_mngr.CreateBuffer( BufferDesc{ pp_size.size, EBufferUsage::ICB_Preprocess }, "preprocess buffer" );
			CHECK_ERR( t.preprocessBuf );
		}

		t.stateCmdPool = res_mngr.CreatePreprocessingStateCommandPool( EQueueType::Graphics );
		CHECK_ERR( t.stateCmdPool );

		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"ICB2.Draw"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( ICB2_DrawTask<CtxType>(t), Tuple{},						{"Draw task"} );
		AsyncTask	task2	= batch->Run( ICB2_CopyTask<CopyCtx>(t), Tuple{task1}, True{"Last"},	{"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));
		CHECK_ERR( t.result );

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == ETaskStatus::Completed );

		// must be reseted / released when GPU complete execution
		t.stateCmdbuf = null;
		t.stateCmdPool = null;

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_IndirectCommandBuffer2 ()
{
	if ( not _icbPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( ICB2Test< DirectCtx,   DirectCtx::Transfer   >( _icbPipelines, img_cmp.get() ));
	RG_CHECK( ICB2Test< DirectCtx,   IndirectCtx::Transfer >( _icbPipelines, img_cmp.get() ));

	RG_CHECK( ICB2Test< IndirectCtx, DirectCtx::Transfer   >( _icbPipelines, img_cmp.get() ));
	RG_CHECK( ICB2Test< IndirectCtx, IndirectCtx::Transfer >( _icbPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
