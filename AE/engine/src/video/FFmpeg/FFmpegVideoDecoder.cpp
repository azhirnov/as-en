// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_FFMPEG
# include "video/FFmpeg/FFmpegVideoDecoder.h"
# include "video/FFmpeg/FFmpegUtils.cpp.h"

namespace AE::Video
{
/*
=================================================
	constructor
=================================================
*/
	FFmpegVideoDecoder::FFmpegVideoDecoder () __NE___
	{
		ASSERT( _ffmpeg.IsLoaded() );
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
	}

/*
=================================================
	_CreateVideoCodec
=================================================
*/
	bool  FFmpegVideoDecoder::_CreateVideoCodec (const AVStream* videoStream, OUT AVCodec const* &outCodec, OUT AVCodecContext* &outCodecCtx) C_NE___
	{
		struct CodecCtx
		{
			FFmpegFnTableRef	ffmpeg;
			AVCodecContext*		ctx		= null;

			CodecCtx (const FFmpegFnTable &fn) : ffmpeg{fn} {}
			~CodecCtx ()								{ if ( ctx != null ) ffmpeg->avcodec_free_context( &ctx ); }

			operator AVCodecContext* ()					{ return ctx; }
			AVCodecContext*  operator -> ()				{ return ctx; }

			void  operator = (AVCodecContext* newCtx)	{ if ( ctx != null ) ffmpeg->avcodec_free_context( &ctx );  ctx = newCtx; }

			ND_ AVCodecContext*  Release ()				{ NonNull( ctx );  return Exchange( ctx, null ); }
		};
		CodecCtx	codec_ctx	{_ffmpeg};
		const auto	codec_id	= videoStream->codecpar->codec_id;
		//const auto	format	= AVPixelFormat(videoStream->codecpar->format);

		ASSERT( outCodec == null );
		ASSERT( outCodecCtx == null );

		const auto	CreateCodecCtx = [&] (const AVCodec* codec) -> bool
		{{
			if ( codec == null )
				return false;

			ASSERT( _ffmpeg->av_codec_is_decoder( codec ) != 0 );

			if ( not (codec->capabilities & AV_CODEC_CAP_HARDWARE) and _config.hwAccelerated == EHwAcceleration::Require )
				return false;

			if ( (codec->capabilities & AV_CODEC_CAP_HARDWARE) and _config.hwAccelerated == EHwAcceleration::Disable )
				return false;

			codec_ctx = _ffmpeg->avcodec_alloc_context3( codec );
			if ( codec_ctx == null )
				return false;

			FF_CHECK_ERR( _ffmpeg->avcodec_parameters_to_context( codec_ctx, videoStream->codecpar ));

			// override some parameters
			if ( AllBits( codec->capabilities, AV_CODEC_CAP_FRAME_THREADS ))
			{
				codec_ctx->thread_type	= FF_THREAD_FRAME;
				codec_ctx->thread_count	= Max( codec_ctx->thread_count, int(_config.threadCount) );
			}
			else
			if ( AllBits( codec->capabilities, AV_CODEC_CAP_SLICE_THREADS ))
			{
				codec_ctx->thread_type	= FF_THREAD_SLICE;
				codec_ctx->thread_count	= Max( codec_ctx->thread_count, int(_config.threadCount) );
			}

			FF_CHECK_ERR( _ffmpeg->avcodec_open2( codec_ctx, codec, null ));

			// free ffmpeg thread pool if used custom thread pool via codec_ctx->execute and execute2
			// _ffmpeg->ff_thread_free( codec_ctx );

			outCodec	= codec;
			outCodecCtx	= codec_ctx.Release();
			return true;
		}};

		const auto	CreateCodecCtx2 = [&] (const char* codecName) -> bool
		{{
			auto*	codec = _ffmpeg->avcodec_find_decoder_by_name( codecName );
			if ( codec == null )
				return false;

			// check format  // TODO: codec created even if format is not supported
			/*if ( codec->pix_fmts != null )
			{
				for (auto* pix_fmts = codec->pix_fmts;  *pix_fmts != -1; ++pix_fmts)
				{
					if ( format == CorrectPixFormat( *pix_fmts ))
						return CreateCodecCtx( codec );
				}
				return false;
			}*/
			return CreateCodecCtx( codec );
		}};

		return _ChooseVideoCodec( _config, codec_id, CreateCodecCtx, CreateCodecCtx2 );
	}

/*
=================================================
	_ChooseVideoCodec
=================================================
*/
	template <typename ConfigType, typename CreateFn1, typename CreateFn2>
	bool  FFmpegVideoDecoder::_ChooseVideoCodec (const ConfigType &cfg, const AVCodecID codecId, CreateFn1 &&createCodec1, CreateFn2 &&createCodec2) C_NE___
	{
		if ( cfg.hwAccelerated == EHwAcceleration::Disable )
		{
			// search for codec with frame threads support
			for (void* opaque = null;;)
			{
				auto*	codec = _ffmpeg->av_codec_iterate( &opaque );
				if ( codec == null )
					break;

				if ( codec->id != codecId )
					continue;

				if ( (codec->capabilities & AV_CODEC_CAP_FRAME_THREADS) and
					 createCodec1( codec ))
					return true;
			}
		}
		else
		{
			const bool	is_NV		= (cfg.targetGPU >= EGraphicsDeviceID::_NV_Begin and cfg.targetGPU <= EGraphicsDeviceID::_NV_End);
			const bool	is_Intel	= cfg.targetCPU == ECPUVendor::Intel or
									  (cfg.targetGPU >= EGraphicsDeviceID::_Intel_Begin and cfg.targetGPU <= EGraphicsDeviceID::_Intel_End);

			// choose known decoder
			switch ( codecId )
			{
				case AV_CODEC_ID_MPEG4 :
				{
					if ( is_NV and createCodec2( "mpeg4_cuvid" ))
						return true;

					break;
				}
				case AV_CODEC_ID_H264 :
				{
					if ( is_NV and createCodec2( "h264_cuvid" ))
						return true;

					if ( is_Intel and createCodec2( "h264_qsv" ))
						return true;

					break;
				}
				case AV_CODEC_ID_H265 :
				{
					if ( is_NV and createCodec2( "hevc_cuvid" ))
						return true;

					if ( is_Intel and createCodec2( "hevc_qsv" ))
						return true;

					break;
				}
				case AV_CODEC_ID_AV1 :
				{
					if ( is_NV and createCodec2( "av1_cuvid" ))
						return true;

					if ( is_Intel and createCodec2( "av1_qsv" ))
						return true;

					break;
				}
			}
		}
		return createCodec1( _ffmpeg->avcodec_find_decoder( codecId ));
	}

/*
=================================================
	_CreateAudioCodec
=================================================
*/
	bool  FFmpegVideoDecoder::_CreateAudioCodec (const AVStream* audioStream, OUT AVCodec const* &codec, OUT AVCodecContext* &codecCtx) C_NE___
	{
		// TODO

		return true;
	}

/*
=================================================
	Begin
=================================================
*/
	bool  FFmpegVideoDecoder::Begin (const Config &cfg, const Path &filename) __NE___
	{
		EXLOCK( _guard );

		CHECK_ERR( _ffmpeg.IsLoaded() );
		CHECK_ERR( not _decodingStarted );

		_Destroy();

		_config = cfg;

		// open media file
		{
			const String	fname = ToString( filename );

			_formatCtx = _ffmpeg->avformat_alloc_context();
			CHECK_ERR( _formatCtx != null );

			FF_CHECK_ERR( _ffmpeg->avformat_open_input( &_formatCtx, fname.c_str(), null, null ));
		}

		return _Begin();
	}

