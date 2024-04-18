// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/MetalTypes.h"

namespace AE::Graphics
{

	//
	// Native Window
	//

	struct NativeWindow
	{
		#if defined(AE_PLATFORM_WINDOWS)
			void*			hInstance		= null;		// HINSTANCE
			void*			hWnd			= null;		// HWND

		ND_ explicit operator bool ()		C_NE___	{ return hInstance != null and hWnd != null; }


		#elif defined(AE_PLATFORM_ANDROID)
			void*			nativeWindow	= null;		// ANativeWindow

		ND_ explicit operator bool ()		C_NE___	{ return nativeWindow != null; }


		#elif defined(AE_PLATFORM_LINUX)
			void*			x11Window		= null;		// X11 Window
			void*			x11Display		= null;		// X11 Display*

		ND_ explicit operator bool ()		C_NE___	{ return x11Window != null and x11Display != null; }


		#elif defined(AE_PLATFORM_APPLE)
			MetalNSViewRC	nsView;						// NSView*
			MetalCALayerRC	metalLayer;					// CAMetalLayer*

		ND_ explicit operator bool ()		C_NE___	{ return bool{nsView} and bool{metalLayer}; }


		#else
		#	error Unsupported platform!
		#endif

		NativeWindow ()	__NE___	{}
	};


} // AE::Graphics
