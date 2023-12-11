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
    using Allocator_t = LinearAllocator<>;


    // H264/H265 supported only in GPL version
    static const auto   c_Codec = EVideoCodec::VP9;


    static void  FFmpeg_Test1 ()
    {
        const Path  path {"ffmpeg_video_1.mp4"};
        const uint  fps         = 30;
        const uint  frame_count = fps * 10;

        // encode
        {
            auto    enc = VideoFactory::CreateFFmpegEncoder();
            TEST( enc );

            IVideoEncoder::Config   cfg;
            cfg.srcFormat       = EPixelFormat::RGBA8_UNorm;
            cfg.dstFormat       = EVideoFormat::YUV420P;
            cfg.colorPreset     = EColorPreset::JPEG_BT709;
            cfg.srcDim          = uint2{320, 320};
            cfg.dstDim          = cfg.srcDim;
            cfg.codec           = c_Codec;
            cfg.filter          = Video::EFilter::Bilinear;
            cfg.quality         = 0.5f;
            cfg.framerate       = IVideoEncoder::FrameRate_t{ fps };
            cfg.bitrate         = IVideoEncoder::BitrateKb_t{ 10 };
            cfg.hwAccelerated   = false;

            TEST( enc->Begin( cfg, path ));

            RGBA8u          pixels [320 * 320] = {};
            ImageMemView    view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.srcFormat, EImageAspect::Color };

            for (uint i = 0; i < frame_count; ++i)
            {
                const RGBA8u    col {RainbowWrap( i * 0.2f )};
                for (auto& c : pixels) { c = col; }

                TEST( enc->AddFrame( view, True{} ));
            }

            TEST( enc->End() );
        }

        // get info
        {
            auto    dec = VideoFactory::CreateFFmpegDecoder();
            TEST( dec );

            const auto  params = dec->GetFileProperties( path );
            TEST( params.streams.size() == 1 );

            const auto& info = params.streams[0];
            //TEST( info.codecName == "h264" );
            TEST( info.index == 0 );
            TEST( info.type == EMediaType::Video );
            TEST( info.codec == c_Codec );
            TEST( info.format == EVideoFormat::YUV420P );
        //  TEST( info.frameCount == frame_count );
            TEST( info.duration == IVideoDecoder::Second_t{10.0} );
            TEST( info.avgFrameRate == IVideoDecoder::FrameRate_t{fps} );
            TEST( info.minFrameRate == IVideoDecoder::FrameRate_t{fps} );
            //TEST( info.bitrate > 0 );
            TEST( All( info.dimension == uint2{320, 320} ));

            AE_LOGI( dec->PrintFileProperties( path ));
        }

        // decode
        {
            auto    dec = VideoFactory::CreateFFmpegDecoder();
            TEST( dec );

            IVideoDecoder::Config   cfg;
            cfg.dstFormat   = EPixelFormat::RGBA8_UNorm;

            TEST( dec->Begin( cfg, path ));

            RGBA8u          pixels [320 * 320] = {};
            ImageMemView    ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
            ImageMemView    view;
            Allocator_t     alloc;

            TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT view, alloc ));

            IVideoDecoder::FrameInfo    fi;

            for (uint i = 0; i < frame_count; ++i)
            {
                const RGBA8u    col {RainbowWrap( i * 0.2f )};
                for (auto& c : pixels) { c = col; }

                TEST( dec->GetNextFrame( INOUT view, OUT fi ));

                //TEST( view == ref_view );
                TEST( view.Format() == cfg.dstFormat );
                TEST( fi.frameIdx == i );
            }

            TEST( dec->End() );
        }
    }


    static void  FFmpeg_Test2 ()
    {
        const Path  path {"ffmpeg_video_2.mp4"};
        const uint  fps         = 30;
        const uint  frame_count = fps * 10;

        // encode
        {
            auto    enc = VideoFactory::CreateFFmpegEncoder();
            TEST( enc );

            IVideoEncoder::Config   cfg;
            cfg.srcFormat       = EPixelFormat::RGBA8_UNorm;
            cfg.dstFormat       = EVideoFormat::YUV420P;
            cfg.colorPreset     = EColorPreset::MPEG_BT709;
            cfg.srcDim          = uint2{320, 320};
            cfg.dstDim          = cfg.srcDim;
            cfg.codec           = c_Codec;
            cfg.filter          = Video::EFilter::Bilinear;
            cfg.quality         = 0.5f;
            cfg.framerate       = IVideoEncoder::FrameRate_t{ fps };
            cfg.bitrate         = IVideoEncoder::BitrateKb_t{ 10 };
            cfg.hwAccelerated   = false;

            TEST( enc->Begin( cfg, path ));

            RGBA8u          pixels [320 * 320] = {};
            ImageMemView    view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.srcFormat, EImageAspect::Color };

            for (uint i = 0; i < frame_count; ++i)
            {
                const RGBA8u    col {RainbowWrap( i * 0.2f )};
                for (auto& c : pixels) { c = col; }

                TEST( enc->AddFrame( view, True{} ));
            }

            TEST( enc->End() );
        }

        // decode
        {
            auto    dec = VideoFactory::CreateFFmpegDecoder();
            TEST( dec );

            IVideoDecoder::Config   cfg;
            cfg.dstFormat   = EPixelFormat::RGBA8_UNorm;

            auto    file = MakeRC<FileRStream>( path );
            TEST( file->IsOpen() );

            TEST( dec->Begin( cfg, file ));

            RGBA8u          pixels [320 * 320] = {};
            ImageMemView    ref_view{ pixels, Sizeof(pixels), uint3{}, uint3{320, 320, 1}, 0_b, 0_b, cfg.dstFormat, EImageAspect::Color };
            ImageMemView    view;
            Allocator_t     alloc;

            TEST( IVideoDecoder::AllocMemView( dec->GetConfig(), OUT view, alloc ));

            IVideoDecoder::FrameInfo    fi;

            for (uint i = 0; i < frame_count; ++i)
            {
                const RGBA8u    col {RainbowWrap( i * 0.2f )};
                for (auto& c : pixels) { c = col; }

                TEST( dec->GetNextFrame( INOUT view, OUT fi ));

                //TEST( view == ref_view );
                TEST( view.Format() == cfg.dstFormat );
                TEST( fi.frameIdx == i );
            }

            TEST( dec->End() );
        }
    }
}


extern void  Test_FFmpeg ()
{
    FFmpeg_Test1();
    FFmpeg_Test2();

    TEST_PASSED();
}

#else

extern void  Test_FFmpeg ()
{}

#endif // AE_ENABLE_FFMPEG
