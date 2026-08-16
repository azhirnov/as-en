// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Test_RenderGraph.h"

#if 0 //def AE_ENABLE_VULKAN

namespace
{
	static constexpr auto&	RTech			= RenderTechs::Ycbcr_RTech;
	static constexpr auto	c_YcbcrFormat	= EPixelFormat::G8_B8R8_420_UNorm;
	static constexpr uint	imgDim [2]		= {320, 128};		// TODO: read from video session
	static constexpr uint	c_FrameCount	= 4;


	struct VE1_TestData
	{
		RenderTechPipelinesPtr			rtech;
		uint2							viewSize;

		GAutorelease<VideoSessionID>	encodeSession;
		GAutorelease<VideoSessionID>	decodeSession;

		GAutorelease<VideoBufferID>		bitStreamBuffer;

		GAutorelease<VideoImageID>		inYcbcrImg [c_FrameCount];
		GAutorelease<VideoImageID>		outYcbcrImg [c_FrameCount];

		GAutorelease<ImageID>			img [c_FrameCount];			// --- render target
		GAutorelease<ImageViewID>		view [c_FrameCount];		// -/

		GraphicsPipelineID				ppln;
		GAutorelease<DescriptorSetID>	descSet;
		DescSetBinding					dsIndex;

		Array<Bytes>					bitStreamOffsets;

		AsyncTask						result [c_FrameCount];
		uint							isOK		= 0;

		ImageComparator *				imgCmp		= null;
		GfxMemAllocatorPtr				gfxAlloc;
	};


