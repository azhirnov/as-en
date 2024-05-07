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
	// variables
	private:
		mutable SharedMutex		_guard;

		FFmpegFnTable			_ffmpeg;

		AVOutputFormat const*	_format				= null;
		AVFormatContext *		_formatCtx			= null;

		AVStream *				_videoStream		= null;
		AVFrame *				_videoFrame			= null;

		AVCodec const*			_codec				= null;
		AVCodecContext *		_codecCtx			= null;

		SwsContext *			_swsCtx				= null;

		AVIOContext *			_ioCtx				= null;		// wrapper for file stream
		RC<WStream>				_tempStream;
		RC<WStream>				_dstStream;

		AVPacket *				_videoPacket		= null;

		slong					_frameCounter		= 0;

		String					_tempFile;
		String					_videoFile;

		bool					_remuxRequired		= false;
		bool					_encodingStarted	= false;
		bool					_hasBFrames			= false;

		Config					_config;


	// methods
	public:
		FFmpegVideoEncoder ()												__NE___;
		~FFmpegVideoEncoder ()												__NE_OV;

		// IVideoEncoder //
		bool  Begin (const Config &cfg, const Path &filename)				__NE_OV;
		bool  Begin (const Config &cfg, RC<WStream> temp, RC<WStream> dst)	__NE_OV;

		bool  AddFrame (const ImageMemView &view, Bool endOnError)			__NE_OV;
		bool  AddFrame (const ImageMemViewArr &view, Bool endOnError)		__NE_OV;

		bool  End ()														__NE_OV;

		bool		IsEncoding ()											C_NE_OV	{ SHAREDLOCK( _guard );  return _encodingStarted; }
		Config		GetConfig ()											C_NE_OV	{ SHAREDLOCK( _guard );  return _config; }
		StringView	GetFileExtension (EVideoCodec codec)					C_NE_OV;
		String		PrintCodecs (EVideoCodec codec)							C_Th_OV;


	private:
		ND_ bool  _CreateCodec ()											__NE___;
		ND_ bool  _CreateCodec2 ()											__NE___;
			void  _Destroy ()												__NE___;

			void  _ValidateResolution ()									__NE___;

		ND_ bool  _CreateStream (const AVCodec* codec, const char* videoFormat,
								 Bool remuxRequired, Bool hasBFrames)		__NE___;
			void  _DestroyStream ()											__NE___;

			void  _ValidatePixelFormat (OUT AVPixelFormat &)				C_NE___;
			void  _SetOptions (INOUT AVDictionary **dict)					C_NE___;

		ND_ bool  _Remux ()													__NE___;
		ND_ bool  _RemuxImpl (AVFormatContext* &ifmtCtx,
							  AVFormatContext* &ofmtCtx,
							  int* &streamMapping)							__NE___;
		ND_ bool  _Finish ()												__NE___;
		ND_ bool  _End ()													__NE___;

		template <typename ViewType>
		ND_ bool  _AddFrame2 (const ViewType &view, Bool endOnError)		__NE___;
		ND_ bool  _AddFrame3 (const ImageMemView &view)						__NE___;
		ND_ bool  _AddFrame3 (const ImageMemViewArr &view)					__NE___;
		ND_ bool  _AddFrame4 ()												__NE___;
		ND_	bool  _ReceivePackets (slong dur)								__NE___;

		ND_ ulong  _PTStoFrameIdx (slong pts)								C_NE___;
		ND_ slong  _FrameIdxToPTS (ulong frameIdx)							C_NE___;
		ND_ slong  _TimestampToPTS (Seconds time)							C_NE___;

		ND_ static Bitrate  _CalcBitrate (const Config &cfg)				__NE___;

		static int  _IOWritePacket (void* opaque, ubyte* buf, int buf_size)	__NE___;
	};


} // AE::Video

#endif // AE_ENABLE_FFMPEG
