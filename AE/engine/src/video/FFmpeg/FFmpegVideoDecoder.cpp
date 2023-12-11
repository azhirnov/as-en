// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_FFMPEG
# include "video/FFmpeg/FFmpegVideoDecoder.h"
# include "video/FFmpeg/FFmpegUtils.cpp.h"

namespace AE::Video
{
namespace {
    static FFmpegLoader     ffmpeg;

/*
=================================================
    constructor
=================================================
*/
    ND_ AVPixelFormat  CorrectPixFormat (AVPixelFormat src)
    {
        switch ( src )
        {
            // Fix swscaler deprecated pixel format warning
            // (YUVJ has been deprecated, change pixel format to regular YUV)
            case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
            case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
            case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
            case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
            default:                  return src;
        }
    }
}
    using namespace AE::Graphics;


/*
=================================================
    constructor
=================================================
*/
    FFmpegVideoDecoder::FFmpegVideoDecoder () __NE___ :
        _ffmpegLoaded{ ffmpeg.Load() }
    {
        EXLOCK( _guard );
        ASSERT( _ffmpegLoaded );
    }

/*
=================================================
    destructor
=================================================
*/
    FFmpegVideoDecoder::~FFmpegVideoDecoder () __NE___
    {
        EXLOCK( _guard );

        _Destroy();

        ffmpeg.Unload();
    }

/*
=================================================
    Begin
=================================================
*/
    bool  FFmpegVideoDecoder::Begin (const Config &cfg, const Path &filename) __NE___
    {
        EXLOCK( _guard );

        CHECK_ERR( _ffmpegLoaded );
        CHECK_ERR( not _decodingStarted );

        _Destroy();

        _config = cfg;

        // open video file
        {
            const String    fname = ToString( filename );

            _formatCtx = ffmpeg.avformat_alloc_context();
            CHECK_ERR( _formatCtx != null );

            FF_CHECK_ERR( ffmpeg.avformat_open_input( &_formatCtx, fname.c_str(), null, null ));
        }

        return _Begin();
    }

