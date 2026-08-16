// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "video/Public/VideoEncoder.h"

namespace AE::Video
{

/*
=================================================
	CalcBitrate
=================================================
*/
	Bitrate  IVideoEncoder::CalcBitrate (const IVideoEncoder::Config &cfg) __NE___
	{
		double	bpp	= 0.0;
		double	q	= double(Saturate( cfg.quality ));

		switch_enum( cfg.codec )
		{
			case EVideoCodec::MPEG4 :
			case EVideoCodec::H264 :	bpp = Lerp( 0.04,  0.12, q );	break;

			case EVideoCodec::H265 :
			case EVideoCodec::H266 :	bpp = Lerp( 0.025, 0.10, q );	break;

			case EVideoCodec::WEBP :
			case EVideoCodec::VP8 :
			case EVideoCodec::VP9 :		bpp = Lerp( 0.025, 0.10, q );	break;

			case EVideoCodec::AV1 :		bpp = Lerp( 0.02,  0.09, q );	break;

			case EVideoCodec::_Count :	break;
		}
		switch_end

		// bitrate is not linearly depends on pixel count
		ulong			pix_count		= ulong(cfg.dstDim.x) * cfg.dstDim.y;
		const ulong		base_pix_cnt	= 1920 * 1080;
		double			pix_scale		= double(pix_count) / double(base_pix_cnt);
						pix_scale		= Pow( pix_scale, 1.3 );	// more bits for more details

		// bitrate is not linearly depends on frame rate
		double			framerate		= cfg.framerate.ToFloat<double>();
		const double	base_framerate	= 30.0;
		double			fr_scale		= framerate / base_framerate;
						fr_scale		= Sqrt( fr_scale );

		double			qual_scale		= double(RemapClamp( Range{0.f, 1.f}, Range{0.8f, 1.5f}, Sqrt( Saturate( cfg.quality )) ));

		double			scale			= pix_scale * fr_scale * qual_scale;

		double			br				= double(base_pix_cnt) * base_framerate * scale * bpp;

		if ( br > 100.0e+6 )
			br = Round( br * 1.0e-6 ) * 1.0e+6;
		else
		if ( br > 100.0e+3 )
			br = Round( br * 1.0e-3 ) * 1.0e+3;

		return Bitrate{ulong(br)};
	}

} // AE::Video
