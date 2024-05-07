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
/*
=================================================
	constructor
=================================================
*/
	FFmpegVideoEncoder::FFmpegVideoEncoder () __NE___
	{
		EXLOCK( _guard );
		ASSERT( _ffmpeg.IsLoaded() );
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
	}

/*
=================================================
	Begin
=================================================
*/
	bool  FFmpegVideoEncoder::Begin (const Config &cfg, const Path &filename) __NE___
	{
		EXLOCK( _guard );

		CHECK_ERR( _ffmpeg.IsLoaded() );
		CHECK_ERR( not _encodingStarted );

		ASSERT( _formatCtx == null	and
				_videoFrame == null	and
				_videoStream == null );

		_videoFile	= ToString( filename );
		_config		= cfg;

		if ( not _CreateCodec() )
		{
			_Destroy();
			return false;
		}

		_frameCounter		= 0;
		_encodingStarted	= true;

		AE_LOG_DBG( "Used codec: "s << _codec->long_name << " (" << _codec->name << ')' );
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

		CHECK_ERR( _ffmpeg.IsLoaded() );
		CHECK_ERR( not _encodingStarted );

		CHECK_ERR( tempStream and tempStream->IsOpen() );
		CHECK_ERR( dstStream  and dstStream->IsOpen()  );

		ASSERT( _formatCtx == null	and
				_videoFrame == null	and
				_videoStream == null );

		_tempStream	= RVRef(tempStream);
		_dstStream	= RVRef(dstStream);
		_config		= cfg;

		if ( not _CreateCodec() )
		{
			_Destroy();
			return false;
		}

		_frameCounter		= 0;
		_encodingStarted	= true;

		AE_LOG_DBG( "Used codec: "s << _codec->long_name << " (" << _codec->name << ')' );
		return true;
	}

