// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/ResourceEnums.h"

namespace AE::Graphics
{

    // ndk: AIMAGE_FORMATS
    // [java](https://developer.android.com/reference/android/graphics/ImageFormat)
    //
    enum class AImageFormat : int
    {
        DEPTH16             = 0x44363159,
        DEPTH_POINT_CLOUD   = 0x00000101,

        JPEG                = 0x00000100,

        NV16                = 0x00000010,
        NV21                = 0x00000011,

        PRIVATE             = 0x00000022,

        RAW10               = 0x00000025,
        RAW12               = 0x00000026,
        RAW_PRIVATE         = 0x00000024,
        RAW_SENSOR          = 0x00000020,
        RAW16               = 0x00000020,   // from NDK, same as RAW_SENSOR

        YUV_420_888         = 0x00000023,
        YUV_422_888         = 0x00000027,
        YUV_444_888         = 0x00000028,

        YUY2                = 0x00000014,
        YV12                = 0x32315659,

        DEPTH_JPEG          = 0x69656963,   // API 29
        HEIC                = 0x48454946,   // API 29
        Y8                  = 0x20203859,   // API 29

        YCBCR_P010          = 0x00000036,   // API 31

        Unknown             = 0,
    };

/*
=================================================
    AndroidFormatCast
=================================================
*/
    ND_ inline EPixelFormatExternal  AndroidFormatCast (AImageFormat format) __NE___
    {
        switch_enum( format )
        {
            case AImageFormat::YCBCR_P010 :         return EPixelFormatExternal::Android_YCBCR_P010;
            case AImageFormat::YUV_420_888 :        return EPixelFormatExternal::Android_YUV_420;
            case AImageFormat::YUV_422_888 :        return EPixelFormatExternal::Android_YUV_422;
            case AImageFormat::YUV_444_888 :        return EPixelFormatExternal::Android_YUV_444;
            case AImageFormat::Y8 :                 return EPixelFormatExternal::Android_Y8;
            case AImageFormat::YUY2 :               return EPixelFormatExternal::Android_YUY2;
            case AImageFormat::YV12 :               return EPixelFormatExternal::Android_YV12;

            case AImageFormat::NV16 :               return EPixelFormatExternal::Android_NV16;
            case AImageFormat::NV21 :               return EPixelFormatExternal::Android_NV21;

            case AImageFormat::JPEG :               return EPixelFormatExternal::Android_JPEG;

            case AImageFormat::PRIVATE :            return EPixelFormatExternal::Android_Private;

            case AImageFormat::RAW_PRIVATE :        return EPixelFormatExternal::Android_RawPrivate;
            case AImageFormat::RAW16 :              return EPixelFormatExternal::Android_Raw16;
            case AImageFormat::RAW12 :              return EPixelFormatExternal::Android_Raw12;
            case AImageFormat::RAW10 :              return EPixelFormatExternal::Android_Raw10;

            case AImageFormat::DEPTH16 :            return EPixelFormatExternal::Android_Depth16;
            case AImageFormat::DEPTH_POINT_CLOUD :  return EPixelFormatExternal::Android_DepthPointCloud;
            case AImageFormat::DEPTH_JPEG :         return EPixelFormatExternal::Android_DepthJPEG;

            case AImageFormat::HEIC :               return EPixelFormatExternal::Android_HEIC;

            case AImageFormat::Unknown :            break;
        }
        switch_end
        //RETURN_ERR( "unsupported android image format" );
        return Default;
    }

    ND_ inline EPixelFormatExternal  AndroidFormatCast (int format) __NE___
    {
        return AndroidFormatCast( AImageFormat(format) );
    }

/*
=================================================
    AndroidFormatCast
=================================================
*/
    ND_ inline AImageFormat  AndroidFormatCast (EPixelFormatExternal format) __NE___
    {
        switch_enum( format )
        {
            case EPixelFormatExternal::Android_YCBCR_P010 :     return AImageFormat::YCBCR_P010;
            case EPixelFormatExternal::Android_YUV_420 :        return AImageFormat::YUV_420_888;
            case EPixelFormatExternal::Android_YUV_422 :        return AImageFormat::YUV_422_888;
            case EPixelFormatExternal::Android_YUV_444 :        return AImageFormat::YUV_444_888;
            case EPixelFormatExternal::Android_Y8 :             return AImageFormat::Y8;
            case EPixelFormatExternal::Android_YUY2 :           return AImageFormat::YUY2;
            case EPixelFormatExternal::Android_YV12 :           return AImageFormat::YV12;

            case EPixelFormatExternal::Android_NV16 :           return AImageFormat::NV16;
            case EPixelFormatExternal::Android_NV21 :           return AImageFormat::NV21;

            case EPixelFormatExternal::Android_JPEG :           return AImageFormat::JPEG;

            case EPixelFormatExternal::Android_Private :        return AImageFormat::PRIVATE;

            case EPixelFormatExternal::Android_RawPrivate :     return AImageFormat::RAW_PRIVATE;
            case EPixelFormatExternal::Android_Raw16 :          return AImageFormat::RAW16;
            case EPixelFormatExternal::Android_Raw12 :          return AImageFormat::RAW12;
            case EPixelFormatExternal::Android_Raw10 :          return AImageFormat::RAW10;

            case EPixelFormatExternal::Android_Depth16 :        return AImageFormat::DEPTH16;
            case EPixelFormatExternal::Android_DepthPointCloud: return AImageFormat::DEPTH_POINT_CLOUD;
            case EPixelFormatExternal::Android_DepthJPEG :      return AImageFormat::DEPTH_JPEG;

            case EPixelFormatExternal::Android_HEIC :           return AImageFormat::HEIC;

            case EPixelFormatExternal::_Android_End :
            case EPixelFormatExternal::_Count :
            case EPixelFormatExternal::Unknown :                break;
        }
        switch_end
        RETURN_ERR( "can not convert external pixel format to android image format" );
    }


} // AE::Graphics
