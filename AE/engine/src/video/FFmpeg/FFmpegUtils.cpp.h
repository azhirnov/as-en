// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_FFMPEG
# include "graphics/Private/EnumUtils.h"
# include "video/Public/Common.h"
# include "video/FFmpeg/FFmpegLoader.h"
# include "video/Impl/EnumToString.cpp.h"

namespace AE::Video
{

/*
=================================================
    ToAVRational
=================================================
*/
    ND_ inline AVRational  ToAVRational (FractionalI fact)
    {
        return AVRational{ fact.num, fact.den };
    }

    ND_ inline AVRational  ToAVRationalRec (FractionalI fact)
    {
        return AVRational{ fact.den, fact.num };
    }

/*
=================================================
    ToFractional
=================================================
*/
    ND_ inline FractionalI  ToFractional (AVRational rational)
    {
        return FractionalI{ rational.num, rational.den };
    }

/*
=================================================
    EnumCast (EVideoFormat)
=================================================
*/
    ND_ inline AVPixelFormat  EnumCast (EVideoFormat fmt)
    {
        BEGIN_ENUM_CHECKS();
        switch ( fmt )
        {
            case EVideoFormat::YUV420P :        return AV_PIX_FMT_YUV420P;
            case EVideoFormat::YUV422P :        return AV_PIX_FMT_YUV422P;
            case EVideoFormat::YUV444P :        return AV_PIX_FMT_YUV444P;
            case EVideoFormat::YUVA444P16LE :   return AV_PIX_FMT_YUVA444P16LE;
            case EVideoFormat::YUV420P10LE :    return AV_PIX_FMT_YUV420P10LE;
            case EVideoFormat::NV12 :           return AV_PIX_FMT_NV12;
            case EVideoFormat::NV21 :           return AV_PIX_FMT_NV21;
            case EVideoFormat::P010LE :         return AV_PIX_FMT_P010LE;
            case EVideoFormat::Unknown :
            case EVideoFormat::_Count :         break;
        }
        END_ENUM_CHECKS();
        return AV_PIX_FMT_NONE;
    }

/*
=================================================
    EnumCast (AVPixelFormat)
=================================================
*/
    ND_ inline EVideoFormat  EnumCast (AVPixelFormat fmt)
    {
        StaticAssert( uint(EVideoFormat::_Count) == 8 );
        switch ( fmt )
        {
            case AV_PIX_FMT_YUV420P :           return EVideoFormat::YUV420P;
            case AV_PIX_FMT_YUVJ420P :          return EVideoFormat::YUV420P;
            case AV_PIX_FMT_YUV422P :           return EVideoFormat::YUV422P;
            case AV_PIX_FMT_YUV444P :           return EVideoFormat::YUV444P;
            case AV_PIX_FMT_YUVA444P16LE :      return EVideoFormat::YUVA444P16LE;
            case AV_PIX_FMT_YUV420P10LE :       return EVideoFormat::YUV420P10LE;
            case AV_PIX_FMT_NV12 :              return EVideoFormat::NV12;
            case AV_PIX_FMT_NV21 :              return EVideoFormat::NV21;
            case AV_PIX_FMT_P010LE :            return EVideoFormat::P010LE;
        }
        return Default;
    }

/*
=================================================
    EnumCast (EPixelFormat)
=================================================
*/
    ND_ inline AVPixelFormat  EnumCast (EPixelFormat fmt)
    {
        switch ( fmt )
        {
            case EPixelFormat::RGB8_UNorm :
            case EPixelFormat::sRGB8 :          return AV_PIX_FMT_RGB24;

            case EPixelFormat::RGBA8_UNorm :
            case EPixelFormat::sRGB8_A8 :       return AV_PIX_FMT_RGB0;     // AV_PIX_FMT_RGBA

            case EPixelFormat::BGR8_UNorm :
            case EPixelFormat::sBGR8 :          return AV_PIX_FMT_BGR24;

            case EPixelFormat::BGRA8_UNorm :
            case EPixelFormat::sBGR8_A8 :       return AV_PIX_FMT_BGR0;     // AV_PIX_FMT_BGRA

            case EPixelFormat::RGB16_UNorm :    return AV_PIX_FMT_RGB48BE;
            case EPixelFormat::RGBA16_UNorm :   return AV_PIX_FMT_RGBA64BE;
        }
        return AV_PIX_FMT_NONE;
    }

/*
=================================================
    EnumCast (EFilter)
=================================================
*/
    ND_ inline int  EnumCast (EFilter value)
    {
        StaticAssert( uint(EFilter::_Count) == 3 );
        switch ( value )
        {
            case EFilter::Fast :        return SWS_FAST_BILINEAR;
            case EFilter::Bilinear :    return SWS_BILINEAR;
            case EFilter::Bicubic :     return SWS_BICUBIC;
            case EFilter::Unknown :
            case EFilter::_Count :      break;
        }
        return SWS_FAST_BILINEAR;
    }

/*
=================================================
    EnumCast (AVMediaType)
=================================================
*/
    ND_ inline EMediaType  EnumCast (AVMediaType value)
    {
        StaticAssert( uint(EMediaType::_Count) == 2 );
        switch ( value )
        {
            case AVMEDIA_TYPE_VIDEO :   return EMediaType::Video;
            case AVMEDIA_TYPE_AUDIO :   return EMediaType::Audio;
        }
        return Default;
    }

/*
=================================================
    EnumCast (AVCodecID)
=================================================
*/
    ND_ inline EVideoCodec  EnumCast (AVCodecID value)
    {
        StaticAssert( uint(EVideoCodec::_Count) == 8 );
        switch ( value )
        {
            case AV_CODEC_ID_GIF :      return EVideoCodec::GIF;
            case AV_CODEC_ID_MPEG4 :    return EVideoCodec::MPEG4;
            case AV_CODEC_ID_H264 :     return EVideoCodec::H264;
            case AV_CODEC_ID_H265 :     return EVideoCodec::H265;
            case AV_CODEC_ID_WEBP :     return EVideoCodec::WEBP;
            case AV_CODEC_ID_VP8 :      return EVideoCodec::VP8;
            case AV_CODEC_ID_VP9 :      return EVideoCodec::VP9;
            case AV_CODEC_ID_AV1 :      return EVideoCodec::AV1;
        }
        return Default;
    }

/*
=================================================
    EnumCast (EColorPreset)
----
    The signal range, in 'color_range'
    The YUV/RGB transformation matrix, in 'colorspace'
    The linearization function (a.k.a. transformation characteristics), in 'color_trc'
    The RGB/XYZ matrix, in 'color_primaries'
=================================================
*/
    ND_ inline EColorPreset  EnumCast (AVColorRange                     colorRange,
                                       AVColorPrimaries                 colorPrimaries,
                                       AVColorTransferCharacteristic    colorTrc,
                                       AVColorSpace                     colorSpace,
                                       AVChromaLocation                 )
    {
        //ASSERT( chromaLocation == AVCHROMA_LOC_UNSPECIFIED    or
        //      chromaLocation == AVCHROMA_LOC_LEFT         or
        //      chromaLocation == AVCHROMA_LOC_TOPLEFT );

        if ( (colorRange    == AVCOL_RANGE_MPEG         or colorRange == AVCOL_RANGE_UNSPECIFIED) and
             colorPrimaries == AVCOL_PRI_UNSPECIFIED    and
             colorTrc       == AVCOL_TRC_UNSPECIFIED    and
             colorSpace     == AVCOL_SPC_UNSPECIFIED    )
            return EColorPreset::Unspecified;

        if ( colorRange     == AVCOL_RANGE_MPEG         and
             (colorPrimaries== AVCOL_PRI_UNSPECIFIED    or colorPrimaries   == AVCOL_PRI_BT709) and
             (colorTrc      == AVCOL_TRC_UNSPECIFIED    or colorTrc         == AVCOL_TRC_BT709) and
             colorSpace     == AVCOL_SPC_BT709          )
            return EColorPreset::MPEG_BT709;

        if ( colorRange     == AVCOL_RANGE_MPEG         and
             (colorPrimaries== AVCOL_PRI_UNSPECIFIED    or colorPrimaries   == AVCOL_PRI_BT709) and
             colorTrc       == AVCOL_TRC_IEC61966_2_4   and
             colorSpace     == AVCOL_SPC_BT709          )
            return EColorPreset::MPEG_BT709_2;

        if ( colorRange     == AVCOL_RANGE_JPEG         and
             (colorPrimaries== AVCOL_PRI_UNSPECIFIED    or colorPrimaries   == AVCOL_PRI_BT709) and
             (colorTrc      == AVCOL_TRC_UNSPECIFIED    or colorTrc         == AVCOL_TRC_BT709) and
             colorSpace     == AVCOL_SPC_BT709          )
            return EColorPreset::JPEG_BT709;

        if ( colorRange     == AVCOL_RANGE_MPEG         and
             colorPrimaries == AVCOL_PRI_BT2020         and
             colorTrc       == AVCOL_TRC_SMPTE2084      and
             colorSpace     == AVCOL_SPC_BT2020_NCL     )
            return EColorPreset::MPEG_BT2020_1;

        if ( colorRange     == AVCOL_RANGE_MPEG         and
             colorPrimaries == AVCOL_PRI_BT2020         and
             colorTrc       == AVCOL_TRC_ARIB_STD_B67   and
             colorSpace     == AVCOL_SPC_BT2020_NCL     )
            return EColorPreset::MPEG_BT2020_2;

        if ( colorRange     == AVCOL_RANGE_MPEG         and
             colorPrimaries == AVCOL_PRI_SMPTE170M      and
             colorTrc       == AVCOL_TRC_BT709          and
             colorSpace     == AVCOL_SPC_SMPTE170M      )
            return EColorPreset::MPEG_SMPTE170M;

        return Default;
    }

/*
=================================================
    EnumCast (EColorPreset)
=================================================
*/
    ND_ inline bool   EnumCast (EColorPreset                        preset,
                                OUT AVColorRange                    &colorRange,
                                OUT AVColorPrimaries                &colorPrimaries,
                                OUT AVColorTransferCharacteristic   &colorTrc,
                                OUT AVColorSpace                    &colorSpace,
                                OUT AVChromaLocation                &chromaLocation)
    {
        BEGIN_ENUM_CHECKS();
        switch ( preset )
        {
            case EColorPreset::Unspecified :
                colorRange      = AVCOL_RANGE_UNSPECIFIED;
                colorPrimaries  = AVCOL_PRI_UNSPECIFIED;
                colorTrc        = AVCOL_TRC_UNSPECIFIED;
                colorSpace      = AVCOL_SPC_UNSPECIFIED;
                chromaLocation  = AVCHROMA_LOC_UNSPECIFIED;
                return true;

            case EColorPreset::MPEG_BT709 :
                colorRange      = AVCOL_RANGE_MPEG;
                colorPrimaries  = AVCOL_PRI_BT709;
                colorTrc        = AVCOL_TRC_BT709;
                colorSpace      = AVCOL_SPC_BT709;
                chromaLocation  = AVCHROMA_LOC_LEFT;
                return true;

            case EColorPreset::MPEG_BT709_2 :
                colorRange      = AVCOL_RANGE_MPEG;
                colorPrimaries  = AVCOL_PRI_BT709;
                colorTrc        = AVCOL_TRC_IEC61966_2_4;
                colorSpace      = AVCOL_SPC_BT709;
                chromaLocation  = AVCHROMA_LOC_LEFT;
                return true;

            case EColorPreset::JPEG_BT709 :
                colorRange      = AVCOL_RANGE_JPEG;
                colorPrimaries  = AVCOL_PRI_BT709;
                colorTrc        = AVCOL_TRC_BT709;
                colorSpace      = AVCOL_SPC_BT709;
                chromaLocation  = AVCHROMA_LOC_LEFT;
                return true;

            case EColorPreset::MPEG_BT2020_1 :
                colorRange      = AVCOL_RANGE_MPEG;
                colorPrimaries  = AVCOL_PRI_BT2020;
                colorTrc        = AVCOL_TRC_SMPTE2084;
                colorSpace      = AVCOL_SPC_BT2020_NCL;
                chromaLocation  = AVCHROMA_LOC_LEFT;
                return true;

            case EColorPreset::MPEG_BT2020_2 :
                colorRange      = AVCOL_RANGE_MPEG;
                colorPrimaries  = AVCOL_PRI_BT2020;
                colorTrc        = AVCOL_TRC_ARIB_STD_B67;
                colorSpace      = AVCOL_SPC_BT2020_NCL;
                chromaLocation  = AVCHROMA_LOC_LEFT;
                return true;

            case EColorPreset::MPEG_SMPTE170M :
                colorRange      = AVCOL_RANGE_MPEG;
                colorPrimaries  = AVCOL_PRI_SMPTE170M;
                colorTrc        = AVCOL_TRC_BT709;
                colorSpace      = AVCOL_SPC_SMPTE170M;
                chromaLocation  = AVCHROMA_LOC_LEFT;
                return true;

            case EColorPreset::Unknown :
            case EColorPreset::_Count :
            default :
                return false;
        }
        END_ENUM_CHECKS();
    }

} // AE::Video
//-----------------------------------------------------------------------------