    bool  FFmpegVideoDecoder::Begin (const Config &cfg, RC<RStream> stream) __NE___
    {
        EXLOCK( _guard );

        CHECK_ERR( _ffmpegLoaded );
        CHECK_ERR( not _decodingStarted );

        CHECK_ERR( stream and stream->IsOpen() );

        _Destroy();

        _config     = cfg;
        _rstream    = RVRef(stream);

        // open video file
        {
            _formatCtx = ffmpeg.avformat_alloc_context();
            CHECK_ERR( _formatCtx != null );

            _ioCtx = ffmpeg.avio_alloc_context( null, 0, 0, _rstream.get(), &_IOReadPacket, null, &_IOSeek );
            CHECK_ERR( _ioCtx != null );

            _ioCtx->seekable    = AVIO_SEEKABLE_NORMAL;
            _ioCtx->direct      = 1;

            _formatCtx->pb      = _ioCtx;
            _formatCtx->flags   |= AVFMT_FLAG_CUSTOM_IO;

            FF_CHECK_ERR( ffmpeg.avformat_open_input( &_formatCtx, null, null, null ));
        }

        return _Begin();
    }

/*
=================================================
    _Begin
=================================================
*/
    bool  FFmpegVideoDecoder::_Begin ()
    {
        // find video stream
        AVStream*   video_stream = null;
        {
            FF_CHECK_ERR( ffmpeg.avformat_find_stream_info( _formatCtx, null ));

            if ( _config.videoStreamIdx < 0 )
            {
                _config.videoStreamIdx = ffmpeg.av_find_best_stream( _formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, null, 0 );
                CHECK_ERR( _config.videoStreamIdx >= 0 );
                video_stream = _formatCtx->streams[ _config.videoStreamIdx ];
            }
            else
            {
                CHECK_ERR( _config.videoStreamIdx < int(_formatCtx->nb_streams) );
                video_stream = _formatCtx->streams[ _config.videoStreamIdx ];
            }

            _codec = ffmpeg.avcodec_find_decoder( video_stream->codecpar->codec_id );
            CHECK_ERR( _codec != null );

            _codecCtx = ffmpeg.avcodec_alloc_context3( _codec );
            CHECK_ERR( _codecCtx != null );

            if ( AllBits( _codec->capabilities, AV_CODEC_CAP_SLICE_THREADS ))
            {
                _codecCtx->thread_type  = FF_THREAD_SLICE;
                _codecCtx->thread_count = Max( _codecCtx->thread_count, int(_config.threadCount) );
            }else
            if ( AllBits( _codec->capabilities, AV_CODEC_CAP_FRAME_THREADS ))
            {
                _codecCtx->thread_type  = FF_THREAD_FRAME;
                _codecCtx->thread_count = Max( _codecCtx->thread_count, int(_config.threadCount) );
            }

            FF_CHECK_ERR( ffmpeg.avcodec_parameters_to_context( _codecCtx, video_stream->codecpar ));
            FF_CHECK_ERR( ffmpeg.avcodec_open2( _codecCtx, _codec, null ));
        }

        // create frame
        {
            _videoFrame = ffmpeg.av_frame_alloc();
            CHECK_ERR( _videoFrame != null );

            _videoPacket = ffmpeg.av_packet_alloc();
            CHECK_ERR( _videoPacket != null );
        }

        const uint2     src_dim { _codecCtx->width, _codecCtx->height };

        if ( All( IsZero( _config.dstDim )) )
            _config.dstDim = src_dim;

        // create scaler
        {
            const AVPixelFormat src_fmt = CorrectPixFormat( _codecCtx->pix_fmt );
            const AVPixelFormat dst_fmt = EnumCast( _config.dstFormat );
            const int           filter  = EnumCast( _config.filter );

            CHECK_ERR( src_fmt != AV_PIX_FMT_NONE and
                       dst_fmt != AV_PIX_FMT_NONE );

            _swsCtx = ffmpeg.sws_getContext( int(src_dim.x), int(src_dim.y), src_fmt,
                                             int(_config.dstDim.x), int(_config.dstDim.y), dst_fmt,
                                             filter, null, null, null );
            CHECK_ERR( _swsCtx != null );
        }

        _decodingStarted = true;
        return true;
    }

/*
=================================================
    _PTStoFrameIdx / _FrameIdxToPTS / _TimestampToPTS
=================================================
*/
    ulong  FFmpegVideoDecoder::_PTStoFrameIdx (slong pts) const
    {
        AVStream*   vstream = _formatCtx->streams[ _config.videoStreamIdx ];

        return (pts * vstream->r_frame_rate.num * vstream->time_base.num) / (vstream->r_frame_rate.den * vstream->time_base.den);
    }

    slong  FFmpegVideoDecoder::_FrameIdxToPTS (ulong frameIdx) const
    {
        AVStream*   vstream = _formatCtx->streams[ _config.videoStreamIdx ];

        return (frameIdx * vstream->r_frame_rate.den * vstream->time_base.den) / (vstream->r_frame_rate.num * vstream->time_base.num);
    }

    slong  FFmpegVideoDecoder::_TimestampToPTS (Second_t timestamp) const
    {
        const auto  duration1   = Second_t{_formatCtx->duration * av_q2d(AVRational{1, AV_TIME_BASE})};
        AVStream*   vstream     = _formatCtx->streams[ _config.videoStreamIdx ];
        const auto  duration    = vstream->duration > 0 ? Second_t{ vstream->duration * av_q2d( vstream->time_base )} : duration1;

        //const auto    factor      = timestamp.GetNonScaled() / duration.GetNonScaled();

        return 0;   // TODO
    }

/*
=================================================
    SeekTo
=================================================
*/
    bool  FFmpegVideoDecoder::SeekTo (ulong frameIdx) __NE___
    {
        EXLOCK( _guard );

        if_unlikely( not _decodingStarted )
            return false;

        ulong   target_pts = _FrameIdxToPTS( frameIdx );

        return _SeekTo( target_pts );
    }

    bool  FFmpegVideoDecoder::SeekTo (Second_t timestamp) __NE___
    {
        EXLOCK( _guard );

        if_unlikely( not _decodingStarted )
            return false;

        ulong   target_pts = _TimestampToPTS( timestamp );

        return _SeekTo( target_pts );
    }

