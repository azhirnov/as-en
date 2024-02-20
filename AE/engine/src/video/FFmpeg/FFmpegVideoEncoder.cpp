// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    based on code from:
    https://stackoverflow.com/questions/46444474/c-ffmpeg-create-mp4-file
    https://ffmpeg.org/doxygen/trunk/encode_video_8c-example.html
    https://ffmpeg.org/doxygen/trunk/remux_8c-example.html
*/

#ifdef AE_ENABLE_FFMPEG
# include "video/FFmpeg/FFmpegVideoEncoder.h"
# include "video/FFmpeg/FFmpegUtils.cpp.h"

namespace AE::Video
{
namespace {
    FFmpegLoader    ffmpeg;
}

/*
=================================================
    constructor
=================================================
*/
    FFmpegVideoEncoder::FFmpegVideoEncoder () __NE___ :
        _videoPacket{},
        _ffmpegLoaded{ ffmpeg.Load() }
    {
        EXLOCK( _guard );
        ASSERT( _ffmpegLoaded );

        if ( _ffmpegLoaded )
            ffmpeg.av_init_packet( OUT &_videoPacket );
    }

/*
=================================================
    destructor
=================================================
*/
    FFmpegVideoEncoder::~FFmpegVideoEncoder () __NE___
    {
        EXLOCK( _guard );

        Unused( _End() );
        _Destroy();

        ffmpeg.Unload();
    }

/*
=================================================
    Begin
=================================================
*/
    bool  FFmpegVideoEncoder::Begin (const Config &cfg, const Path &filename) __NE___
    {
        EXLOCK( _guard );

        CHECK_ERR( _ffmpegLoaded );
        CHECK_ERR( not _encodingStarted );

        ASSERT( _formatCtx == null  and
                _videoFrame == null and
                _videoStream == null );

        _videoFile  = ToString( filename );
        _config     = cfg;

        if ( not _CreateCodec() )
        {
            _Destroy();
            return false;
        }

        _frameCounter       = 0;
        _encodingStarted    = true;

        AE_LOG_DBG( "Used codec: "s << _codec->long_name );
        AE_LOG_DBG( "Begin recording to temporary: '"s << _tempFile << "', resulting: '" << _videoFile << "'" );
        return true;
    }

/*
=================================================
    Begin
=================================================
*/
    bool  FFmpegVideoEncoder::Begin (const Config &cfg, RC<WStream> tempStream, RC<WStream> dstStream) __NE___
    {
        EXLOCK( _guard );

        CHECK_ERR( _ffmpegLoaded );
        CHECK_ERR( not _encodingStarted );

        CHECK_ERR( tempStream and tempStream->IsOpen() );
        CHECK_ERR( dstStream  and dstStream->IsOpen()  );

        ASSERT( _formatCtx == null  and
                _videoFrame == null and
                _videoStream == null );

        _tempStream = RVRef(tempStream);
        _dstStream  = RVRef(dstStream);
        _config     = cfg;

        if ( not _CreateCodec() )
        {
            _Destroy();
            return false;
        }

        _frameCounter       = 0;
        _encodingStarted    = true;

        AE_LOG_DBG( "Used codec: "s << _codec->long_name );
        return true;
    }

/*
=================================================
    _ValidateResolution
=================================================
*/
    void  FFmpegVideoEncoder::_ValidateResolution ()
    {
        // round to multiple of 4
        _config.dstDim.x = AlignDown( _config.srcDim.x, 2_pot );
        _config.dstDim.y = AlignDown( _config.srcDim.y, 2_pot );
    }

/*
=================================================
    _CreateCodec
=================================================
*/
    bool  FFmpegVideoEncoder::_CreateCodec ()
    {
        bool    scaling = false;

        if ( All( IsZero( _config.dstDim )) )
            _config.dstDim = _config.srcDim;

        if ( _config.bitrate == Bitrate_t{0} )
            _config.bitrate = _CalcBitrate( _config );

        for (;;)
        {
            if ( _CreateCodecImpl() )
                break;

            if ( not scaling )
            {
                // fix resolution and try again
                _ValidateResolution();
                scaling = true;
                continue;
            }

            // can't find codec
            return false;
        }
        return true;
    }

/*
=================================================
    _CreateCodec2
=================================================
*/
    bool  FFmpegVideoEncoder::_CreateCodecImpl ()
    {
        const CodecInfo     codec_info  = _GetEncoderInfo( _config );
        const AVCodec*      codec       = null;

        _remuxRequired  = codec_info.remux;
        _hasBFrames     = codec_info.hasBFrames;

        for (auto* codec_name : codec_info.codecs)
        {
            codec = ffmpeg.avcodec_find_encoder_by_name( codec_name );
            if ( codec != null and _CreateStream( codec, codec_info.format ))
                return true;
        }

        const AVOutputFormat*   format = ffmpeg.av_guess_format( codec_info.format, null, null );
        if ( format == null )
            return false;

        AE_LOG_DBG( "Failed to find encoder for format: "s << format->long_name );
        return false;
    }

/*
=================================================
    _GetEncoderInfo
----
    add list of hardware accelerated encoders
    https://trac.ffmpeg.org/wiki/HWAccelIntro
=================================================
*/
    FFmpegVideoEncoder::CodecInfo  FFmpegVideoEncoder::_GetEncoderInfo (const Config &cfg)
    {
        CodecInfo   result;

        result.remux        = true;
        result.hasBFrames   = false;

        switch_enum( cfg.codec )
        {
            case EVideoCodec::H264 :
            {
                result.format = "h264";

                if ( cfg.hwAccelerated )
                {
                    // nvidia gpu
                    // see https://developer.nvidia.com/video-encode-and-decode-gpu-support-matrix-new
                    if ( cfg.targetGPU >= EGraphicsDeviceID::_NV_Begin and cfg.targetGPU <= EGraphicsDeviceID::_NV_End )
                    {
                        result.codecs.push_back( "h264_nvenc" );
                        result.hasBFrames   = (cfg.targetGPU >= EGraphicsDeviceID::NV_Turing);  // requires 7th Gen+
                    }

                    // amd gpu (windows only)
                    if ( cfg.targetGPU >= EGraphicsDeviceID::_AMD_Begin and cfg.targetGPU <= EGraphicsDeviceID::_AMD_End )
                        result.codecs.push_back( "h264_amf" );

                    // intel cpu
                    if ( cfg.targetCPU == ECPUVendor::Intel )
                        result.codecs.push_back( "h264_qsv" );

                //  result.codecs.push_back( "h264_v4l2m2m" );      // linux only   // TODO
                //  result.codecs.push_back( "h264_vaapi" );        // linux only
                //  result.codecs.push_back( "h264_videotoolbox" ); // MacOS only
                }
                result.codecs.push_back( "libx264" );
                break;
            }
            case EVideoCodec::H265 :
            {
                result.format = "hevc";

                if ( cfg.hwAccelerated )
                {
                    // nvidia gpu
                    // see https://developer.nvidia.com/video-encode-and-decode-gpu-support-matrix-new
                    if ( cfg.targetGPU >= EGraphicsDeviceID::_NV_Begin and cfg.targetGPU <= EGraphicsDeviceID::_NV_End )
                    {
                        result.codecs.push_back( "hevc_nvenc" );
                        result.hasBFrames   = (cfg.targetGPU >= EGraphicsDeviceID::NV_Turing);  // requires 7th Gen+
                    }

                    // amd gpu (windows only)
                    if ( cfg.targetGPU >= EGraphicsDeviceID::_AMD_Begin and cfg.targetGPU <= EGraphicsDeviceID::_AMD_End )
                        result.codecs.push_back( "hevc_amf" );

                    // intel cpu
                    if ( cfg.targetCPU == ECPUVendor::Intel )
                        result.codecs.push_back( "hevc_qsv" );
                }
                result.codecs.push_back( "libx265" );
                break;
            }
            case EVideoCodec::VP8 :
            {
                result.format = "webm";
                result.codecs.push_back( "libvpx" );
                break;
            }
            case EVideoCodec::VP9 :
            {
                result.format = "webm";
                result.codecs.push_back( "libvpx-vp9" );
                break;
            }
            case EVideoCodec::WEBP :
            {
                result.remux    = false;
                result.format   = "webp";
                result.codecs.push_back( "libwebp_anim" );
                break;
            }
            case EVideoCodec::AV1 :
            {
                result.format   = "av1";
                result.codecs.push_back( "libdav1d" );
                result.codecs.push_back( "libaom-av1" );
                result.codecs.push_back( "libsvtav1" );
                result.codecs.push_back( "librav1e" );
                break;
            }

            case EVideoCodec::GIF :
            case EVideoCodec::MPEG4 :       // TODO
            case EVideoCodec::Unknown :
            case EVideoCodec::_Count :
                break;
        }
        switch_end

        return result;
    }

/*
=================================================
    _CalcBitrate
=================================================
*/
    IVideoEncoder::Bitrate_t  FFmpegVideoEncoder::_CalcBitrate (const Config &)
    {
        // TODO
        return {};
    }

/*
=================================================
    _CreateStream
=================================================
*/
    bool  FFmpegVideoEncoder::_CreateStream (const AVCodec* codec, const char* videoFormat)
    {
        _DestroyStream();

        _codec      = codec;
        _tempFile   = _remuxRequired ? (_videoFile + ".temp") : _videoFile;

        // create codec context
        {
            _format = ffmpeg.av_guess_format( videoFormat, null, null );
            CHECK_ERR( _format != null );

            FF_CHECK_ERR( ffmpeg.avformat_alloc_output_context2( OUT &_formatCtx, _format, null, _tempFile.c_str() ));

            _codecCtx = ffmpeg.avcodec_alloc_context3( _codec );
            CHECK_ERR( _codecCtx != null );
        }

        // create video stream
        {
            _videoStream = ffmpeg.avformat_new_stream( _formatCtx, _codec );
            CHECK_ERR( _videoStream != null );

            _videoStream->codecpar->codec_id    = _codec->id;
            _videoStream->codecpar->codec_type  = _codec->type;
            _videoStream->codecpar->width       = int(_config.dstDim.x);
            _videoStream->codecpar->height      = int(_config.dstDim.y);
            _videoStream->codecpar->format      = EnumCast( _config.dstFormat );
            _videoStream->codecpar->bit_rate    = slong(_config.bitrate.GetNonScaled());
            _videoStream->time_base             = ToAVRational( _config.framerate );

            _ValidatePixelFormat( OUT _videoStream->codecpar->format );

            CHECK( EnumCast( _config.colorPreset,
                             OUT _videoStream->codecpar->color_range,
                             OUT _videoStream->codecpar->color_primaries,
                             OUT _videoStream->codecpar->color_trc,
                             OUT _videoStream->codecpar->color_space,
                             OUT _videoStream->codecpar->chroma_location ));

            FF_CHECK( ffmpeg.avcodec_parameters_to_context( _codecCtx, _videoStream->codecpar ));

            _codecCtx->time_base        = ToAVRationalRec( _config.framerate );
            _codecCtx->framerate        = ToAVRational( _config.framerate );
            _codecCtx->max_b_frames     = _hasBFrames ? 1 : 0;
            _codecCtx->gop_size         = 10;   // emit one intra frame every X frames

            if ( _formatCtx->oformat->flags & AVFMT_GLOBALHEADER )
                _codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            FF_CHECK( ffmpeg.avcodec_parameters_from_context( _videoStream->codecpar, _codecCtx ));
        }

        // create IO context
        if ( _tempStream )
        {
            _ioCtx = ffmpeg.avio_alloc_context( null, 0, 1, _tempStream.get(), null, &_IOWritePacket, null );
            CHECK_ERR( _ioCtx != null );

            _formatCtx->pb      = _ioCtx;
            _formatCtx->flags   |= AVFMT_FLAG_CUSTOM_IO;
        }

        // create output file
        {
            AVDictionary*   codec_options   = null;
            _SetOptions( INOUT &codec_options );

            int err = ffmpeg.avcodec_open2( _codecCtx, _codec, INOUT &codec_options );
            ffmpeg.av_dict_free( &codec_options );
            FF_CHECK( err );

            // try without options
            if ( err != 0 )
            {
                err = ffmpeg.avcodec_open2( _codecCtx, _codec, null );
                FF_CHECK_ERR( err );
            }

            if ( _ioCtx == null and not (_format->flags & AVFMT_NOFILE) )
                FF_CHECK_ERR( ffmpeg.avio_open( OUT &_formatCtx->pb, _tempFile.c_str(), AVIO_FLAG_WRITE ));

            FF_CHECK_ERR( ffmpeg.avformat_write_header( _formatCtx, null ));

            ffmpeg.av_dump_format( _formatCtx, 0, _tempFile.c_str(), 1 );

            _videoFrame                     = ffmpeg.av_frame_alloc();
            _videoFrame->format             = _codecCtx->pix_fmt;
            _videoFrame->width              = int(_config.dstDim.x);
            _videoFrame->height             = int(_config.dstDim.y);
            _videoFrame->color_range        = _codecCtx->color_range;
            _videoFrame->color_primaries    = _codecCtx->color_primaries;
            _videoFrame->color_trc          = _codecCtx->color_trc;
            _videoFrame->colorspace         = _codecCtx->colorspace;
            _videoFrame->chroma_location    = _codecCtx->chroma_sample_location;

            FF_CHECK_ERR( ffmpeg.av_frame_get_buffer( _videoFrame, 32 ));
        }

        // create scaler
        {
            const AVPixelFormat src_fmt = PixelFormatCast( _config.srcFormat );
            const int           filter  = EnumCast( _config.filter );

            CHECK_ERR( src_fmt != AV_PIX_FMT_NONE );

            _swsCtx = ffmpeg.sws_getContext( int(_config.srcDim.x), int(_config.srcDim.y), src_fmt,
                                             _codecCtx->width, _codecCtx->height, _codecCtx->pix_fmt,
                                             filter, null, null, null );
            CHECK_ERR( _swsCtx != null );
        }
        return true;
    }

/*
=================================================
    _ValidatePixelFormat
=================================================
*/
    void  FFmpegVideoEncoder::_ValidatePixelFormat (OUT int &outFormat) const
    {
        // check supported pixel format
        EnumSet<EVideoFormat>   supported_fmts;

        for (auto* fmt = _codec->pix_fmts; (fmt != null) and (*fmt != AV_PIX_FMT_NONE); ++fmt)
        {
            EVideoFormat    vf = EnumCast( *fmt );
            if ( vf != Default )
                supported_fmts.set( vf, true );
        }

        // choose another format
        EVideoFormat    required_fmt = _config.dstFormat;

        if ( required_fmt > EVideoFormat::_Count or
             not supported_fmts.contains( required_fmt ))
        {
            // remplace by any supported format
            required_fmt = supported_fmts.ExtractFirst();
        }

        outFormat = EnumCast( required_fmt );
    }

/*
=================================================
    _SetOptions
=================================================
*/
    void  FFmpegVideoEncoder::_SetOptions (INOUT AVDictionary **dict) const
    {
        if ( _config.quality < 0.f )
            return;

        const uint          quality     = Clamp( uint(_config.quality * 9.f + 0.5f), 0u, 9u );
        FixedString<64>     preset;
        StringView          codec_name {_codec->name};

        if ( codec_name == "h264_nvec" or
             codec_name == "hevc_nvenc" )
        {
            if ( quality <= 4  )
                preset = "fast";
            else
            if ( quality < 6 )
                preset = "medium";
            else
                preset = "slow";
        }
        else
        //if ( codec_name == "h264" or codec_name == "hevc" )
        {
            switch ( quality )
            {
                case 0 :    preset = "ultrafast";   break;
                case 1 :    preset = "superfast";   break;
                case 2 :    preset = "veryfast";    break;
                case 3 :    preset = "faster";      break;
                case 4 :    preset = "fast";        break;
                case 5 :    preset = "medium";      break;
                case 6 :    preset = "slow";        break;
                case 7 :    preset = "slower";      break;
                case 8 :    preset = "veryslow";    break;
                case 9 :    break;
            }
        }

        if ( preset.size() )
            FF_CHECK( ffmpeg.av_dict_set( INOUT dict, "preset", preset.c_str(), 0 ));
    }

/*
=================================================
    AddFrame
=================================================
*/
    bool  FFmpegVideoEncoder::AddFrame (const ImageMemView &view, Bool endOnError) __NE___
    {
        EXLOCK( _guard );

        if_unlikely( not _encodingStarted )
            return false;

        ASSERT( _codecCtx != null );
        CHECK_ERR( view.Parts().size() == 1 );
        CHECK_ERR( int(view.Dimension().x) == _codecCtx->width );
        CHECK_ERR( int(view.Dimension().y) == _codecCtx->height );
        CHECK_ERR( view.Dimension().z == 1 );
        CHECK_ERR( view.Format() == _config.srcFormat );

        if_likely( _AddFrameImpl( view ))
        {
            ++_frameCounter;
            return true;
        }

        // if failed to add first frame, try to recreate stream with aligned resolution
        if_unlikely( _frameCounter == 0 )
        {
            AE_LOG_DBG( "Failed to encode frame, try to change resolution" );
            _ValidateResolution();

            const AVCodec*  codec       = _codec;
            String          fmt_name    = _format->name;

            if ( _CreateStream( codec, fmt_name.c_str() ))
            {
                if ( _AddFrameImpl( view ))
                {
                    ++_frameCounter;
                    return true;
                }
            }
        }

        if_unlikely( endOnError )
        {
            Unused( _End() );
        }

        return false;
    }

/*
=================================================
    AddFrame
=================================================
*
    bool  FFmpegVideoEncoder::AddFrame (VideoImageID, Bool) __NE___
    {
        // not supported
        return false;
    }

/*
=================================================
    _AddFrameImpl
=================================================
*/
    bool  FFmpegVideoEncoder::_AddFrameImpl (const ImageMemView &view)
    {
        ASSERT( _codecCtx != null   and
                _formatCtx != null  and
                _videoFrame != null and
                _swsCtx != null );

        const int       src_stride  [AV_NUM_DATA_POINTERS]  = { int(view.RowPitch()) };
        const ubyte*    src_slice   [AV_NUM_DATA_POINTERS]  = { Cast<ubyte>( view.Parts().front().ptr )};
        const int       src_slice_y                         = 0;

        const int   scaled_h = ffmpeg.sws_scale( _swsCtx, src_slice, src_stride, src_slice_y, int(_config.srcDim.y),
                                                 OUT _videoFrame->data, _videoFrame->linesize );

        if_unlikely( scaled_h < 0 or scaled_h != _codecCtx->height )
        {
            FF_CHECK( scaled_h );
            return false;
        }

        _videoFrame->pts = _frameCounter;

        FF_CHECK_ERR( ffmpeg.avcodec_send_frame( _codecCtx, _videoFrame ));

        for (int err = 0; err >= 0;)
        {
            err = ffmpeg.avcodec_receive_packet( _codecCtx, &_videoPacket );

            if ( err == AVERROR(EAGAIN) or err == AVERROR_EOF )
                return true;

            FF_CHECK_ERR( err );

            _videoPacket.flags      |= AV_PKT_FLAG_KEY;
            _videoPacket.pts        = _frameCounter;
            _videoPacket.dts        = _frameCounter;
            _videoPacket.duration   = 1;            // TODO
            _videoPacket.pos        = -1;

            err = ffmpeg.av_interleaved_write_frame( _formatCtx, &_videoPacket );
            ffmpeg.av_packet_unref( &_videoPacket );

            FF_CHECK_ERR( err );
        }
        return true;
    }

/*
=================================================
    _Finish
=================================================
*/
    bool  FFmpegVideoEncoder::_Finish ()
    {
        ASSERT( _codecCtx != null   and
                _formatCtx != null );

        FF_CHECK_ERR( ffmpeg.avcodec_send_frame( _codecCtx, null ));

        for (int err = 0; err >= 0;)
        {
            err = ffmpeg.avcodec_receive_packet( _codecCtx, &_videoPacket );

            if ( err == AVERROR(EAGAIN) or err == AVERROR_EOF )
                break;

            FF_CHECK_ERR( err );

            FF_CHECK( ffmpeg.av_interleaved_write_frame( _formatCtx, &_videoPacket ));
            ffmpeg.av_packet_unref( &_videoPacket );
        }

        FF_CHECK( ffmpeg.av_write_trailer( _formatCtx ));
        return true;
    }

/*
=================================================
    End
=================================================
*/
    bool  FFmpegVideoEncoder::End () __NE___
    {
        EXLOCK( _guard );
        return _End();
    }