namespace AE::Base
{
/*
=================================================
    ToString (AVMediaType)
=================================================
*/
    ND_ inline StringView  ToString (AVMediaType value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AVMEDIA_TYPE_UNKNOWN :     return "Unknown";
            case AVMEDIA_TYPE_VIDEO :       return "Video";
            case AVMEDIA_TYPE_AUDIO :       return "Audio";
            case AVMEDIA_TYPE_DATA :        return "Data";
            case AVMEDIA_TYPE_SUBTITLE :    return "Subtitle";
            case AVMEDIA_TYPE_ATTACHMENT :  return "Attachment";
            case AVMEDIA_TYPE_NB :          break;
        }
        END_ENUM_CHECKS();
        return "Undefined";
    }

/*
=================================================
    ToString (AVPixelFormat)
=================================================
*/
#ifdef AE_CFG_RELEASE
    ND_ inline StringView  ToString (AVPixelFormat value)
    {
        return ToString( Video::EnumCast( value ));
    }

#else

    ND_ inline StringView  ToString (AVPixelFormat value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AV_PIX_FMT_NONE :              break;
            case AV_PIX_FMT_YUV420P :           return "YUV420P  - planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)";
            case AV_PIX_FMT_YUYV422 :           return "YUYV422  - packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr";
            case AV_PIX_FMT_RGB24 :             return "RGB24  - packed RGB 8:8:8, 24bpp, RGBRGB";
            case AV_PIX_FMT_BGR24 :             return "BGR24  - packed RGB 8:8:8, 24bpp, BGRBGR";
            case AV_PIX_FMT_YUV422P :           return "YUV422P  - planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)";
            case AV_PIX_FMT_YUV444P :           return "YUV444P  - planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)";
            case AV_PIX_FMT_YUV410P :           return "YUV410P  - planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)";
            case AV_PIX_FMT_YUV411P :           return "YUV411P  - planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)";
            case AV_PIX_FMT_GRAY8 :             return "GRAY8  - Y, 8bpp";
            case AV_PIX_FMT_MONOWHITE :         return "MONOWHITE  - Y,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb";
            case AV_PIX_FMT_MONOBLACK :         return "MONOBLACK  - Y,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb";
            case AV_PIX_FMT_PAL8 :              return "PAL8  - 8 bits with AV_PIX_FMT_RGB32 palette";
            case AV_PIX_FMT_YUVJ420P :          return "YUVJ420P  - planar YUV 4:2:0, 12bpp, full scale (JPEG)";
            case AV_PIX_FMT_YUVJ422P :          return "YUVJ422P  - planar YUV 4:2:2, 16bpp, full scale (JPEG)";
            case AV_PIX_FMT_YUVJ444P :          return "YUVJ444P  - planar YUV 4:4:4, 24bpp, full scale (JPEG)";
            case AV_PIX_FMT_UYVY422 :           return "UYVY422  - packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1";
            case AV_PIX_FMT_UYYVYY411 :         return "UYYVYY411  - packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3";
            case AV_PIX_FMT_BGR8 :              return "BGR8  - packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)";
            case AV_PIX_FMT_BGR4 :              return "BGR4  - packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits";
            case AV_PIX_FMT_BGR4_BYTE :         return "BGR4_BYTE  - packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)";
            case AV_PIX_FMT_RGB8 :              return "RGB8  - packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)";
            case AV_PIX_FMT_RGB4 :              return "RGB4  - packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits";
            case AV_PIX_FMT_RGB4_BYTE :         return "RGB4_BYTE  - packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)";
            case AV_PIX_FMT_NV12 :              return "NV12  - planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)";
            case AV_PIX_FMT_NV21 :              return "NV21  - planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, U and V bytes are swapped";
            case AV_PIX_FMT_ARGB :              return "ARGB  - packed ARGB 8:8:8:8, 32bpp, ARGBARGB";
            case AV_PIX_FMT_RGBA :              return "RGBA  - packed RGBA 8:8:8:8, 32bpp, RGBARGBA";
            case AV_PIX_FMT_ABGR :              return "ABGR  - packed ABGR 8:8:8:8, 32bpp, ABGRABGR";
            case AV_PIX_FMT_BGRA :              return "BGRA  - packed BGRA 8:8:8:8, 32bpp, BGRABGRA";
            case AV_PIX_FMT_GRAY16BE :          return "GRAY16BE  - Y, 16bpp, big-endian";
            case AV_PIX_FMT_GRAY16LE :          return "GRAY16LE  - Y, 16bpp, little-endian";
            case AV_PIX_FMT_YUV440P :           return "YUV440P  - planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)";
            case AV_PIX_FMT_YUVJ440P :          return "YUVJ440P  - planar YUV 4:4:0 full scale (JPEG)";
            case AV_PIX_FMT_YUVA420P :          return "YUVA420P  - planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)";
            case AV_PIX_FMT_RGB48BE :           return "RGB48BE  - packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian";
            case AV_PIX_FMT_RGB48LE :           return "RGB48LE  - packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian";
            case AV_PIX_FMT_RGB565BE :          return "RGB565BE  - packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian";
            case AV_PIX_FMT_RGB565LE :          return "RGB565LE  - packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian";
            case AV_PIX_FMT_RGB555BE :          return "RGB555BE  - packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined";
            case AV_PIX_FMT_RGB555LE :          return "RGB555LE  - packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined";
            case AV_PIX_FMT_BGR565BE :          return "BGR565BE  - packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian";
            case AV_PIX_FMT_BGR565LE :          return "BGR565LE  - packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian";
            case AV_PIX_FMT_BGR555BE :          return "BGR555BE  - packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined";
            case AV_PIX_FMT_BGR555LE :          return "BGR555LE  - packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined";
            case AV_PIX_FMT_VAAPI :             return "VAAPI";
            case AV_PIX_FMT_YUV420P16LE :       return "YUV420P16LE  - planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV420P16BE :       return "YUV420P16BE  - planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian";
            case AV_PIX_FMT_YUV422P16LE :       return "YUV422P16LE  - planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV422P16BE :       return "YUV422P16BE  - planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV444P16LE :       return "YUV444P16LE  - planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV444P16BE :       return "YUV444P16BE  - planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian";
            case AV_PIX_FMT_DXVA2_VLD :         return "DXVA2_VLD  - HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer";
            case AV_PIX_FMT_RGB444LE :          return "RGB444LE  - packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined";
            case AV_PIX_FMT_RGB444BE :          return "RGB444BE  - packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined";
            case AV_PIX_FMT_BGR444LE :          return "BGR444LE  - packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined";
            case AV_PIX_FMT_BGR444BE :          return "BGR444BE  - packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined";
            case AV_PIX_FMT_YA8 :               return "YA8  - 8 bits gray, 8 bits alpha";
            case AV_PIX_FMT_BGR48BE :           return "BGR48BE  - packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian";
            case AV_PIX_FMT_BGR48LE :           return "BGR48LE  - packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian";
            case AV_PIX_FMT_YUV420P9BE :        return "YUV420P9BE  - planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian";
            case AV_PIX_FMT_YUV420P9LE :        return "YUV420P9LE  - planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV420P10BE :       return "YUV420P10BE  - planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian";
            case AV_PIX_FMT_YUV420P10LE :       return "YUV420P10LE  - planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV422P10BE :       return "YUV422P10BE  - planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV422P10LE :       return "YUV422P10LE  - planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV444P9BE :        return "YUV444P9BE  - planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV444P9LE :        return "YUV444P9LE  - planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV444P10BE :       return "YUV444P10BE  - planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV444P10LE :       return "YUV444P10LE  - planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV422P9BE :        return "YUV422P9BE  - planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV422P9LE :        return "YUV422P9LE  - planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian";
            case AV_PIX_FMT_GBRP :              return "GBRP  - planar GBR 4:4:4 24bpp";
            case AV_PIX_FMT_GBRP9BE :           return "GBRP9BE  - planar GBR 4:4:4 27bpp, big-endian";
            case AV_PIX_FMT_GBRP9LE :           return "GBRP9LE  - planar GBR 4:4:4 27bpp, little-endian";
            case AV_PIX_FMT_GBRP10BE :          return "GBRP10BE  - planar GBR 4:4:4 30bpp, big-endian";
            case AV_PIX_FMT_GBRP10LE :          return "GBRP10LE  - planar GBR 4:4:4 30bpp, little-endian";
            case AV_PIX_FMT_GBRP16BE :          return "GBRP16BE  - planar GBR 4:4:4 48bpp, big-endian";
            case AV_PIX_FMT_GBRP16LE :          return "GBRP16LE  - planar GBR 4:4:4 48bpp, little-endian";
            case AV_PIX_FMT_YUVA422P :          return "YUVA422P  - planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)";
            case AV_PIX_FMT_YUVA444P :          return "YUVA444P  - planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)";
            case AV_PIX_FMT_YUVA420P9BE :       return "YUVA420P9BE  - planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian";
            case AV_PIX_FMT_YUVA420P9LE :       return "YUVA420P9LE  - planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian";
            case AV_PIX_FMT_YUVA422P9BE :       return "YUVA422P9BE  - planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian";
            case AV_PIX_FMT_YUVA422P9LE :       return "YUVA422P9LE  - planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian";
            case AV_PIX_FMT_YUVA444P9BE :       return "YUVA444P9BE  - planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian";
            case AV_PIX_FMT_YUVA444P9LE :       return "YUVA444P9LE  - planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian";
            case AV_PIX_FMT_YUVA420P10BE :      return "YUVA420P10BE  - planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)";
            case AV_PIX_FMT_YUVA420P10LE :      return "YUVA420P10LE  - planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)";
            case AV_PIX_FMT_YUVA422P10BE :      return "YUVA422P10BE  - planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)";
            case AV_PIX_FMT_YUVA422P10LE :      return "YUVA422P10LE  - planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)";
            case AV_PIX_FMT_YUVA444P10BE :      return "YUVA444P10BE  - planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)";
            case AV_PIX_FMT_YUVA444P10LE :      return "YUVA444P10LE  - planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)";
            case AV_PIX_FMT_YUVA420P16BE :      return "YUVA420P16BE  - planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)";
            case AV_PIX_FMT_YUVA420P16LE :      return "YUVA420P16LE  - planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)";
            case AV_PIX_FMT_YUVA422P16BE :      return "YUVA422P16BE  - planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)";
            case AV_PIX_FMT_YUVA422P16LE :      return "YUVA422P16LE  - planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)";
            case AV_PIX_FMT_YUVA444P16BE :      return "YUVA444P16BE  - planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)";
            case AV_PIX_FMT_YUVA444P16LE :      return "YUVA444P16LE  - planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)";
            case AV_PIX_FMT_VDPAU :             return "VDPAU";
            case AV_PIX_FMT_XYZ12LE :           return "XYZ12LE  - packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0";
            case AV_PIX_FMT_XYZ12BE :           return "XYZ12BE  - packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0";
            case AV_PIX_FMT_NV16 :              return "NV16  - interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)";
            case AV_PIX_FMT_NV20LE :            return "NV20LE  - interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian";
            case AV_PIX_FMT_NV20BE :            return "NV20BE  - interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian";
            case AV_PIX_FMT_RGBA64BE :          return "RGBA64BE  - packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian";
            case AV_PIX_FMT_RGBA64LE :          return "RGBA64LE  - packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian";
            case AV_PIX_FMT_BGRA64BE :          return "BGRA64BE  - packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian";
            case AV_PIX_FMT_BGRA64LE :          return "BGRA64LE  - packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian";
            case AV_PIX_FMT_YVYU422 :           return "YVYU422  - packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb";
            case AV_PIX_FMT_YA16BE :            return "YA16BE  - 16 bits gray, 16 bits alpha (big-endian)";
            case AV_PIX_FMT_YA16LE :            return "YA16LE  - 16 bits gray, 16 bits alpha (little-endian)";
            case AV_PIX_FMT_GBRAP :             return "GBRAP  - planar GBRA 4:4:4:4 32bpp";
            case AV_PIX_FMT_GBRAP16BE :         return "GBRAP16BE  - planar GBRA 4:4:4:4 64bpp, big-endian";
            case AV_PIX_FMT_GBRAP16LE :         return "GBRAP16LE  - planar GBRA 4:4:4:4 64bpp, little-endian";
            case AV_PIX_FMT_QSV :               return "QSV";
            case AV_PIX_FMT_MMAL :              return "MMAL";
            case AV_PIX_FMT_D3D11VA_VLD :       return "D3D11VA_VLD";
            case AV_PIX_FMT_CUDA :              return "CUDA";
            case AV_PIX_FMT_0RGB :              return "0RGB  - packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined";
            case AV_PIX_FMT_RGB0 :              return "RGB0  - packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined";
            case AV_PIX_FMT_0BGR :              return "0BGR  - packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined";
            case AV_PIX_FMT_BGR0 :              return "BGR0  - packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined";
            case AV_PIX_FMT_YUV420P12BE :       return "YUV420P12BE  - planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian";
            case AV_PIX_FMT_YUV420P12LE :       return "YUV420P12LE  - planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV420P14BE :       return "YUV420P14BE  - planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian";
            case AV_PIX_FMT_YUV420P14LE :       return "YUV420P14LE  - planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV422P12BE :       return "YUV422P12BE  - planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV422P12LE :       return "YUV422P12LE  - planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV422P14BE :       return "YUV422P14BE  - planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV422P14LE :       return "YUV422P14LE  - planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV444P12BE :       return "YUV444P12BE  - planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV444P12LE :       return "YUV444P12LE  - planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian";
            case AV_PIX_FMT_YUV444P14BE :       return "YUV444P14BE  - planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian";
            case AV_PIX_FMT_YUV444P14LE :       return "YUV444P14LE  - planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian";
            case AV_PIX_FMT_GBRP12BE :          return "GBRP12BE  - planar GBR 4:4:4 36bpp, big-endian";
            case AV_PIX_FMT_GBRP12LE :          return "GBRP12LE  - planar GBR 4:4:4 36bpp, little-endian";
            case AV_PIX_FMT_GBRP14BE :          return "GBRP14BE  - planar GBR 4:4:4 42bpp, big-endian";
            case AV_PIX_FMT_GBRP14LE :          return "GBRP14LE  - planar GBR 4:4:4 42bpp, little-endian";
            case AV_PIX_FMT_YUVJ411P :          return "YUVJ411P  - planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG)";
            case AV_PIX_FMT_BAYER_BGGR8 :       return "BAYER_BGGR8  - bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples";
            case AV_PIX_FMT_BAYER_RGGB8 :       return "BAYER_RGGB8  - bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples";
            case AV_PIX_FMT_BAYER_GBRG8 :       return "BAYER_GBRG8  - bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples";
            case AV_PIX_FMT_BAYER_GRBG8 :       return "BAYER_GRBG8  - bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples";
            case AV_PIX_FMT_BAYER_BGGR16LE :    return "BAYER_BGGR16LE  - bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian";
            case AV_PIX_FMT_BAYER_BGGR16BE :    return "BAYER_BGGR16BE  - bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian";
            case AV_PIX_FMT_BAYER_RGGB16LE :    return "BAYER_RGGB16LE  - bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian";
            case AV_PIX_FMT_BAYER_RGGB16BE :    return "BAYER_RGGB16BE  - bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian";
            case AV_PIX_FMT_BAYER_GBRG16LE :    return "BAYER_GBRG16LE  - bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian";
            case AV_PIX_FMT_BAYER_GBRG16BE :    return "BAYER_GBRG16BE  - bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian";
            case AV_PIX_FMT_BAYER_GRBG16LE :    return "BAYER_GRBG16LE  - bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian";
            case AV_PIX_FMT_BAYER_GRBG16BE :    return "BAYER_GRBG16BE  - bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian";
            case AV_PIX_FMT_XVMC :              return "XVMC  - XVideo Motion Acceleration via common packet passing";
            case AV_PIX_FMT_YUV440P10LE :       return "YUV440P10LE  - planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV440P10BE :       return "YUV440P10BE  - planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian";
            case AV_PIX_FMT_YUV440P12LE :       return "YUV440P12LE  - planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian";
            case AV_PIX_FMT_YUV440P12BE :       return "YUV440P12BE  - planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian";
            case AV_PIX_FMT_AYUV64LE :          return "AYUV64LE  - packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian";
            case AV_PIX_FMT_AYUV64BE :          return "AYUV64BE  - packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian";
            case AV_PIX_FMT_VIDEOTOOLBOX :      return "VIDEOTOOLBOX  - hardware decoding through Videotoolbox";
            case AV_PIX_FMT_P010LE :            return "P010LE  - like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian";
            case AV_PIX_FMT_P010BE :            return "P010BE  - like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian";
            case AV_PIX_FMT_GBRAP12BE :         return "GBRAP12BE  - planar GBR 4:4:4:4 48bpp, big-endian";
            case AV_PIX_FMT_GBRAP12LE :         return "GBRAP12LE  - planar GBR 4:4:4:4 48bpp, little-endian";
            case AV_PIX_FMT_GBRAP10BE :         return "GBRAP10BE  - planar GBR 4:4:4:4 40bpp, big-endian";
            case AV_PIX_FMT_GBRAP10LE :         return "GBRAP10LE  - planar GBR 4:4:4:4 40bpp, little-endian";
            case AV_PIX_FMT_MEDIACODEC :        return "MEDIACODEC  - hardware decoding through MediaCodec";
            case AV_PIX_FMT_GRAY12BE :          return "GRAY12BE  - Y, 12bpp, big-endian";
            case AV_PIX_FMT_GRAY12LE :          return "GRAY12LE  - Y, 12bpp, little-endian";
            case AV_PIX_FMT_GRAY10BE :          return "GRAY10BE  - Y, 10bpp, big-endian";
            case AV_PIX_FMT_GRAY10LE :          return "GRAY10LE  - Y, 10bpp, little-endian";
            case AV_PIX_FMT_P016LE :            return "P016LE  - like NV12, with 16bpp per component, little-endian";
            case AV_PIX_FMT_P016BE :            return "P016BE  - like NV12, with 16bpp per component, big-endian";
            case AV_PIX_FMT_D3D11 :             return "D3D11";
            case AV_PIX_FMT_GRAY9BE :           return "GRAY9BE  - Y, 9bpp, big-endian";
            case AV_PIX_FMT_GRAY9LE :           return "GRAY9LE  - Y, 9bpp, little-endian";
            case AV_PIX_FMT_GBRPF32BE :         return "GBRPF32BE  - IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian";
            case AV_PIX_FMT_GBRPF32LE :         return "GBRPF32LE  - IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian";
            case AV_PIX_FMT_GBRAPF32BE :        return "GBRAPF32BE  - IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian";
            case AV_PIX_FMT_GBRAPF32LE :        return "GBRAPF32LE  - IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian";
            case AV_PIX_FMT_DRM_PRIME :         return "DRM_PRIME";
            case AV_PIX_FMT_OPENCL :            return "OPENCL";
            case AV_PIX_FMT_GRAY14BE :          return "GRAY14BE  - Y, 14bpp, big-endian";
            case AV_PIX_FMT_GRAY14LE :          return "GRAY14LE  - Y, 14bpp, little-endian";
            case AV_PIX_FMT_GRAYF32BE :         return "GRAYF32BE  - IEEE-754 single precision Y, 32bpp, big-endian";
            case AV_PIX_FMT_GRAYF32LE :         return "GRAYF32LE  - IEEE-754 single precision Y, 32bpp, little-endian";
            case AV_PIX_FMT_YUVA422P12BE :      return "YUVA422P12BE  - planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, big-endian";
            case AV_PIX_FMT_YUVA422P12LE :      return "YUVA422P12LE  - planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, little-endian";
            case AV_PIX_FMT_YUVA444P12BE :      return "YUVA444P12BE  - planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, big-endian";
            case AV_PIX_FMT_YUVA444P12LE :      return "YUVA444P12LE  - planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, little-endian";
            case AV_PIX_FMT_NV24 :              return "NV24  - planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)";
            case AV_PIX_FMT_NV42 :              return "NV42";
            case AV_PIX_FMT_VULKAN :            return "VULKAN";
            case AV_PIX_FMT_Y210BE :            return "Y210BE  - packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, big-endian";
            case AV_PIX_FMT_Y210LE :            return "Y210LE  - packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, little-endian";
            case AV_PIX_FMT_X2RGB10LE :         return "X2RGB10LE  - packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), little-endian, X=unused/undefined";
            case AV_PIX_FMT_X2RGB10BE :         return "X2RGB10BE  - packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), big-endian, X=unused/undefined";
            case AV_PIX_FMT_X2BGR10LE :         return "X2BGR10LE  - packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), little-endian, X=unused/undefined";
            case AV_PIX_FMT_X2BGR10BE :         return "X2BGR10BE  - packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), big-endian, X=unused/undefined";
            case AV_PIX_FMT_P210BE :            return "P210BE  - interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, big-endian";
            case AV_PIX_FMT_P210LE :            return "P210LE  - interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, little-endian";
            case AV_PIX_FMT_P410BE :            return "P410BE  - interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, big-endian";
            case AV_PIX_FMT_P410LE :            return "P410LE  - interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, little-endian";
            case AV_PIX_FMT_P216BE :            return "P216BE  - interleaved chroma YUV 4:2:2, 32bpp, big-endian";
            case AV_PIX_FMT_P216LE :            return "P216LE  - interleaved chroma YUV 4:2:2, 32bpp, little-endian";
            case AV_PIX_FMT_P416BE :            return "P416BE  - interleaved chroma YUV 4:4:4, 48bpp, big-endian";
            case AV_PIX_FMT_P416LE :            return "P416LE  - interleaved chroma YUV 4:4:4, 48bpp, little-endian";
            case AV_PIX_FMT_NB :                break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }
