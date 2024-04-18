// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_TEST_SHADER_DEBUGGER
# include "Test_RenderGraph.h"

namespace
{
	struct Db1_TestData
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

				const bool	is_equal = (Equal( dst[0], r, 1u )	and
										Equal( dst[1], g, 1u )	and
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

		Db1_ComputeTask (Db1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			typename CtxTypes::Transfer	copy_ctx{ *this };

			ShaderDebugger::Result	dbg;
			CHECK_TE( t.debugger.AllocForCompute( OUT dbg, copy_ctx, t.ppln, uint3{8,8,0} ));

			typename CtxTypes::Compute	comp_ctx{ *this, copy_ctx.ReleaseCommandBuffer() };

			const auto	img_state = EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;

			comp_ctx.AccumBarriers()
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state );

			comp_ctx.BindPipeline( t.ppln );
			comp_ctx.BindDescriptorSet( t.ds_index, t.ds );
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

		Db1_CopyTask (Db1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			DeferExLock	lock {t.guard};
			CHECK_TE( lock.try_lock() );

			Ctx		ctx{ *this };

			auto	task1 = ctx.ReadbackImage( t.img, Default );
			auto	task2 = t.debugger.ReadAll( ctx );

			t.result = AsyncTask{ MakePromiseFrom( task1, task2 )
				.Then( [p = &t] (const Tuple<ImageMemView, Array<String>> &view_and_str)
				{
					bool	ok = Db1_CheckImageData( view_and_str.Get<ImageMemView>(), 8 );
					ASSERT( ok );

					const auto&	trace_str = view_and_str.Get<Array<String>>();
					ASSERT( trace_str.size() == 1 );

					if ( trace_str.size() == 1 )
					{
						const StringView	ref_str =
R"(//> gl_GlobalInvocationID: uint3 {8, 8, 0}
//> gl_LocalInvocationID: uint3 {0, 0, 0}
//> gl_WorkGroupID: uint3 {1, 1, 0}
no source

//> color: float4 {0.000000, 0.000000, 1.000000, 0.000000}
//  gl_LocalInvocationID: uint3 {0, 0, 0}
7. color = vec4(float(gl_LocalInvocationID.x) / float(gl_WorkGroupSize.x),
8. 					  float(gl_LocalInvocationID.y) / float(gl_WorkGroupSize.y),
9. 					  1.0, 0.0);

//> imageStore(): void
//  color: float4 {0.000000, 0.000000, 1.000000, 0.000000}
//  gl_GlobalInvocationID: uint3 {8, 8, 0}
11. 	imageStore( un_OutImage, ivec2(gl_GlobalInvocationID.xy), color );

)";
						ok &= (trace_str[0] == ref_str);
						ASSERT( ok );

						p->isOK = ok;
					}
				})};

			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			Execute( ctx );
		}
	};


	template <typename CtxTypes, typename CopyCtx>
	static bool  Debugger1Test ()
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		Db1_TestData	t;
		const uint2		img_dim		{16, 16};
		const auto		format		= EPixelFormat::RGBA8_UNorm;

		t.gfxAlloc = res_mngr.CreateLinearGfxMemAllocator();

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
			StructSet( t.ds, t.ds_index ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"dbg1_compute.ds1"} );
			CHECK_ERR( t.ds );

			DescriptorUpdater	updater;

			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage( UniformName{"un_OutImage"}, t.view );

			CHECK_ERR( updater.Flush() );
		}


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Debugger1"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run< Db1_ComputeTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{},					 {"Compute task"} );
		AsyncTask	task2	= batch->Run< Db1_CopyTask<CopyCtx>     >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );

		return true;
	}

} // namespace


bool RGTest::Test_Debugger1 ()
{
	if ( not _dbgPipelines )
		return true;

	bool	result = true;

	RG_CHECK( Debugger1Test< DirectCtx, DirectCtx::Transfer >());

	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}

#endif // AE_TEST_SHADER_DEBUGGER
