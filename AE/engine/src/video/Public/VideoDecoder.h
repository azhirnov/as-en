// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Thread-safe:	yes
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
		using ImagePlanesMemView	= FixedArray< ImageMemView, 3 >;	// TODO 3 planes + alpha
		using ImageMemViewArr		= MutableArrayView< ImageMemView >;
		using AudioSampleArr		= MutableArrayView< AudioSampleView >;

		struct CodecConfig
		{
			EHwAcceleration		hwAccelerated	= Default;			// use hardware acceleration on GPU or CPU
			EGraphicsDeviceID	targetGPU		= Default;
			ECPUVendor			targetCPU		= Default;

			CodecConfig ()	__NE___ {}
		};

		struct Config
		{
			uint2				dstDim			= {};				// can be used for scaling
			EPixelFormat		dstFormat		= Default;
			EFilter				filter			= Default;
			EHwAcceleration		hwAccelerated	= Default;			// use hardware acceleration on GPU or CPU
			EGraphicsDeviceID	targetGPU		= Default;
			ECPUVendor			targetCPU		= Default;
			int					videoStreamIdx	= -1;				// use 'GetFileProperties()' to enum all streams
			int					audioStreamIdx	= -1;
			uint				threadCount		= 0;
			Bytes32u			ioBufferSize	= 64_KiB;			// for custom stream, it decrease number of Read() calls

			Config ()									__NE___	{}
			explicit Config (const CodecConfig &src)	__NE___ :
				hwAccelerated{ src.hwAccelerated },
				targetGPU{ src.targetGPU },
				targetCPU{ src.targetCPU }
			{}
		};


		struct StreamInfo
		{
			FixedString<32>			codecName;
			int						index			= -1;
			EMediaType				type			= Default;

			ND_ bool  IsValid ()	C_NE___	{ return index >= 0; }
		};

		struct VideoStreamInfo : StreamInfo
		{
			EVideoCodec				codec			= Default;
			EPixelFormat			pixFormat		= Default;
			EVideoFormat			videoFormat		= Default;
			EColorPreset			colorPreset		= Default;
			ulong					frameCount		= 0;		// may be undefined
			Seconds					duration;
			FrameRate				avgFrameRate;				// average
			FrameRate				minFrameRate;				// minimal
			Bitrate					bitrate;
			uint2					dimension		= {};
			ESamplerChromaLocation	xChromaOffset	= Default;
			ESamplerChromaLocation	yChromaOffset	= Default;
			ESamplerYcbcrRange		ycbcrRange		= Default;

			VideoStreamInfo ()	__NE___	{}
		};

		struct AudioStreamInfo : StreamInfo
		{
			// TODO
		};
		using AudioStreamInfos_t = FixedArray< AudioStreamInfo, 8 >;


		struct Properties
		{
			VideoStreamInfo		videoStream;
			AudioStreamInfos_t	audioStreams;

			ND_ String				ToString ()			C_Th___;
			ND_ StreamInfo const*	GetStream (int idx)	C_NE___;
		};


		struct FrameInfo
		{
			Seconds		timestamp;
			Seconds		duration;			// next frame will be presented in 'timestamp + duration'
			ulong		frameIdx	= 0;
		};


	// interface
	public:
		virtual ~IVideoDecoder ()															__NE___	{}

		ND_ virtual bool  Begin (const Config &cfg, const Path &filename)					__NE___	= 0;
		ND_ virtual bool  Begin (const Config &cfg, RC<RStream> stream)						__NE___	= 0;

		ND_ virtual bool  SeekTo (ulong frameIdx)											__NE___ = 0;
		ND_ virtual bool  SeekTo (Seconds timestamp)										__NE___ = 0;

		ND_ virtual EResult  GetVideoFrame (INOUT ImageMemViewArr&,
											OUT FrameInfo &)								__NE___ = 0;

		ND_ virtual EResult  GetAudioVideoFrame (INOUT ImageMemViewArr &,
												 INOUT AudioSampleArr &,
												 OUT FrameInfo &)							__NE___ = 0;

		ND_ virtual bool  End ()															__NE___	= 0;

		ND_ virtual Config		GetConfig ()												C_NE___ = 0;
		ND_ virtual Properties	GetProperties ()											C_NE___ = 0;


		ND_ EResult  GetVideoFrame (INOUT ImageMemView&	memView,
									OUT FrameInfo &		info)								__NE___;

		ND_ EResult  GetVideoFrame (INOUT ImagePlanesMemView&	memView,
									OUT FrameInfo &				info)						__NE___;

		ND_ EResult  GetAudioVideoFrame (INOUT ImageMemView &,
										 INOUT AudioSampleView &,
										 OUT FrameInfo &)									__NE___;

		ND_ EResult  GetAudioVideoFrame (INOUT ImagePlanesMemView &,
										 INOUT AudioSampleView &,
										 OUT FrameInfo &)									__NE___;


	// stateless //
		ND_ virtual Properties	GetFileProperties (const Path        &filename,
												   const CodecConfig &cfg		= Default)	C_NE___ = 0;
		ND_ virtual Properties	GetFileProperties (RC<RStream>        stream,
												   const CodecConfig &cfg		= Default)	C_NE___ = 0;

		ND_			String		PrintFileProperties (const Path        &filename,
													 const CodecConfig &cfg		= Default)	C_Th___;
		ND_			String		PrintFileProperties (RC<RStream>        stream,
													 const CodecConfig &cfg		= Default)	C_Th___;

		ND_ virtual String		PrintCodecs (EVideoCodec codec)								C_Th___ = 0;


	// helpers //

		// Allocate memory for ImageMemView.
		// Destructor is not needed, you can use 'allocator.Discard()' if supported.
		//
		ND_ static bool  AllocMemView (const Config				&cfg,
									   OUT ImageMemView			&memView,
									   IAllocator				&allocator,
									   Bytes					minAlign = 1_b)				__NE___	{ return AllocMemView( OUT memView, cfg.dstFormat, cfg.dstDim, allocator, minAlign ); }

		ND_ static bool  AllocMemView (const Config				&cfg,
									   OUT ImagePlanesMemView	&memView,
									   IAllocator				&allocator,
									   Bytes					minAlign = 1_b)				__NE___	{ return AllocMemView( OUT memView, cfg.dstFormat, cfg.dstDim, allocator, minAlign ); }

		ND_ static bool  AllocMemView (OUT ImageMemView			&memView,
									   EPixelFormat				format,
									   const uint2				&dim,
									   IAllocator				&allocator,
									   Bytes					minAlign = 1_b)				__NE___;

		ND_ static bool  AllocMemView (OUT ImagePlanesMemView	&memView,
									   EPixelFormat				format,
									   const uint2				&dim,
									   IAllocator				&allocator,
									   Bytes					minAlign = 1_b)				__NE___;

			static void  DeallocMemView (INOUT ImageMemView			&memView,
										 IAllocator					&allocator)				__NE___;

			static void  DeallocMemView (INOUT ImagePlanesMemView	&memView,
										 IAllocator					&allocator)				__NE___;

	};



	inline EResult  IVideoDecoder::GetVideoFrame (INOUT ImageMemView &memView, OUT FrameInfo &info) __NE___
	{
		ImageMemViewArr		arr {memView};
		return GetVideoFrame( INOUT arr, OUT info );
	}

	inline EResult  IVideoDecoder::GetVideoFrame (INOUT ImagePlanesMemView &memView, OUT FrameInfo &info) __NE___
	{
		ImageMemViewArr		arr {memView};
		return GetVideoFrame( INOUT arr, OUT info );
	}

	inline EResult  IVideoDecoder::GetAudioVideoFrame (INOUT ImageMemView &memView, INOUT AudioSampleView &sampleView, OUT FrameInfo &info) __NE___
	{
		ImageMemViewArr		img_arr		{memView};
		AudioSampleArr		samp_arr	{sampleView};
		return GetAudioVideoFrame( INOUT img_arr, INOUT samp_arr, OUT info );
	}

	inline EResult  IVideoDecoder::GetAudioVideoFrame (INOUT ImagePlanesMemView &memView, INOUT AudioSampleView &sampleView, OUT FrameInfo &info) __NE___
	{
		ImageMemViewArr		img_arr		{memView};
		AudioSampleArr		samp_arr	{sampleView};
		return GetAudioVideoFrame( INOUT img_arr, INOUT samp_arr, OUT info );
	}


} // AE::Video