#endif

/*
=================================================
    ToString (AVSampleFormat)
=================================================
*/
    ND_ inline StringView  ToString (AVSampleFormat value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AV_SAMPLE_FMT_U8 :     return "U8  - unsigned 8 bits";
            case AV_SAMPLE_FMT_S16 :    return "S16  - signed 16 bits";
            case AV_SAMPLE_FMT_S32 :    return "S32  - signed 32 bits";
            case AV_SAMPLE_FMT_FLT :    return "FLT  - float";
            case AV_SAMPLE_FMT_DBL :    return "DBL  - double";
            case AV_SAMPLE_FMT_U8P :    return "U8P  - unsigned 8 bits, planar";
            case AV_SAMPLE_FMT_S16P :   return "S16P  - signed 16 bits, planar";
            case AV_SAMPLE_FMT_S32P :   return "S32P  - signed 32 bits, planar";
            case AV_SAMPLE_FMT_FLTP :   return "FLTP  - float, planar";
            case AV_SAMPLE_FMT_DBLP :   return "DBLP  - double, planar";
            case AV_SAMPLE_FMT_S64 :    return "S64  - signed 64 bits";
            case AV_SAMPLE_FMT_S64P :   return "S64P  - signed 64 bits, planar";
            case AV_SAMPLE_FMT_NONE :
            case AV_SAMPLE_FMT_NB :     break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }

