// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_FFMPEG
# include "video/Public/VideoDecoder.h"
# include "video/FFmpeg/FFmpegLoader.h"

namespace AE::Video
{

    //
    // FFmpeg Video Decoder
    //

    class FFmpegVideoDecoder final : public IVideoDecoder
    {
    // variables
    private:
        mutable SharedMutex     _guard;

        AVFormatContext *       _formatCtx          = null;

        AVFrame *               _videoFrame         = null;
        AVPacket *              _videoPacket        = null;

        AVCodec const*          _codec              = null;
        AVCodecContext *        _codecCtx           = null;

        SwsContext *            _swsCtx             = null;

        AVIOContext *           _ioCtx              = null;     // wrapper for file stream
        RC<RStream>             _rstream;

        bool                    _decodingStarted    = false;
        const bool              _ffmpegLoaded       = false;

        Config                  _config;


    // methods
    public:
        FFmpegVideoDecoder ()                                       __NE___;
        ~FFmpegVideoDecoder ()                                      __NE_OV;

        bool  Begin (const Config &cfg, const Path &filename)       __NE_OV;
        bool  Begin (const Config &cfg, RC<RStream> stream)         __NE_OV;
        bool  SeekTo (ulong frameIdx)                               __NE_OV;
        bool  SeekTo (Second_t timestamp)                           __NE_OV;

        bool  GetNextFrame (INOUT ImageMemView &    memView,
                            OUT FrameInfo &         info)           __NE_OV;

    //  bool  GetNextFrame (OUT VideoImageID &  id,
    //                      OUT FrameInfo &     info)               __NE_OV;

        bool  End ()                                                __NE_OV;

        Properties  GetFileProperties (const Path &filename)        C_NE_OV;
        Properties  GetFileProperties (RC<RStream> stream)          C_NE_OV;
        String      PrintFileProperties (const Path &filename)      C_Th_OV;
        String      PrintFileProperties (RC<RStream> stream)        C_Th_OV;

        Config      GetConfig ()                                    C_NE_OV { SHAREDLOCK( _guard );  return _config; }
        Properties  GetProperties ()                                C_NE_OV;

    private:
        ND_ bool  _Begin ();
        ND_ bool  _End ();
            void  _Destroy ();

        ND_ bool  _SeekTo (slong targetPTS);

        ND_ ulong  _PTStoFrameIdx (slong pts)       const;
        ND_ slong  _FrameIdxToPTS (ulong frameIdx)  const;
        ND_ slong  _TimestampToPTS (Second_t time)  const;

        static Properties  _ReadProperties (AVFormatContext* formatCtx);

        static int      _IOReadPacket (void* opaque, ubyte* buf, int buf_size);
        static slong    _IOSeek (void* opaque, slong offset, int whence);
    };


} // AE::Video

#endif // AE_ENABLE_FFMPEG
