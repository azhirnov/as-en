// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Thread-safe:    yes
*/

#pragma once

#include "video/Public/Factory.h"

namespace AE::Video
{

    //
    // Video Decoder interface
    //

    class IVideoDecoder : public EnableRC<IVideoDecoder>
    {
    // types
    public:
        using Bitrate_t     = DefaultPhysicalQuantity<ulong>::BitPerSecond;
        using Second_t      = DefaultPhysicalQuantity<double>::Second;
        using FrameRate_t   = FractionalI;

        struct Config
        {
            uint2               dstDim          = {};               // can be used for scaling
            EPixelFormat        dstFormat       = Default;
            EFilter             filter          = Default;
        //  bool                hwAccelerated   = false;            // use hardware acceleration on GPU or CPU
        //  EGraphicsDeviceID   targetGPU       = Default;
        //  ECPUVendor          targetCPU       = Default;
            int                 videoStreamIdx  = -1;               // use 'GetFileProperties()' to enum all streams
        //  int                 audioStreamIdx  = -1;
            uint                threadCount     = 0;
        };

        struct StreamInfo
        {
            FixedString<32>     codecName;
            int                 index           = -1;
            EMediaType          type            = Default;
            EVideoCodec         codec           = Default;
            EVideoFormat        format          = Default;
            EColorPreset        colorPreset     = Default;
            ulong               frameCount      = 0;        // may be undefined
            Second_t            duration;
            FrameRate_t         avgFrameRate;               // average
            FrameRate_t         minFrameRate;               // minimal
            Bitrate_t           bitrate;
            uint2               dimension       = {};       // video only

            StreamInfo ()   __NE___ {}
        };
        using StreamInfos_t = FixedArray< StreamInfo, 8 >;

        struct Properties
        {
            StreamInfos_t       streams;

            ND_ String              ToString ()         C_Th___;
            ND_ StreamInfo const*   GetStream (int idx) C_NE___;
        };

        struct FrameInfo
        {
            Second_t        timestamp;
            Second_t        duration;           // next frame will be presented in 'timestamp + duration'
            ulong           frameIdx    = 0;
        };


    // interface
    public:
        virtual ~IVideoDecoder ()                                               __NE___ {}

        ND_ virtual bool  Begin (const Config &cfg, const Path &filename)       __NE___ = 0;
        ND_ virtual bool  Begin (const Config &cfg, RC<RStream> stream)         __NE___ = 0;
        ND_ virtual bool  SeekTo (ulong frameIdx)                               __NE___ = 0;
        ND_ virtual bool  SeekTo (Second_t timestamp)                           __NE___ = 0;

        ND_ virtual bool  GetNextFrame (INOUT ImageMemView &    memView,
                                        OUT FrameInfo &         info)           __NE___ = 0;

    //  ND_ virtual bool  GetNextFrame (OUT VideoImageID &  id,
    //                                  OUT FrameInfo &     info)               __NE___ = 0;

        ND_ virtual bool  End ()                                                __NE___ = 0;

        ND_ virtual Config      GetConfig ()                                    C_NE___ = 0;
        ND_ virtual Properties  GetProperties ()                                C_NE___ = 0;


        // stateless
        ND_ virtual Properties  GetFileProperties (const Path &filename)        C_NE___ = 0;
        ND_ virtual Properties  GetFileProperties (RC<RStream> stream)          C_NE___ = 0;
        ND_ virtual String      PrintFileProperties (const Path &filename)      C_Th___ = 0;
        ND_ virtual String      PrintFileProperties (RC<RStream> stream)        C_Th___ = 0;
        // TODO: get codecs

        // helpers
        template <typename A>
        ND_ static bool  AllocMemView (const Config         &cfg,
                                       OUT ImageMemView     &memView,
                                       A                    &allocator)         __NE___;
    };


/*
=================================================
    AllocMemView
=================================================
*/
    template <typename A>
    bool  IVideoDecoder::AllocMemView (const Config         &cfg,
                                       OUT ImageMemView     &memView,
                                       A                    &allocator) __NE___
    {
        auto&   fmt_info    = EPixelFormat_GetInfo( cfg.dstFormat );
        Bytes   row_pitch   = Graphics::ImageUtils::RowSize( cfg.dstDim.x, fmt_info.bitsPerBlock, fmt_info.TexBlockDim() );
        Bytes   img_size    = row_pitch * cfg.dstDim.y;
        void*   data        = allocator.Allocate( SizeAndAlign{ img_size, AE_CACHE_LINE });
        CHECK_ERR( data != null );

        memView = ImageMemView{ data, img_size, uint3{}, uint3{cfg.dstDim, 1}, row_pitch, img_size, cfg.dstFormat, Graphics::EImageAspect::Color };
        return true;
    }


} // AE::Video
