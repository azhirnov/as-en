// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/NativeWindow.h"
#include "graphics_rhi/Vulkan/VCommon.h"

namespace AE::Graphics
{

	//
	// RenderDoc API
	//

	class RenderDocApi final
	{
		friend class VDevice;
		friend class VDeviceInitializer;
		friend class VSwapchainInitializer;

	// variables
	private:
		Library			_lib;
		void*			_api		= null;		// RENDERDOC_API_x_x_x

		void*			_device		= null;

		mutable Atomic<void*>	_wndHandle;

		mutable Atomic<uint>	_captureIdx {0};


	// methods
	private:
		RenderDocApi ()											__NE___	{}

		ND_ static bool  EnableVkLayer ()						__NE___;
		ND_ bool  Initialize (VkInstance instance)				__NE___;
			void  Deinitialize ()								__NE___;
			void  SetWindow (const NativeWindow &wndHandle)		C_NE___;

	public:
		ND_ bool  IsInitialized ()								C_NE___	{ return _api != null; }

			void  PrintCaptures ()								C_NE___;
			void  CaptureFolder (NtStringView path)				C_NE___;

		// manual frame capture
		ND_ bool  BeginFrame (NtStringView name = {})			C_NE___;
			bool  CancelFrame ()								C_NE___;
			bool  EndFrame ()									C_NE___;
		ND_ bool  IsFrameCapturing ()							C_NE___;

		// next frame
			bool  TriggerFrameCapture (NtStringView name = {})	C_NE___;
			bool  TriggerMultiFrameCapture (uint count)			C_NE___;
	};


} // AE::Graphics
