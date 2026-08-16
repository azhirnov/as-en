// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	based on code from:
	https://stackoverflow.com/questions/46444474/c-ffmpeg-create-mp4-file
	https://ffmpeg.org/doxygen/trunk/encode_video_8c-example.html
	https://ffmpeg.org/doxygen/trunk/remux_8c-example.html
*/

#ifdef AE_ENABLE_FFMPEG
# include "graphics_rhi/GraphicsImpl.h"
# include "video/FFmpeg/FFmpegVideoEncoder.h"
# include "video/FFmpeg/FFmpegUtils.cpp.h"

# ifdef AE_ENABLE_VULKAN
#	include "graphics_rhi/Vulkan/Video/VVideoUtils.cpp.h"
# endif

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

		ASSERT( _outputCtx == null	and
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

		if ( _RemuxRequired() ){
			AE_LOG_DBG( "Begin recording to temporary: '"s << _tempFile << "', resulting: '" << _videoFile << "'" );
		}else{
			AE_LOG_DBG( "Begin recording to: '"s << _videoFile << "'" );
		}
		return true;
	}

/*
=================================================
	Begin
=================================================
*/
	bool  FFmpegVideoEncoder::Begin (const Config &cfg, RC<WStream> dstStream) __NE___
	{
		EXLOCK( _guard );

		CHECK_ERR( _ffmpeg.IsLoaded() );
		CHECK_ERR( not _encodingStarted );
		CHECK_ERR( NoBits( cfg.flags, EEncoderFlags::Remux ));

		CHECK_ERR( dstStream  and dstStream->IsOpen() );

		if ( NoBits( cfg.flags, EEncoderFlags::Bitstream ))
			CHECK_ERR( AllBits( dstStream->GetSourceType(), IDataSource::ESourceType::RandomAccess ));	// require 'SeekSet()' and 'UpdateAt()'

		ASSERT( _outputCtx == null	and
				_videoFrame == null	and
				_videoStream == null );

		_dstStream			= RVRef(dstStream);
		_config				= cfg;
		_dstStreamBeginPos	= _dstStream->Position();

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
			_config.bitrate = CalcBitrate( _config );

		if ( _config.hwAccelerated == EHwAcceleration::Disable )
		{
			_config.targetCPU	= Default;
			_config.targetGPU	= Default;
		}

		if ( _config.hwAccelerated == EHwAcceleration::Require_Vulkan )
		{
		  #ifdef AE_ENABLE_VULKAN
			auto&	rts = GraphicsScheduler();
			if ( not AllBits( rts.GetDevice().GetAvailableQueues(), EQueueMask::VideoEncode ))
			{
				AE_LOGW( "Vulkan Encoder require 'VideoEncode' queue which is not enabled, reset 'hwAccelerated' to 'Require'." );
				_config.hwAccelerated = EHwAcceleration::Require;	// any HW acceleration
			}
		  #else
			AE_LOGW( "Vulkan Encoder is not supported, reset 'hwAccelerated' to 'Require'." );
			_config.hwAccelerated = EHwAcceleration::Require;
		  #endif
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
		const AVPixelFormat	pix_format		= EnumCast( _config.dstFormat );
		String				format_name;

		if ( AllBits( _config.flags, EEncoderFlags::Bitstream ))
		{
			switch_enum( _config.codec )
			{
				case EVideoCodec::MPEG4 :	format_name = "mpeg4";	break;
				case EVideoCodec::H264 :	format_name = "h264";	break;
				case EVideoCodec::H265 :	format_name = "hevc";	break;
				case EVideoCodec::H266 :	format_name = "vvc";	break;
				case EVideoCodec::VP8 :
				case EVideoCodec::VP9 :		format_name = "webm";	break;
				case EVideoCodec::WEBP :	format_name = "webp";	break;
				case EVideoCodec::AV1 :		format_name = "av1";	break;
				case EVideoCodec::Unknown :
				default :					RETURN_ERR( "codec is not supported" );
			}
			switch_end
		}
		else
		if ( not _videoFile.empty() )
			format_name = ToString( Path{_videoFile}.extension() ).substr( 1 );
		else
			format_name = GetFileExtension( _config.codec );

		CHECK_ERR( not format_name.empty() );

		const auto	CreateCodec = [&] (const char* codecName, bool hasBFrames = false)
		{{
			const AVCodec*	codec = _ffmpeg->avcodec_find_encoder_by_name( codecName );
			if ( codec == null )
				return false;

			ASSERT( _ffmpeg->av_codec_is_encoder( codec ) != 0 );

			if ( not (codec->capabilities & (AV_CODEC_CAP_HARDWARE | AV_CODEC_CAP_HYBRID)) and
				 _config.hwAccelerated == EHwAcceleration::Require )
				return false;

			if ( (codec->capabilities & AV_CODEC_CAP_HARDWARE)		and
				 _config.hwAccelerated == EHwAcceleration::Disable )
				return false;

			int						num_formats = 0;
			const AVPixelFormat*	pix_formats = null;
			_ffmpeg->avcodec_get_supported_config( null, codec, AV_CODEC_CONFIG_PIX_FORMAT, 0, OUT Cast<const void*>( &pix_formats ), OUT &num_formats );

			if ( pix_formats != null and num_formats > 0 )
			{
				if ( _config.hwAccelerated == EHwAcceleration::Require_Vulkan and
					 num_formats == 1 and pix_formats[0] == AV_PIX_FMT_VULKAN )
				{
					return _CreateStream( codec, format_name.c_str(), Bool{hasBFrames} );
				}

				for (int i = 0; i < num_formats; ++i)
				{
					if ( pix_format == CorrectPixFormat( pix_formats[i] ))
						return _CreateStream( codec, format_name.c_str(), Bool{hasBFrames} );
				}
				return false;
			}
			return _CreateStream( codec, format_name.c_str(), Bool{hasBFrames} );
		}};

		const bool	is_NV		= EGraphicsDeviceID_IsNVIDIA( _config.targetGPU );
		const bool	is_Intel	= _config.targetCPU == ECPUVendor::Intel or EGraphicsDeviceID_IsIntel( _config.targetGPU );
		const bool	is_AMD		= EGraphicsDeviceID_IsAMD( _config.targetGPU );

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
				if ( _config.hwAccelerated == EHwAcceleration::Require_Vulkan )
				{
					if ( CreateCodec( "h264_vulkan" ))
						return true;
				}

				if ( _config.hwAccelerated != EHwAcceleration::Disable )
				{
					if ( is_NV and CreateCodec( "h264_nvenc", (_config.targetGPU >= EGraphicsDeviceID::NV_Turing) ))	// dx11 compatible
						return true;

					if ( is_AMD and CreateCodec( "h264_amf" ))	// dx11 compatible
						return true;

					if ( is_Intel and CreateCodec( "h264_qsv" ))
						return true;

					// "h264_v4l2m2m"		// linux only	// TODO
					// "h264_vaapi"			// linux only
					// "h264_videotoolbox"	// MacOS only

				  #ifdef AE_PLATFORM_WINDOWS
					if ( CreateCodec( "h264_mf" ))
						return true;
				  #endif
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
				if ( _config.hwAccelerated == EHwAcceleration::Require_Vulkan )
				{
					if ( CreateCodec( "hevc_vulkan" ))
						return true;
				}

				if ( _config.hwAccelerated != EHwAcceleration::Disable )
				{
					if ( is_NV and CreateCodec( "hevc_nvenc", (_config.targetGPU >= EGraphicsDeviceID::NV_Turing) ))	// dx11 compatible
						return true;

					if ( is_AMD and CreateCodec( "hevc_amf" ))	// dx11 compatible
						return true;

					if ( is_Intel and CreateCodec( "hevc_qsv" ))
						return true;

				  #ifdef AE_PLATFORM_WINDOWS
					if ( CreateCodec( "hevc_mf" ))
						return true;
				  #endif
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
				if ( _config.hwAccelerated == EHwAcceleration::Require_Vulkan )
				{
					if ( CreateCodec( "av1_vulkan" ))
						return true;
				}

				if ( _config.hwAccelerated != EHwAcceleration::Disable )
				{
					if ( is_NV and CreateCodec( "av1_nvenc", (_config.targetGPU >= EGraphicsDeviceID::NV_Ampere) ))	// dx11 compatible
						return true;

					if ( is_AMD and CreateCodec( "av1_amf" ))	// dx11 compatible
						return true;

					if ( is_Intel and CreateCodec( "av1_qsv" ))
						return true;

				  #ifdef AE_PLATFORM_WINDOWS
					if ( CreateCodec( "av1_mf" ))
						return true;
				  #endif
				}
				break;
			}

			case EVideoCodec::Unknown :
				break;
		}
		switch_end

		auto	codec_id = EnumCast( _config.codec );
		if ( codec_id != AV_CODEC_ID_NONE )
		{
			auto*	codec = _ffmpeg->avcodec_find_encoder( codec_id );
			if ( codec != null and _CreateStream( codec, format_name.c_str(), False{"no b-frames"} ))
				return true;
		}

		AE_LOG_DBG( "Failed to find encoder for format: "s << ToString( _config.codec ));
		return false;
	}

/*
=================================================
	_CreateStream
=================================================
*/
	bool  FFmpegVideoEncoder::_CreateStream (const AVCodec* codec, const char* outputFormat, Bool hasBFrames) __NE___
	{
		_DestroyStream();

		_hasBFrames		= hasBFrames;
		_codec			= codec;
		_tempFile		= _RemuxRequired() ? (_videoFile + ".temp") : _videoFile;

		int						num_formats = 0;
		const AVPixelFormat*	pix_formats = null;
		_ffmpeg->avcodec_get_supported_config( null, codec, AV_CODEC_CONFIG_PIX_FORMAT, 0, OUT Cast<const void*>( &pix_formats ), OUT &num_formats );

		// create format context
		{
			_outputFormat = _ffmpeg->av_guess_format( outputFormat, null, null );
			CHECK_ERR( _outputFormat != null );

			FF_CHECK_ERR( _ffmpeg->avformat_alloc_output_context2( OUT &_outputCtx, _outputFormat, null, _tempFile.c_str() ));

			_outputCtx->max_interleave_delta	= INT64_MAX;
			_outputCtx->strict_std_compliance	= FF_COMPLIANCE_VERY_STRICT;

			_ffmpeg->av_dict_set( &_outputCtx->metadata, "encoding_tool", AE_ENGINE_NAME, 0 );
		}

		// create IO context
		if ( _dstStream )
		{
			// if stream creation fails it may write some data before error, so we need to restart stream
			if ( _dstStreamBeginPos != _dstStream->Position() )
				CHECK_ERR( _dstStream->UpdateAt( _dstStreamBeginPos ));

			const int	avio_buf_size = int(AlignUp( _config.ioBufferSize, _dstStream->DirectAccessAlign().offsetAlign ));

			auto*	buf = Cast<unsigned char>( _ffmpeg->av_malloc( avio_buf_size ));
			CHECK_ERR( buf != null );

			_ioCtx = _ffmpeg->avio_alloc_context( buf, avio_buf_size, 1, _dstStream.get(), null, &_IOWritePacket, &_IOSeek );

			if ( _ioCtx == null )
				_ffmpeg->av_free( buf );

			CHECK_ERR( _ioCtx != null );

			_outputCtx->pb		= _ioCtx;
			_outputCtx->flags	|= AVFMT_FLAG_CUSTOM_IO;
		}

		// create HW device
		{
			for (int i = 0;; ++i)
			{
				const AVCodecHWConfig*	config = _ffmpeg->avcodec_get_hw_config( _codec, i );
				if ( config == null )
					break;

			  #ifdef AE_ENABLE_VULKAN
				if ( config->device_type == AV_HWDEVICE_TYPE_VULKAN and
					 AnyBits( config->methods, AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX | AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX ))
				{
					if ( _CreateVulkanCtx( config ))
					{
						ASSERT( _codecHwCfg  != null and
								_hwDeviceCtx != null );
						break;
					}
					_DestroyVulkanCtx();
				}
			  #endif
			}
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

			if ( _codecHwCfg != null )
			{
				if ( AllBits( _codecHwCfg->methods, AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX ))
				{
					CHECK_ERR( _hwDeviceCtx != null );
					_codecCtx->hw_device_ctx	= _ffmpeg->av_buffer_ref( _hwDeviceCtx );
					_codecCtx->pix_fmt			= _codecHwCfg->pix_fmt;
				}
				else
				if ( AllBits( _codecHwCfg->methods, AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX ))
				{
					CHECK_ERR( _hwFrameCtx != null );
					_codecCtx->hw_frames_ctx	= _ffmpeg->av_buffer_ref( _hwFrameCtx );
					_codecCtx->pix_fmt			= _codecHwCfg->pix_fmt;
				}
			}

			_ValidatePixelFormat( INOUT _codecCtx->pix_fmt );

			CHECK( EnumCast( _config.colorPreset,
							 OUT _codecCtx->color_range,
							 OUT _codecCtx->color_primaries,
							 OUT _codecCtx->color_trc,
							 OUT _codecCtx->colorspace,
							 OUT _codecCtx->chroma_sample_location ));

			if ( _outputCtx->oformat->flags & AVFMT_GLOBALHEADER )
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

			if ( AnyBits( _config.flags, EEncoderFlags::LowLatency ))
			{
				_codecCtx->max_b_frames	= 0;
				_codecCtx->gop_size		= int(_config.framerate.ToFloat<float>());
				_codecCtx->flags		|= AV_CODEC_FLAG_LOW_DELAY;
			}

			AVDictionary*	codec_options	= null;
			_SetCodecOptions( INOUT &codec_options );

			int	err = _ffmpeg->avcodec_open2( _codecCtx, _codec, INOUT &codec_options );
			_ffmpeg->av_dict_free( &codec_options );
			FF_CHECK( err );

			if_unlikely( err != 0 )
			{
				AE_LOGI( "Try create codec without options" );
				err = _ffmpeg->avcodec_open2( _codecCtx, _codec, null );
				FF_CHECK_ERR( err );
			}
		}

		// create video stream
		{
			_videoStream = _ffmpeg->avformat_new_stream( _outputCtx, _codec );
			CHECK_ERR( _videoStream != null );

			_videoStream->time_base			= _codecCtx->time_base;
			_videoStream->avg_frame_rate	= av_inv_q( _codecCtx->time_base );
			_videoStream->start_time		= 0;
			_videoStream->id				= _outputCtx->nb_streams - 1;

			FF_CHECK( _ffmpeg->avcodec_parameters_from_context( INOUT _videoStream->codecpar, _codecCtx ));
		}

		// create output file
		{
			if ( _ioCtx == null and NoBits( _outputFormat->flags, AVFMT_NOFILE ))
				FF_CHECK_ERR( _ffmpeg->avio_open( OUT &_outputCtx->pb, _tempFile.c_str(), AVIO_FLAG_WRITE ));

			AVDictionary*	mux_options	= null;
			_SetMuxOptions( INOUT &mux_options );

			int err = _ffmpeg->avformat_write_header( _outputCtx, &mux_options );
			if ( not AnyEqual( err, AVSTREAM_INIT_IN_WRITE_HEADER, AVSTREAM_INIT_IN_INIT_OUTPUT ))
				FF_CHECK_ERR( err );

			if ( AllBits( _config.flags, EEncoderFlags::LowLatency ))
			{
				if ( _outputCtx->pb != null )
					_ffmpeg->avio_flush( _outputCtx->pb );
			}

			DEBUG_ONLY( _ffmpeg->av_dump_format( _outputCtx, 0, _tempFile.c_str(), 1 );)

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

		_PrintEncoderInfo();
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
	_SetCodecOptions
=================================================
*/
	void  FFmpegVideoEncoder::_SetCodecOptions (INOUT AVDictionary **dict) C_NE___
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
	_SetMuxOptions
=================================================
*/
	void  FFmpegVideoEncoder::_SetMuxOptions (INOUT AVDictionary **dict) C_NE___
	{
		if ( AllBits( _config.flags, EEncoderFlags::Fragmented ))
			FF_CHECK( _ffmpeg->av_dict_set( INOUT dict, "movflags", "frag_keyframe+empty_moov+default_base_moof", 0 ));
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
					_outputCtx != null	and
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
			const String	fmt_name	= _outputFormat->name;		// '_outputFormat' will be recreated

			if ( _CreateStream( codec, fmt_name.c_str(), Bool{_hasBFrames} ))
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
			POTVec2			dim_scale;
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
		{
			for (usize i = 0; i < memViewArr.size(); ++i)
			{
				auto&		view = memViewArr[i];
				ASSERT( view.RowPitch() <= _videoFrame->linesize[i] );

				const auto	src_pitch	= view.RowPitch();
				const auto	dst_pitch	= Bytes{uint(_videoFrame->linesize[i])};
				const auto	pitch		= Min( src_pitch, dst_pitch );
				Bytes		dst_off;

				for (auto& part : view.Parts())
				{
					Bytes	src_off;
					for_likely (; src_off < part.size; )
					{
						MemCopy( OUT _videoFrame->data[i] + dst_off, part.ptr + src_off, pitch );
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

			err = _ffmpeg->av_interleaved_write_frame( _outputCtx, _videoPacket );
			_ffmpeg->av_packet_unref( _videoPacket );

			FF_CHECK_ERR( err );

			if ( AllBits( _config.flags, EEncoderFlags::LowLatency ))
			{
				if ( _outputCtx->pb != null )
					_ffmpeg->avio_flush( _outputCtx->pb );
			}
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
		NonNull( _outputCtx );

		// flush codec
		FF_CHECK_ERR( _ffmpeg->avcodec_send_frame( _codecCtx, null ));
		CHECK_ERR( _ReceivePackets( 1 ));

		FF_CHECK( _ffmpeg->av_write_trailer( _outputCtx ));
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

		if ( _RemuxRequired() ){
			AE_LOG_DBG( "End recording to: '"s << _tempFile << "', start remuxing to: '" << _videoFile << "'" );
		}else{
			AE_LOG_DBG( "End recording to: '"s << _videoFile << "'" );
		}

		_DestroyStream();

		bool	res = true;

		if ( _RemuxRequired() )
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
				break;
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
		if ( (_outputCtx != null) and (_outputFormat != null) and _ioCtx == null and NoBits( _outputFormat->flags, AVFMT_NOFILE ))
			FF_CHECK( _ffmpeg->avio_closep( &_outputCtx->pb ));

		if ( _swsCtx != null )
			_ffmpeg->sws_freeContext( _swsCtx );

		if ( _videoFrame != null )
			_ffmpeg->av_frame_free( &_videoFrame );

		if ( _codecCtx != null )
			_ffmpeg->avcodec_free_context( &_codecCtx );

		if ( _ioCtx != null )
		{
			_ffmpeg->av_freep( &_ioCtx->buffer );
			_ffmpeg->avio_context_free( &_ioCtx );
		}

		// also delete '_videoStream'
		if ( _outputCtx != null )
			_ffmpeg->avformat_free_context( _outputCtx );

		_outputFormat	= null;
		_outputCtx		= null;
		_videoStream	= null;
		_videoFrame		= null;
		_codec			= null;
		_codecCtx		= null;
		_codecHwCfg		= null;
		_swsCtx			= null;
		_ioCtx			= null;

	  #ifdef AE_ENABLE_VULKAN
		_DestroyVulkanCtx();
	  #endif
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  FFmpegVideoEncoder::_Destroy () __NE___
	{
		_DestroyStream();

		// used in Remux too
		if ( _videoPacket != null )
			_ffmpeg->av_packet_free( &_videoPacket );

		_videoPacket		= null;

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

		if ( (ofmt_ctx != null) and NoBits( ofmt_ctx->oformat->flags, AVFMT_NOFILE ))
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
		}

		DEBUG_ONLY( _ffmpeg->av_dump_format( ofmtCtx, 0, _videoFile.c_str(), 1 );)

		if ( NoBits( ofmtCtx->oformat->flags, AVFMT_NOFILE ))
			FF_CHECK_ERR( _ffmpeg->avio_open( &ofmtCtx->pb, _videoFile.c_str(), AVIO_FLAG_WRITE ));

		FF_CHECK_ERR( _ffmpeg->avformat_write_header( ofmtCtx, null ));

		bool	result	= true;
		auto&	pkt		= *_videoPacket;
		slong	pts		= 0;
		slong	dts		= 0;

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

			if ( pkt.pts == AV_NOPTS_VALUE )
			{
				pkt.pts	 = pts;
				pkt.dts	 = dts;

				pkt.duration = _ffmpeg->av_rescale_q( pkt.duration, in_stream->time_base, out_stream->time_base );

				dts	= pts;
				pts += pkt.duration;
			}
			else
			{
				_ffmpeg->av_packet_rescale_ts( INOUT &pkt, in_stream->time_base, out_stream->time_base );
			}

			pkt.pos	 = -1;

			int		err = _ffmpeg->av_interleaved_write_frame( ofmtCtx, &pkt );
			_ffmpeg->av_packet_unref( &pkt );

			if_unlikely( err < 0 )
			{
				FF_CHECK( err );
				result = false;
				break;
			}
		}

		FF_CHECK( _ffmpeg->av_write_trailer( ofmtCtx ));
		return result;
	}

/*
=================================================
	_IOWritePacket
=================================================
*/
	int  FFmpegVideoEncoder::_IOWritePacket (void* opaque, const ubyte* buf, int buf_size) __NE___
	{
		auto*	stream = Cast<WStream>( opaque );

		if_unlikely( buf_size < 0 )
			return AVERROR_UNKNOWN;

		return int(stream->WriteSeq( buf, Bytes{ulong(buf_size)} ));
	}

/*
=================================================
	_IOSeek
----
	on success: return the new absolute byte position
	on failure: return a negative error code
=================================================
*/
	slong  FFmpegVideoEncoder::_IOSeek (void* opaque, slong offset, int whence) __NE___
	{
		auto*	stream		= Cast<WStream>( opaque );
		//bool	force		= AllBits( whence, AVSEEK_FORCE );
		Bytes	pos_off		= Bytes{ulong(Max( offset, 0 ))};

		whence &= ~AVSEEK_FORCE;

		switch ( whence )
		{
			case SEEK_SET :		// absolute pos
				if ( not stream->UpdateAt( pos_off ))
				{
					ASSERT( false );
					return AVERROR_UNKNOWN;
				}
				return slong{stream->Position()};

			case SEEK_CUR :		// move from current pos
				if ( not stream->SeekFwd( pos_off ))
				{
					ASSERT( false );
					return AVERROR_UNKNOWN;
				}
				return slong{stream->Position()};

			case SEEK_END :		// move from end
				if ( not stream->UpdateAt( stream->Position() - pos_off ))
				{
					ASSERT( false );
					return AVERROR_UNKNOWN;
				}
				return slong{stream->Position()};


			case AVSEEK_SIZE :	// return the file size without seeking anywhere
				return slong{stream->Position()};

			default :
				RETURN_ERR( "unsupported 'whence' argument", AVERROR_UNKNOWN );
		}
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

			int						num_formats = 0;
			const AVPixelFormat*	pix_formats = null;
			_ffmpeg->avcodec_get_supported_config( null, codec, AV_CODEC_CONFIG_PIX_FORMAT, 0, OUT Cast<const void*>( &pix_formats ), OUT &num_formats );

			if ( pix_formats != null and num_formats > 0 )
			{
				str << "  pix_formats:   { ";
				for (int i = 0; i < num_formats; ++i)
				{
					auto	fmt		= EnumCast( pix_formats[i] );
					auto	name	= (fmt != Default ? ToString( fmt ) : PixFmtToString( pix_formats[i] ));
					if ( name.empty() ) {
						AE_LOGI( "skip format: "s << ToString(pix_formats[i]) );
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
	_PrintEncoderInfo
=================================================
*/
	void  FFmpegVideoEncoder::_PrintEncoderInfo () C_NE___
	{
	#ifdef AE_ENABLE_LOGS
		String	str = "Created ffmpeg Video Encoder";
		str << "\n  codec:  " << _codec->name;
		str << "\n  desc:   " << _codec->long_name;

		AE_LOGI( str );
	#endif
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
//-----------------------------------------------------------------------------



# ifdef AE_ENABLE_VULKAN
/*
=================================================
	_CreateVulkanCtx
=================================================
*/
	bool  FFmpegVideoEncoder::_CreateVulkanCtx (AVCodecHWConfig const* config) __NE___
	{
		CHECK_ERR( _hwDeviceCtx == null and
				   _hwFrameCtx  == null and
				   _codecHwCfg  == null );

		CHECK_ERR( config != null );
		CHECK_ERR( AnyBits( config->methods, AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX | AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX ));
		CHECK_ERR( config->device_type == AV_HWDEVICE_TYPE_VULKAN );

		auto&	dev		= GraphicsScheduler().GetDevice();
		auto&	vk_ext	= dev.GetVExtensions();

		_hwDeviceCtx = _ffmpeg->av_hwdevice_ctx_alloc( AV_HWDEVICE_TYPE_VULKAN );
		CHECK_ERR( _hwDeviceCtx != null );

		auto*	hw_dev = Cast<AVHWDeviceContext>( _hwDeviceCtx->data );
		auto*	vk_ctx	= Cast<AVVulkanDeviceContext>( hw_dev->hwctx );

		hw_dev->user_opaque = this;

		vk_ctx->get_proc_addr	= Graphics::VulkanInstanceFn::_var_vkGetInstanceProcAddr;
		vk_ctx->inst			= dev.GetVkInstance();
		vk_ctx->phys_dev		= dev.GetVkPhysicalDevice();
		vk_ctx->act_dev			= dev.GetVkDevice();
		vk_ctx->device_features = dev.GetVProperties().features2;

		// 'enabled_inst_extensions' and 'enabled_dev_extensions' must be valid pointers until '_hwDeviceCtx' is alive

		Array<const char*>	ext_arr;
		{
			for (auto& ext : dev.GetInstanceExtensions()) {
				ext_arr.push_back( ext.c_str() );
			}

			auto**	ptr = _hwAlloc.Allocate<const char*>( ext_arr.size() );
			MemCopy( OUT ptr, ext_arr.data(), ext_arr.size() * Sizeof(ext_arr[0]) );

			vk_ctx->enabled_inst_extensions		= ptr;
			vk_ctx->nb_enabled_inst_extensions	= int(ext_arr.size());
		}{
			ext_arr.clear();
			for (auto& ext : dev.GetDeviceExtensions()) {
				ext_arr.push_back( ext.c_str() );
			}

			auto**	ptr = _hwAlloc.Allocate<const char*>( ext_arr.size() );
			MemCopy( OUT ptr, ext_arr.data(), ext_arr.size() * Sizeof(ext_arr[0]) );

			vk_ctx->enabled_dev_extensions		= ptr;
			vk_ctx->nb_enabled_dev_extensions	= int(ext_arr.size());
		}

		// init queues
		vk_ctx->nb_qf = 0;
		for (auto& src : dev.GetQueues())
		{
			if ( src.queueIndex != 0 )
				continue;

			VkVideoCodecOperationFlagsKHR	video_caps = 0;
			if ( src.type == EQueueType::VideoDecode )
			{
				if ( vk_ext.videoDecodeAV1 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_DECODE_AV1_BIT_KHR;
				if ( vk_ext.videoDecodeH264 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR;
				if ( vk_ext.videoDecodeH265 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_DECODE_H265_BIT_KHR;
				if ( vk_ext.videoDecodeVP9 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_DECODE_VP9_BIT_KHR;
			}

			if ( src.type == EQueueType::VideoEncode )
			{
				if ( vk_ext.videoDecodeAV1 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_ENCODE_AV1_BIT_KHR;
				if ( vk_ext.videoDecodeH264 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_ENCODE_H264_BIT_KHR;
				if ( vk_ext.videoDecodeH265 )	video_caps	|= VK_VIDEO_CODEC_OPERATION_ENCODE_H265_BIT_KHR;
			}

			auto&	dst		= vk_ctx->qf[ vk_ctx->nb_qf ];
			dst.idx			= int(src.familyIndex);
			dst.num			= 1;
			dst.flags		= src.familyFlags;
			dst.video_caps	= VkVideoCodecOperationFlagBitsKHR(video_caps);

			++vk_ctx->nb_qf;
		}
		CHECK_ERR( vk_ctx->nb_qf > 0 );

		vk_ctx->lock_queue		= _LockVkQueue;		// TODO
		vk_ctx->unlock_queue	= _UnlockVkQueue;

		int err = _ffmpeg->av_hwdevice_ctx_init( _hwDeviceCtx );
		if ( err < 0 )
		{
			FF_CHECK( err );
			_DestroyVulkanCtx();
			return false;
		}

		_codecHwCfg = config;

		if ( NoBits( config->methods, AV_CODEC_HW_CONFIG_METHOD_HW_FRAMES_CTX ))
			return true;


		VkVideoProfileListInfoKHR*	prof_list	= _hwAlloc.Allocate<VkVideoProfileListInfoKHR>();
		VkImageCreateInfo			image_ci	= {};
		{
			VideoProfileList	in_profiles;
			auto&				profile = in_profiles.emplace_back();

			profile.mode	= EVideoCodecMode::Encode;
			profile.codec	= _config.codec;
			CHECK_ERR( VideoFormatToChromaSubsamplingAndBitDepth( _config.dstFormat, OUT profile.chromaSubsampling, OUT profile.lumaBitDepth, OUT profile.chromaBitDepth ));

			// validate dimension
			{
				VkVideoProfileInfoKHR			profile_info;
				VkVideoCapabilitiesKHR			capabilities;
				InPlaceLinearAllocator<1024>	alloc;

				CHECK_ERR( GetProfileWithCapabilities( dev, profile, alloc, OUT profile_info, OUT capabilities ));

				CHECK_ERR( _config.srcDim.x >= capabilities.minCodedExtent.width );
				CHECK_ERR( _config.srcDim.y >= capabilities.minCodedExtent.height );
				CHECK_ERR( _config.srcDim.x <= capabilities.maxCodedExtent.width );
				CHECK_ERR( _config.srcDim.y <= capabilities.maxCodedExtent.height );
				CHECK_ERR( IsMultipleOf( _config.srcDim.x, capabilities.pictureAccessGranularity.width ));
				CHECK_ERR( IsMultipleOf( _config.srcDim.y, capabilities.pictureAccessGranularity.height ));
			}

			CHECK_ERR( ConvertProfiles( dev, in_profiles, _hwAlloc, OUT prof_list->pProfiles ));

			// validate image usage
			VkPhysicalDeviceVideoFormatInfoKHR				vinfo		= {};
			StaticArray< VkVideoFormatPropertiesKHR, 16 >	vformats	= {};
			uint											count		= uint(vformats.size());
			for (auto& vf : vformats) { vf.sType = VK_STRUCTURE_TYPE_VIDEO_FORMAT_PROPERTIES_KHR; }

			prof_list->sType		= VK_STRUCTURE_TYPE_VIDEO_PROFILE_LIST_INFO_KHR;
			prof_list->pNext		= null;
			prof_list->profileCount	= uint(in_profiles.size());

			vinfo.sType				= VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VIDEO_FORMAT_INFO_KHR;
			vinfo.pNext				= prof_list;
			vinfo.imageUsage		= VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;

			VK_CHECK_ERR( dev.vkGetPhysicalDeviceVideoFormatPropertiesKHR( dev.GetVkPhysicalDevice(), &vinfo, INOUT &count, OUT vformats.data() ));

			const auto	pix_fmts	= VideoFormatToPixelFormat( _config.dstFormat );
			const auto	main_fmt	= VEnumCast( pix_fmts.format );
			const auto	alt_fmt		= pix_fmts.altFormat != Default ? VEnumCast( pix_fmts.altFormat ) : VK_FORMAT_UNDEFINED;

			image_ci.tiling		= VK_IMAGE_TILING_OPTIMAL;
			image_ci.format		= VK_FORMAT_UNDEFINED;
			image_ci.flags		= VK_IMAGE_CREATE_MUTABLE_FORMAT_BIT | VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;
			image_ci.usage		= VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
								  VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR | VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;

			const auto	FindFormat = [count, vformats, &image_ci] (VkFormat reqFormat) -> bool
			{{
				for (uint i = 0; i < count; ++i)
				{
					auto&	vf = vformats[i];
					if ( vf.imageTiling	== image_ci.tiling and
						 vf.format		== reqFormat )
					{
						image_ci.format	= vf.format;
						image_ci.flags	&= vf.imageCreateFlags;
						image_ci.usage	&= vf.imageUsageFlags;
						return true;
					}
				}
				return false;
			}};

			if ( count == 1 )
			{
				auto&	vf		= vformats[0];
				image_ci.tiling	= vf.imageTiling;
				image_ci.format	= vf.format;
				image_ci.flags	&= vf.imageCreateFlags;
				image_ci.usage	&= vf.imageUsageFlags;
			}
			else
			{
				if ( not FindFormat( main_fmt ))
					FindFormat( alt_fmt );
			}

			CHECK_ERR( image_ci.usage != Zero );
		}

		// TODO: create image pool

		_hwFrameCtx = _ffmpeg->av_hwframe_ctx_alloc( _hwDeviceCtx );
		CHECK_ERR( _hwFrameCtx != null );

		auto*	frame_ctx		= Cast<AVHWFramesContext>( _hwFrameCtx->data );
		frame_ctx->format		= config->pix_fmt;
		frame_ctx->width		= _config.srcDim.x;
		frame_ctx->height		= _config.srcDim.y;
		frame_ctx->sw_format	= AV_PIX_FMT_NV12;	//EnumCast( _config.dstFormat );	// TODO
		frame_ctx->user_opaque	= this;

		auto*	vk_format		= Cast<AVVulkanFramesContext>( frame_ctx->hwctx );
		vk_format->tiling		= image_ci.tiling;
		vk_format->usage		= VkImageUsageFlagBits(image_ci.usage);
		vk_format->flags		= AV_VK_FRAME_FLAG_NONE;
		vk_format->img_flags	= image_ci.flags;
		vk_format->create_pnext	= prof_list;
		vk_format->format[0]	= image_ci.format;

		err = _ffmpeg->av_hwframe_ctx_init( _hwFrameCtx );
		if ( err < 0 )
		{
			FF_CHECK( err );
			_DestroyVulkanCtx();
			return false;
		}

		return true;
	}

/*
=================================================
	_DestroyVulkanCtx
=================================================
*/
	void  FFmpegVideoEncoder::_DestroyVulkanCtx () __NE___
	{
		if ( _hwDeviceCtx != null )
			_ffmpeg->av_buffer_unref( &_hwDeviceCtx );

		if ( _hwFrameCtx != null )
			_ffmpeg->av_buffer_unref( &_hwFrameCtx );

		_hwDeviceCtx	= null;
		_hwFrameCtx		= null;
		_codecHwCfg		= null;

		_hwAlloc.Discard();
	}

/*
=================================================
	_LockVkQueue
=================================================
*/
	void  FFmpegVideoEncoder::_LockVkQueue (AVHWDeviceContext *ctx, uint queueFamily, uint index) __NE___
	{
		Unused( ctx );
		auto&	dev = GraphicsScheduler().GetDevice();

		for (auto& q : dev.GetQueues())
		{
			if ( uint(q.familyIndex) == queueFamily and
				 q.queueIndex == index )
			{
				q.guard.lock();
				return;
			}
		}
		ASSERT_MSG( false, "failed to lock vulkan queue" );
	}

/*
=================================================
	_UnlockVkQueue
=================================================
*/
	void  FFmpegVideoEncoder::_UnlockVkQueue (AVHWDeviceContext *ctx, uint queueFamily, uint index) __NE___
	{
		Unused( ctx );
		auto&	dev = GraphicsScheduler().GetDevice();

		for (auto& q : dev.GetQueues())
		{
			if ( uint(q.familyIndex) == queueFamily and
				 q.queueIndex == index )
			{
				q.guard.unlock();
				return;
			}
		}
		ASSERT_MSG( false, "failed to unlock vulkan queue" );
	}

# endif // AE_ENABLE_VULKAN

} // AE::Video

#else

# include "video/Public/VideoEncoder.h"

namespace AE::Video
{
	RC<IVideoEncoder>  VideoFactory::CreateFFmpegEncoder () __NE___  { return Default; }
}

#endif // AE_ENABLE_FFMPEG
