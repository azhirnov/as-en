// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "video/Public/VideoDecoder.h"
# include "graphics/Public/GraphicsImpl.h"

namespace AE::Video
{
	using namespace AE::Graphics;


	//
	// Graphics HW Video Decoder
	//

	class GfxVideoDecoder final : public IVideoDecoder
	{
	// variables
	private:
		Strong<VideoSessionID>		_sessionId;


	// methods
	public:
		GfxVideoDecoder ()											__NE___;
		~GfxVideoDecoder ()											__NE_OV;
		
		bool  Begin (const Config &cfg, const Path &filename)		__NE_OV;
		bool  Begin (const Config &cfg, RC<RStream> stream)			__NE_OV;
		bool  SeekTo (ulong frameIdx)								__NE_OV;
		bool  SeekTo (Second_t timestamp)							__NE_OV;

		bool  GetFrame (OUT ImageMemView &view, OUT FrameInfo &info)__NE_OV;
		bool  GetFrame (OUT VideoImageID &id, OUT FrameInfo &info)	__NE_OV;

		bool  End ()												__NE_OV;
		
		Config		GetConfig ()									C_NE_OV;
		Properties	GetProperties ()								C_NE_OV;
		
		
		// stateless
		Properties	GetFileProperties (const Path &filename)		C_NE_OV;
		Properties	GetFileProperties (RC<RStream> stream)			C_NE_OV;
		String		PrintFileProperties (const Path &filename)		C_NE_OV;
		String		PrintFileProperties (RC<RStream> stream)		C_NE_OV;
	};


} // AE::Video

#endif // AE_ENABLE_VULKAN
