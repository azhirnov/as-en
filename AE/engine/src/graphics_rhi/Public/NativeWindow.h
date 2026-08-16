// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/Common.h"
#include "graphics_rhi/Public/MetalTypes.h"

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
			void*			activity		= null;		// jobject
			void*			nativeWindow	= null;		// ANativeWindow

			ND_ explicit operator bool ()		C_NE___	{ return activity != null and nativeWindow != null; }


		#elif defined(AE_PLATFORM_LINUX)
			struct X11 {
				void*			window		= null;		// X11 Window
				void*			display		= null;		// X11 Display*

				ND_ explicit operator bool ()	C_NE___	{ return window != null and display != null; }
			};
			struct Wayland {
				void*			surface		= null;		// wl_surface*
				void*			display		= null;		// wl_display*

				ND_ explicit operator bool ()	C_NE___	{ return surface != null and display != null; }
			};

			Union< NullUnion, X11, Wayland >	impl;

			ND_ explicit operator bool ()		C_NE___
			{
				return Visit( impl,
							  [](const X11 &x11)	{ return bool{x11}; },
							  [](const Wayland &wl)	{ return bool{wl}; },
							  [](const NullUnion &)	{ return false; }
							);
			}


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
