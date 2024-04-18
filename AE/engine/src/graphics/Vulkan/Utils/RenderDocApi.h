// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/NativeWindow.h"
#include "graphics/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// RenderDoc API
	//

	class RenderDocApi final
	{
		friend class VDevice;
		friend class VDeviceInitializer;

	// variables
	private:
		Library		_lib;
		void*		_api		= null;		// RENDERDOC_API_x_x_x

		void*		_device		= null;
		void*		_wndHandle	= null;


	// methods
	private:
		RenderDocApi ()											__NE___	{}

		ND_ bool  Initialize (VkInstance instance,
							  const NativeWindow &wndHandle)	__NE___;

	public:
		ND_ bool  IsInitialized ()								C_NE___	{ return _api != null; }

			void  PrintCaptures ()								C_NE___;

		// manual frame capture
		ND_ bool  BeginFrame ()									C_NE___;
			bool  CancelFrame ()								C_NE___;
			bool  EndFrame ()									C_NE___;
		ND_ bool  IsFrameCapturing ()							C_NE___;

		// next frame
			bool  TriggerFrameCapture ()						C_NE___;
			bool  TriggerMultiFrameCapture (uint count)			C_NE___;

		ND_ static const char*  GetVkLayer ()					__NE___	{ return "VK_LAYER_RENDERDOC_Capture"; }
	};


} // AE::Graphics
