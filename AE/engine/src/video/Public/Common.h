// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/InputSurface.h"
#include "platform/Public/OutputSurface.h"

namespace AE::Video
{
	using namespace AE::Base;

	using App::IInputSurface;
	using App::IOutputSurface;

	using AE::Graphics::ImageMemView;
	using AE::Graphics::EPixelFormat;
	using AE::Graphics::EGraphicsDeviceID;
	using AE::Graphics::EVideoCodec;
	using AE::Graphics::EVideoFormat;
	using AE::Graphics::VideoImageID;

	using AE::Threading::AsyncTask;
	using AE::Threading::Atomic;


	enum class EFilter : ubyte
	{
		Fast,
		Bilinear,
		Bicubic,
		_Count,
		Unknown		= 0xFF,
	};


	enum class EMediaType : ubyte
	{
		Video,
		Audio,
		_Count,
		Unknown		= 0xFF,
	};


	enum class EColorPreset : ubyte
	{
		Unspecified,
		MPEG_BT709,			// mpeg - limited range,  color space: BT709,     linearization function: BT709
		MPEG_BT709_2,		// mpeg - limited range,  color space: BT709,     linearization function: IEC61966_2_4
		JPEG_BT709,			// jpeg - full range,     color space: BT709,     linearization function: BT709
		MPEG_BT2020_1,		// mpeg - limited range,  color space: BT2020,    linearization function: SMPTE2084
		MPEG_BT2020_2,		// mpeg - limited range,  color space: BT2020,    linearization function: ARIB_STD_B67
		MPEG_SMPTE170M,		// mpeg - limited range,  color space: SMPTE170M, linearization function: BT709
		_Count,
		Unknown		= 0xFF,
	};


} // AE::Video
