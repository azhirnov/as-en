// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "video/Public/VideoEncoder.h"

namespace AE::Video
{
	using Graphics::CommandBatchPtr;


	//
	// Video Output Surface
	//

	class VideoOutputSurface : public IOutputSurface
	{
	// types
	private:
		class ReadbackImageTask;


	// variables
	private:
		Unique<IVideoEncoder>	_encoder;


	// methods
	public:
		VideoOutputSurface ()																						__NE___;
		~VideoOutputSurface ()																						__NE_OV;

		bool  Begin (const IVideoEncoder::Config &cfg, const Path &filename)										__NE___;
		bool  End ()																								__NE___;

	  // IOutputSurface //
		bool			IsInitialized ()																			C_NE_OV;
		RenderPassInfo	GetRenderPassInfo ()																		C_NE_OV;

		AsyncTask	Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)	__NE_OV;
		bool		GetTargets (OUT RenderTargets_t &targets)														C_NE_OV;
		AsyncTask	End (ArrayView<AsyncTask> deps)																	__NE_OV;
	};


} // AE::Video
