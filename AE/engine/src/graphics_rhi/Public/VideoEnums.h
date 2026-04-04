// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/Common.h"

namespace AE::Graphics
{

	enum class EVideoCodecMode : ubyte
	{
		Encode,
		Decode,

		_Count,
		Unknown		= _Count,
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
		Unknown		= _Count,
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
		Unknown	= _Count,
	};


	enum class EStdVideoH265ProfileIdc : ubyte
	{
		Main,
		Main10,
		MainStillPicture,
		RangeExtensions,
		SCC_Extensions,

		_Count,
		Unknown	= _Count,
	};


	enum class EVideoDecodeH264PictureLayout : ubyte
	{
		Progressive,
		InterlacedInterleavedLines,
		InterlacedSeparatePlanes,

		_Count,
		Unknown	= _Count,
	};


	enum class EVideoFormat : ubyte
	{
		YUV420P,		// 12bpp	Y/U/V
		YUV422P,		// 16bpp	Y/U/V
		YUV444P,		// 24bpp	Y/U/V

		YUV420P10,		// 15bpp	Y/U/V
		YUV422P10,		// 20bpp	Y/U/V
		YUV444P10,		// 30bpp	Y/U/V

		YUV420P12,		// 18bpp	Y/U/V
		YUV422P12,		// 24bpp	Y/U/V
		YUV444P12,		// 36bpp	Y/U/V

		YUV420P16,		// 24bpp	Y/U/V
		YUV422P16,		// 32bpp	Y/U/V
		YUV444P16,		// 48bpp	Y/U/V

		YUVA420P,		// 12+8bpp	Y/U/V/A
		YUVA422P,		// 16+8bpp	Y/U/V/A
		YUVA444P,		// 24+8bpp	Y/U/V/A
		YUVA420P10,		// 25bpp	Y/U/V/A
		YUVA422P10,		// 30bpp	Y/U/V/A
		YUVA444P10,		// 40bpp	Y/U/V/A
		YUVA420P16,		// 40bpp	Y/U/V/A
		YUVA422P16,		// 48bpp	Y/U/V/A
		YUVA444P16,		// 64bpp	Y/U/V/A

		NV12,			// 12bpp	Y+UV (interleaved)
		P010,			// 10bpp	Y+UV (interleaved)
		P012,			// 12bpp	Y+UV (interleaved)
		P016,			// 16bpp	Y+UV (interleaved)

		NV16,			// 16bpp	Y+UV (interleaved)
		P210,			// 20bpp	Y+UV (interleaved)
		P212,			// 24bpp	Y+UV (interleaved)
		P216,			// 32bpp	Y+UV (interleaved)

		NV24,			// 24bpp	Y+UV (interleaved)
		P410,			// 30bpp	Y+UV (interleaved)
		P412,			// 36bpp	Y+UV (interleaved)
		P416,			// 48bpp	Y+UV (interleaved)

		NV21,			// 12bpp	Y+VU (interleaved)
		NV42,			// 24bpp	Y+VU (interleaved)
		NV20,			// 20bpp	Y+VU (interleaved)

		YUYV422,		// 16bpp	YUYV
		UYVY422,		// 16bpp	UYVY
		Y210,			// 20bpp	YUYV
		Y212,			// 24bpp	YUYV
		XV30,			// 32bpp
		XV36,			// 48bpp

		BGR0,			// 32bpp
		BGRA,			// 32bpp
		RGB0,			// 32bpp
		RGBA,			// 32bpp

		_Count,
		Unknown			= _Count,
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


	struct VideoH264
	{
		enum class EPictureType : ubyte
		{
			P,
			B,
			I,
			IDR,			// Instantaneous Decoder Refresh. After an IDR, the decoder can throw away all old references and still decode correctly.
			Unknown
		};
	};


	struct VideoH265
	{
		enum class EPictureType : ubyte
		{
			P,
			B,
			I,
			IDR,			// Instantaneous Decoder Refresh. After an IDR, the decoder can throw away all old references and still decode correctly.
			Unknown
		};
	};


	struct VideoAV1
	{
		enum class EFrameType : ubyte
		{
			Key,
			Inter,
			IntraOnly,
			Switch,
			Unknown
		};

		enum class EPredictionMode : ubyte
		{
			Unknown,
			InfraOnly,
			SingleReference,
			UnidirectionalCompound,
			BidirectionalCompound,
		};
	};


} // AE::Graphics