/*
=================================================
    ToString (AVColorSpace)
=================================================
*/
    ND_ inline StringView  ToString (AVColorSpace value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AVCOL_SPC_BT709 :              return "BT709";                 // - also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / derived in SMPTE RP 177 Annex B";
            case AVCOL_SPC_UNSPECIFIED :        return "UNSPECIFIED";
            case AVCOL_SPC_RESERVED :           return "RESERVED";              // - reserved for future use by ITU-T and ISO/IEC just like 15-255 are";
            case AVCOL_SPC_FCC :                return "FCC";                   // - FCC Title 47 Code of Federal Regulations 73.682 (a)(20)";
            case AVCOL_SPC_BT470BG :            return "BT470BG";               // - also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
            case AVCOL_SPC_SMPTE170M :          return "SMPTE170M";             // - also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
            case AVCOL_SPC_SMPTE240M :          return "SMPTE240M";             // - derived from 170M primaries and D65 white point, 170M is derived from BT470 System M's primaries
            case AVCOL_SPC_YCGCO :              return "YCGCO";                 // - used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
            case AVCOL_SPC_BT2020_NCL :         return "BT2020_NCL";            // - ITU-R BT2020 non-constant luminance system
            case AVCOL_SPC_BT2020_CL :          return "BT2020_CL";             // - ITU-R BT2020 constant luminance system
            case AVCOL_SPC_SMPTE2085 :          return "SMPTE2085";             // - SMPTE 2085, Y'D'zD'x
            case AVCOL_SPC_CHROMA_DERIVED_NCL : return "CHROMA_DERIVED_NCL";    // - Chromaticity-derived non-constant luminance system
            case AVCOL_SPC_CHROMA_DERIVED_CL :  return "CHROMA_DERIVED_CL";     // - Chromaticity-derived constant luminance system
            case AVCOL_SPC_ICTCP :              return "ICTCP";                 // - ITU-R BT.2100-0, ICtCp
            case AVCOL_SPC_RGB :                return "RGB";                   // - order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB), YZX and ST 428-1
            case AVCOL_SPC_NB :                 break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }

