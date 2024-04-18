// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Thread-safe:	yes
*/

#pragma once

#include "video/Public/Common.h"

namespace AE::Video
{
	class IVideoEncoder;
	class IVideoDecoder;


	//
	// Video Encoder/Decoder Factory
	//

	class VideoFactory : public Noninstanceable
	{
	public:
		ND_ static RC<IVideoEncoder>  CreateFFmpegEncoder ()		__NE___;
		ND_ static RC<IVideoEncoder>  CreateGraphicsHWEncoder ()	__NE___;

		ND_ static RC<IVideoDecoder>  CreateFFmpegDecoder ()		__NE___;
		ND_ static RC<IVideoDecoder>  CreateGraphicsHWDecoder ()	__NE___;
	};


} // AE::Video