    bool  FFmpegVideoDecoder::_SeekTo (slong targetPTS)
    {
        ASSERT( _decodingStarted            and
                _formatCtx      != null     and
                _config.videoStreamIdx >= 0 and
                _config.videoStreamIdx < int(_formatCtx->nb_streams) );

        AVStream*   vstream = _formatCtx->streams[ _config.videoStreamIdx ];
        Unused( vstream );
        ASSERT( vstream->nb_frames == 0 or targetPTS < vstream->nb_frames );

        FF_CHECK_ERR( ffmpeg.av_seek_frame( _formatCtx, _config.videoStreamIdx, targetPTS, AVSEEK_FLAG_BACKWARD ));

        // av_seek_frame takes effect after one frame
        for (; ffmpeg.av_read_frame( _formatCtx, _videoPacket ) >= 0;)
        {
            if ( _videoPacket->stream_index != _config.videoStreamIdx )
            {
                ffmpeg.av_packet_unref( _videoPacket );
                continue;   // skip other streams
            }

            int err = ffmpeg.avcodec_send_packet( _codecCtx, _videoPacket );
            FF_CHECK_ERR( err );

            err = ffmpeg.avcodec_receive_frame( _codecCtx, _videoFrame );

            if ( err == AVERROR(EAGAIN) or err == AVERROR_EOF )
            {
                ffmpeg.av_packet_unref( _videoPacket );
                continue;
            }
            else
                FF_CHECK_ERR( err );

            ffmpeg.av_packet_unref( _videoPacket );
            break;
        }
        return true;
    }

/*
=================================================
    GetNextFrame
=================================================
*/
    bool  FFmpegVideoDecoder::GetNextFrame (INOUT ImageMemView &    memView,
                                            OUT FrameInfo &         outInfo) __NE___
    {
        EXLOCK( _guard );

        if_unlikely( not _decodingStarted )
            return false;

        ASSERT( _formatCtx      != null     and
                _videoPacket    != null     and
                _codecCtx       != null     and
                _swsCtx         != null     and
                _config.videoStreamIdx >= 0 and
                _config.videoStreamIdx < int(_formatCtx->nb_streams) );
        ASSERT( memView.Parts().size() == 1 );
        ASSERT( memView.Format() == _config.dstFormat );
        ASSERT( All( memView.Dimension() == uint3{_config.dstDim, 1} ));

        AVStream*   vstream = _formatCtx->streams[ _config.videoStreamIdx ];

        for (; ffmpeg.av_read_frame( _formatCtx, _videoPacket ) >= 0;)
        {
            slong   packed_pts = -1;

            // read frame to packet
            {
                if ( _videoPacket->stream_index != _config.videoStreamIdx )
                {
                    ffmpeg.av_packet_unref( _videoPacket );
                    continue;   // skip other streams
                }

                int err = ffmpeg.avcodec_send_packet( _codecCtx, _videoPacket );

                packed_pts          = _videoPacket->pts;
                outInfo.timestamp   = Second_t{ _videoPacket->pts * av_q2d( vstream->time_base )};
                outInfo.duration    = Second_t{ _videoPacket->duration * av_q2d( vstream->time_base )};
                outInfo.frameIdx    = _PTStoFrameIdx( _videoPacket->pts );

                //ASSERT_Eq( _videoPacket->pts, _FrameIdxToPTS( outInfo.frameIdx ));

                ffmpeg.av_packet_unref( _videoPacket );
                FF_CHECK_ERR( err );
            }

            // receive frame into '_videoFrame'
            for (;;)
            {
                int err = ffmpeg.avcodec_receive_frame( _codecCtx, _videoFrame );

                if_unlikely( err == AVERROR_EOF )
                    return false;

                if_unlikely( err == AVERROR(EAGAIN) )
                    break;

                if_unlikely( err < 0 )
                {
                    FF_CHECK( err );
                    return false;
                }

                //ASSERT( packed_pts == _videoFrame->pts );

                const int   src_slice_y                         = 0;
                const int   dst_stride [AV_NUM_DATA_POINTERS]   = { int(memView.RowPitch()) };
                ubyte*      dst_data   [AV_NUM_DATA_POINTERS]   = { Cast<ubyte>(memView.Parts().front().ptr) };

                const int   scaled_h = ffmpeg.sws_scale( _swsCtx, _videoFrame->data, _videoFrame->linesize, src_slice_y, _videoFrame->height,
                                                         OUT dst_data, dst_stride );

                if_unlikely( scaled_h < 0 or scaled_h != int(_config.dstDim.y) )
                {
                    FF_CHECK( scaled_h );
                    return false;
                }

                return true;
            }
        }

        return false;
    }

/*
=================================================
    GetFrame
=================================================
*
    bool  FFmpegVideoDecoder::GetFrame (OUT VideoImageID &, OUT FrameInfo &) __NE___
    {
        // not supported
        return false;
    }

/*
=================================================
    End
=================================================
*/
    bool  FFmpegVideoDecoder::End () __NE___
    {
        EXLOCK( _guard );
        return _End();
    }

