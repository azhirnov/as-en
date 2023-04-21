// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../shared/UnitTest_Shared.h"

using namespace AE::Graphics;
using namespace AE::Video;


#ifdef AE_ENABLE_FFMPEG

namespace
{
	static void  FFmpeg_Test1 ()
	{
		const Path	path {"ffmpeg_video_1.mp4"};
		const uint	fps			= 30;
		const uint	frame_count	= fps * 10;

		// encode
		{
			auto	enc = IVideoEncoder::CreateFFmpegEncoder();
			TEST( enc );

			IVideoEncoder::Config	cfg;
			cfg.srcFormat		= EPixelFormat::RGBA8_UNorm;
			cfg.dstFormat		= EVideoFormat::YUV420P;
			cfg.colorPreset		= EColorPreset::JPEG_BT709;
			cfg.srcSize			= uint2{320, 320};
			cfg.dstSize			= cfg.srcSize;
			cfg.codec			= EVideoCodec::H264;
			cfg.filter			= Video::EFilter::Bilinear;
			cfg.quality			= 0.5f;
			cfg.framerate		= IVideoEncoder::FrameRate_t{ fps };
			cfg.bitrate			= IVideoEncoder::BitrateKb_t{ 10 };
			cfg.hwAccelerated	= false;

			TEST( enc->Begin( cfg, path ));

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.srcFormat, EImageAspect::Color };
		
			for (uint i = 0; i < frame_count; ++i)
			{
				const float		h	= Wrap( i * 0.2f, 0.f, 0.75f );
				const RGBA8u	col	{ RGBA32f{ HSVColor{ h }}};
				for (auto& c : pixels) { c = col; }

				TEST( enc->AddFrame( view, True{} ));
			}

			TEST( enc->End() );
		}

		// get info
		{
			auto	dec = IVideoDecoder::CreateFFmpegDecoder();
			TEST( dec );

			const auto	params = dec->GetFileProperties( path );
			TEST( params.streams.size() == 1 );

			const auto&	info = params.streams[0];
			TEST( info.codecName == "h264" );
			TEST( info.index == 0 );
			TEST( info.type == EMediaType::Video );
			TEST( info.codec == EVideoCodec::H264 );
			TEST( info.format == EVideoFormat::YUV420P );
			TEST( info.frameCount == frame_count );
			TEST( info.duration == IVideoDecoder::Seconds_t{10.0} );
			TEST( info.avgFrameRate == IVideoDecoder::FrameRate_t{fps} );
			TEST( info.minFrameRate == IVideoDecoder::FrameRate_t{fps} );
			//TEST( info.bitrate > 0 );
			TEST( All( info.size == uint2{320, 320} ));
			
			AE_LOGI( dec->PrintFileProperties( path ));
		}

		// decode
		{
			auto	dec = IVideoDecoder::CreateFFmpegDecoder();
			TEST( dec );

			IVideoDecoder::Config	cfg;
			cfg.dstFormat	= EPixelFormat::RGBA8_UNorm;

			TEST( dec->Begin( cfg, path ));
		
			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
			ImageMemView	view;
		
			IVideoDecoder::FrameInfo	fi;

			for (uint i = 0; i < frame_count; ++i)
			{
				const float		h	= Wrap( i * 0.2f, 0.f, 0.75f );
				const RGBA8u	col	{ RGBA32f{ HSVColor{ h }}};
				for (auto& c : pixels) { c = col; }

				TEST( dec->GetFrame( OUT view, OUT fi ));

				//TEST( view == ref_view );
				TEST( view.Format() == cfg.dstFormat );
				TEST( fi.frameIdx == i );
			}

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
			auto	enc = IVideoEncoder::CreateFFmpegEncoder();
			TEST( enc );

			IVideoEncoder::Config	cfg;
			cfg.srcFormat		= EPixelFormat::RGBA8_UNorm;
			cfg.dstFormat		= EVideoFormat::YUV420P;
			cfg.colorPreset		= EColorPreset::MPEG_BT709;
			cfg.srcSize			= uint2{320, 320};
			cfg.dstSize			= cfg.srcSize;
			cfg.codec			= EVideoCodec::H264;
			cfg.filter			= Video::EFilter::Bilinear;
			cfg.quality			= 0.5f;
			cfg.framerate		= IVideoEncoder::FrameRate_t{ fps };
			cfg.bitrate			= IVideoEncoder::BitrateKb_t{ 10 };
			cfg.hwAccelerated	= false;

			TEST( enc->Begin( cfg, path ));

			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.srcFormat, EImageAspect::Color };
		
			for (uint i = 0; i < frame_count; ++i)
			{
				const float		h	= Wrap( i * 0.2f, 0.f, 0.75f );
				const RGBA8u	col	{ RGBA32f{ HSVColor{ h }}};
				for (auto& c : pixels) { c = col; }

				TEST( enc->AddFrame( view, True{} ));
			}

			TEST( enc->End() );
		}

		// decode
		{
			auto	dec = IVideoDecoder::CreateFFmpegDecoder();
			TEST( dec );

			IVideoDecoder::Config	cfg;
			cfg.dstFormat	= EPixelFormat::RGBA8_UNorm;

			auto	file = MakeRC<FileRStream>( path );
			TEST( file->IsOpen() );

			TEST( dec->Begin( cfg, file ));
		
			RGBA8u			pixels [320 * 320] = {};
			ImageMemView	ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
			ImageMemView	view;
		
			IVideoDecoder::FrameInfo	fi;

			for (uint i = 0; i < frame_count; ++i)
			{
				const float		h	= Wrap( i * 0.2f, 0.f, 0.75f );
				const RGBA8u	col	{ RGBA32f{ HSVColor{ h }}};
				for (auto& c : pixels) { c = col; }

				TEST( dec->GetFrame( OUT view, OUT fi ));

				//TEST( view == ref_view );
				TEST( view.Format() == cfg.dstFormat );
				TEST( fi.frameIdx == i );
			}

			TEST( dec->End() );
		}
	}


	static void  FFmpeg_Test3 ()
	{
		auto	dec = IVideoDecoder::CreateFFmpegDecoder();
		TEST( dec );
		
		Array< Path >	stack;
		//stack.push_back( R"()" );

		for (; not stack.empty();)
		{
			Path	dir = stack.back();
			stack.pop_back();

			for (auto& path : FileSystem::Enum( dir ))
			{
				if ( path.IsDirectory() )
				{
					stack.push_back( path );
					continue;
				}

				AE_LOGI( dec->PrintFileProperties( path ));
			}
		}
	}
}


extern void  Test_FFmpeg ()
{
	FFmpeg_Test1();
	FFmpeg_Test2();
	FFmpeg_Test3();

	TEST_PASSED();
}

#else

extern void  Test_FFmpeg ()
{}

#endif // AE_ENABLE_FFMPEG
