// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/File.h"
#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Graphics;
using namespace AE::Video;


#ifdef AE_ENABLE_FFMPEG

namespace
{
	using Allocator_t		= LinearAllocator<>;
	using ImageMemViewArr	= IVideoDecoder::ImageMemViewArr;


	static const auto		c_Codec		= EVideoCodec::H265;
	static const StringView	c_CodecName	= "hevc";

	template <typename ConfigType>
	static void  EnableHWAcceleration (ConfigType &cfg)
	{
		cfg.hwAccelerated	= EHwAcceleration::Disable;
	//	cfg.targetCPU		= ECPUVendor::AMD;
	//	cfg.targetGPU		= EGraphicsDeviceID::NV_Turing;
	}


	static void  FFmpeg_Test1 ()
	{
		const Path	path {"ffmpeg_video_1.mp4"};
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
			cfg.bitrate		= BitrateKb{ fps/2 };
			EnableHWAcceleration( cfg );

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
			TEST( info.bitrate > BitrateKb{10} and info.bitrate < BitrateKb{30} );
			TEST( All( info.dimension == uint2{320, 320} ));
		}

		// decode
		{
			auto	dec = VideoFactory::CreateFFmpegDecoder();
			TEST( dec );

			IVideoDecoder::Config	cfg;
			cfg.dstFormat	= EPixelFormat::RGBA8_UNorm;
			EnableHWAcceleration( cfg );

			TEST( dec->Begin( cfg, path ));

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
			ImageMemView	view;
			Allocator_t		alloc;

			TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT view, alloc ));

			IVideoDecoder::FrameInfo	fi;

			for (uint i = 0; i < frame_count; ++i)
			{
				TEST( dec->GetVideoFrame( INOUT view, OUT fi ));
				TEST( view.Format() == cfg.dstFormat );

				auto	tp = Seconds{ double(i) / fps };
				TEST( BitEqual( fi.timestamp, tp ));
				TEST_Eq( fi.frameIdx, i );
			}

			TEST( not dec->GetVideoFrame( INOUT view, OUT fi ));
			TEST( dec->End() );
		}
	}


	static void  FFmpeg_Test2 ()
	{
		const Path	path {"ffmpeg_video_2.mp4"};
		const uint	fps			= 30;
		const uint	frame_count	= fps * 10;

		// encode
		{
			auto	enc = VideoFactory::CreateFFmpegEncoder();
			TEST( enc );

			IVideoEncoder::Config	cfg;
			cfg.srcFormat	= EPixelFormat::RGBA8_UNorm;
			cfg.dstFormat	= EVideoFormat::YUV420P;
			cfg.colorPreset	= EColorPreset::MPEG_BT709;
			cfg.srcDim		= uint2{320, 320};
			cfg.dstDim		= cfg.srcDim;
			cfg.codec		= c_Codec;
			cfg.filter		= Video::EFilter::Bilinear;
			cfg.quality		= 0.5f;
			cfg.framerate	= FrameRate{ fps };
			cfg.bitrate		= BitrateKb{ fps/2 };
			EnableHWAcceleration( cfg );

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
			TEST( info.bitrate > BitrateKb{10} and info.bitrate < BitrateKb{30} );
			TEST( All( info.dimension == uint2{320, 320} ));
		}

		// decode
		{
			auto	dec = VideoFactory::CreateFFmpegDecoder();
			TEST( dec );

			IVideoDecoder::Config	cfg;
			cfg.dstFormat	= EPixelFormat::RGBA8_UNorm;
			EnableHWAcceleration( cfg );

			auto	file = MakeRC<FileRStream>( path );
			TEST( file->IsOpen() );

			TEST( dec->Begin( cfg, file ));

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
			ImageMemView	view;
			Allocator_t		alloc;

			TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT view, alloc ));

			IVideoDecoder::FrameInfo	fi;

			for (uint i = 0; i < frame_count; ++i)
			{
				TEST( dec->GetVideoFrame( INOUT view, OUT fi ));
				TEST( view.Format() == cfg.dstFormat );

				auto	tp = Seconds{ double(i) / fps };
				TEST( BitEqual( fi.timestamp, tp ));
				TEST_Eq( fi.frameIdx, i );
			}

			TEST( not dec->GetVideoFrame( INOUT view, OUT fi ));
			TEST( dec->End() );
		}
	}


	static void  FFmpeg_Test3 ()
	{
		const Path	path {"ffmpeg_video_3.mp4"};
		const uint	fps			= 30;
		const uint	frame_count	= fps * 10;

		// encode
		{
			auto	enc = VideoFactory::CreateFFmpegEncoder();
			TEST( enc );

			IVideoEncoder::Config	cfg;
			cfg.srcFormat	= EPixelFormat::RGBA8_UNorm;
			cfg.dstFormat	= EVideoFormat::YUV420P;
			cfg.colorPreset	= EColorPreset::MPEG_BT709;
			cfg.srcDim		= uint2{320, 320};
			cfg.dstDim		= cfg.srcDim;
			cfg.codec		= c_Codec;
			cfg.filter		= Video::EFilter::Bilinear;
			cfg.quality		= 0.5f;
			cfg.framerate	= FrameRate{ fps };
			cfg.bitrate		= BitrateKb{ fps/2 };
			EnableHWAcceleration( cfg );

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
			TEST( info.bitrate > BitrateKb{10} and info.bitrate < BitrateKb{30} );
			TEST( All( info.dimension == uint2{320, 320} ));
		}

		// decode
		{
			auto	dec = VideoFactory::CreateFFmpegDecoder();
			TEST( dec );

			IVideoDecoder::Config	cfg;
			cfg.dstFormat	= EPixelFormat::G8_B8_R8_420_UNorm;
			EnableHWAcceleration( cfg );

			auto	file = MakeRC<FileRStream>( path );
			TEST( file->IsOpen() );

			TEST( dec->Begin( cfg, file ));

			IVideoDecoder::ImagePlanesMemView	view_arr;
			Allocator_t							alloc;

			TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT view_arr, alloc ));

			IVideoDecoder::FrameInfo	fi;

			for (uint i = 0; i < frame_count; ++i)
			{
				TEST( dec->GetVideoFrame( INOUT view_arr, OUT fi ));
				TEST_Eq( view_arr.size(), 3 );

				auto	tp = Seconds{ double(i) / fps };
				TEST( BitEqual( fi.timestamp, tp ));
				TEST_Eq( fi.frameIdx, i );
			}

			TEST( not dec->GetVideoFrame( INOUT view_arr, OUT fi ));
			TEST( dec->End() );
		}
	}


	static void  FFmpeg_Print ()
	{
		AE_LOGI( "============ Decoders ============" );

		auto	dec = VideoFactory::CreateFFmpegDecoder();
		TEST( dec );

		for (uint i = 0, cnt = uint(EVideoCodec::_Count); i < cnt; ++i)
		{
			AE_LOGI( dec->PrintCodecs( EVideoCodec(i) ));
		}

		AE_LOGI( "============ Encoders ============" );

		auto	enc = VideoFactory::CreateFFmpegEncoder();
		TEST( enc );

		for (uint i = 0, cnt = uint(EVideoCodec::_Count); i < cnt; ++i)
		{
			AE_LOGI( enc->PrintCodecs( EVideoCodec(i) ));
		}
	}
}


extern void  Test_FFmpeg ()
{
	//FFmpeg_Print();

	FFmpeg_Test1();
	FFmpeg_Test2();
	FFmpeg_Test3();

	TEST_PASSED();
}

#else

extern void  Test_FFmpeg ()
{}

#endif // AE_ENABLE_FFMPEG