	bool  FFmpegVideoDecoder::Begin (const Config &cfg, RC<RStream> stream) __NE___
	{
		EXLOCK( _guard );

		CHECK_ERR( _ffmpeg.IsLoaded() );
		CHECK_ERR( not _decodingStarted );

		CHECK_ERR( stream and stream->IsOpen() );

		_Destroy();

		_config		= cfg;
		_rstream	= RVRef(stream);

		// open media file
		{
			_formatCtx = _ffmpeg->avformat_alloc_context();
			CHECK_ERR( _formatCtx != null );

			_ioCtx = _ffmpeg->avio_alloc_context( null, 0, 0, _rstream.get(), &_IOReadPacket, null, &_IOSeek );
			CHECK_ERR( _ioCtx != null );

			_ioCtx->seekable	= AVIO_SEEKABLE_NORMAL;
			_ioCtx->direct		= 1;

			_formatCtx->pb		= _ioCtx;
			_formatCtx->flags	|= AVFMT_FLAG_CUSTOM_IO;

			FF_CHECK_ERR( _ffmpeg->avformat_open_input( &_formatCtx, null, null, null ));
		}

		return _Begin();
	}

/*
=================================================
	_Begin
=================================================
*/
	bool  FFmpegVideoDecoder::_Begin () __NE___
	{
		FF_CHECK_ERR( _ffmpeg->avformat_find_stream_info( _formatCtx, null ));

		// find video stream
		{
			AVStream*	video_stream = null;

			if ( _config.videoStreamIdx < 0 )
			{
				_config.videoStreamIdx = _ffmpeg->av_find_best_stream( _formatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, null, 0 );
				CHECK_ERR( _config.videoStreamIdx >= 0 );
				video_stream = _formatCtx->streams[ _config.videoStreamIdx ];
			}
			else
			{
				CHECK_ERR( _config.videoStreamIdx < int(_formatCtx->nb_streams) );
				video_stream = _formatCtx->streams[ _config.videoStreamIdx ];
			}

			CHECK_ERR( video_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO );
			CHECK_ERR( _CreateVideoCodec( video_stream, OUT _video.codec, OUT _video.codecCtx ));
		}

		// find audio stream
		{
			if ( _config.audioStreamIdx < 0 )
				_config.audioStreamIdx = _ffmpeg->av_find_best_stream( _formatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, null, 0 );

			if ( _config.audioStreamIdx >= 0 )
			{
				CHECK_ERR( _config.audioStreamIdx < int(_formatCtx->nb_streams) );

				AVStream*	audio_stream = _formatCtx->streams[ _config.audioStreamIdx ];

				CHECK_ERR( audio_stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO );
				CHECK_ERR( _CreateAudioCodec( audio_stream, OUT _audio.codec, OUT _audio.codecCtx ));
			}
		}

		// create frame
		{
			_video.frame = _ffmpeg->av_frame_alloc();
			CHECK_ERR( _video.frame != null );

			_avPacket = _ffmpeg->av_packet_alloc();
			CHECK_ERR( _avPacket != null );
		}

		const uint2		src_dim {int2{ _video.codecCtx->width, _video.codecCtx->height }};

		if ( All( IsZero( _config.dstDim )) )
			_config.dstDim = src_dim;

		// create scaler
		{
			const AVPixelFormat	src_fmt = CorrectPixFormat( _video.codecCtx->pix_fmt );
			AVPixelFormat		dst_fmt = PixelFormatCast( _config.dstFormat );

			if ( dst_fmt == AV_PIX_FMT_NONE )
			{
				dst_fmt				= src_fmt;
				_config.dstFormat	= PixelFormatCast( src_fmt );
			}

			CHECK_ERR(	src_fmt != AV_PIX_FMT_NONE and
						dst_fmt != AV_PIX_FMT_NONE and
						_config.dstFormat != Default );

			if ( src_fmt		== dst_fmt			and
				 All( src_dim	== _config.dstDim ))
			{
				// don't create '_video.swsCtx'
			}
			else
			{
				const int	filter = EnumCast( _config.filter );
				_video.swsCtx = _ffmpeg->sws_getContext( int(src_dim.x), int(src_dim.y), src_fmt,
														 int(_config.dstDim.x), int(_config.dstDim.y), dst_fmt,
														 filter, null, null, null );
				CHECK_ERR( _video.swsCtx != null );
			}
		}

		_decodingStarted = true;

		AE_LOG_DBG( "Created decoder with codec: "s << _video.codec->long_name << " (" << _video.codec->name << ')' );
		return true;
	}

/*
=================================================
	_PTStoFrameIdx / _FrameIdxToPTS / _TimestampToPTS
=================================================
*/
	ulong  FFmpegVideoDecoder::_PTStoFrameIdx (slong pts) C_NE___
	{
		return PTStoFrameIndex( _formatCtx->streams[ _config.videoStreamIdx ], pts );
	}