    bool  FFmpegVideoEncoder::_End ()
    {
        if_unlikely( not _encodingStarted )
            return false;

        CHECK( _Finish() );

        AE_LOG_DBG( "End recording to: '"s << _tempFile << "', start remuxing to: '" << _videoFile << "'" );

        _DestroyStream();

        bool    res = true;

        if ( _remuxRequired )
            res = _Remux();

        _Destroy();
        return res;
    }

/*
=================================================
    GetFileExtension
=================================================
*/
    StringView  FFmpegVideoEncoder::GetFileExtension (EVideoCodec codec) C_NE___
    {
        switch_enum( codec )
        {
            case EVideoCodec::H264 :
            case EVideoCodec::H265 :
            case EVideoCodec::AV1 :
            case EVideoCodec::MPEG4 :
                return "mp4";

            case EVideoCodec::WEBP :
                return "webp";

            case EVideoCodec::VP8 :
            case EVideoCodec::VP9 :
                return "webm";

            case EVideoCodec::GIF :
                return "gif";

            case EVideoCodec::Unknown :
            case EVideoCodec::_Count :  break;
        }
        switch_end
        RETURN_ERR( "unknown codec" );
    }

/*
=================================================
    _DestroyStream
=================================================
*/
    void  FFmpegVideoEncoder::_DestroyStream ()
    {
        if ( (_formatCtx != null) and (_format != null) and not (_format->flags & AVFMT_NOFILE) )
            FF_CHECK( ffmpeg.avio_closep( &_formatCtx->pb ));

        if ( _formatCtx != null )
            ffmpeg.avformat_free_context( _formatCtx );

        if ( _codecCtx != null )
            ffmpeg.avcodec_free_context( &_codecCtx );

        if ( _videoFrame != null )
            ffmpeg.av_frame_free( &_videoFrame );

        if ( _swsCtx != null )
            ffmpeg.sws_freeContext( _swsCtx );

        if ( _ioCtx != null )
            ffmpeg.avio_context_free( &_ioCtx );

        _format         = null;
        _formatCtx      = null;
        _videoStream    = null;
        _videoFrame     = null;
        _codec          = null;
        _codecCtx       = null;
        _swsCtx         = null;
        _ioCtx          = null;
    }

/*
=================================================
    _Destroy
=================================================
*/
    void  FFmpegVideoEncoder::_Destroy ()
    {
        _DestroyStream();

        _remuxRequired      = false;
        _encodingStarted    = false;
        _hasBFrames         = false;

        _frameCounter       = 0;
        _config             = Default;

        _tempFile.clear();
        _videoFile.clear();

        _tempStream         = null;
        _dstStream          = null;
    }

/*
=================================================
    _Remux
=================================================
*/
    bool  FFmpegVideoEncoder::_Remux ()
    {
        CHECK_ERR( not _tempFile.empty() and not _videoFile.empty() );

        AVFormatContext*    ifmt_ctx        = null;
        AVFormatContext*    ofmt_ctx        = null;
        int *               stream_mapping  = null;
        bool                remuxed         = _RemuxImpl( ifmt_ctx, ofmt_ctx, stream_mapping );

        if ( ifmt_ctx != null )
            ffmpeg.avformat_close_input( &ifmt_ctx );

        if ( (ofmt_ctx != null) and not (ofmt_ctx->oformat->flags & AVFMT_NOFILE) )
            ffmpeg.avio_closep( &ofmt_ctx->pb );

        if ( ofmt_ctx != null )
            ffmpeg.avformat_free_context( ofmt_ctx );

        ffmpeg.av_freep( &stream_mapping );

        if ( remuxed )
        {
            CHECK( FileSystem::Remove( _tempFile ));
        }
        else
        {
            // keep temporary file if remux failed
            FileSystem::Remove( _videoFile );
            FileSystem::Rename( _tempFile, _videoFile );
        }
        AE_LOG_DBG( "End remuxing: '"s << _videoFile << "'" );

        return remuxed;
    }

/*
=================================================
    _RemuxImpl
=================================================
*/
    bool  FFmpegVideoEncoder::_RemuxImpl (AVFormatContext* &ifmtCtx, AVFormatContext* &ofmtCtx, int* &streamMapping)
    {
        FF_CHECK_ERR( ffmpeg.avformat_open_input( OUT &ifmtCtx, _tempFile.c_str(), 0, 0 ));
        FF_CHECK_ERR( ffmpeg.avformat_find_stream_info( ifmtCtx, 0 ));
        ffmpeg.av_dump_format( ifmtCtx, 0, _tempFile.c_str(), 0 );

        FF_CHECK_ERR( ffmpeg.avformat_alloc_output_context2( OUT &ofmtCtx, null, null, _videoFile.c_str() ));

        int stream_mapping_size = ifmtCtx->nb_streams;
        streamMapping           = Cast<int>( ffmpeg.av_mallocz_array( stream_mapping_size, sizeof(*streamMapping) ));
        CHECK_ERR( streamMapping != null );

        int stream_index = 0;
        for (uint i = 0; i < ifmtCtx->nb_streams; ++i)
        {
            AVStream *          out_stream  = null;
            AVStream *          in_stream   = ifmtCtx->streams[i];
            AVCodecParameters*  in_codecpar = in_stream->codecpar;

            if ( in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO   and
                 in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO   and
                 in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE )
            {
                streamMapping[i] = -1;
                continue;
            }

            streamMapping[i] = stream_index++;

            out_stream = ffmpeg.avformat_new_stream( ofmtCtx, null );
            CHECK_ERR( out_stream != null );

            FF_CHECK_ERR( ffmpeg.avcodec_parameters_copy( out_stream->codecpar, in_codecpar ));

            out_stream->codecpar->codec_tag = 0;
            out_stream->time_base       = in_stream->time_base;
            out_stream->r_frame_rate    = in_stream->r_frame_rate;
            out_stream->nb_frames       = _frameCounter;
            out_stream->duration        = (ToFractional( out_stream->time_base ) * _config.framerate).Div_RTS( _frameCounter );
            out_stream->start_time      = 0;
        }

        ffmpeg.av_dump_format( ofmtCtx, 0, _videoFile.c_str(), 1 );

        if ( not (ofmtCtx->oformat->flags & AVFMT_NOFILE) )
            FF_CHECK_ERR( ffmpeg.avio_open( &ofmtCtx->pb, _videoFile.c_str(), AVIO_FLAG_WRITE ));

        FF_CHECK_ERR( ffmpeg.avformat_write_header( ofmtCtx, null ));

        AVPacket    pkt = {};
        slong       ts  = 0;

        for (;;)
        {
            AVStream*   in_stream   = null;
            AVStream*   out_stream  = null;

            if ( ffmpeg.av_read_frame( ifmtCtx, OUT &pkt ) < 0 )
                break;

            in_stream  = ifmtCtx->streams[ pkt.stream_index ];
            if ( pkt.stream_index >= stream_mapping_size or
                 streamMapping[ pkt.stream_index ] < 0 )
            {
                ffmpeg.av_packet_unref( &pkt );
                continue;
            }

            pkt.stream_index    = streamMapping[ pkt.stream_index ];
            out_stream          = ofmtCtx->streams[ pkt.stream_index ];

            pkt.pts     = ts;
            pkt.dts     = ts;
            pkt.duration= ffmpeg.av_rescale_q( pkt.duration, in_stream->time_base, out_stream->time_base );
            pkt.pos     = -1;

            ts += pkt.duration;

            int     err = ffmpeg.av_interleaved_write_frame( ofmtCtx, &pkt );
            ffmpeg.av_packet_unref( &pkt );

            if_unlikely( err < 0 )
            {
                FF_CHECK( err );
                break;
            }
        }

        FF_CHECK( ffmpeg.av_write_trailer( ofmtCtx ));
        return true;
    }

/*
=================================================
    _IOWritePacket
=================================================
*/
    int  FFmpegVideoEncoder::_IOWritePacket (void* opaque, ubyte* buf, int buf_size)
    {
        auto*   stream = Cast<WStream>( opaque );

        if_unlikely( buf_size < 0 )
            return AVERROR_UNKNOWN;

        return int(stream->WriteSeq( buf, Bytes{ulong(buf_size)} ));
    }

/*
=================================================
    CreateFFmpegEncoder
=================================================
*/
    RC<IVideoEncoder>  VideoFactory::CreateFFmpegEncoder () __NE___
    {
        return MakeRC<FFmpegVideoEncoder>();
    }


} // AE::Video

#else

# include "video/Public/VideoEncoder.h"

namespace AE::Video
{
    RC<IVideoEncoder>  VideoFactory::CreateFFmpegEncoder () __NE___  { return Default; }
}

#endif // AE_ENABLE_FFMPEG