/*
=================================================
	_ValidateResolution
=================================================
*/
	void  FFmpegVideoEncoder::_ValidateResolution () __NE___
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
	bool  FFmpegVideoEncoder::_CreateCodec () __NE___
	{
		bool	scaling = false;

		if ( All( IsZero( _config.dstDim )) )
			_config.dstDim = _config.srcDim;

		if ( _config.bitrate == Bitrate{0} )
			_config.bitrate = _CalcBitrate( _config );

		if ( _config.hwAccelerated == EHwAcceleration::Disable )
		{
			_config.targetCPU	= Default;
			_config.targetGPU	= Default;
		}

		for (;;)
		{
			if ( _CreateCodec2() )
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
----
	add list of hardware accelerated encoders
	https://trac.ffmpeg.org/wiki/HWAccelIntro
----
	https://developer.nvidia.com/video-encode-and-decode-gpu-support-matrix-new
=================================================
*/
	bool  FFmpegVideoEncoder::_CreateCodec2 () __NE___
	{
		const auto	format			= EnumCast( _config.dstFormat );
		const char*	format_name		= null;
		Bool		remux_required	= True{};

		switch_enum( _config.codec )
		{
			case EVideoCodec::MPEG4 :	format_name = "mpeg4";	break;
			case EVideoCodec::H264 :	format_name = "h264";	break;
			case EVideoCodec::H265 :	format_name = "hevc";	break;
			case EVideoCodec::H266 :	format_name = "vvc";	break;
			case EVideoCodec::VP8 :
			case EVideoCodec::VP9 :		format_name = "webm";	break;
			case EVideoCodec::WEBP :	format_name = "webp";	remux_required = false;	break;
			case EVideoCodec::AV1 :		format_name = "av1";	break;

			case EVideoCodec::Unknown :
			case EVideoCodec::_Count :
			default :					RETURN_ERR( "codec is not supported" );
		}
		switch_end

		const auto	CreateCodec = [&] (const char* codecName, bool hasBFrames = false)
		{{
			auto*	codec = _ffmpeg->avcodec_find_encoder_by_name( codecName );
			if ( codec == null )
				return false;

			ASSERT( _ffmpeg->av_codec_is_encoder( codec ));

			if ( not (codec->capabilities & AV_CODEC_CAP_HARDWARE) and _config.hwAccelerated == EHwAcceleration::Require )
				return false;

			if ( (codec->capabilities & AV_CODEC_CAP_HARDWARE) and _config.hwAccelerated == EHwAcceleration::Disable )
				return false;

			// check format
			if ( codec->pix_fmts != null )
			{
				for (auto* pix_fmts = codec->pix_fmts;  *pix_fmts != -1; ++pix_fmts)
				{
					if ( format == CorrectPixFormat( *pix_fmts ))
						return _CreateStream( codec, format_name, remux_required, Bool{hasBFrames} );
				}
				return false;
			}
			return _CreateStream( codec, format_name, remux_required, Bool{hasBFrames} );
		}};

		const bool	is_NV		= (_config.targetGPU >= EGraphicsDeviceID::_NV_Begin and _config.targetGPU <= EGraphicsDeviceID::_NV_End);
		const bool	is_Intel	= _config.targetCPU == ECPUVendor::Intel or
								  (_config.targetGPU >= EGraphicsDeviceID::_Intel_Begin and _config.targetGPU <= EGraphicsDeviceID::_Intel_End);
		const bool	is_AMD		= (_config.targetGPU >= EGraphicsDeviceID::_AMD_Begin and _config.targetGPU <= EGraphicsDeviceID::_AMD_End);

		switch_enum( _config.codec )
		{
			case EVideoCodec::MPEG4 :
			{
				if ( CreateCodec( "mpeg4" ))
					return true;
				break;
			}
			case EVideoCodec::H264 :
			{
				if ( _config.hwAccelerated != EHwAcceleration::Disable )
				{
					if ( is_NV and CreateCodec( "h264_nvenc", (_config.targetGPU >= EGraphicsDeviceID::NV_Turing) ))
						return true;

					if ( is_AMD and CreateCodec( "h264_amf" ))
						return true;

					if ( is_Intel and CreateCodec( "h264_qsv" ))
						return true;

					// "h264_v4l2m2m"		// linux only	// TODO
					// "h264_vaapi"			// linux only
					// "h264_videotoolbox"	// MacOS only

					if ( CreateCodec( "h264_mf" ))		// windows only?
						return true;
				}

				// sorted by priority
				{
					if ( CreateCodec( "libx264" ))
						return true;

					if ( CreateCodec( "libopenh264" ))
						return true;
				}
				break;
			}
			case EVideoCodec::H265 :
			{
				if ( _config.hwAccelerated != EHwAcceleration::Disable )
				{
					if ( is_NV and CreateCodec( "hevc_nvenc", (_config.targetGPU >= EGraphicsDeviceID::NV_Turing) ))
						return true;

					if ( is_AMD and CreateCodec( "hevc_amf" ))
						return true;

					if ( is_Intel and CreateCodec( "hevc_qsv" ))
						return true;

					if ( CreateCodec( "hevc_mf" ))	// windows only?
						return true;
				}

				// sorted by priority
				{
					if ( CreateCodec( "libx265" ))
						return true;

					if ( CreateCodec( "libkvazaar" ))
						return true;
				}
				break;
			}
			case EVideoCodec::H266 :
			{
				// not known codecs
				break;
			}
			case EVideoCodec::VP8 :
			{
				if ( CreateCodec( "libvpx" ))
					return true;

				break;
			}
			case EVideoCodec::VP9 :
			{
				if ( _config.hwAccelerated != EHwAcceleration::Disable )
				{
					if ( is_Intel and CreateCodec( "vp9_qsv" ))
						return true;
				}
				if ( CreateCodec( "libvpx-vp9" ))
					return true;

				break;
			}
			case EVideoCodec::WEBP :
			{
				// "libwebp_anim"
				// "libwebp"
				break;
			}
			case EVideoCodec::AV1 :
			{
				// "libdav1d"
				// "libaom-av1"
				// "libsvtav1"
				// "librav1e"
				break;
			}

			case EVideoCodec::Unknown :
			case EVideoCodec::_Count :
				break;
		}
		switch_end

		auto	codec_id = EnumCast( _config.codec );
		if ( codec_id != AV_CODEC_ID_NONE )
		{
			auto*	codec = _ffmpeg->avcodec_find_encoder( codec_id );
			if ( codec != null and _CreateStream( codec, format_name, remux_required, False{"no b-frames"} ))
				return true;
		}

		AE_LOG_DBG( "Failed to find encoder for format: "s << ToString( _config.codec ));
		return false;
	}

/*
=================================================
	_CalcBitrate
=================================================
*/
	Bitrate  FFmpegVideoEncoder::_CalcBitrate (const Config &) __NE___
	{
		// TODO
		return {};
	}

/*
=================================================
	_CreateStream
=================================================
*/
	bool  FFmpegVideoEncoder::_CreateStream (const AVCodec* codec, const char* videoFormat, Bool remuxRequired, Bool hasBFrames) __NE___
	{
		_DestroyStream();

		_remuxRequired	= remuxRequired;
		_hasBFrames		= hasBFrames;
		_codec			= codec;
		_tempFile		= _remuxRequired ? (_videoFile + ".temp") : _videoFile;

		// create format context
		{
			_format = _ffmpeg->av_guess_format( videoFormat, null, null );
			CHECK_ERR( _format != null );

			FF_CHECK_ERR( _ffmpeg->avformat_alloc_output_context2( OUT &_formatCtx, _format, null, _tempFile.c_str() ));

			_formatCtx->max_interleave_delta	= INT64_MAX;
			_formatCtx->strict_std_compliance	= FF_COMPLIANCE_VERY_STRICT;

			_ffmpeg->av_dict_set( &_formatCtx->metadata, "encoding_tool", AE_ENGINE_NAME, 0 );
		}

		// create IO context
		if ( _tempStream )
		{
			_ioCtx = _ffmpeg->avio_alloc_context( null, 0, 1, _tempStream.get(), null, &_IOWritePacket, null );
			CHECK_ERR( _ioCtx != null );

			_formatCtx->pb		= _ioCtx;
			_formatCtx->flags	|= AVFMT_FLAG_CUSTOM_IO;
		}

		// create codec context
		{
			_codecCtx = _ffmpeg->avcodec_alloc_context3( _codec );
			CHECK_ERR( _codecCtx != null );

			_codecCtx->strict_std_compliance = FF_COMPLIANCE_EXPERIMENTAL;
			_codecCtx->width		= int(_config.dstDim.x);
			_codecCtx->height		= int(_config.dstDim.y);
			_codecCtx->time_base	= ToAVRationalRec( _config.framerate );
			_codecCtx->framerate	= ToAVRational( _config.framerate );
			_codecCtx->pix_fmt		= EnumCast( _config.dstFormat );
			_codecCtx->bit_rate		= slong(_config.bitrate.GetNonScaled());

			_ValidatePixelFormat( INOUT _codecCtx->pix_fmt );

			CHECK( EnumCast( _config.colorPreset,
							 OUT _codecCtx->color_range,
							 OUT _codecCtx->color_primaries,
							 OUT _codecCtx->color_trc,
							 OUT _codecCtx->colorspace,
							 OUT _codecCtx->chroma_sample_location ));

			if ( _formatCtx->oformat->flags & AVFMT_GLOBALHEADER )
				_codecCtx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

			// override some parameters
			if ( AllBits( codec->capabilities, AV_CODEC_CAP_FRAME_THREADS ))
			{
				_codecCtx->thread_type	= FF_THREAD_FRAME;
				_codecCtx->thread_count	= Max( _codecCtx->thread_count, int(_config.threadCount) );
			}
			else
			if ( AllBits( codec->capabilities, AV_CODEC_CAP_SLICE_THREADS ))
			{
				_codecCtx->thread_type	= FF_THREAD_SLICE;
				_codecCtx->thread_count	= Max( _codecCtx->thread_count, int(_config.threadCount) );
			}

			AVDictionary*	codec_options	= null;
			_SetOptions( INOUT &codec_options );

			int	err = _ffmpeg->avcodec_open2( _codecCtx, _codec, INOUT &codec_options );
			_ffmpeg->av_dict_free( &codec_options );
			FF_CHECK( err );

			// try without options
			if_unlikely( err != 0 )
			{
				err = _ffmpeg->avcodec_open2( _codecCtx, _codec, null );
				FF_CHECK_ERR( err );
			}
		}

		// create video stream
		{
			_videoStream = _ffmpeg->avformat_new_stream( _formatCtx, _codec );
			CHECK_ERR( _videoStream != null );

			_videoStream->time_base			= _codecCtx->time_base;
			_videoStream->avg_frame_rate	= av_inv_q( _codecCtx->time_base );
			_videoStream->start_time		= 0;
			_videoStream->id				= _formatCtx->nb_streams - 1;

			FF_CHECK( _ffmpeg->avcodec_parameters_from_context( INOUT _videoStream->codecpar, _codecCtx ));
		}

		// create output file
		{
			if ( _ioCtx == null and not (_format->flags & AVFMT_NOFILE) )
				FF_CHECK_ERR( _ffmpeg->avio_open( OUT &_formatCtx->pb, _tempFile.c_str(), AVIO_FLAG_WRITE ));

			int err = _ffmpeg->avformat_write_header( _formatCtx, null );
			if ( not AnyEqual( err, AVSTREAM_INIT_IN_WRITE_HEADER, AVSTREAM_INIT_IN_INIT_OUTPUT ))
				FF_CHECK_ERR( err );

			DEBUG_ONLY( _ffmpeg->av_dump_format( _formatCtx, 0, _tempFile.c_str(), 1 );)

			_videoFrame = _ffmpeg->av_frame_alloc();
			CHECK_ERR( _videoFrame != null );

			_videoFrame->format				= _codecCtx->pix_fmt;
			_videoFrame->width				= int(_config.dstDim.x);
			_videoFrame->height				= int(_config.dstDim.y);
			_videoFrame->time_base			= _codecCtx->time_base;
			_videoFrame->color_range		= _codecCtx->color_range;
			_videoFrame->color_primaries	= _codecCtx->color_primaries;
			_videoFrame->color_trc			= _codecCtx->color_trc;
			_videoFrame->colorspace			= _codecCtx->colorspace;
			_videoFrame->chroma_location	= _codecCtx->chroma_sample_location;

			FF_CHECK_ERR( _ffmpeg->av_frame_get_buffer( INOUT _videoFrame, 0 ));
		}

		// create scaler
		{
			AVPixelFormat		src_fmt		= PixelFormatCast( _config.srcFormat );
			const AVPixelFormat	dst_fmt		= _codecCtx->pix_fmt;
			const int			filter		= EnumCast( _config.filter );

			if ( src_fmt == AV_PIX_FMT_NONE )
			{
				src_fmt				= dst_fmt;
				_config.srcFormat	= PixelFormatCast( src_fmt );
			}
			CHECK_ERR( src_fmt != AV_PIX_FMT_NONE );

			if ( src_fmt			 == dst_fmt			and
				 All( _config.srcDim == _config.dstDim ))
			{
				// don't create '_swsCtx'
			}
			else
			{
				_swsCtx = _ffmpeg->sws_getContext( int(_config.srcDim.x), int(_config.srcDim.y), src_fmt,
												 _codecCtx->width, _codecCtx->height, dst_fmt,
												 filter, null, null, null );
				CHECK_ERR( _swsCtx != null );
			}
		}

		_videoPacket = _ffmpeg->av_packet_alloc();
		CHECK_ERR( _videoPacket != null );

		return true;
	}

/*
=================================================
	_ValidatePixelFormat
=================================================
*/
	void  FFmpegVideoEncoder::_ValidatePixelFormat (OUT AVPixelFormat &outFormat) C_NE___
	{
		// check supported pixel format
		EnumSet<EVideoFormat>	supported_fmts;

		for (auto* fmt = _codec->pix_fmts; (fmt != null) and (*fmt != AV_PIX_FMT_NONE); ++fmt)
		{
			EVideoFormat	vf = EnumCast( *fmt );
			if ( vf != Default )
				supported_fmts.set( vf, true );
		}

		// choose another format
		EVideoFormat	required_fmt = _config.dstFormat;

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
	void  FFmpegVideoEncoder::_SetOptions (INOUT AVDictionary **dict) C_NE___
	{
		if ( _config.quality < 0.f )
			return;

		const uint			quality		= Clamp( uint(_config.quality * 9.f + 0.5f), 0u, 9u );
		FixedString<64>		preset;
		StringView			codec_name	{_codec->name};

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
				case 0 :	preset = "ultrafast";	break;
				case 1 :	preset = "superfast";	break;
				case 2 :	preset = "veryfast";	break;
				case 3 :	preset = "faster";		break;
				case 4 :	preset = "fast";		break;
				case 5 :	preset = "medium";		break;
				case 6 :	preset = "slow";		break;
				case 7 :	preset = "slower";		break;
				case 8 :	preset = "veryslow";	break;
				case 9 :	break;
			}
		}

		if ( preset.size() )
			FF_CHECK( _ffmpeg->av_dict_set( INOUT dict, "preset", preset.c_str(), 0 ));
	}

/*
=================================================
	AddFrame
=================================================
*/
	bool  FFmpegVideoEncoder::AddFrame (const ImageMemView &view, Bool endOnError) __NE___
	{
		return _AddFrame2( view, endOnError );
	}

	bool  FFmpegVideoEncoder::AddFrame (const ImageMemViewArr &memViewArr, Bool endOnError) __NE___
	{
		return _AddFrame2( memViewArr, endOnError );
	}

/*
=================================================
	_AddFrame2
=================================================
*/
	template <typename ViewType>
	bool  FFmpegVideoEncoder::_AddFrame2 (const ViewType &view, Bool endOnError) __NE___
	{
		EXLOCK( _guard );

		if_unlikely( not _encodingStarted )
			return false;

		CHECK_ERR(	_codecCtx != null	and
					_formatCtx != null	and
					_videoFrame != null );

		if_likely( _AddFrame3( view ))
		{
			++_frameCounter;
			return true;
		}

		// if failed to add first frame, try to recreate stream with aligned resolution
		if_unlikely( _frameCounter == 0 )
		{
			AE_LOG_DBG( "Failed to encode frame, try to change resolution" );
			_ValidateResolution();

			const AVCodec*	codec		= _codec;
			String			fmt_name	= _format->name;

			if ( _CreateStream( codec, fmt_name.c_str(), Bool{_remuxRequired}, Bool{_hasBFrames} ))
			{
				if ( _AddFrame3( view ))
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
	_AddFrame3
=================================================
*/
	inline bool  FFmpegVideoEncoder::_AddFrame3 (const ImageMemView &view) __NE___
	{
		CHECK_ERR( view.Parts().size() == 1 );
		CHECK_ERR( int(view.Dimension().x) == _codecCtx->width );
		CHECK_ERR( int(view.Dimension().y) == _codecCtx->height );
		CHECK_ERR( view.Dimension().z == 1 );
		CHECK_ERR( view.Format() == _config.srcFormat );
		CHECK_ERR( _swsCtx != null );

		FF_CHECK_ERR( _ffmpeg->av_frame_make_writable( _videoFrame ));

		const int		src_stride  [AV_NUM_DATA_POINTERS]	= { int(view.RowPitch()) };
		const ubyte*	src_slice   [AV_NUM_DATA_POINTERS]	= { Cast<ubyte>( view.Parts().front().ptr )};
		const int		src_slice_y							= 0;

		const int	scaled_h = _ffmpeg->sws_scale( _swsCtx, src_slice, src_stride, src_slice_y, int(_config.srcDim.y),
												   OUT _videoFrame->data, _videoFrame->linesize );

		if_unlikely( scaled_h < 0 or scaled_h != _codecCtx->height )
		{
			FF_CHECK( scaled_h );
			return false;
		}

		return _AddFrame4();
	}

/*
=================================================
	_AddFrame3
=================================================
*/
	inline bool  FFmpegVideoEncoder::_AddFrame3 (const ImageMemViewArr &memViewArr) __NE___
	{
		ASSERT( memViewArr.size() == 3 );
		ASSERT( EPixelFormat_IsYcbcr( _config.srcFormat ));

		// validate
	  #ifdef AE_DEBUG
		for (usize i = 0; i < memViewArr.size(); ++i)
		{
			EPixelFormat	plane_fmt;
			uint2			dim_scale;
			CHECK( EPixelFormat_GetPlaneInfo( _config.srcFormat, EImageAspect_Plane(i), OUT plane_fmt, OUT dim_scale ));

			auto&	view = memViewArr[i];
			CHECK( view.Format() == plane_fmt );
			CHECK( All( IsMultipleOf( _config.dstDim, dim_scale )));
			CHECK( All( uint2{view.Dimension()} == (_config.dstDim / dim_scale) ));
		}
	  #endif

		FF_CHECK_ERR( _ffmpeg->av_frame_make_writable( _videoFrame ));

		// scale frame
		if ( _swsCtx != null )
		{
			const int		src_slice_y							= 0;
			int				src_stride [AV_NUM_DATA_POINTERS]	= {};
			const ubyte*	src_slice  [AV_NUM_DATA_POINTERS]	= {};

			for (usize i = 0; i < memViewArr.size(); ++i)
			{
				auto&	view = memViewArr[i];
				ASSERT( view.Parts().size() == 1 );	// TODO: error?
				ASSERT( view.Format() == _config.srcFormat );

				src_stride[i]	= int(view.RowPitch());
				src_slice[i]	= Cast<ubyte>( view.Parts().front().ptr );
			}

			const int	scaled_h = _ffmpeg->sws_scale( _swsCtx, src_slice, src_stride, src_slice_y, int(_config.srcDim.y),
													   OUT _videoFrame->data, _videoFrame->linesize );

			if_unlikely( scaled_h < 0 or scaled_h != _codecCtx->height )
			{
				FF_CHECK( scaled_h );
				return false;
			}
		}
		else
		// copy without scaling
		// memory must be aligned to 16 bytes
		{
			for (usize i = 0; i < memViewArr.size(); ++i)
			{
				auto&		view = memViewArr[i];
				ASSERT( view.RowPitch() <= _videoFrame->linesize[i] );
				ASSERT( view.Format() == _config.srcFormat );

				const auto	src_pitch	= view.RowPitch();
				const auto	dst_pitch	= Bytes{uint(_videoFrame->linesize[i])};
				const auto	pitch		= Min( src_pitch, dst_pitch );
				Bytes		dst_off;

				for (auto& part : view.Parts())
				{
					Bytes	src_off;
					for_likely (; src_off < part.size; )
					{
						MemCopy16( OUT _videoFrame->data[i] + dst_off, part.ptr + src_off, pitch );
						src_off += src_pitch;
						dst_off += dst_pitch;
					}
					ASSERT( src_off == part.size );
				}
			}
		}

		return _AddFrame4();
	}

/*
=================================================
	_AddFrame4
=================================================
*/
	inline bool  FFmpegVideoEncoder::_AddFrame4 () __NE___
	{
		const slong		dur	= 1;

		_videoFrame->pts = _frameCounter;

		int err = _ffmpeg->avcodec_send_frame( _codecCtx, _videoFrame );
		if_unlikely( err < 0 )
		{
			if ( err == AVERROR(EAGAIN) )
			{
				CHECK( _ReceivePackets( dur ));
				FF_CHECK_ERR( _ffmpeg->avcodec_send_frame( _codecCtx, _videoFrame ));
			}
			else
				FF_CHECK_ERR( err );
		}

		return _ReceivePackets( dur );
	}

/*
=================================================
	_ReceivePackets
=================================================
*/
	inline bool  FFmpegVideoEncoder::_ReceivePackets (slong dur) __NE___
	{
		for (int err = 0; err >= 0;)
		{
			err = _ffmpeg->avcodec_receive_packet( _codecCtx, OUT _videoPacket );

			if ( err == AVERROR(EAGAIN) or err == AVERROR_EOF )
				return true;

			FF_CHECK_ERR( err );

			_videoPacket->duration		= dur;
			_videoPacket->stream_index	= _videoStream->index;

			_ffmpeg->av_packet_rescale_ts( INOUT _videoPacket, _codecCtx->time_base, _videoStream->time_base );

			err = _ffmpeg->av_interleaved_write_frame( _formatCtx, _videoPacket );
			_ffmpeg->av_packet_unref( _videoPacket );

			FF_CHECK_ERR( err );
		}
		return true;
	}

/*
=================================================
	_Finish
=================================================
*/
	bool  FFmpegVideoEncoder::_Finish () __NE___
	{
		NonNull( _codecCtx );
		NonNull( _formatCtx );

		// flush codec
		FF_CHECK_ERR( _ffmpeg->avcodec_send_frame( _codecCtx, null ));
		CHECK_ERR( _ReceivePackets( 1 ));

		FF_CHECK( _ffmpeg->av_write_trailer( _formatCtx ));
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

	bool  FFmpegVideoEncoder::_End () __NE___
	{
		if_unlikely( not _encodingStarted )
			return false;

		CHECK( _Finish() );

		AE_LOG_DBG( "End recording to: '"s << _tempFile << "', start remuxing to: '" << _videoFile << "'" );

		_DestroyStream();

		bool	res = true;

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
			case EVideoCodec::H266 :
			case EVideoCodec::AV1 :
			case EVideoCodec::MPEG4 :
				return "mp4";	// TODO: .mkv ?

			case EVideoCodec::WEBP :
				return "webp";

			case EVideoCodec::VP8 :
			case EVideoCodec::VP9 :
				return "webm";

			case EVideoCodec::Unknown :
			case EVideoCodec::_Count :	break;
		}
		switch_end
		RETURN_ERR( "unknown codec" );
	}

/*
=================================================
	_DestroyStream
=================================================
*/
	void  FFmpegVideoEncoder::_DestroyStream () __NE___
	{
		if ( (_formatCtx != null) and (_format != null) and not (_format->flags & AVFMT_NOFILE) )
			FF_CHECK( _ffmpeg->avio_closep( &_formatCtx->pb ));

		if ( _formatCtx != null )
			_ffmpeg->avformat_free_context( _formatCtx );

		if ( _codecCtx != null )
			_ffmpeg->avcodec_free_context( &_codecCtx );

		if ( _videoFrame != null )
			_ffmpeg->av_frame_free( &_videoFrame );

		if ( _swsCtx != null )
			_ffmpeg->sws_freeContext( _swsCtx );

		if ( _ioCtx != null )
			_ffmpeg->avio_context_free( &_ioCtx );

		_format			= null;
		_formatCtx		= null;
		_videoStream	= null;
		_videoFrame		= null;
		_codec			= null;
		_codecCtx		= null;
		_swsCtx			= null;
		_ioCtx			= null;
		_tempStream		= null;
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  FFmpegVideoEncoder::_Destroy () __NE___
	{
		_DestroyStream();

		if ( _videoPacket != null )
			_ffmpeg->av_packet_free( &_videoPacket );

		_videoPacket		= null;

		_remuxRequired		= false;
		_encodingStarted	= false;
		_hasBFrames			= false;

		_frameCounter		= 0;
		_config				= Default;

		_tempFile.clear();
		_videoFile.clear();

		_dstStream			= null;
	}

/*
=================================================
	_Remux
=================================================
*/
	bool  FFmpegVideoEncoder::_Remux () __NE___
	{
		CHECK_ERR( not _tempFile.empty() and not _videoFile.empty() );

		AVFormatContext*	ifmt_ctx		= null;
		AVFormatContext*	ofmt_ctx		= null;
		int *				stream_mapping	= null;
		bool				remuxed			= _RemuxImpl( ifmt_ctx, ofmt_ctx, stream_mapping );

		if ( ifmt_ctx != null )
			_ffmpeg->avformat_close_input( &ifmt_ctx );

		if ( (ofmt_ctx != null) and not (ofmt_ctx->oformat->flags & AVFMT_NOFILE) )
			_ffmpeg->avio_closep( &ofmt_ctx->pb );

		if ( ofmt_ctx != null )
			_ffmpeg->avformat_free_context( ofmt_ctx );

		_ffmpeg->av_free( stream_mapping );

		if ( remuxed )
		{
			CHECK( FileSystem::DeleteFile( _tempFile ));
		}
		else
		{
			// keep temporary file if remux failed
			FileSystem::DeleteFile( _videoFile );
			CHECK( FileSystem::Rename( _tempFile, _videoFile ));
		}
		AE_LOG_DBG( "End remuxing: '"s << _videoFile << "'" );

		return remuxed;
	}

/*
=================================================
	_RemuxImpl
=================================================
*/
	bool  FFmpegVideoEncoder::_RemuxImpl (AVFormatContext* &ifmtCtx, AVFormatContext* &ofmtCtx, int* &streamMapping) __NE___
	{
		CHECK_ERR( _videoPacket != null );

		FF_CHECK_ERR( _ffmpeg->avformat_open_input( OUT &ifmtCtx, _tempFile.c_str(), 0, 0 ));
		FF_CHECK_ERR( _ffmpeg->avformat_find_stream_info( ifmtCtx, 0 ));
		DEBUG_ONLY( _ffmpeg->av_dump_format( ifmtCtx, 0, _tempFile.c_str(), 0 );)

		FF_CHECK_ERR( _ffmpeg->avformat_alloc_output_context2( OUT &ofmtCtx, null, null, _videoFile.c_str() ));

		int stream_mapping_size	= ifmtCtx->nb_streams;
		streamMapping			= Cast<int>( _ffmpeg->av_calloc( stream_mapping_size, sizeof(*streamMapping) ));
		CHECK_ERR( streamMapping != null );

		int stream_index = 0;
		for (uint i = 0; i < ifmtCtx->nb_streams; ++i)
		{
			AVStream *			out_stream	= null;
			AVStream *			in_stream	= ifmtCtx->streams[i];
			AVCodecParameters*	in_codecpar	= in_stream->codecpar;

			if ( in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO   and
				 in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO   and
				 in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE )
			{
				streamMapping[i] = -1;
				continue;
			}

			streamMapping[i] = stream_index++;

			out_stream = _ffmpeg->avformat_new_stream( ofmtCtx, null );
			CHECK_ERR( out_stream != null );

			FF_CHECK_ERR( _ffmpeg->avcodec_parameters_copy( INOUT out_stream->codecpar, in_codecpar ));

			out_stream->codecpar->codec_tag = 0;
			out_stream->time_base		= ToAVRationalRec( _config.framerate );
			out_stream->avg_frame_rate	= av_inv_q( out_stream->time_base );
			out_stream->nb_frames		= _frameCounter;
		//	out_stream->duration		= _frameCounter;
		}

		DEBUG_ONLY( _ffmpeg->av_dump_format( ofmtCtx, 0, _videoFile.c_str(), 1 );)

		if ( (ofmtCtx->oformat->flags & AVFMT_NOFILE) == 0 )
			FF_CHECK_ERR( _ffmpeg->avio_open( &ofmtCtx->pb, _videoFile.c_str(), AVIO_FLAG_WRITE ));

		FF_CHECK_ERR( _ffmpeg->avformat_write_header( ofmtCtx, null ));

		auto&	pkt = *_videoPacket;
		slong	pts	= 0;
		slong	dts	= 0;

		for (;;)
		{
			AVStream*	in_stream	= null;
			AVStream*	out_stream	= null;

			if ( _ffmpeg->av_read_frame( ifmtCtx, OUT &pkt ) < 0 )
				break;

			if ( pkt.stream_index >= stream_mapping_size or
				 streamMapping[ pkt.stream_index ] < 0 )
			{
				_ffmpeg->av_packet_unref( &pkt );
				continue;
			}

			in_stream			= ifmtCtx->streams[ pkt.stream_index ];
			pkt.stream_index	= streamMapping[ pkt.stream_index ];
			out_stream			= ofmtCtx->streams[ pkt.stream_index ];

			pkt.duration = _ffmpeg->av_rescale_q( pkt.duration, in_stream->time_base, out_stream->time_base );

			pkt.pts	 = pts;
			pkt.dts	 = dts;
			pkt.pos	 = -1;

			dts	= pts;
			pts	+= pkt.duration;

			int		err = _ffmpeg->av_interleaved_write_frame( ofmtCtx, &pkt );
			_ffmpeg->av_packet_unref( &pkt );

			if_unlikely( err < 0 )
			{
				FF_CHECK( err );
				break;
			}
		}

		FF_CHECK( _ffmpeg->av_write_trailer( ofmtCtx ));
		return true;
	}

/*
=================================================
	_IOWritePacket
=================================================
*/
	int  FFmpegVideoEncoder::_IOWritePacket (void* opaque, ubyte* buf, int buf_size) __NE___
	{
		auto*	stream = Cast<WStream>( opaque );

		if_unlikely( buf_size < 0 )
			return AVERROR_UNKNOWN;

		return int(stream->WriteSeq( buf, Bytes{ulong(buf_size)} ));
	}

/*
=================================================
	PrintCodecs
=================================================
*/
	String  FFmpegVideoEncoder::PrintCodecs (const EVideoCodec type) C_Th___
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

			if ( _ffmpeg->av_codec_is_encoder( codec ) == 0 )
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
					codec_ctx->pix_fmt = *codec->pix_fmts;

					if ( _ffmpeg->avcodec_open2( codec_ctx, codec, null ) == 0 )
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
