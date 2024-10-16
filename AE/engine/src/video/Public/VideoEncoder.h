// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:	yes
*/

#pragma once

#include "video/Public/Factory.h"

namespace AE::Video
{

	//
	// Video Encoder interface
	//

	class IVideoEncoder : public EnableRC<IVideoEncoder>
	{
	// types
	public:
		using ImageMemViewArr	= FixedArray< ImageMemView, 3 >;

		struct Config
		{
			EPixelFormat		srcFormat		= Default;
			EVideoFormat		dstFormat		= EVideoFormat::YUV420P;
			EVideoCodec			codec			= EVideoCodec::H265;
			EColorPreset		colorPreset		= EColorPreset::Unspecified;
			EFilter				filter			= Default;
			float				quality			= -1.f;						// valid: 0..1,  1.0 - high quality
			FrameRate			framerate		{30, 1};
			uint2				srcDim			= {1920, 1080};
			uint2				dstDim			= {0, 0};					// can be used for scaling
			Bitrate				bitrate			= BitrateMb{50};
			EHwAcceleration		hwAccelerated	= Default;					// use hardware acceleration on GPU or CPU
			EGraphicsDeviceID	targetGPU		= Default;					// hint for hardware acceleration on GPU
			ECPUVendor			targetCPU		= Default;					// hint for hardware acceleration on CPU (intrinsics)
			uint				threadCount		= 0;
		};


	// interface
	public:
		virtual ~IVideoEncoder ()														__NE___	{}

		ND_ virtual bool  Begin (const Config &cfg, const Path &filename)				__NE___	= 0;
		ND_ virtual bool  Begin (const Config &cfg, RC<WStream> temp, RC<WStream> dst)	__NE___	= 0;

		ND_ virtual bool  AddFrame (const ImageMemView &view, Bool endOnError)			__NE___	= 0;
		ND_ virtual bool  AddFrame (const ImageMemViewArr &view, Bool endOnError)		__NE___ = 0;

		ND_ virtual bool  End ()														__NE___	= 0;

		ND_ virtual bool	IsEncoding ()												C_NE___	= 0;
		ND_ virtual Config	GetConfig ()												C_NE___ = 0;


		// stateless
		ND_ virtual StringView	GetFileExtension (EVideoCodec codec)					C_NE___ = 0;
		ND_ virtual String		PrintCodecs (EVideoCodec codec)							C_Th___ = 0;
	};


} // AE::Video
