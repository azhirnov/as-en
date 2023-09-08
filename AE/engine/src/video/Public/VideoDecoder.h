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
            uint2               dstSize         = {};               // can be used for scaling
            EPixelFormat        dstFormat       = Default;
            EFilter             filter          = Default;
        //  bool                hwAccelerated   = false;            // use hardware acceleration on GPU or CPU
        //  EGraphicsDeviceID   targetGPU       = Default;
        //  ECPUVendor          targetCPU       = Default;
            int                 videoStreamIdx  = -1;               // use 'GetFileProperties()' to enum all streams
        //  int                 audioStreamIdx  = -1;
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
            uint2               size            = {};       // video only
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

        ND_ virtual bool  GetFrame (OUT ImageMemView &  view,
                                    OUT FrameInfo &     info)                   __NE___ = 0;

        ND_ virtual bool  GetFrame (OUT VideoImageID &  id,
                                    OUT FrameInfo &     info)                   __NE___ = 0;

        // TODO: GetFrameAsync()

        ND_ virtual bool  End ()                                                __NE___ = 0;

        ND_ virtual Config      GetConfig ()                                    C_NE___ = 0;
        ND_ virtual Properties  GetProperties ()                                C_NE___ = 0;


        // stateless
        ND_ virtual Properties  GetFileProperties (const Path &filename)        C_NE___ = 0;
        ND_ virtual Properties  GetFileProperties (RC<RStream> stream)          C_NE___ = 0;
        ND_ virtual String      PrintFileProperties (const Path &filename)      C_NE___ = 0;
        ND_ virtual String      PrintFileProperties (RC<RStream> stream)        C_NE___ = 0;
        // TODO: get codecs
    };


} // AE::Video