/*
=================================================
    ToString (AVColorRange)
=================================================
*/
    ND_ inline StringView  ToString (AVColorRange value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AVCOL_RANGE_UNSPECIFIED :  return "UNSPECIFIED";
            case AVCOL_RANGE_MPEG :         return "MPEG";
            case AVCOL_RANGE_JPEG :         return "JPEG";
            case AVCOL_RANGE_NB :           break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }

/*
=================================================
    ToString (AVColorPrimaries)
=================================================
*/
    ND_ inline StringView  ToString (AVColorPrimaries value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AVCOL_PRI_RESERVED0 :      return "RESERVED0";
            case AVCOL_PRI_BT709 :          return "BT709";
            case AVCOL_PRI_UNSPECIFIED :    return "UNSPECIFIED";
            case AVCOL_PRI_RESERVED :       return "RESERVED";
            case AVCOL_PRI_BT470M :         return "BT470M";
            case AVCOL_PRI_BT470BG :        return "BT470BG";
            case AVCOL_PRI_SMPTE170M :      return "SMPTE170M";
            case AVCOL_PRI_SMPTE240M :      return "SMPTE240M";
            case AVCOL_PRI_FILM :           return "FILM";
            case AVCOL_PRI_BT2020 :         return "BT2020";
            case AVCOL_PRI_SMPTE428 :       return "SMPTE428";
            case AVCOL_PRI_SMPTE431 :       return "SMPTE431";
            case AVCOL_PRI_SMPTE432 :       return "SMPTE432";
            case AVCOL_PRI_EBU3213 :        return "EBU3213";
            case AVCOL_PRI_NB :             break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }

