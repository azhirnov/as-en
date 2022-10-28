// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct Db1_TestData
	{
		Mutex						guard;

		Strong<ImageID>				img;
		Strong<ImageViewID>			view;
		
		Strong<ComputePipelineID>	ppln;
		
		Strong<DescriptorSetID>		ds0;
		uint						ds0_index	= UMax;
		
		ShaderDebugger				debugger;

		AsyncTask					result;
		bool						isOK		= false;

		RC<GfxLinearMemAllocator>	gfxAlloc;
	};

	
	static bool  Db1_CheckImageData (const ImageMemView &view, uint blockSize)
	{
		RWImageMemView	img_data{ view };
		bool			is_correct = true;

		for (uint y = 0; y < img_data.Dimension().y; ++y)
		{
			for (uint x = 0; x < img_data.Dimension().x; ++x)
			{
				RGBA32u		dst;
				img_data.Load( uint3(x,y,0), OUT dst );

				const uint	r	= uint(float(x % blockSize) * 255.0f / float(blockSize) + 0.5f);
				const uint	g	= uint(float(y % blockSize) * 255.0f / float(blockSize) + 0.5f);
				
				const bool	is_equal = (Equals( dst[0], r, 1u )	and
										Equals( dst[1], g, 1u )	and
										dst[2] == 255			and
										dst[3] == 0);

				//ASSERT( is_equal );
				is_correct &= is_equal;
			}
		}
		return is_correct;
	};

	
	template <typename CtxTypes>
	class Db1_ComputeTask final : public RenderTask
	{
	public:
		Db1_TestData&	t;

		Db1_ComputeTask (Db1_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			typename CtxTypes::Transfer	copy_ctx{ *this };
			CHECK_TE( copy_ctx.IsValid() );

			ShaderDebugger::Result	dbg;
			CHECK_TE( t.debugger.AllocForCompute( OUT dbg, copy_ctx, t.ppln, uint3{8,8,0} ));

			typename CtxTypes::Compute	comp_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };
			CHECK_TE( comp_ctx.IsValid() );
			
			const auto	img_state = EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;

			comp_ctx.AccumBarriers()
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );
			
			comp_ctx.BindPipeline( t.ppln );
			comp_ctx.BindDescriptorSet( t.ds0_index, t.ds0 );
			comp_ctx.BindDescriptorSet( dbg.DSIndex(), dbg.DescSet() );
			comp_ctx.Dispatch({ 2, 2, 1 });
			
			comp_ctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );

			Execute( comp_ctx );
		}
	};
	
	template <typename Ctx>
	class Db1_CopyTask final : public RenderTask
	{
	public:
		Db1_TestData&	t;

		Db1_CopyTask (Db1_TestData& t, CommandBatchPtr batch, StringView dbgName) :
			RenderTask{ batch, dbgName },
			t{ t }
		{}

		void  Run () override
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );
			
			Ctx		ctx{ *this };
			CHECK_TE( ctx.IsValid() );

			auto	task1 = ctx.ReadbackImage( t.img, Default );
			auto	task2 = t.debugger.ReadAll( ctx );

			t.result = AsyncTask{MakePromiseFrom( task1, task2 )
				.Then( [p = &t] (const Tuple<ImageMemView, Array<String>> &view_and_str)
				{
					bool	ok = Db1_CheckImageData( view_and_str.Get<ImageMemView>(), 8 );
					ASSERT( ok );

					const auto&	trace_str = view_and_str.Get<Array<String>>();
					ASSERT( trace_str.size() == 1 );

					if ( trace_str.size() == 1 )
					{
						const char	ref_str[] =
R"(//> gl_GlobalInvocationID: uint3 {8, 8, 0}
//> gl_LocalInvocationID: uint3 {0, 0, 0}
//> gl_WorkGroupID: uint3 {1, 1, 0}
no source

//> color: float4 {0.000000, 0.000000, 1.000000, 0.000000}
//  gl_LocalInvocationID: uint3 {0, 0, 0}
46. color = vec4(float(gl_LocalInvocationID.x) / float(gl_WorkGroupSize.x),
47. 					  float(gl_LocalInvocationID.y) / float(gl_WorkGroupSize.y),
48. 					  1.0, 0.0);

//> imageStore(): void
//  color: float4 {0.000000, 0.000000, 1.000000, 0.000000}
//  gl_GlobalInvocationID: uint3 {8, 8, 0}
50. 	imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );

)";
						ok &= (trace_str[0] == ref_str);
						ASSERT( ok );

						p->isOK = ok;
					}
				})};
			
			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
			
			CHECK_TE( ExecuteAndSubmit( ctx ));
		}
	};

	
	template <typename CtxTypes, typename CopyCtx>
	static bool  Debugger1Test ()
	{
		auto&			rts			= RenderTaskScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		Db1_TestData	t;
		const uint2		img_dim		{16, 16};
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		
		t.gfxAlloc = MakeRC<GfxLinearMemAllocator>();

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( img_dim ).SetFormat( format ).SetUsage( EImageUsage::Storage | EImageUsage::TransferSrc ), "Image", t.gfxAlloc );
		CHECK_ERR( t.img );
	
		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		{
			ComputePipelineDesc	desc;

			t.ppln = res_mngr.CreateComputePipeline( Default, PipelineTmplName{"dbg1_compute"}, desc );
			CHECK_ERR( t.ppln );
		}
		
		{
			auto [ds0, idx0] = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"dbg1_compute.ds1"} );

			t.ds0_index = idx0;
			t.ds0 = RVRef(ds0);
			CHECK_ERR( t.ds0 );

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds0, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );

			CHECK_ERR( updater.Flush() );
		}

		AsyncTask	begin	= rts.BeginFrame();
		CHECK_ERR( begin );

		auto		batch	= rts.CreateBatch( EQueueType::Graphics, 0, "Debugger1" );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Add< Db1_ComputeTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{begin}, "Compute task" );
		CHECK_ERR( task1 );
		
		AsyncTask	task2	= batch->Add< Db1_CopyTask<CopyCtx> >( Tuple{ArgRef(t)}, Tuple{task1}, "Readback task" );
		CHECK_ERR( task2 );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );
		CHECK_ERR( end );

		CHECK_ERR( Scheduler().Wait({ end }));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll() );
		
		CHECK_ERR( Scheduler().Wait({ t.result }));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( res_mngr.ReleaseResources( t.view, t.img, t.ds0, t.ppln ));
		CHECK_ERR( t.isOK );

		return true;
	}

} // namespace


bool RGTest::Test_Debugger1 ()
{
	CHECK_ERR(( Debugger1Test< DirectCtx, DirectCtx::Transfer >()));
	
	CHECK_ERR( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return true;
}