    bool  FFmpegVideoDecoder::_End ()
    {
        if_unlikely( not _decodingStarted )
            return false;

        _Destroy();
        return true;
    }

/*
=================================================
    _Destroy
=================================================
*/
    void  FFmpegVideoDecoder::_Destroy ()
    {
        if ( _swsCtx != null )
            ffmpeg.sws_freeContext( _swsCtx );

        if ( _videoPacket != null )
            ffmpeg.av_packet_free( &_videoPacket );

        if ( _videoFrame != null )
            ffmpeg.av_frame_free( &_videoFrame );

        if ( _codecCtx != null )
            ffmpeg.avcodec_free_context( &_codecCtx );

        if ( _formatCtx != null )
            ffmpeg.avformat_close_input( &_formatCtx );

        if ( _formatCtx != null )
            ffmpeg.avformat_free_context( _formatCtx );

        if ( _ioCtx != null )
            ffmpeg.avio_context_free( &_ioCtx );

        _videoPacket        = null;
        _videoFrame         = null;
        _formatCtx          = null;
        _codecCtx           = null;
        _codec              = null;
        _swsCtx             = null;
        _ioCtx              = null;

        _decodingStarted    = false;
        _config             = Default;
        _rstream            = null;
    }

/*
=================================================
    GetProperties
=================================================
*/
    IVideoDecoder::Properties  FFmpegVideoDecoder::GetProperties () C_NE___
    {
        SHAREDLOCK( _guard );
        CHECK_ERR( _decodingStarted );
        return _ReadProperties( _formatCtx );
    }

/*
=================================================
    _IOReadPacket
=================================================
*/
    int  FFmpegVideoDecoder::_IOReadPacket (void* opaque, ubyte* buf, int buf_size)
    {
        auto*   stream = Cast<RStream>( opaque );

        if_unlikely( buf_size < 0 )
            return AVERROR_UNKNOWN;

        int result = int(stream->ReadSeq( buf, Bytes{ulong(buf_size)} ));

        if_unlikely( result == 0 )
        {
            auto    pos_size = stream->PositionAndSize();
            result = (pos_size.pos >= pos_size.size) ? AVERROR_EOF : result;
        }

        return result;
    }

/*
=================================================
    _IOSeek
=================================================
*/
    slong  FFmpegVideoDecoder::_IOSeek (void* opaque, slong offset, int whence)
    {
        auto*   stream = Cast<RStream>( opaque );

        if_unlikely( offset < 0 )
            return AVERROR_UNKNOWN;

        switch ( whence & ~AVSEEK_FORCE )
        {
            case SEEK_SET :     return stream->SeekSet( Bytes{ulong(offset)} )                  ? slong(stream->Position()) : AVERROR_UNKNOWN;
            case SEEK_CUR :     return stream->SeekFwd( Bytes{ulong(offset)} )                  ? slong(stream->Position()) : AVERROR_UNKNOWN;
            case SEEK_END :     return stream->SeekSet( stream->Size() - Bytes{ulong(offset)} ) ? slong(stream->Position()) : AVERROR_UNKNOWN;
            case AVSEEK_SIZE :  return slong(stream->Size());
        }
        return AVERROR_UNKNOWN;
    }

/*
=================================================
    _ReadProperties
=================================================
*/
    IVideoDecoder::Properties  FFmpegVideoDecoder::_ReadProperties (AVFormatContext* formatCtx)
    {
        Properties  result;
        const auto  duration = Second_t{formatCtx->duration * av_q2d(AVRational{1, AV_TIME_BASE})};

        for (uint i = 0; i < formatCtx->nb_streams; ++i)
        {
            AVStream*           stream = formatCtx->streams[i];
            AVCodecParameters*  params = stream->codecpar;

            if ( AnyEqual( params->codec_id, AV_CODEC_ID_PROBE, AV_CODEC_ID_PNG, AV_CODEC_ID_MJPEG ))
                continue;

            const AVCodec*  codec = ffmpeg.avcodec_find_decoder( params->codec_id );
            if ( codec == null )
                continue;

            if ( codec->type != AVMEDIA_TYPE_VIDEO ) //and codec->type != AVMEDIA_TYPE_AUDIO )
                continue;

            auto&   dst         = result.streams.emplace_back();
            dst.codecName       = codec->name;
            dst.index           = stream->index;
            dst.type            = EnumCast( codec->type );
            dst.codec           = EnumCast( params->codec_id );
            dst.format          = EnumCast( AVPixelFormat( params->format ));
            dst.colorPreset     = EnumCast( params->color_range, params->color_primaries, params->color_trc, params->color_space, params->chroma_location );
            dst.frameCount      = stream->nb_frames;
            dst.duration        = stream->duration > 0 ? Second_t{ stream->duration * av_q2d( stream->time_base )} : duration;
            dst.avgFrameRate    = ToFractional( stream->avg_frame_rate );
            dst.minFrameRate    = ToFractional( stream->r_frame_rate );
            dst.bitrate         = Bitrate_t{ulong( params->bit_rate > 0 ? params->bit_rate : formatCtx->bit_rate )};
            dst.dimension.x     = params->width;
            dst.dimension.y     = params->height;
        }

        return result;
    }

/*
=================================================
    GetFileProperties
=================================================
*/
    IVideoDecoder::Properties  FFmpegVideoDecoder::GetFileProperties (RC<RStream> stream) C_NE___
    {
        struct Context
        {
            AVFormatContext*    formatCtx   = null;
            AVIOContext *       ioCtx       = null;

            ~Context ()
            {
                if ( formatCtx != null )
                    ffmpeg.avformat_close_input( &formatCtx );

                if ( formatCtx != null )
                    ffmpeg.avformat_free_context( formatCtx );

                if ( ioCtx != null )
                    ffmpeg.avio_context_free( &ioCtx );
            }
        };
        Context ctx;

        CHECK_ERR( stream and stream->IsOpen() );

        ctx.formatCtx = ffmpeg.avformat_alloc_context();
        CHECK_ERR( ctx.formatCtx != null );

        ctx.ioCtx = ffmpeg.avio_alloc_context( null, 0, 0, stream.get(), &_IOReadPacket, null, &_IOSeek );
        CHECK_ERR( ctx.ioCtx != null );

        ctx.ioCtx->seekable = AVIO_SEEKABLE_NORMAL;
        ctx.ioCtx->direct   = 1;

        ctx.formatCtx->pb       = ctx.ioCtx;
        ctx.formatCtx->flags    |= AVFMT_FLAG_CUSTOM_IO;

        FF_CHECK_ERR( ffmpeg.avformat_open_input( &ctx.formatCtx, null, null, null ));
        FF_CHECK_ERR( ffmpeg.avformat_find_stream_info( ctx.formatCtx, null ));

        return _ReadProperties( ctx.formatCtx );
    }

