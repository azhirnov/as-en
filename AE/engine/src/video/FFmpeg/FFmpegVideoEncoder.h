// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_FFMPEG
# include "video/Public/VideoEncoder.h"
# include "video/FFmpeg/FFmpegLoader.h"

namespace AE::Video
{

    //
    // FFmpeg Video Encoder
    //

    class FFmpegVideoEncoder final : public IVideoEncoder
    {
    // types
    private:
        struct CodecInfo
        {
            const char*                 format      = null;
            FixedArray<const char*, 8>  codecs;
            bool                        remux       = true;
            bool                        hasBFrames  = false;
        };


    // variables
    private:
        mutable SharedMutex     _guard;

        AVOutputFormat const*   _format             = null;
        AVFormatContext *       _formatCtx          = null;

        AVStream *              _videoStream        = null;
        AVFrame *               _videoFrame         = null;

        AVCodec const*          _codec              = null;
        AVCodecContext *        _codecCtx           = null;

        SwsContext *            _swsCtx             = null;

        AVIOContext *           _ioCtx              = null;     // wrapper for file stream
        RC<WStream>             _tempStream;
        RC<WStream>             _dstStream;

        AVPacket                _videoPacket;

        slong                   _frameCounter       = 0;

        String                  _tempFile;
        String                  _videoFile;

        bool                    _remuxRequired      = false;
        bool                    _encodingStarted    = false;
        bool                    _hasBFrames         = false;
        const bool              _ffmpegLoaded       = false;

        Config                  _config;


    // methods
    public:
        FFmpegVideoEncoder ()                                               __NE___;
        ~FFmpegVideoEncoder ()                                              __NE_OV;

        // IVideoEncoder //
        bool  Begin (const Config &cfg, const Path &filename)               __NE_OV;
        bool  Begin (const Config &cfg, RC<WStream> temp, RC<WStream> dst)  __NE_OV;

        bool  AddFrame (const ImageMemView &view, Bool endOnError)          __NE_OV;
    //  bool  AddFrame (VideoImageID id, Bool endOnError)                   __NE_OV;

        bool  End ()                                                        __NE_OV;

        bool            IsEncoding ()                                       C_NE_OV { SHAREDLOCK( _guard );  return _encodingStarted; }
        Config          GetConfig ()                                        C_NE_OV { SHAREDLOCK( _guard );  return _config; }
        StringView      GetFileExtension (EVideoCodec codec)                C_NE_OV;


    private:
        ND_ bool  _CreateCodec ();
        ND_ bool  _CreateCodecImpl ();
            void  _Destroy ();

            void  _ValidateResolution ();

        ND_ bool  _CreateStream (const AVCodec* codec, const char* videoFormat);
            void  _DestroyStream ();
            void  _ValidatePixelFormat (OUT int &) const;
            void  _SetOptions (INOUT AVDictionary **dict) const;

        ND_ bool  _Remux ();
        ND_ bool  _RemuxImpl (AVFormatContext* &ifmtCtx, AVFormatContext* &ofmtCtx, int* &streamMapping);
        ND_ bool  _Finish ();
        ND_ bool  _End ();

        ND_ bool  _AddFrameImpl (const ImageMemView &view);

        ND_ static Bitrate_t  _CalcBitrate (const Config &cfg);
        ND_ static CodecInfo  _GetEncoderInfo (const Config &cfg);

        static int  _IOWritePacket (void* opaque, ubyte* buf, int buf_size);
    };


} // AE::Video

#endif // AE_ENABLE_FFMPEG