	slong  FFmpegVideoDecoder::_FrameIdxToPTS (ulong frameIdx) C_NE___
	{
		return FrameIndexToPTS( _formatCtx->streams[ _config.videoStreamIdx ], frameIdx );
	}

	slong  FFmpegVideoDecoder::_TimestampToPTS (Seconds_t timestamp) C_NE___
	{
		return TimestampToPTS( _formatCtx->streams[ _config.videoStreamIdx ], _formatCtx->duration, timestamp );
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

		ulong	target_pts = _FrameIdxToPTS( frameIdx );

		return _SeekTo( target_pts );
	}

	bool  FFmpegVideoDecoder::SeekTo (Seconds_t timestamp) __NE___
	{
		EXLOCK( _guard );

		if_unlikely( not _decodingStarted )
			return false;

		ulong	target_pts = _TimestampToPTS( timestamp );

		return _SeekTo( target_pts );
	}

	bool  FFmpegVideoDecoder::_SeekTo (slong targetPTS) __NE___
	{
		ASSERT( _decodingStarted			and
				_formatCtx		!= null		and
				_config.videoStreamIdx >= 0	and
				_config.videoStreamIdx < int(_formatCtx->nb_streams) );

		AVStream*	vstream = _formatCtx->streams[ _config.videoStreamIdx ];
		Unused( vstream );
		ASSERT( vstream->nb_frames == 0 or targetPTS < vstream->nb_frames );

		FF_CHECK_ERR( _ffmpeg->av_seek_frame( _formatCtx, _config.videoStreamIdx, targetPTS, AVSEEK_FLAG_BACKWARD ));

		// av_seek_frame takes effect after one frame
		for (; _ffmpeg->av_read_frame( _formatCtx, OUT _avPacket ) >= 0;)
		{
			if ( _avPacket->stream_index != _config.videoStreamIdx )
			{
				_ffmpeg->av_packet_unref( _avPacket );
				continue;	// skip other streams
			}

			int	err = _ffmpeg->avcodec_send_packet( _video.codecCtx, _avPacket );
			FF_CHECK_ERR( err );

			err = _ffmpeg->avcodec_receive_frame( _video.codecCtx, OUT _video.frame );
			_ffmpeg->av_packet_unref( _avPacket );

			if ( err == AVERROR(EAGAIN) or err == AVERROR_EOF )
				continue;

			FF_CHECK_ERR( err );
			break;
		}
		return true;
	}

/*
=================================================
	GetVideoFrame
=================================================
*/
	bool  FFmpegVideoDecoder::GetVideoFrame (INOUT ImageMemViewArr& imagePlanes, OUT FrameInfo &info) __NE___
	{
		EXLOCK( _guard );

		if_unlikely( not _decodingStarted )
			return false;

		CHECK_ERR(	_formatCtx	!= null			and
					_avPacket	!= null			and
					_config.videoStreamIdx >= 0	and
					_config.videoStreamIdx < int(_formatCtx->nb_streams) );

		for (; _ffmpeg->av_read_frame( _formatCtx, OUT _avPacket ) >= 0;)
		{
			if ( _avPacket->stream_index == _config.videoStreamIdx )
			{
				bool	res = _GetVideoFrame( INOUT imagePlanes, OUT info );
				_ffmpeg->av_packet_unref( _avPacket );
				return res;
			}
		}

		return false;
	}

/*
=================================================
	GetAudioVideoFrame
=================================================
*/
	bool  FFmpegVideoDecoder::GetAudioVideoFrame (INOUT ImageMemViewArr &imagePlanes, INOUT AudioSampleArr &samples, OUT FrameInfo &info) __NE___
	{
		EXLOCK( _guard );

		if_unlikely( not _decodingStarted )
			return false;

		CHECK_ERR(	_formatCtx	!= null	and
					_avPacket	!= null	);

		uint		success		= 0;
		uint		complete	= 0;
		const uint	expected	= uint(_config.videoStreamIdx >= 0) + uint(_config.audioStreamIdx >= 0);

		for (; _ffmpeg->av_read_frame( _formatCtx, OUT _avPacket ) >= 0;)
		{
			if ( _avPacket->stream_index == _config.videoStreamIdx )
			{
				success += uint{_GetVideoFrame( INOUT imagePlanes, OUT info )};
				complete++;
			}
			else
			if ( _avPacket->stream_index == _config.audioStreamIdx )
			{
				success += uint{_GetAudioSamples( INOUT samples )};
				complete++;
			}

			_ffmpeg->av_packet_unref( _avPacket );

			if ( complete == expected )
				break;
		}

		return success == expected;
	}

/*
=================================================
	_GetVideoFrame
=================================================
*/
	inline bool  FFmpegVideoDecoder::_GetVideoFrame (INOUT ImageMemViewArr &memView, OUT FrameInfo &outInfo) __NE___
	{
		CHECK_ERR( _video.codecCtx != null );

		ASSERT( (memView.size() > 1) == EPixelFormat_IsYcbcr( _config.dstFormat ));
		ASSERT( _avPacket->stream_index == _config.videoStreamIdx );

		AVStream*	vstream = _formatCtx->streams[ _config.videoStreamIdx ];

		// read frame to packet
		{
			int		err		= _ffmpeg->avcodec_send_packet( _video.codecCtx, _avPacket );
			double	scale	= av_q2d( vstream->time_base );

			outInfo.timestamp	= Seconds_t{ _avPacket->pts * scale };
			outInfo.duration	= Seconds_t{ _avPacket->duration * scale };
			outInfo.frameIdx	= _PTStoFrameIdx( _avPacket->pts );

			FF_CHECK_ERR( err );
		}

		// receive frame into '_video.frame'
		for (;;)
		{
			int err = _ffmpeg->avcodec_receive_frame( _video.codecCtx, OUT _video.frame );

			if_unlikely( err == AVERROR_EOF )
				return false;

			if_unlikely( err == AVERROR(EAGAIN) )
				break;

			if_unlikely( err < 0 )
			{
				FF_CHECK( err );
				return false;
			}

			if ( memView.size() == 1 )
				return _ScaleFrame1( INOUT memView[0] );
			else
				return _ScaleFrame2( INOUT memView );
		}
		return false;
	}

/*
=================================================
	_ScaleFrame1
=================================================
*/
	inline bool  FFmpegVideoDecoder::_ScaleFrame1 (INOUT ImageMemView & memView) __NE___
	{
		ASSERT( memView.Parts().size() == 1 );
		ASSERT( memView.Format() == _config.dstFormat );
		ASSERT( All( memView.Dimension() == uint3{_config.dstDim, 1} ));

		const int	src_slice_y							= 0;
		const int	dst_stride [AV_NUM_DATA_POINTERS]	= { int(memView.RowPitch()) };
		ubyte*		dst_data   [AV_NUM_DATA_POINTERS]	= { Cast<ubyte>(memView.Parts().front().ptr) };

		const int	scaled_h = _ffmpeg->sws_scale( _video.swsCtx, _video.frame->data, _video.frame->linesize, src_slice_y, _video.frame->height,
													OUT dst_data, dst_stride );

		if_unlikely( scaled_h < 0 or scaled_h != int(_config.dstDim.y) )
		{
			FF_CHECK( scaled_h );
			return false;
		}

		return true;
	}

/*
=================================================
	_ScaleFrame2
=================================================
*/
	inline bool  FFmpegVideoDecoder::_ScaleFrame2 (INOUT ImageMemViewArr &memViewArr) __NE___
	{
		ImageMemViewArr	image_planes = memViewArr;

		// validate
	  #ifdef AE_DEBUG
		for (usize i = 0; i < image_planes.size(); ++i)
		{
			EPixelFormat	plane_fmt;
			uint2			dim_scale;
			CHECK( EPixelFormat_GetPlaneInfo( _config.dstFormat, EImageAspect_Plane(i), OUT plane_fmt, OUT dim_scale ));

			auto&	view = image_planes[i];
			CHECK( view.Format() == plane_fmt );
			CHECK( All( IsMultipleOf( _config.dstDim, dim_scale )));
			CHECK( All( uint2{view.Dimension()} == (_config.dstDim / dim_scale) ));
		}
	  #endif

		{
			usize	count = 0;
			for (; _video.frame->data[count] != null and (count < AV_NUM_DATA_POINTERS); ++count) {}

			CHECK_ERR( image_planes.size() <= count );
			image_planes.resize( count );
		}

		// scale frame
		if ( _video.swsCtx != null )
		{
			const int	src_slice_y							= 0;
			int			dst_stride [AV_NUM_DATA_POINTERS]	= {};
			ubyte*		dst_data   [AV_NUM_DATA_POINTERS]	= {};

			for (usize i = 0; i < image_planes.size(); ++i)
			{
				auto&	view = image_planes[i];
				ASSERT( view.Parts().size() == 1 );	// TODO: error?

				dst_stride[i]	= int(view.RowPitch());
				dst_data[i]		= Cast<ubyte>( view.Parts().front().ptr );
			}

			const int	scaled_h = _ffmpeg->sws_scale( _video.swsCtx, _video.frame->data, _video.frame->linesize, src_slice_y, _video.frame->height,
														OUT dst_data, dst_stride );

			if_unlikely( scaled_h < 0 or scaled_h != int(_config.dstDim.y) )
			{
				FF_CHECK( scaled_h );
				return false;
			}
		}
		else
		// copy without scaling
		// memory must be aligned to 16 bytes
		{
			for (usize i = 0; i < image_planes.size(); ++i)
			{
				auto&		view = image_planes[i];
				ASSERT( view.RowPitch() <= _video.frame->linesize[i] );

				const auto	dst_pitch	= view.RowPitch();
				const auto	src_pitch	= Bytes{uint(_video.frame->linesize[i])};
				const auto	pitch		= Min( src_pitch, dst_pitch );
				Bytes		src_off;

				for (auto& part : view.Parts())
				{
					Bytes	dst_off;
					for_likely (; dst_off < part.size; )
					{
						MemCopy16( OUT part.ptr + dst_off, _video.frame->data[i] + src_off, pitch );
						dst_off += dst_pitch;
						src_off += src_pitch;
					}
					ASSERT( dst_off == part.size );
				}
			}
		}

		memViewArr = image_planes;
		return true;
	}

/*
=================================================
	_GetAudioSamples
=================================================
*/
	bool  FFmpegVideoDecoder::_GetAudioSamples (INOUT AudioSampleArr &inoutSamples) __NE___
	{
	  #ifdef AE_ENABLE_AUDIO

		//CHECK_ERR( _audio.codecCtx != null );

		// TODO
		return false;

	  #else
		Unused( memView );
		return false;
	  #endif
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

	bool  FFmpegVideoDecoder::_End () __NE___
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
	void  FFmpegVideoDecoder::_Destroy () __NE___
	{
		if ( _video.swsCtx != null )
			_ffmpeg->sws_freeContext( _video.swsCtx );

		if ( _video.frame != null )
			_ffmpeg->av_frame_free( &_video.frame );

		if ( _video.codecCtx != null )
			_ffmpeg->avcodec_free_context( &_video.codecCtx );

		if ( _audio.frame != null )
			_ffmpeg->av_frame_free( &_audio.frame );

		if ( _audio.codecCtx != null )
			_ffmpeg->avcodec_free_context( &_audio.codecCtx );

		if ( _avPacket != null )
			_ffmpeg->av_packet_free( &_avPacket );

		if ( _formatCtx != null )
			_ffmpeg->avformat_close_input( &_formatCtx );

		if ( _formatCtx != null )
			_ffmpeg->avformat_free_context( _formatCtx );

		if ( _ioCtx != null )
			_ffmpeg->avio_context_free( &_ioCtx );

		_avPacket			= null;
		_formatCtx			= null;
		_ioCtx				= null;

		_video.frame		= null;
		_video.codecCtx		= null;
		_video.codec		= null;
		_video.swsCtx		= null;

		_audio.frame		= null;
		_audio.codecCtx		= null;
		_audio.codec		= null;

		_decodingStarted	= false;
		_config				= Default;
		_rstream			= null;
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

		CodecConfig	cfg;
		cfg.hwAccelerated	= _config.hwAccelerated;
		cfg.targetCPU		= _config.targetCPU;
		cfg.targetGPU		= _config.targetGPU;

		return _ReadProperties( _formatCtx, cfg );
	}

/*
=================================================
	_IOReadPacket
=================================================
*/
	int  FFmpegVideoDecoder::_IOReadPacket (void* opaque, ubyte* buf, int buf_size) __NE___
	{
		auto*	stream = Cast<RStream>( opaque );

		if_unlikely( buf_size < 0 )
			return AVERROR_UNKNOWN;

		int	result = int(stream->ReadSeq( buf, Bytes{ulong(buf_size)} ));

		if_unlikely( result == 0 )
		{
			auto	pos_size = stream->PositionAndSize();
			result = (pos_size.pos >= pos_size.size) ? AVERROR_EOF : result;
		}

		return result;
	}

/*
=================================================
	_IOSeek
=================================================
*/
	slong  FFmpegVideoDecoder::_IOSeek (void* opaque, slong offset, int whence) __NE___
	{
		auto*	stream = Cast<RStream>( opaque );

		if_unlikely( offset < 0 )
			return AVERROR_UNKNOWN;

		switch ( whence & ~AVSEEK_FORCE )
		{
			case SEEK_SET :		return stream->SeekSet( Bytes{ulong(offset)} )					? slong(stream->Position()) : AVERROR_UNKNOWN;
			case SEEK_CUR :		return stream->SeekFwd( Bytes{ulong(offset)} )					? slong(stream->Position()) : AVERROR_UNKNOWN;
			case SEEK_END :		return stream->SeekSet( stream->Size() - Bytes{ulong(offset)} )	? slong(stream->Position()) : AVERROR_UNKNOWN;
			case AVSEEK_SIZE :	return slong(stream->Size());
		}
		return AVERROR_UNKNOWN;
	}

/*
=================================================
	_ReadProperties
=================================================
*/
	IVideoDecoder::Properties  FFmpegVideoDecoder::_ReadProperties (AVFormatContext* formatCtx, const CodecConfig &cfg) C_NE___
	{
		Properties	result;
		const auto	duration = Seconds_t{formatCtx->duration * av_q2d(AVRational{1, AV_TIME_BASE})};

		for (uint i = 0; i < formatCtx->nb_streams; ++i)
		{
			AVStream*			stream = formatCtx->streams[i];
			AVCodecParameters*	params = stream->codecpar;

			if ( AnyEqual( params->codec_id, AV_CODEC_ID_PROBE, AV_CODEC_ID_PNG, AV_CODEC_ID_MJPEG ))
				continue;

			const AVCodec*	codec = _ffmpeg->avcodec_find_decoder( params->codec_id );
			if ( codec == null )
				continue;

			if ( codec->type == AVMEDIA_TYPE_VIDEO )
			{
				// only one video stream is supported
				CHECK( not result.videoStream.IsValid() );

				auto&	dst			= result.videoStream;
				dst.codecName		= codec->name;
				dst.index			= stream->index;
				dst.type			= EnumCast( codec->type );
				dst.codec			= EnumCast( params->codec_id );
				dst.videoFormat		= EnumCast( AVPixelFormat( params->format ));
				dst.pixFormat		= VideoFormatToPixelFormat( dst.videoFormat, 3 );
				dst.colorPreset		= EnumCast( params->color_range, params->color_primaries, params->color_trc, params->color_space, params->chroma_location );
				dst.frameCount		= stream->nb_frames;
				dst.duration		= stream->duration > 0 ? Seconds_t{ stream->duration * av_q2d( stream->time_base )} : duration;
				dst.avgFrameRate	= ToFractional( stream->avg_frame_rate );
				dst.minFrameRate	= ToFractional( stream->r_frame_rate );
				dst.bitrate			= Bitrate{ulong( params->bit_rate > 0 ? params->bit_rate : formatCtx->bit_rate )};
				dst.dimension.x		= params->width;
				dst.dimension.y		= params->height;

				int		cxpos = 0, cypos = 0;
				_ffmpeg->av_chroma_location_enum_to_pos( OUT &cxpos, OUT &cypos, params->chroma_location );

				dst.xChromaOffset	= (cxpos >> 7) ? ESamplerChromaLocation::CositedEven : ESamplerChromaLocation::Midpoint;
				dst.yChromaOffset	= (cypos >> 7) ? ESamplerChromaLocation::CositedEven : ESamplerChromaLocation::Midpoint;
				dst.ycbcrRange		= (params->color_range == AVCOL_RANGE_MPEG ? ESamplerYcbcrRange::ITU_Narrow : ESamplerYcbcrRange::ITU_Full);

				if ( _video.codecCtx != null )
				{
					dst.videoFormat	= EnumCast( _video.codecCtx->pix_fmt );
					dst.pixFormat	= VideoFormatToPixelFormat( dst.videoFormat, 3 );
				}
				else
				{
					// create codec context to get actual pixel format
					const auto	UseCodec = [this, &dst, params] (const AVCodec* codec) -> bool
					{{
						if ( codec == null )
							return false;

						ASSERT( _ffmpeg->av_codec_is_decoder( codec ) != 0 );

						if ( not (codec->capabilities & AV_CODEC_CAP_HARDWARE) and _config.hwAccelerated == EHwAcceleration::Require )
							return false;

						if ( (codec->capabilities & AV_CODEC_CAP_HARDWARE) and _config.hwAccelerated == EHwAcceleration::Disable )
							return false;

						auto*	codec_ctx = _ffmpeg->avcodec_alloc_context3( codec );
						if ( codec_ctx == null )
							return false;

						if ( _ffmpeg->avcodec_parameters_to_context( codec_ctx, params ) == 0 )
						{
							if ( _ffmpeg->avcodec_open2( codec_ctx, codec, null ) == 0 )
							{
								dst.videoFormat	= EnumCast( codec_ctx->pix_fmt );
								dst.pixFormat	= VideoFormatToPixelFormat( dst.videoFormat, 3 );
							}
						}
						_ffmpeg->avcodec_free_context( &codec_ctx );
						return true;
					}};

					const auto	UseHwCodec = [this, &UseCodec] (const char* codecName) -> bool
					{{
						auto*	codec = _ffmpeg->avcodec_find_decoder_by_name( codecName );
						return UseCodec( codec );
					}};

					Unused( _ChooseVideoCodec( cfg, params->codec_id, UseCodec, UseHwCodec ));
				}
			}
			else
			if ( codec->type == AVMEDIA_TYPE_AUDIO )
			{
				// TODO
			}
		}

		return result;
	}

/*
=================================================
	GetFileProperties
=================================================
*/
	IVideoDecoder::Properties  FFmpegVideoDecoder::GetFileProperties (RC<RStream> stream, const CodecConfig &cfg) C_NE___
	{
		struct Context
		{
			FFmpegFnTableRef	ffmpeg;
			AVFormatContext*	formatCtx	= null;
			AVIOContext *		ioCtx		= null;

			Context (const FFmpegFnTable &fn) : ffmpeg{fn}
			{}

			~Context ()
			{
				if ( formatCtx != null )
					ffmpeg->avformat_close_input( &formatCtx );

				if ( formatCtx != null )
					ffmpeg->avformat_free_context( formatCtx );

				if ( ioCtx != null )
					ffmpeg->avio_context_free( &ioCtx );
			}
		};
		Context	ctx {_ffmpeg};

		CHECK_ERR( stream and stream->IsOpen() );

		ctx.formatCtx = _ffmpeg->avformat_alloc_context();
		CHECK_ERR( ctx.formatCtx != null );

		ctx.ioCtx = _ffmpeg->avio_alloc_context( null, 0, 0, stream.get(), &_IOReadPacket, null, &_IOSeek );
		CHECK_ERR( ctx.ioCtx != null );

		ctx.ioCtx->seekable	= AVIO_SEEKABLE_NORMAL;
		ctx.ioCtx->direct	= 1;

		ctx.formatCtx->pb		= ctx.ioCtx;
		ctx.formatCtx->flags	|= AVFMT_FLAG_CUSTOM_IO;

		FF_CHECK_ERR( _ffmpeg->avformat_open_input( &ctx.formatCtx, null, null, null ));
		FF_CHECK_ERR( _ffmpeg->avformat_find_stream_info( ctx.formatCtx, null ));

		return _ReadProperties( ctx.formatCtx, cfg );
	}

	IVideoDecoder::Properties  FFmpegVideoDecoder::GetFileProperties (const Path &filename, const CodecConfig &cfg) C_NE___
	{
		return GetFileProperties( MakeRC<FileRStream>( filename ), cfg );
	}

/*
=================================================
	PrintCodecs
=================================================
*/
	String  FFmpegVideoDecoder::PrintCodecs (const EVideoCodec type) C_Th___
	{
		const AVCodecID	codec_id	= EnumCast( type );
		String			str			= "codec id:  "s << ToString(type) << '\n';

		if ( codec_id == AV_CODEC_ID_NONE )
			return {};

		for (void* opaque = null;;)
		{
			auto*	codec = _ffmpeg->av_codec_iterate( &opaque );
			if ( codec == null )
				break;

			if ( codec->id != codec_id )
				continue;

			if ( _ffmpeg->av_codec_is_decoder( codec ) == 0 )
				continue;

			str << "  name:. . . . . " << codec->name << " (" << codec->long_name << ")\n";

			if ( codec->pix_fmts != null )
			{
				str << "  pix_formats:   { ";
				for (auto* pix_fmts = codec->pix_fmts;  *pix_fmts != -1; ++pix_fmts)
				{
					auto	fmt		= EnumCast( *pix_fmts );
					auto	name	= (fmt != Default ? ToString( fmt ) : PixFmtToString( *pix_fmts ));
					if ( name.empty() ) {
						AE_LOGI( "skip format: "s << ToString(*pix_fmts) );
						continue;
					}
					str << name << ", ";
				}
				str.pop_back();
				str.pop_back();
				str << " }\n";
			}

			str << "  frame threads: " << ToString((codec->capabilities & AV_CODEC_CAP_FRAME_THREADS) != 0) << '\n';
			str << "  slice threads: " << ToString((codec->capabilities & AV_CODEC_CAP_SLICE_THREADS) != 0) << '\n';
			str << "  delay:         " << ToString((codec->capabilities & AV_CODEC_CAP_DELAY) != 0) << '\n';

		  #if 0
			bool	supported = false;
			{
				auto	codec_ctx = _ffmpeg->avcodec_alloc_context3( _codec );
				if ( codec_ctx != null and codec->pix_fmts != null )
				{
					for (auto* pix_fmts = codec->pix_fmts;  *pix_fmts != -1; ++pix_fmts)
					{
						if ( EnumCast( *pix_fmts ) != Default ) {
							codec_ctx->pix_fmt = *pix_fmts;
							break;
						}
					}

					int err = _ffmpeg->avcodec_open2( codec_ctx, codec, null );
					FF_CHECK( err );

					if ( err == 0 )
					{
						supported = true;
						_ffmpeg->avcodec_free_context( &codec_ctx );
					}
				}
			}
			str << "  supported:     " << ToString( supported ) << '\n';
		  #endif

			if ( codec->capabilities & AV_CODEC_CAP_HARDWARE )
			{
				str << "  hw config:     {\n";
				for (int i = 0; i < 1000; ++i)
				{
					auto*	hw_cfg = _ffmpeg->avcodec_get_hw_config( codec, i );
					if ( hw_cfg == null )
						break;

					auto	fmt		= EnumCast( hw_cfg->pix_fmt );
					auto	name	= (fmt != Default ? ToString( fmt ) : PixFmtToString( hw_cfg->pix_fmt ));
					if ( name.empty() ) {
						AE_LOGI( "skip format: "s << ToString(hw_cfg->pix_fmt) );
						continue;
					}

					str << "    pix fmt:  " << name << '\n';

					if ( hw_cfg->methods & (AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX | AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX) )
					{
						str << "    device:   " << ToString( hw_cfg->device_type ) << '\n';
					}
					str << "    ----\n";
				}
				str << "  }\n";
			}

			str << "----------\n";
		}

		return str;
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
