// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Graphics;
using namespace AE::Video;
using namespace AE::Threading;

#if defined(AE_ENABLE_VULKAN) and defined(AE_ENABLE_FFMPEG)

namespace
{
	using Allocator_t		= LinearAllocator<>;
	using ImageMemViewArr	= IVideoDecoder::ImageMemViewArr;

	static const auto		c_Codec		= EVideoCodec::H265;
	static const StringView	c_CodecName	= "hevc";


	static void  WithVulkan (Function<void()> fn)
	{
		TaskScheduler::InstanceCtor::Create();

		TaskScheduler::Config	cfg;
		CHECK_FATAL( Scheduler().Setup( cfg ));

		VDeviceInitializer		device{ True{"enable info log"}, False{"disable allocator stats"} };
		RenderTaskScheduler::InstanceCtor::Create( device );

		Graphics::GraphicsCreateInfo	info;
		info.device.requiredQueues	= EQueueMask::Graphics | EQueueMask::VideoDecode | EQueueMask::VideoEncode;

		TEST( device.Init( info, Default ));

		fn();

		RenderTaskScheduler::InstanceCtor::Destroy();
		Scheduler().Release();
		TaskScheduler::InstanceCtor::Destroy();

		CHECK( device.DestroyLogicalDevice() );
		CHECK( device.DestroyInstance() );
	}


	static void  FFmpegVk_Test1 ()
	{
		const Path	path		{"ffmpeg_video_1.mp4"};
		const uint	fps			= 40;
		const uint	frame_count	= fps * 10;

		// encode
		{
			auto	enc = VideoFactory::CreateFFmpegEncoder();
			TEST( enc );

			IVideoEncoder::Config	cfg;
			cfg.srcFormat	= EPixelFormat::RGBA8_UNorm;
			cfg.dstFormat	= EVideoFormat::YUV420P;
			cfg.colorPreset	= EColorPreset::JPEG_BT709;
			cfg.srcDim		= uint2{320, 320};
			cfg.dstDim		= cfg.srcDim;
			cfg.codec		= c_Codec;
			cfg.filter		= Video::EFilter::Bilinear;
			cfg.quality		= 0.5f;
			cfg.framerate	= FrameRate{ fps };
			cfg.bitrate		= BitrateKib{ fps/2 };
			cfg.hwAccelerated = EHwAcceleration::Require_Vulkan;

			TEST( enc->Begin( cfg, path ));

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.srcFormat, EImageAspect::Color };

			for (uint i = 0; i < frame_count; ++i)
			{
				const RGBA8u	col	{RainbowWrap( i * 0.2f )};
				for (auto& c : pixels) { c = col; }

				TEST( enc->AddFrame( view, True{} ));
			}

			TEST( enc->End() );
		}

		// get info
		{
			auto	dec = VideoFactory::CreateFFmpegDecoder();
			TEST( dec );

			const auto	params = dec->GetFileProperties( path );
			TEST( params.videoStream.IsValid() );

			AE_LOGI( dec->PrintFileProperties( path ));

			const auto&	info = params.videoStream;
			TEST( info.codecName == c_CodecName );
			TEST( info.index == 0 );
			TEST( info.type == EMediaType::Video );
			TEST( info.codec == c_Codec );
			TEST( info.videoFormat == EVideoFormat::YUV420P );
			TEST( info.frameCount == frame_count );
			TEST( info.duration > Seconds{9.9} and info.duration < Seconds{10.1} );
			TEST( info.avgFrameRate.Get_RTN() == fps );
			TEST( info.minFrameRate == FrameRate{fps} );
			TEST( info.bitrate > BitrateKib{10} and info.bitrate < BitrateKib{100} );
			TEST( All( info.dimension == uint2{320, 320} ));
		}

		// decode
		{
			auto	dec = VideoFactory::CreateFFmpegDecoder();
			TEST( dec );

			IVideoDecoder::Config	cfg;
			cfg.dstFormat	= EPixelFormat::RGBA8_UNorm;

			TEST( dec->Begin( cfg, path ));

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
			ImageMemView	view;
			Allocator_t		alloc;

			TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT view, alloc ));

			IVideoDecoder::FrameInfo	fi;

			for (uint i = 0; i < frame_count; ++i)
			{
				TEST( dec->GetVideoFrame( INOUT view, OUT fi ) == EResult::OK );
				TEST( view.Format() == cfg.dstFormat );

				auto	tp = Seconds{ double(i) / fps };
				TEST( BitEqual( fi.timestamp, tp ));
				TEST_Eq( fi.frameIdx, i );
			}

			TEST( dec->GetVideoFrame( INOUT view, OUT fi ) == EResult::EndOfFile );
			TEST( dec->End() );
		}
	}
}


extern void  Test_FFmpegVulkan ()
{
	WithVulkan( [](){ FFmpegVk_Test1(); });

	TEST_PASSED();
}

#else

extern void  Test_FFmpegVulkan ()
{}

#endif // AE_ENABLE_VULKAN and AE_ENABLE_FFMPEG
