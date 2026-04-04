// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct Y1_TestData
	{
		RenderTechPipelinesPtr			rtech;
		uint2							viewSize;

		GAutorelease<ImageID>			img;	// --- render target
		GAutorelease<ImageViewID>		view;	// -/

		GAutorelease<VideoImageID>		ycbcrImg;

		GraphicsPipelineID				ppln;
		GAutorelease<DescriptorSetID>	descSet;
		DescSetBinding					dsIndex;

		AsyncTask						result;
		bool							isOK		= false;

		ImageComparator *				imgCmp		= null;
		GfxMemAllocatorPtr				gfxAlloc;
	};

	static constexpr auto&	RTech			= RenderTechs::Ycbcr_RTech;
	static constexpr auto	c_YcbcrFormat	= EPixelFormat::G8_B8R8_420_UNorm;
	static constexpr uint	imgDim [2]		= {64, 64};


	template <typename CtxTypes>
	static RenderCoro  Y1_DrawTask (Y1_TestData& t)
	{
		auto&	ycbcr = *GraphicsScheduler().GetResourceManager().GetResource( t.ycbcrImg.Get() );

		typename CtxTypes::Transfer		tctx{ RenderCoro_Get() };
		{
			tctx.AccumBarriers()
				.ResourceBarrier( ycbcr.GetImageID(), EResourceState::Invalidate, EResourceState::CopyDst );

			EPixelFormat	plane0_fmt, plane1_fmt;
			POTVec2			plane0_dim, plane1_dim;

			CHECK( EPixelFormat_GetPlaneInfo( c_YcbcrFormat, EImageAspect::Plane_0, OUT plane0_fmt, OUT plane0_dim ));
			CHECK( EPixelFormat_GetPlaneInfo( c_YcbcrFormat, EImageAspect::Plane_1, OUT plane1_fmt, OUT plane1_dim ));

			CHECK( All( plane0_dim == POTVec2::c_1_1() ));
			CHECK( All( plane1_dim == POTVec2::c_2_2() ));

			const YUV8u		yuv		  { YUV32f{ RGBA32f{ 1.f, 1.f, 0.f, 1.f }}};
			ubyte			g_pixels  [ imgDim[0] * imgDim[1] ];
			ubyte			rb_pixels [ (imgDim[0] * imgDim[1] * 2) / 4 ];

			for (ubyte& c : g_pixels) {
				c = yuv[0];
			}
			for (usize i = 0; i < CountOf(rb_pixels); i += 2) {
				rb_pixels[i+0] = yuv[1];
				rb_pixels[i+1] = yuv[2];
			}

			UploadImageDesc			upload;
			upload.imageDim			= ImageDim_t{uint3{ imgDim[0], imgDim[1], 1u }};
			upload.dataRowPitch		= 1_b * imgDim[0];
			upload.dataSlicePitch	= Bytes{CountOf(g_pixels)};
			upload.aspectMask		= EImageAspect::Plane_0;

			Unused( tctx.UploadImage( t.ycbcrImg, upload, ArrayView<ubyte>{g_pixels} ));

			upload.imageDim			= ImageDim_t{uint3{ imgDim[0]/2, imgDim[1]/2, 1u }};
			upload.dataRowPitch		= 2_b * (imgDim[0]/2);
			upload.dataSlicePitch	= Bytes{CountOf(rb_pixels)};
			upload.aspectMask		= EImageAspect::Plane_1;

			Unused( tctx.UploadImage( t.ycbcrImg, upload, ArrayView<ubyte>{rb_pixels} ));
		}

		typename CtxTypes::Graphics		gctx{ RenderCoro_Get(), tctx.ReleaseCommandBuffer() };

		gctx.AccumBarriers()
			.ResourceBarrier( ycbcr.GetImageID(), EResourceState::ClearDst, EResourceState::ShaderSample | EResourceState::FragmentShader );

		constexpr auto&	rtech_pass = RTech.Main;
		StaticAssert( rtech_pass.attachmentsCount == 1 );

		auto	dctx = gctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
								.AddViewport( t.viewSize )
								.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black}, EResourceState::Invalidate, EResourceState::CopySrc ));
		{
			dctx.BindPipeline( t.ppln );
			dctx.BindDescriptorSet( t.dsIndex, t.descSet );

			dctx.Draw( 3 );

			gctx.EndRenderPass( dctx );
		}

		RenderCoro_Execute( gctx );
	}


	template <typename Ctx>
	static RenderCoro  Y1_CopyTask (Y1_TestData& t)
	{
		Ctx		ctx{ RenderCoro_Get() };

		t.result = ctx.ReadbackImage( t.img, Default ).IfFullyRead( t,
							[] (Promise<ImageMemView> readRes, CoSafe<Y1_TestData &> t) -> InlineCoro<>
							{
								auto view = co_await readRes;
								t->isOK = t->imgCmp->Compare( view );
							});

		ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes, typename CopyCtx>
	static bool  Ycbcr1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		const auto		format		= EPixelFormat::RGBA8_UNorm;
		Y1_TestData		t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		t.img = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( format )
										.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
									  "Image", t.gfxAlloc );
		CHECK_ERR( t.img );

		t.view = res_mngr.CreateImageView( ImageViewDesc{}, t.img, "ImageView" );
		CHECK_ERR( t.view );

		t.ycbcrImg = res_mngr.CreateVideoImage( VideoImageDesc{}.SetDimension( imgDim[0], imgDim[1] )
													.SetFormat( c_YcbcrFormat )
													.SetUsage( EImageUsage::Sampled | EImageUsage::TransferDst )
													.SetYcbcrConversion( SamplerName{"NearestClamp|ycbcr|G8_B8R8_420_UNorm"} )
													.SetMemory( EMemoryType::DeviceLocal ),
												"ycbcr", t.gfxAlloc );
		CHECK_ERR( t.ycbcrImg );

		t.ppln = t.rtech->GetGraphicsPipeline( RTech.Main.ycbcr_draw1 );
		CHECK_ERR( t.ppln );

		StructSet( t.descSet, t.dsIndex ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"ycbcr.ds0"} );
		CHECK_ERR( t.descSet );

		{
			DescriptorUpdater	updater;
			CHECK( updater.Set( t.descSet, EDescUpdateMode::Partialy ));
			updater.BindVideoImage( UniformName{"un_YcbcrTexture"}, t.ycbcrImg );
			CHECK( updater.Flush() );
		}

		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch	= rts.BeginCmdBatch( EQueueType::Graphics, 0, {"Ycbcr1"} );
		CHECK_ERR( batch );

		AsyncTask	task1	= batch->Run( Y1_DrawTask<CtxTypes>(t), Tuple{},					{"Draw"} );
		AsyncTask	task2	= batch->Run( Y1_CopyTask<CopyCtx>(t),  Tuple{task1}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task1, task2} );


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


RGTest::ECode  RGTest::Test_Ycbcr1 ()
{
	#ifdef AE_ENABLE_REMOTE_GRAPHICS
		return ECode::Skipped;	// skip
	#endif

	if ( not _ycbcrPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Ycbcr1Test< DirectCtx, DirectCtx::Transfer >( _ycbcrPipelines, img_cmp.get() ));
	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}
