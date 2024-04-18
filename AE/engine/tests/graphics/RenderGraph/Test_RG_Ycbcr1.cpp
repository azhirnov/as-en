// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Test_RenderGraph.h"

namespace
{
	struct Y1_TestData
	{
		RenderTechPipelinesPtr			rtech;
		uint2							viewSize;

		GAutorelease<ImageID>			img;	// -- render target
		GAutorelease<ImageViewID>		view;	// /

		GAutorelease<VideoImageID>		ycbcrImg;

		GraphicsPipelineID				ppln;
		GAutorelease<DescriptorSetID>	descSet;
		DescSetBinding					dsIndex;

		AsyncTask						result;
		bool							isOK		= false;

		ImageComparator *				imgCmp		= null;
		GfxMemAllocatorPtr				gfxAlloc;
	};

	static constexpr auto&	RTech		= RenderTechs::Ycbcr_RTech;
	static constexpr auto	ycbcrFormat	= EPixelFormat::G8_B8R8_420_UNorm;
	static constexpr uint	imgDim [2]	= {64, 64};


	ND_ ubyte3  RGBtoYCbCr (RGBA32f rgb)
	{
		float	y  =  0.2989f * rgb.b + 0.5866f * rgb.g + 0.1145f * rgb.r;
		float	cb = -0.1687f * rgb.b - 0.3313f * rgb.g + 0.5000f * rgb.r;
		float	cr =  0.5000f * rgb.b - 0.4184f * rgb.g - 0.0816f * rgb.r;
		return ubyte3{ ubyte(y * 255.f), ubyte(cb * 255.f), ubyte(cr * 255.f) };
	}


	template <typename CtxTypes>
	class Y1_DrawTask final : public RenderTask
	{
	public:
		Y1_TestData&	t;

		Y1_DrawTask (Y1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			auto&	ycbcr = *GraphicsScheduler().GetResourceManager().GetResource( t.ycbcrImg.Get() );

			typename CtxTypes::Transfer		tctx{ *this };
			{
				tctx.AccumBarriers()
					.ImageBarrier( ycbcr.GetImageID(), EResourceState::Invalidate, EResourceState::CopyDst );

				EPixelFormat	plane0_fmt, plane1_fmt;
				uint2			plane0_dim, plane1_dim;

				CHECK( EPixelFormat_GetPlaneInfo( ycbcrFormat, EImageAspect::Plane_0, OUT plane0_fmt, OUT plane0_dim ));
				CHECK( EPixelFormat_GetPlaneInfo( ycbcrFormat, EImageAspect::Plane_1, OUT plane1_fmt, OUT plane1_dim ));

				CHECK( All( plane0_dim == uint2{1,1} ));
				CHECK( All( plane1_dim == uint2{2,2} ));

				const ubyte3	yuv		= RGBtoYCbCr( RGBA32f{ 1.f, 1.f, 0.f, 1.f });
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
				upload.imageDim			= uint3{ imgDim[0], imgDim[1], 1u };
				upload.dataRowPitch		= 1_b * imgDim[0];
				upload.dataSlicePitch	= Bytes{CountOf(g_pixels)};
				upload.aspectMask		= EImageAspect::Plane_0;

				Unused( tctx.UploadImage( t.ycbcrImg, upload, ArrayView<ubyte>{g_pixels} ));

				upload.imageDim			= uint3{ imgDim[0]/2, imgDim[1]/2, 1u };
				upload.dataRowPitch		= 2_b * (imgDim[0]/2);
				upload.dataSlicePitch	= Bytes{CountOf(rb_pixels)};
				upload.aspectMask		= EImageAspect::Plane_1;

				Unused( tctx.UploadImage( t.ycbcrImg, upload, ArrayView<ubyte>{rb_pixels} ));
			}

			typename CtxTypes::Graphics		gctx{ *this, tctx.ReleaseCommandBuffer() };

			const auto	img_state = EResourceState::ShaderSample | EResourceState::FragmentShader;

			gctx.AccumBarriers()
				.ImageBarrier( t.img, EResourceState::Invalidate, img_state )
				.ImageBarrier( ycbcr.GetImageID(), EResourceState::ClearDst, EResourceState::ShaderSample | EResourceState::FragmentShader );

			constexpr auto&	rtech_pass = RTech.Main;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			const auto	rp_desc = RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( rtech_pass.att_Color, t.view, RGBA32f{HtmlColor::Black} );

			auto	dctx = gctx.BeginRenderPass( rp_desc );
			{
				dctx.BindPipeline( t.ppln );
				dctx.BindDescriptorSet( t.dsIndex, t.descSet );

				dctx.Draw( 3 );

				gctx.EndRenderPass( dctx, rp_desc );
			}

			gctx.AccumBarriers()
				.ImageBarrier( t.img, img_state, EResourceState::CopySrc );

			Execute( gctx );
		}
	};


	template <typename Ctx>
	class Y1_CopyTask final : public RenderTask
	{
	public:
		Y1_TestData&	t;

		Y1_CopyTask (Y1_TestData& t, CommandBatchPtr batch, DebugLabel dbg) __NE___ :
			RenderTask{ RVRef(batch), dbg },
			t{ t }
		{}

		void  Run () __Th_OV
		{
			Ctx		ctx{ *this };

			t.result = AsyncTask{ ctx.ReadbackImage( t.img, Default )
						.Then( [p = &t] (const ImageMemView &view)
								{
									p->isOK = p->imgCmp->Compare( view );
								})};

			ctx.AccumBarriers().MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

			Execute( ctx );
		}
	};


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
													.SetFormat( ycbcrFormat )
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

		AsyncTask	task1	= batch->Run< Y1_DrawTask<CtxTypes> >( Tuple{ArgRef(t)}, Tuple{},					 {"Draw"} );
		AsyncTask	task2	= batch->Run< Y1_CopyTask<CopyCtx>  >( Tuple{ArgRef(t)}, Tuple{task1}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task1, task2} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == EStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		CHECK_ERR( Scheduler().Wait( {t.result}, c_MaxTimeout ));
		CHECK_ERR( t.result->Status() == EStatus::Completed );

		CHECK_ERR( t.isOK );
		return true;
	}

} // namespace


bool RGTest::Test_Ycbcr1 ()
{
	if ( not _ycbcrPipelines )
		return true;

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( Ycbcr1Test< DirectCtx, DirectCtx::Transfer >( _ycbcrPipelines, img_cmp.get() ));
	RG_CHECK( _CompareDumps( TEST_NAME ));

	AE_LOGI( TEST_NAME << " - passed" );
	return result;
}