	template <typename Ctx>
	static RenderCoro  VE1_UploadTask (VE1_TestData& t)
	{
		Ctx		ctx{ RenderCoro_Get() };

		for (VideoImageID ycbcr_id : t.inYcbcrImg)
		{
			ctx.AccumBarriers()
				.ResourceBarrier( ycbcr_id,	EResourceState::Invalidate, EResourceState::CopyDst );

			EPixelFormat	plane0_fmt, plane1_fmt;
			POTVec2			plane0_dim, plane1_dim;

			CHECK( EPixelFormat_GetPlaneInfo( c_YcbcrFormat, EImageAspect::Plane_0, OUT plane0_fmt, OUT plane0_dim ));
			CHECK( EPixelFormat_GetPlaneInfo( c_YcbcrFormat, EImageAspect::Plane_1, OUT plane1_fmt, OUT plane1_dim ));

			CHECK( All( plane0_dim == POTVec2::c_1_1() ));
			CHECK( All( plane1_dim == POTVec2::c_2_2() ));

			const YUV8u		yuv		  { YUV32f{ RGBA32f{ 1.f, 1.f, 0.f, 1.f }}};	// TODO
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

			Unused( ctx.UploadImage( ycbcr_id, upload, ArrayView<ubyte>{g_pixels} ));

			upload.imageDim			= ImageDim_t{uint3{ imgDim[0]/2, imgDim[1]/2, 1u }};
			upload.dataRowPitch		= 2_b * (imgDim[0]/2);
			upload.dataSlicePitch	= Bytes{CountOf(rb_pixels)};
			upload.aspectMask		= EImageAspect::Plane_1;

			Unused( ctx.UploadImage( ycbcr_id, upload, ArrayView<ubyte>{rb_pixels} ));

			ctx.AccumBarriers()
				.ResourceBarrier( ycbcr_id,	EResourceState::CopyDst,	EResourceState::General );
		}

		ctx.AccumBarriers()
			.ResourceBarrier( t.bitStreamBuffer,	EResourceState::Invalidate,	EResourceState::General );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  VE1_EncodeTask (VE1_TestData& t)
	{
		Ctx		ctx{ RenderCoro_Get() };

		auto&	bit_stream	= *ctx.GetResourceManager().GetResource( t.bitStreamBuffer.Get() );

		ctx.Begin( t.encodeSession );

		// encode
		{
			VideoEncodeCmd	cmd;
			cmd.dstBuffer	= t.bitStreamBuffer;

			auto&	spec = cmd.spec.emplace< VideoEncodeCmd::H264 >();
			spec.picType = VideoH264::EPictureType::IDR;

			for (uint i = 0; i < c_FrameCount; ++i)
			{
				cmd.srcPicture.imageViewBinding	= t.inYcbcrImg[i];

				ctx.AccumBarriers()
					.ResourceBarrier( cmd.srcPicture.imageViewBinding,	EResourceState::General,	EResourceState::VideoEncodeSrc );


				cmd.dstBufferSize	= 100_MiB;

				ctx.Encode( cmd );
			}
		}

		ctx.End();

		ctx.AccumBarriers()
			.ResourceBarrier( bit_stream.GetBufferID(), EResourceState::VideoEncodeDst, EResourceState::General );

		RenderCoro_Execute( ctx );
	}


	template <typename Ctx>
	static RenderCoro  VE1_DecodeTask (VE1_TestData& t)
	{
		Ctx		ctx{ RenderCoro_Get() };

	/*	auto&	ycbcr = *ctx.GetResourceManager().GetResource( t.ycbcrImg.Get() );

		ctx.Begin( t.decodeSession );

		// decode
		{
			VideoDecodeCmd	cmd;
			cmd.srcBuffer					= t.bitStreamBuffer;
			cmd.dstPicture.imageViewBinding	= t.ycbcrImg;

			for (uint i = 0; i < c_FrameCount; ++i)
			{
				cmd.srcBufferSize				= 100_MiB;

				ctx.Decode( cmd );
			}
		}

		ctx.End();

		ctx.AccumBarriers()
			.ResourceBarrier( ycbcr.GetImageID(), EResourceState::VideoDecode_Write, EResourceState::General );
			*/
		RenderCoro_Execute( ctx );
	}


	template <typename CtxTypes>
	static RenderCoro  VE1_ReadbackTask (VE1_TestData& t)
	{
		typename CtxTypes::Graphics		gctx{ RenderCoro_Get() };

		for (uint i = 0; i < c_FrameCount; ++i)
		{
			gctx.AccumBarriers()
				.ResourceBarrier( t.outYcbcrImg[i],	EResourceState::General,	EResourceState::ShaderSample | EResourceState::FragmentShader );

			constexpr auto&	rtech_pass = RTech.Main;
			StaticAssert( rtech_pass.attachmentsCount == 1 );

			auto	dctx = gctx.BeginRenderPass( RenderPassDesc{ *t.rtech, rtech_pass, t.viewSize }
									.AddViewport( t.viewSize )
									.AddTarget( rtech_pass.att_Color, t.view[i], RGBA32f{HtmlColor::Black}, EResourceState::Invalidate, EResourceState::CopySrc ));
			{
				dctx.BindPipeline( t.ppln );
				dctx.BindDescriptorSet( t.dsIndex, t.descSet );

				dctx.Draw( 3 );
			}
			gctx.EndRenderPass( dctx );
		}

		typename CtxTypes::Transfer		tctx{ RenderCoro_Get(), gctx.ReleaseCommandBuffer() };

		for (uint i = 0; i < c_FrameCount; ++i)
		{
			t.result[i] = tctx.ReadbackImage( t.img[i], Default ).IfFullyRead( t,
								[] (Promise<ImageMemView> readRes, CoSafe<VE1_TestData &> t) -> InlineCoro<>
								{
									auto view = co_await readRes;
									t->isOK += uint{t->imgCmp->Compare( view )};
								});
		}

		tctx.AccumBarriers()
			.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );

		RenderCoro_Execute( tctx );
	}


