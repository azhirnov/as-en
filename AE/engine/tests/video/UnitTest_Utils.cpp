// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Graphics;
using namespace AE::Video;

#undef  TEST_Gt
#define TEST_Gt		CHECK_Gt

#undef  TEST_Lt
#define TEST_Lt		CHECK_Lt

namespace
{
	static void  H264Bitrate_Test1 ()
	{
		const ulong	Mbps = 1'000'000;

		IVideoEncoder::Config	cfg;
		cfg.codec	= EVideoCodec::H264;

		{
			cfg.dstDim		= uint2{ 1920, 1080 };
			cfg.framerate	= FrameRate{ 30, 1 };
			cfg.quality		= 0.2f;

			Bitrate		br1 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.5f;

			Bitrate		br2 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.8f;

			Bitrate		br3 = IVideoEncoder::CalcBitrate( cfg );

			TEST_Gt( br1, Bitrate{3'5 * Mbps/10} );
			TEST_Lt( br1, Bitrate{4'5 * Mbps/10} );

			TEST_Gt( br2, Bitrate{6'0 * Mbps/10} );
			TEST_Lt( br2, Bitrate{7'0 * Mbps/10} );

			TEST_Gt( br3, Bitrate{8'0 * Mbps/10} );
			TEST_Lt( br3, Bitrate{10'0 * Mbps/10} );
		}

		{
			cfg.dstDim		= uint2{ 1920, 1080 };
			cfg.framerate	= FrameRate{ 60, 1 };
			cfg.quality		= 0.2f;

			Bitrate		br1 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.5f;

			Bitrate		br2 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.8f;

			Bitrate		br3 = IVideoEncoder::CalcBitrate( cfg );

			TEST_Gt( br1, Bitrate{5'0 * Mbps/10} );
			TEST_Lt( br1, Bitrate{6'0 * Mbps/10} );

			TEST_Gt( br2, Bitrate{8'0 * Mbps/10} );
			TEST_Lt( br2, Bitrate{9'5 * Mbps/10} );

			TEST_Gt( br3, Bitrate{10'0 * Mbps/10} );
			TEST_Lt( br3, Bitrate{15'0 * Mbps/10} );
		}

		{
			cfg.dstDim		= uint2{ 1920, 1080 } * 2;	// 4K
			cfg.framerate	= FrameRate{ 30, 1 };
			cfg.quality		= 0.2f;

			Bitrate		br1 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.5f;

			Bitrate		br2 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.8f;

			Bitrate		br3 = IVideoEncoder::CalcBitrate( cfg );

			TEST_Gt( br1, Bitrate{20 * Mbps} );
			TEST_Lt( br1, Bitrate{35 * Mbps} );

			TEST_Gt( br2, Bitrate{35 * Mbps} );
			TEST_Lt( br2, Bitrate{50 * Mbps} );

			TEST_Gt( br3, Bitrate{50 * Mbps} );
			TEST_Lt( br3, Bitrate{60 * Mbps} );
		}

		{
			cfg.dstDim		= uint2{ 1920, 1080 } * 2;	// 4K
			cfg.framerate	= FrameRate{ 60, 1 };
			cfg.quality		= 0.2f;

			Bitrate		br1 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.5f;

			Bitrate		br2 = IVideoEncoder::CalcBitrate( cfg );

			cfg.quality		= 0.8f;

			Bitrate		br3 = IVideoEncoder::CalcBitrate( cfg );

			TEST_Gt( br1, Bitrate{30 * Mbps} );
			TEST_Lt( br1, Bitrate{40 * Mbps} );

			TEST_Gt( br2, Bitrate{50 * Mbps} );
			TEST_Lt( br2, Bitrate{70 * Mbps} );

			TEST_Gt( br3, Bitrate{75 * Mbps} );
			TEST_Lt( br3, Bitrate{90 * Mbps} );
		}
	}
}


extern void  UnitTest_Utils ()
{
	H264Bitrate_Test1();
}