/*
=================================================
    ToString (AVColorTransferCharacteristic)
=================================================
*/
    ND_ inline StringView  ToString (AVColorTransferCharacteristic value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AVCOL_TRC_RESERVED0 :      return "RESERVED0";
            case AVCOL_TRC_BT709 :          return "BT709";
            case AVCOL_TRC_UNSPECIFIED :    return "UNSPECIFIED";
            case AVCOL_TRC_RESERVED :       return "RESERVED";
            case AVCOL_TRC_GAMMA22 :        return "GAMMA22";
            case AVCOL_TRC_GAMMA28 :        return "GAMMA28";
            case AVCOL_TRC_SMPTE170M :      return "SMPTE170M";
            case AVCOL_TRC_SMPTE240M :      return "SMPTE240M";
            case AVCOL_TRC_LINEAR :         return "LINEAR";
            case AVCOL_TRC_LOG :            return "LOG";
            case AVCOL_TRC_LOG_SQRT :       return "LOG_SQRT";
            case AVCOL_TRC_IEC61966_2_4 :   return "IEC61966_2_4";
            case AVCOL_TRC_BT1361_ECG :     return "BT1361_ECG";
            case AVCOL_TRC_IEC61966_2_1 :   return "IEC61966_2_1";
            case AVCOL_TRC_BT2020_10 :      return "BT2020_10";
            case AVCOL_TRC_BT2020_12 :      return "BT2020_12";
            case AVCOL_TRC_SMPTE2084 :      return "SMPTE2084";
            case AVCOL_TRC_SMPTE428 :       return "SMPTE428";
            case AVCOL_TRC_ARIB_STD_B67 :   return "ARIB_STD_B67";
            case AVCOL_TRC_NB :             break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }

/*
=================================================
    ToString (AVChromaLocation)
=================================================
*/
    ND_ inline StringView  ToString (AVChromaLocation value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case AVCHROMA_LOC_UNSPECIFIED : return "UNSPECIFIED";
            case AVCHROMA_LOC_LEFT :        return "LEFT";
            case AVCHROMA_LOC_CENTER :      return "CENTER";
            case AVCHROMA_LOC_TOPLEFT :     return "TOPLEFT";
            case AVCHROMA_LOC_TOP :         return "TOP";
            case AVCHROMA_LOC_BOTTOMLEFT :  return "BOTTOMLEFT";
            case AVCHROMA_LOC_BOTTOM :      return "BOTTOM";
            case AVCHROMA_LOC_NB :          break;
        }
        END_ENUM_CHECKS();
        return "Unknown";
    }


} // AE::Base

#endif // AE_ENABLE_FFMPEG
