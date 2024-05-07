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
		mutable SharedMutex		_guard;

		FFmpegFnTable			_ffmpeg;

		AVFormatContext *		_formatCtx			= null;
		AVPacket *				_avPacket			= null;

		struct {
			AVFrame *				frame			= null;

			AVCodec const*			codec			= null;
			AVCodecContext *		codecCtx		= null;

			SwsContext *			swsCtx			= null;
		}						_video;

		struct {
			AVFrame *				frame			= null;

			AVCodec const*			codec			= null;
			AVCodecContext *		codecCtx		= null;
		}						_audio;

		AVIOContext *			_ioCtx				= null;		// wrapper for file stream
		RC<RStream>				_rstream;

		bool					_decodingStarted	= false;

		Config					_config;


	// methods
	public:
		FFmpegVideoDecoder ()												__NE___;
		~FFmpegVideoDecoder ()												__NE_OV;

		bool  Begin (const Config &cfg, const Path &filename)				__NE_OV;
		bool  Begin (const Config &cfg, RC<RStream> stream)					__NE_OV;
		bool  SeekTo (ulong frameIdx)										__NE_OV;
		bool  SeekTo (Seconds timestamp)									__NE_OV;

		bool  GetVideoFrame (INOUT ImageMemViewArr&	memView,
							 OUT FrameInfo &		info)					__NE_OV;

		bool  GetAudioVideoFrame (INOUT ImageMemViewArr &,
								  INOUT AudioSampleArr &,
								  OUT FrameInfo &)							__NE_OV;

		bool  End ()														__NE_OV;

		Properties	GetFileProperties (const Path &, const CodecConfig &)	C_NE_OV;
		Properties	GetFileProperties (RC<RStream>, const CodecConfig &)	C_NE_OV;
		String		PrintCodecs (EVideoCodec codec)							C_Th_OV;

		Config		GetConfig ()											C_NE_OV	{ SHAREDLOCK( _guard );  return _config; }
		Properties	GetProperties ()										C_NE_OV;


	private:
		ND_ bool  _Begin ()													__NE___;
		ND_ bool  _End ()													__NE___;
			void  _Destroy ()												__NE___;

		ND_ bool  _SeekTo (slong targetPTS)									__NE___;

		ND_ ulong  _PTStoFrameIdx (slong pts)								C_NE___;
		ND_ slong  _FrameIdxToPTS (ulong frameIdx)							C_NE___;
		ND_ slong  _TimestampToPTS (Seconds time)							C_NE___;

		ND_ bool  _CreateVideoCodec (const AVStream*,
									 OUT AVCodec const* &codec,
									 OUT AVCodecContext* &codecCtx)			C_NE___;

		ND_ bool  _CreateAudioCodec (const AVStream*,
									 OUT AVCodec const* &codec,
									 OUT AVCodecContext* &codecCtx)			C_NE___;

		ND_ Properties	_ReadProperties (AVFormatContext* formatCtx,
										 const CodecConfig &)				C_NE___;

		ND_ bool  _GetVideoFrame (INOUT ImageMemViewArr&	memView,
								  OUT FrameInfo &			info)			__NE___;
		ND_ bool  _GetAudioSamples (INOUT AudioSampleArr &)					__NE___;

		ND_ bool  _ScaleFrame1 (INOUT ImageMemView &	memView)			__NE___;
		ND_ bool  _ScaleFrame2 (INOUT ImageMemViewArr &	memView)			__NE___;

		template <typename ConfigType, typename CreateFn1, typename CreateFn2>
		ND_ bool  _ChooseVideoCodec (const ConfigType &cfg,
									 AVCodecID codecId,
									 CreateFn1 &&createCodec1,
									 CreateFn2 &&createCodec2)				C_NE___;

		static int		_IOReadPacket (void*, ubyte*, int)					__NE___;
		static slong	_IOSeek (void*, slong, int)							__NE___;
	};


} // AE::Video

#endif // AE_ENABLE_FFMPEG