	template <typename CtxTypes>
	static bool  VideoEncoder1Test (RenderTechPipelinesPtr renderTech, ImageComparator* imageCmp)
	{
		auto&			rts			= GraphicsScheduler();
		auto&			res_mngr	= rts.GetResourceManager();
		VE1_TestData	t;

		t.rtech		= renderTech;
		t.gfxAlloc	= res_mngr.CreateLinearGfxMemAllocator();
		t.imgCmp	= imageCmp;
		t.viewSize	= uint2{800, 600};

		CHECK_ERR( t.rtech->Name() == RenderTechName{RTech} );

		VideoProfile	enc_profile;
		{
			enc_profile.mode				= EVideoCodecMode::Encode;
			enc_profile.codec				= EVideoCodec::H264;
			enc_profile.chromaSubsampling	= EVideoChromaSubsampling::_420;
			enc_profile.lumaBitDepth		= EVideoComponentBitDepth::_8;
			enc_profile.chromaBitDepth		= EVideoComponentBitDepth::_8;

			auto&	spec = enc_profile.spec.emplace< VideoProfile::Encode_H264 >();
			spec.stdProfileIdc	= EStdVideoH264ProfileIdc::Main;
		}

		VideoProfile	dec_profile = enc_profile;
		{
			dec_profile.mode = EVideoCodecMode::Decode;

			auto&	spec = dec_profile.spec.emplace< VideoProfile::Decode_H264 >();
			spec.stdProfileIdc	= EStdVideoH264ProfileIdc::Main;
		}


		// encode session
		{
			VideoSessionDesc	desc;
			desc.queue			= EQueueType::VideoEncode;
			desc.pictureFormat	= c_YcbcrFormat;
			desc.profile		= enc_profile;

			t.encodeSession = res_mngr.CreateVideoSession( desc, "VideoEncodeSession", t.gfxAlloc );
			CHECK_ERR( t.encodeSession );
		}

		// decode session
		{
			VideoSessionDesc	desc;
			desc.queue			= EQueueType::VideoDecode;
			desc.pictureFormat	= c_YcbcrFormat;
			desc.profile		= dec_profile;

			t.decodeSession = res_mngr.CreateVideoSession( desc, "VideoDecodeSession", t.gfxAlloc );
			CHECK_ERR( t.decodeSession );
		}

		// create input image
		{
			VideoImageDesc	desc;
			desc.dimension			= VideoImageDim_t{uint2{ imgDim[0], imgDim[1] }};
			desc.format				= c_YcbcrFormat;
			desc.usage				= EImageUsage::Transfer | EImageUsage::Sampled;
			desc.ycbcrConversion	= SamplerName{"NearestClamp|ycbcr|G8_B8R8_420_UNorm"};
			desc.queues				= EQueueMask::Graphics | EQueueMask::VideoDecode | EQueueMask::VideoEncode;
			desc.profiles.push_back( enc_profile );
			desc.profiles.push_back( dec_profile );

			for (auto& img : t.inYcbcrImg)
			{
				desc.videoUsage = EVideoImageUsage::EncodeSrc;
				img = res_mngr.CreateVideoImage( desc, "Input Image", t.gfxAlloc );
				CHECK_ERR( img );
			}
			/*for (auto& img : t.outYcbcrImg)
			{
				desc.videoUsage = EVideoImageUsage::DecodeDst;
				img = res_mngr.CreateVideoImage( desc, "Output Image", t.gfxAlloc );
				CHECK_ERR( img );
			}*/
		}

		// create bit stream buffer
		{
			VideoBufferDesc		desc;
			desc.size			= 100_MiB;
			desc.usage			= EBufferUsage::Transfer;
			desc.videoUsage		= EVideoBufferUsage::DecodeSrc | EVideoBufferUsage::EncodeDst;
			desc.queues			= EQueueMask::Graphics | EQueueMask::VideoDecode | EQueueMask::VideoEncode;
			desc.profiles.push_back( enc_profile );
			desc.profiles.push_back( dec_profile );

			t.bitStreamBuffer = res_mngr.CreateVideoBuffer( desc, "BitStreamBuffer", t.gfxAlloc );
		}

		// create render target
		for (uint i = 0; i < c_FrameCount; ++i)
		{
			t.img[i] = res_mngr.CreateImage( ImageDesc{}.SetDimension( t.viewSize ).SetFormat( EPixelFormat::RGBA8_UNorm )
												.SetUsage( EImageUsage::Sampled | EImageUsage::ColorAttachment | EImageUsage::TransferSrc ),
											 "RGB Image", t.gfxAlloc );
			CHECK_ERR( t.img[i] );

			t.view[i] = res_mngr.CreateImageView( ImageViewDesc{}, t.img[i], "RGB ImageView" );
			CHECK_ERR( t.view[i] );
		}

		// setup pipeline
		{
			t.ppln = t.rtech->GetGraphicsPipeline( RTech.Main.ycbcr_draw1 );
			CHECK_ERR( t.ppln );

			StructSet( t.descSet, t.dsIndex ) = res_mngr.CreateDescriptorSet( t.ppln, DescriptorSetName{"ycbcr.ds0"} );
			CHECK_ERR( t.descSet );

			DescriptorUpdater	updater;
			CHECK( updater.Set( t.descSet, EDescUpdateMode::Partialy ));
		//	updater.BindVideoImage( UniformName{"un_YcbcrTexture"}, t.ycbcrImg );
			CHECK( updater.Flush() );
		}

		CHECK_ERR( rts.WaitNextFrame( c_ThreadArr, c_MaxTimeout ));
		CHECK_ERR( rts.BeginFrame() );

		auto		batch_upl	= rts.BeginCmdBatch( EQueueType::Graphics,		0, {"Upload"} );
		auto		batch_enc	= rts.BeginCmdBatch( EQueueType::VideoEncode,	0, {"Encode"} );
		auto		batch_dec	= rts.BeginCmdBatch( EQueueType::VideoDecode,	0, {"Decode"} );
		auto		batch_rb	= rts.BeginCmdBatch( EQueueType::Graphics,		1, {"Readback"} );
		CHECK_ERR( batch_upl and batch_enc and batch_dec and batch_rb );

		CHECK_ERR( batch_enc->AddInputDependency( *batch_upl ));
		CHECK_ERR( batch_dec->AddInputDependency( *batch_enc ));
		CHECK_ERR( batch_rb ->AddInputDependency( *batch_dec ));

		AsyncTask	task1	= batch_upl->Run( VE1_UploadTask< typename CtxTypes::Transfer	>(t), Tuple{}, True{"Last"}, {"Upload task"} );
		AsyncTask	task2	= batch_enc->Run( VE1_EncodeTask< typename CtxTypes::VideoEncode>(t), Tuple{}, True{"Last"}, {"Encode task"} );
		AsyncTask	task3	= batch_dec->Run( VE1_DecodeTask< typename CtxTypes::VideoDecode>(t), Tuple{}, True{"Last"}, {"Decode task"} );
		AsyncTask	task4	= batch_rb ->Run( VE1_ReadbackTask< CtxTypes					>(t), Tuple{}, True{"Last"}, {"Readback task"} );

		AsyncTask	end		= rts.EndFrame( Tuple{task1, task2, task3, task4} );


		CHECK_ERR( Scheduler().Wait( {end}, c_MaxTimeout ));
		CHECK_ERR( end->Status() == ETaskStatus::Completed );

		CHECK_ERR( rts.WaitAll( c_MaxTimeout ));

		for (auto& res : t.result) {
			CHECK_ERR( res );
		}

		CHECK_ERR( Scheduler().Wait( {ArrayView{t.result}}, c_MaxTimeout ));

		for (auto& res : t.result) {
			CHECK_ERR( res->Status() == ETaskStatus::Completed );
		}
		CHECK_ERR( t.isOK == c_FrameCount );

		return true;
	}

} // namespace


RGTest::ECode  RGTest::Test_VideoEncoder1 ()
{
	#ifdef AE_ENABLE_REMOTE_GRAPHICS
		return ECode::Skipped;	// skip
	#endif

	if ( not AllBits( _device.GetAvailableQueues(), EQueueMask::VideoDecode | EQueueMask::VideoEncode ))
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	if ( not _ycbcrPipelines )
	{
		AE_LOGI( TEST_NAME << " - skipped" );
		return ECode::Skipped;
	}

	auto	img_cmp = _LoadReference( TEST_NAME );
	bool	result	= true;

	RG_CHECK( VideoEncoder1Test< DirectCtx >( _ycbcrPipelines, img_cmp.get() ));
	//RG_CHECK( VideoEncoder1Test< IndirectCtx >( _ycbcrPipelines, img_cmp.get() ));

	RG_CHECK( _CompareDumps( TEST_NAME ));

	if ( result )
	{
		AE_LOGI( TEST_NAME << " - passed" );
		return ECode::Passed;
	}
	return ECode::Failed;
}

#endif // AE_ENABLE_VULKAN