    IVideoDecoder::Properties  FFmpegVideoDecoder::GetFileProperties (const Path &filename) C_NE___
    {
        return GetFileProperties( MakeRC<FileRStream>( filename ));
    }

/*
=================================================
    PrintFileProperties
=================================================
*/
    String  FFmpegVideoDecoder::PrintFileProperties (RC<RStream> rstream) C_Th___
    {
        struct Context
        {
            AVFormatContext*    formatCtx   = null;
            AVIOContext *       ioCtx       = null;

            ~Context ()
            {
                if ( formatCtx != null )
                    ffmpeg.avformat_close_input( &formatCtx );

                if ( formatCtx != null )
                    ffmpeg.avformat_free_context( formatCtx );

                if ( ioCtx != null )
                    ffmpeg.avio_context_free( &ioCtx );
            }
        };

        Properties      result;
        Context         ctx;

        CHECK_ERR( rstream and rstream->IsOpen() );

        ctx.formatCtx = ffmpeg.avformat_alloc_context();
        CHECK_ERR( ctx.formatCtx != null );

        ctx.ioCtx = ffmpeg.avio_alloc_context( null, 0, 0, rstream.get(), &_IOReadPacket, null, &_IOSeek );
        CHECK_ERR( ctx.ioCtx != null );

        ctx.ioCtx->seekable = AVIO_SEEKABLE_NORMAL;
        ctx.ioCtx->direct   = 1;

        ctx.formatCtx->pb       = ctx.ioCtx;
        ctx.formatCtx->flags    |= AVFMT_FLAG_CUSTOM_IO;

        FF_CHECK_ERR( ffmpeg.avformat_open_input( &ctx.formatCtx, null, null, null ));
        FF_CHECK_ERR( ffmpeg.avformat_find_stream_info( ctx.formatCtx, null ));

        const double    duration = ctx.formatCtx->duration * av_q2d(AVRational{1, AV_TIME_BASE});
        String          str;

        for (uint i = 0; i < ctx.formatCtx->nb_streams; ++i)
        {
            AVStream*           stream = ctx.formatCtx->streams[i];
            AVCodecParameters*  params = stream->codecpar;

            if ( AnyEqual( params->codec_id, AV_CODEC_ID_PROBE, AV_CODEC_ID_PNG, AV_CODEC_ID_MJPEG ))
                continue;

            const AVCodec*  codec = ffmpeg.avcodec_find_decoder( params->codec_id );
            if ( codec == null )
                continue;

            str << "\n--------"
                << "\n  stream:          " << ToString( stream->index )
                << "\n  codec: . . . . . " << codec->name << "  - " << codec->long_name
                << "\n  codec_id:        " << ToString( uint( codec->id ))
                << "\n  type:  . . . . . " << ToString( codec->type )
                << "\n  duration:        " << ToString_HMS( stream->duration > 0 ? (stream->duration * av_q2d( stream->time_base )) : duration );

            if ( codec->type == AVMEDIA_TYPE_VIDEO )
            {
                ASSERT( EnumCast( codec->id ) != Default );
                ASSERT( EnumCast( params->color_range, params->color_primaries, params->color_trc, params->color_space, params->chroma_location ) != Default );

                str << "\n  format:  . . . . " << ToString( AVPixelFormat( params->format ))
                    << "\n  color_range:     " << ToString( params->color_range )
                    << "\n  color_primaries: " << ToString( params->color_primaries )
                    << "\n  color_trc:       " << ToString( params->color_trc )
                    << "\n  color_space: . . " << ToString( params->color_space )
                    << "\n  chroma_location: " << ToString( params->chroma_location )
                    << "\n  nb_frames: . . . " << DivStringBySteps( ToString( stream->nb_frames ))
                    << "\n  avg_frame_rate:  " << ToString( av_q2d( stream->avg_frame_rate )) << " (" << ToString( ToFractional( stream->avg_frame_rate )) << ')'
                    << "\n  r_frame_rate:  . " << ToString( av_q2d( stream->r_frame_rate )) << " (" << ToString( ToFractional( stream->r_frame_rate )) << ')'
                    << "\n  bitrate:         " << DivStringBySteps( ToString( ulong( params->bit_rate > 0 ? params->bit_rate : ctx.formatCtx->bit_rate )))
                    << "\n  dimension: . . . " << ToString( params->width ) << ", " << ToString( params->height );
            }
            if ( codec->type == AVMEDIA_TYPE_AUDIO )
            {
                str << "\n  format:  . . . . " << ToString( AVSampleFormat( params->format ));
                if ( params->bit_rate > 0 )
                    str << "\n  bitrate:         " << DivStringBySteps( ToString( ulong(params->bit_rate) ));

            }
        }
        str << "\n--------";

        // TODO
        /*if ( ctx.formatCtx->metadata != null )
        {
            str << "\n  metadata = {";
            for (const AVDictionaryEntry *tag = ffmpeg.av_dict_iterate( ctx.formatCtx->metadata, null );
                 tag != null;
                 tag = ffmpeg.av_dict_iterate( ctx.formatCtx->metadata, tag ))
            {
                str << "\n    " << tag->key << " = " << tag->value;
            }
            str << "\n  }";
        }*/

        return str;
    }

    String  FFmpegVideoDecoder::PrintFileProperties (const Path &filename) C_Th___
    {
        return  "\nfile: "s << ToString( filename ) <<
                PrintFileProperties( MakeRC<FileRStream>( filename ));
    }

/*
=================================================
    CreateFFmpegDecoder
=================================================
*/
    RC<IVideoDecoder>  VideoFactory::CreateFFmpegDecoder () __NE___
    {
        return MakeRC<FFmpegVideoDecoder>();
    }


} // AE::Video

#else

# include "video/Public/VideoDecoder.h"

namespace AE::Video
{
    RC<IVideoDecoder>  VideoFactory::CreateFFmpegDecoder () __NE___  { return Default; }
}

#endif // AE_ENABLE_FFMPEG
