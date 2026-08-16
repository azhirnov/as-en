// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "pch/Base.h"
#include "pch/Platform.h"
#include "video/Public/VideoDecoder.h"
#include "video/Public/VideoEncoder.h"
#include "../tests/shared/UnitTest_Shared.h"

using namespace AE::Threading;
using namespace AE::Graphics;
using namespace AE::Video;
using namespace AE::App;


#ifdef AE_ENABLE_FFMPEG
# if defined(AE_ENABLE_GLFW)
#	define ENABLE_TEST
#	include "platform/GLFW/ApplicationGLFW.h"
	using Application_t = AE::App::ApplicationGLFW;

# elif defined(AE_PLATFORM_WINDOWS)
#	define ENABLE_TEST
#	include "platform/WinAPI/ApplicationWinAPI.h"
	using Application_t = AE::App::ApplicationWinAPI;

# endif
#endif // AE_ENABLE_FFMPEG


#ifdef ENABLE_TEST

namespace
{
	using Allocator_t		= LinearAllocator<>;
	using ImageMemViewArr	= IVideoDecoder::ImageMemViewArr;


	static const auto		c_Codec		= EVideoCodec::H265;
	static const StringView	c_CodecName	= "hevc";


	class AppListener final : public IApplication::IAppListener
	{
	private:
		RC<IScreenCapture>		_screenCapture;
		Atomic<int>				_frameCount	{0};
		RC<IVideoEncoder>		_encoder;

		const Path				path	{"screen_capture_1.mp4"};
		const int				fps		= 40;

	public:
		AppListener () __NE___
		{
			TaskScheduler::InstanceCtor::Create();

			TaskScheduler::Config	cfg;
			TEST( Scheduler().Setup( cfg ));
		}

		~AppListener () __NE_OV
		{
			Scheduler().Release();
			TaskScheduler::InstanceCtor::Destroy();
		}


		void  OnStart (IApplication &app) __NE_OV
		{
			auto	monitors = app.GetCachedMonitors();
			TEST( not monitors.empty() );

			IScreenCapture::Config	cfg;
			cfg.hostImageFormat	= EPixelFormat::RGBA8_UNorm;
			cfg.monitorHandle	= monitors.front().native;

			TEST( cfg.monitorHandle != null );

			_screenCapture = app.StartScreenCapture( cfg );
			TEST( _screenCapture );

			_screenCapture->SetReadImageCallback(
				[this] (auto memView, auto frameInfo, auto err)
				{
					ReadImage( memView, frameInfo, err );
					return true;  // continue
				});
		}

		void  OnStop (IApplication &) __NE_OV
		{
			_screenCapture = null;
		}


		void  BeforeWndUpdate (IApplication &) __NE_OV
		{}

		void  AfterWndUpdate (IApplication &app) __NE_OV
		{
			if ( _frameCount.load() > 100 )
			{
				_screenCapture->Finish();
				app.Terminate();
			}
		}


		void  ReadImage (const Graphics::ImageMemView		&memView,
						 const IScreenCapture::FrameInfo	&,
						 IScreenCapture::ErrorCode			err)
		{
			if ( _frameCount.load() >= 100 )
				return;  // skip

			if ( err != IScreenCapture::ErrorCode::OK )
			{
				_frameCount.store( 101 );
				if ( _encoder )
				{
					TEST( _encoder->End() );
					_encoder = null;
				}
				return;  // skip
			}

			if ( not _encoder )
			{
				_encoder = VideoFactory::CreateFFmpegEncoder();
				TEST( _encoder );

				IVideoEncoder::Config	cfg;
				cfg.srcFormat	= memView.Format();
				cfg.dstFormat	= EVideoFormat::YUV420P;
				cfg.colorPreset	= EColorPreset::JPEG_BT709;
				cfg.srcDim		= uint2(memView.Dimension());
				cfg.dstDim		= cfg.srcDim;
				cfg.codec		= c_Codec;
				cfg.filter		= Video::EFilter::Bilinear;
				cfg.quality		= 0.5f;
				cfg.framerate	= FrameRate{ fps };
				cfg.bitrate		= BitrateMib{ ulong(fps/2) };

				TEST( _encoder->Begin( cfg, path ));
			}

			//AE_LOG_DBG( "Encode frame "s << ToString(_frameCount.load()) );

			TEST( _encoder->AddFrame( memView, True{} ));
			_frameCount.Inc();

			if ( _frameCount.load() >= 100 )
			{
				TEST( _encoder->End() );
				_encoder = null;

				_frameCount.Inc();

				// play video in external video player
				CHECK( PlatformUtils::OpenURL( path ));
			}
		}
	};
}


extern void  Test_ScreenCapture ()
{
	Application_t::Run( MakeUnique<AppListener>() );

	// when tests end folder will be deleted
	ThreadUtils::MilliSleep( seconds{10} );

	TEST_PASSED();
}

#else

extern void  Test_ScreenCapture ()
{}

#endif // ENABLE_TEST
