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
		GIF,
		MPEG4,
		H264,
		H265,
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
		YUVA444P16LE,	// 64bpp
		YUV420P10LE,	// 15bpp
		NV12,			// 12bpp
		NV21,			// 12bpp
		P010LE,			// 10bpp
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
