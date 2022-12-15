// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct Db2_TestData
	{
		Mutex						guard;

		uint2						viewSize;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;
		
		GraphicsPipelineID			ppln;
		
		ShaderDebugger				debugger;

		AsyncTask					result;

		CommandBatchPtr				batch;
		bool						isOK	= false;

		ImageComparator *			imgCmp	= null;
		RC<GfxLinearMemAllocator>	gfxAlloc;
	};


	template <typename CtxType>
	class Db2_DrawTask final : public RenderTask
	{
	public:
		Db2_TestData&	t;

		Db2_DrawTask (Db2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;
			
			typename CtxType::Transfer	copy_ctx{ *this };

			ShaderDebugger::Result	dbg;
			CHECK_TE( t.debugger.AllocForGraphics( OUT dbg, copy_ctx, t.ppln ));

			typename CtxType::Graphics	ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
			
			ctx.AccumBarriers()
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			// draw
			{
				auto	dctx = ctx.BeginRenderPass( RenderPassDesc{ RenderPassName{"DrawTest.Draw_1"}, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( AttachmentName{"Color"}, t.view, RGBA32f{HtmlColor::Black} ));

				dctx.BindPipeline( t.ppln );
				dctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
				dctx.Draw( 3 );
				
				ctx.EndRenderPass( dctx );
			}

			ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );
				
			Execute( ctx );
		}
	};
	
	template <typename Ctx>
	class Db2_CopyTask final : public RenderTask
	{
	public:
		Db2_TestData&	t;

		Db2_CopyTask (Db2_TestData& t, CommandBatchPtr batch, DebugLabel dbg) :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };
			
			auto	task1 = ctx.ReadbackImage( t.img, Default );
			auto	task2 = t.debugger.ReadAll( ctx );
			
			t.result = AsyncTask{ MakePromiseFrom( task1, task2 )
				.Then( [p = &t] (const Tuple<ImageMemView, Array<String>> &view_and_str)
				{
					bool	ok = p->imgCmp->Compare( view_and_str.Get<ImageMemView>() );
					ASSERT( ok );

					Array<String>	trace_str = view_and_str.Get<Array<String>>();
					ASSERT( trace_str.size() == 3 );
					std::sort( trace_str.begin(), trace_str.end(), [](auto &lhs, auto &rhs) { return lhs.size() < rhs.size(); });

					if ( trace_str.size() == 3 )
					{
						const char	vs1_ref_str[] =
R"(//> gl_VertexIndex: int {1}
//> gl_InstanceIndex: int {0}
no source

//> (out): float4 {0.500000, 0.500000, 0.000000, 1.000000}
//  gl_VertexIndex: int {1}
51. gl_Position	= vec4( g_Positions[gl_VertexIndex], 0.0, 1.0 );

//> v_Color: float3 {0.000000, 1.000000, 0.000000}
//  gl_VertexIndex: int {1}
52. v_Color		= g_Colors[gl_VertexIndex];

)";
						const char	vs2_ref_str[] = 
R"(//> gl_VertexIndex: int {2}
//> gl_InstanceIndex: int {0}
no source

//> (out): float4 {-0.500000, 0.500000, 0.000000, 1.000000}
//  gl_VertexIndex: int {2}
51. gl_Position	= vec4( g_Positions[gl_VertexIndex], 0.0, 1.0 );

//> v_Color: float3 {0.000000, 0.000000, 1.000000}
//  gl_VertexIndex: int {2}
52. v_Color		= g_Colors[gl_VertexIndex];

)";
						const char	fs_ref_str[] =
R"(//> gl_FragCoord: float4 {400.500000, 300.500000, 0.000000, 1.000000}
//> gl_PrimitiveID: int {0}
//> v_Color: float3 {0.498333, 0.252083, 0.249583}
no source

//> out_Color: float3 {0.498333, 0.252083, 0.249583}
//  v_Color: float3 {0.498333, 0.252083, 0.249583}
40. out_Color.rgb = v_Color.rgb;

//> out_Color: float4 {0.498333, 0.252083, 0.249583, 0.500000}
//  v_Color: float3 {0.498333, 0.252083, 0.249583}
41. out_Color.a   = fract(v_Color.r + v_Color.g + v_Color.b + 0.5f);

)";

						ok &= (trace_str[0] == vs1_ref_str);
						ok &= (trace_str[1] == vs2_ref_str);
						ok &= (trace_str[2] == fs_ref_str);
						ASSERT( ok );

						p->isOK = ok;
					}
			  })};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			Execute( ctx );
		}
	};

	
	template <typename CtxType, typename CopyCtx>
	static bool  Debugger2Test (RenderTechPipelinesPtr renderTech, ImageComparator *imageCmp)
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		Db2_TestData	t;
		
		t.gfxAlloc	= MakeRC<GfxLinearMemAllocator>();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ppln = renderTech->GetGraphicsPipeline( PipelineName{"dbg2_draw"} );
		CHECK_ERR( t.ppln );

		AsyncTask	begin	= rts.BeginFrame();

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, ESubmitMode::Immediately, {"Debugger2"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< Db2_DrawTask<CtxType> >( Tuple{ArgRef(t)}, Tuple{begin},				 {"Draw task"} );
		AsyncTask	task2	= batch->Add< Db2_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

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


bool RGTest::Test_Debugger2 ()
{
	auto	img_cmp = _LoadReference( TEST_NAME );

	CHECK_ERR(( Debugger2Test< DirectCtx, DirectCtx::Transfer >( _pipelines, img_cmp.get() )));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
