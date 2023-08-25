// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "video/Public/Common.h"

namespace AE::Base
{

/*
=================================================
    ToString (EVideoFormat)
=================================================
*/
    ND_ inline String  ToString (Video::EVideoFormat value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case Video::EVideoFormat::YUV420P :         return "YUV420P";
            case Video::EVideoFormat::YUV422P :         return "YUV422P";
            case Video::EVideoFormat::YUV444P :         return "YUV444P";
            case Video::EVideoFormat::YUVA444P16LE :    return "YUVA444P16LE";
            case Video::EVideoFormat::YUV420P10LE :     return "YUV420P10LE";
            case Video::EVideoFormat::NV12 :            return "NV12";
            case Video::EVideoFormat::NV21 :            return "NV21";
            case Video::EVideoFormat::P010LE :          return "P010LE";
            case Video::EVideoFormat::Unknown :
            case Video::EVideoFormat::_Count :          break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown video format" );
    }

/*
=================================================
    ToString (EVideoCodec)
=================================================
*/
    ND_ inline String  ToString (Video::EVideoCodec value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case Video::EVideoCodec::GIF :      return "GIF";
            case Video::EVideoCodec::MPEG4 :    return "MPEG4";
            case Video::EVideoCodec::H264 :     return "H264";
            case Video::EVideoCodec::H265 :     return "H265";
            case Video::EVideoCodec::WEBP :     return "WEBP";
            case Video::EVideoCodec::VP8 :      return "VP8";
            case Video::EVideoCodec::VP9 :      return "VP9";
            case Video::EVideoCodec::AV1 :      return "AV1";
            case Video::EVideoCodec::Unknown :
            case Video::EVideoCodec::_Count :   break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown video codec" );
    }

/*
=================================================
    ToString (EMediaType)
=================================================
*/
    ND_ inline String  ToString (Video::EMediaType value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case Video::EMediaType::Video :     return "Video";
            case Video::EMediaType::Audio :     return "Audio";
            case Video::EMediaType::Unknown :
            case Video::EMediaType::_Count :    break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown media type" );
    }

/*
=================================================
    ToString (EColorPreset)
=================================================
*/
    ND_ inline String  ToString (Video::EColorPreset value)
    {
        BEGIN_ENUM_CHECKS();
        switch ( value )
        {
            case Video::EColorPreset::Unspecified :     return "Unspecified";
            case Video::EColorPreset::MPEG_BT709 :      return "MPEG_BT709";
            case Video::EColorPreset::MPEG_BT709_2 :    return "MPEG_BT709_2";
            case Video::EColorPreset::JPEG_BT709 :      return "JPEG_BT709";
            case Video::EColorPreset::MPEG_BT2020_1 :   return "MPEG_BT2020_1";
            case Video::EColorPreset::MPEG_BT2020_2 :   return "MPEG_BT2020_2";
            case Video::EColorPreset::MPEG_SMPTE170M :  return "MPEG_SMPTE170M";
            case Video::EColorPreset::Unknown :
            case Video::EColorPreset::_Count :          break;
        }
        END_ENUM_CHECKS();
        RETURN_ERR( "unknown color preset" );
    }


} // AE::Base
