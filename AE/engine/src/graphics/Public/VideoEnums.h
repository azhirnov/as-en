// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

	enum class EVideoCodecMode : ubyte
	{
		Encode,
		Decode,

		_Count,
		Unknown		= 0xFF,
	};


	enum class EVideoCodec : ubyte
	{
		MPEG4,
		H264,
		H265,	// HEVC
		H266,	// VVC
		WEBP,
		VP8,
		VP9,
		AV1,

		_Count,
		Unknown		= 0xFF,
	};


	enum class EVideoChromaSubsampling : ubyte
	{
		Unknown			= 0,
		Monochrome,
		_420,			// 4:2:0
		_422,			// 4:2:2
		_444,			// 4:4:4
	};


	enum class EVideoComponentBitDepth : ubyte
	{
		Unknown			= 0,
		_8,
		_10,
		_12,
	};


	enum class EStdVideoH264ProfileIdc : ubyte
	{
		Baseline,
		Main,
		High,
		Hight444_Predictive,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EStdVideoH265ProfileIdc : ubyte
	{
		Main,
		Main10,
		MainStillPicture,
		RangeExtensions,
		SCC_Extensions,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EVideoDecodeH264PictureLayout : ubyte
	{
		Progressive,
		InterlacedInterleavedLines,
		InterlacedSeparatePlanes,

		_Count,
		Unknown	= 0xFF,
	};


	enum class EVideoFormat : ubyte
	{
		YUV420P,		// 12bpp
		YUV422P,		// 16bpp
		YUV444P,		// 24bpp

		YUV420P10,		// 15bpp
		YUV422P10,		// 20bpp
		YUV444P10,		// 30bpp

		YUV420P12,		// 18bpp
		YUV422P12,		// 24bpp
		YUV444P12,		// 36bpp

		YUV420P16,		// 24bpp
		YUV422P16,		// 32bpp
		YUV444P16,		// 48bpp

		YUVA420P,		// 12+8bpp
		YUVA422P,		// 16+8bpp
		YUVA444P,		// 24+8bpp
		YUVA420P10,		// 25bpp
		YUVA422P10,		// 30bpp
		YUVA444P10,		// 40bpp
		YUVA420P16,		// 40bpp
		YUVA422P16,		// 48bpp
		YUVA444P16,		// 64bpp

		NV12,			// 12bpp
		P010,			// 10bpp
		P012,			// 12bpp
		P016,			// 16bpp

		NV16,			// 16bpp
		P210,			// 20bpp
		P212,			// 24bpp
		P216,			// 32bpp

		NV24,			// 24bpp
		P410,			// 30bpp
		P412,			// 36bpp
		P416,			// 48bpp

		NV21,			// 12bpp
		NV42,			// 24bpp
		NV20,			// 20bpp

		YUYV422,		// 16bpp
		UYVY422,		// 16bpp
		Y210,			// 20bpp
		Y212,			// 24bpp
		XV30,			// 32bpp
		XV36,			// 48bpp

		BGR0,			// 32bpp
		BGRA,			// 32bpp
		RGB0,			// 32bpp
		RGBA,			// 32bpp

		_Count,
		Unknown			= 0xFF,
	};


	enum class EVideoBufferUsage : ubyte
	{
		Unknown		= 0,

		DecodeSrc	= 1 << 0,		// source video bitstream
		DecodeDst	= 1 << 1,		// reserved

		EncodeSrc	= 1 << 2,		// reserved
		EncodeDst	= 1 << 3,		// destination video bitstream

		_Last,
		All			= ((_Last - 1) << 1) - 1
	};
	AE_BIT_OPERATORS( EVideoBufferUsage );


	enum class EVideoImageUsage : ubyte
	{
		Unknown		= 0,

		DecodeSrc	= 1 << 0,		// reserved
		DecodeDst	= 1 << 1,		// decode output picture
		DecodeDpb	= 1 << 2,		// decode output picture and reconstructed picture

		EncodeSrc	= 1 << 3,		// encode input picture
		EncodeDst	= 1 << 4,		// reserved
		EncodeDpb	= 1 << 5,		// encode input picture and reconstructed picture

		_Last,
		All			= ((_Last - 1) << 1) - 1
	};
	AE_BIT_OPERATORS( EVideoImageUsage );


} // AE::Graphics
