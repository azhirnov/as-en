// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:	yes
*/

#pragma once

#include "video/Public/Common.h"

namespace AE::Video
{

	//
	// Video Encoder interface
	//

	class IVideoEncoder : public EnableRC<IVideoEncoder>
	{
	// types
	public:
		using Bitrate_t		= DefaultPhysicalQuantity<ulong>::BitsPerSecond;
		using BitrateKb_t	= DefaultPhysicalQuantity<ulong>::KibiBitsPerSecond;
		using BitrateMb_t	= DefaultPhysicalQuantity<ulong>::MebiBitsPerSecond;
		using FrameRate_t	= FractionalI;

		struct Config
		{
			EPixelFormat		srcFormat		= Default;
			EVideoFormat		dstFormat		= EVideoFormat::YUV420P;
			EVideoCodec			codec			= EVideoCodec::H265;
			EColorPreset		colorPreset		= EColorPreset::Unspecified;
			EFilter				filter			= Default;
			float				quality			= -1.f;						// valid: 0..1,  1.0 - high quality
			FrameRate_t			framerate		{30, 1};
			uint2				srcSize			= {1920, 1080};
			uint2				dstSize			= {0, 0};					// can be used for scaling
			Bitrate_t			bitrate			= BitrateMb_t{50};
			bool				hwAccelerated	= false;					// use hardware acceleration on GPU or CPU
			EGraphicsDeviceID	targetGPU		= Default;					// hint for hardware acceleration on GPU
			ECPUVendor			targetCPU		= Default;					// hint for hardware acceleration on CPU (intrinsics)
		};


	// interface
	public:
		virtual ~IVideoEncoder ()														__NE___	{}

		ND_ virtual bool  Begin (const Config &cfg, const Path &filename)				__NE___	= 0;
		ND_ virtual bool  Begin (const Config &cfg, RC<WStream> temp, RC<WStream> dst)	__NE___	= 0;

		ND_ virtual bool  AddFrame (const ImageMemView &view, Bool endOnError)			__NE___	= 0;
		ND_ virtual bool  AddFrame (VideoImageID id, Bool endOnError)					__NE___	= 0;
		
		// TODO: AddFrameAsync()

		ND_ virtual bool  End ()														__NE___	= 0;

		ND_ virtual bool		IsEncoding ()											C_NE___	= 0;
		ND_ virtual Config		GetConfig ()											C_NE___ = 0;

		
		// stateless
		ND_ virtual StringView	GetFileExtension (EVideoCodec codec)					C_NE___ = 0;
		// TODO: get codecs


	// factory
	public:
		ND_ static RC<IVideoEncoder>  CreateFFmpegEncoder ()							__NE___;
		ND_ static RC<IVideoEncoder>  CreateGraphicsHWEncoder ()						__NE___;
	};


} // AE::Video
