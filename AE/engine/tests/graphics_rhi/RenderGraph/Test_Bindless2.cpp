// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Test_RenderGraph.h"

namespace
{
	struct Bls2_TestData
	{
		Mutex								guard;

		uint2								viewSize;
		uint								texCount;

		GAutorelease<ImageID>				img;
		GAutorelease<ImageViewID>			view;

		GAutorelease<ImageID>				tex [4];
		GAutorelease<ImageViewID>			texView [4];

		ComputePipelineID					ppln;
		GAutorelease<DescriptorSetID>		ds;
		DescSetBinding						ds_index;
		const PushConstantIndex				pc_index	{0, EShader::Compute};

		AsyncTask							result;
		bool								isOK		= false;

		ImageComparator *					imgCmp		= null;
		GfxMemAllocatorPtr					gfxAlloc;
	};

	static constexpr auto&	RTech = RenderTechs::Bindless_RTech;


	template <typename Ctx, typename CopyCtx>
	static RenderCoro  Bls2_ComputeTask (Bls2_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		CopyCtx	tctx{ RenderCoro_Get() };

		#ifdef AE_ENABLE_VULKAN
		{
			for (auto& tex : t.tex) {
				tctx.ResourceBarrier( tex, EResourceState::Invalidate, EResourceState::ClearDst );
			}
			tctx.CommitBarriers();

			ImageSubresourceRange	range {EImageAspect::Color};

			for (uint i = 0; i < CountOf(t.tex); ++i)
			{
				RGBA8u	col {Rainbow( float(i) / CountOf(t.tex) )};

				tctx.ClearColorImage( t.tex[i], col, {range} );
			}
		}
		#endif

		Ctx		ctx{ RenderCoro_Get(), tctx.ReleaseCommandBuffer() };
		{
			const auto	img_state = EResourceState::ShaderStorage_Write | EResourceState::ComputeShader;
			const auto	tex_state = EResourceState::ShaderSample | EResourceState::ComputeShader;

			ctx.ResourceBarrier( t.img, EResourceState::Invalidate, img_state );
			for (auto& tex : t.tex) {
				ctx.ResourceBarrier( tex, EResourceState::ClearDst, tex_state );
			}
			ctx.CommitBarriers();

			ShaderTypes::PC_bindless1	pc;
			pc.texCount	= t.texCount;

			ctx.BindPipeline( t.ppln );
			ctx.BindDescriptorSet( t.ds_index, t.ds );
			ctx.PushConstant( t.pc_index, pc );

			ctx.Dispatch( DivCeil( t.viewSize, 8u ));

			ctx.AccumBarriers()
				.ResourceBarrier( t.img, img_state, EResourceState::CopySrc );
		}
		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  Bls2_CopyTask (Bls2_TestData& t)
	{
		DeferExLock	lock {t.guard};
		CHECK_CE( lock.try_lock() );

		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<Bls2_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CompCtx, typename CopyCtx>
	static bool  Bindless2Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		Bls2_TestData	t;
		const auto		format		= EPixelFormat::RGBA8_UNorm;

		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};
		t.texCount	= 500;	// must be <= 1000

		CHECK_ERR( renderTech->Name() == RenderTechName{RTech} );

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format ).SetUsage( EImageUsage::Storage | EImageUsage::TransferSrc ), "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		for (uint i = 0; i < CountOf(t.tex); ++i)
		{
			t.tex[i] = res_mngr.CreateImage( ImageDesc{}.SetDimension( 32, 32 ).SetFormat( format ).SetUsage( EImageUsage::Sampled | EImageUsage::Transfer ), "Texture-"s << ToString(i), t.gfxAlloc );
			CHECK_ERR( t.tex[i] );

			t.texView[i] = res_mngr.CreateImageView( ImageViewDesc{}, t.tex[i], "TextureView-"s << ToString(i) );
			CHECK_ERR( t.texView[i] );
		}


		t.ppln = renderTech->GetComputePipeline( RTech.Compute_1.bindless1_def );
		CHECK_ERR( t.ppln );

		{
			DescSetParams	params;
			params.variableArraySize = t.texCount;

			auto [ds, idx] = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"bindless1.ds"}, null, Default, &params );
			t.ds_index = idx;

			t.ds = RVRef(ds);
			CHECK_ERR( t.ds );

			Array<ImageViewID>	textures;
			textures.resize( t.texCount );

			for (uint i = 0; i < t.texCount; ++i) {
				textures[i] = t.texView[i&3];
			}

			DescriptorUpdater	updater;
			CHECK_ERR( updater.Set( t.ds, EDescUpdateMode::Partialy ));
			updater.BindImage(  UniformName{"un_OutImage"}, t.view );
			updater.BindImages( UniformName{"un_Textures"}, textures );
			CHECK_ERR( updater.Flush() );
		}


		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Compute1"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( Bls2_ComputeTask<CompCtx, CopyCtx>(t),	Tuple{},		 			{"Compute task"} );
		AsyncTask	task2	= batch->Run( Bls2_CopyTask<CopyCtx>(t),				Tuple{task1}, True{"Last"},	{"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( Scheduler().Wait( List{ t.result }, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == ETaskStatus::Completed );
		CHECK_ERR( t.isOK );

		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_Bindless2 ()
{
	if ( _dhPipelines == null )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Bindless2Test< DirectCtx::Compute,   DirectCtx::Transfer   >( _dhPipelines, img_cmp.get() ));
	RG_CHECK( Bindless2Test< IndirectCtx::Compute, IndirectCtx::Transfer >( _dhPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
