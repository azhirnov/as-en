// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	State chain:
		Created -> Destroyed
		Created -> Started -> Stopped -> Destroyed
		Created -> Started -> InForeground -> InBackground -> Stopped -> Destroyed
		Created -> Started -> InForeground -> Focused -> InBackground -> Stopped -> Destroyed

	Thread safe:  see method description.
*/

#pragma once

#include "graphics_rhi/Public/SwapchainDesc.h"
#include "graphics_rhi/Public/NativeWindow.h"

#include "platform/Public/Monitor.h"
#include "platform/Public/InputActions.h"

namespace AE::App
{
	using Graphics::NativeWindow;
	using Graphics::EColorSpace;


	enum class EWindowMode : ubyte
	{
		Resizable,
		NonResizable,
		Borderless,

		FullScreenWindow,		// borderless, always on top
		FullScreen,

		_Count,
		Unknown			= Resizable,
	};

	NdCx__ bool  EWindowMode_IsFullScreen (EWindowMode value) __NE___ { return value >= EWindowMode::FullScreenWindow; }


	//
	// Window Description
	//

	struct WindowDesc
	{
		String			title;
		uint2			size			= {640, 480};
		Monitor::ID		monitorId		= Default;
		EWindowMode		mode			= Default;
		ubyte			androidWndId	= UMax;		// used to match Activity and Window in Engine

		WindowDesc () __NE___ {}
	};



	//
	// Window interface
	//

	class IWindow
	{
	// types
	public:
		class IWndListener : public NothrowAllocatable
		{
		// types
		public:
			enum class EState
			{					//|			all			|		desktop			|		mobile			|				VR				|
								//|---------------------|-----------------------|-----------------------|-------------------------------|
				Unknown,		//|						|						|						|								|
				Created,		//|						|						|						|								|
				Started,		//|						|						| screen turned ON		| HMD screen is turned ON		|
				InForeground,	//|						| window is on screen	| window is on screen	| HMD is mounted				|
				Focused,		//|						| window is on top		|						|								|
				InBackground,	//|						| window is minimized	| window is hidden		| HMD is active but not mounted	|
				Stopped,		//| before window close	|						| screen turned OFF		| HMD screen is turned OFF		|
				Destroyed,		//| window closed		|						|						|								|
			};

		// interface
		public:
			virtual ~IWndListener ()										__NE___	{}

			//   Thread safe: main thread only
			virtual void  OnSurfaceCreated (IWindow &wnd)					__NE___	= 0;
			virtual void  OnSurfaceDestroyed (IWindow &wnd)					__NE___	= 0;

			//   Thread safe: main thread only
			virtual void  OnStateChanged (IWindow &wnd, EState state)		__NE___	= 0;
		};

		using EState = IWndListener::EState;


	// interface
	public:
		virtual ~IWindow ()															__NE___ {}

		// Close window.
		//   Thread safe: no
		//
		virtual void  Close ()														__NE___	= 0;

		// Return surface size.
		//   Note: window size with border will be greater then surface size.
		//   Thread safe: main thread only, use thread-safe 'GetSurface().GetTargetInfo()' instead.
		//
		ND_ virtual uint2  GetSurfaceSize ()										C_NE___ = 0;

		// Returns current state.
		//   Thread safe: no
		//
		ND_ virtual EState  GetState ()												C_NE___ = 0;

		// Returns current monitor for window.
		//   Thread safe: main thread only
		//
		ND_ virtual Monitor  GetMonitor ()											C_NE___ = 0;

		// Returns window native handles.
		//   Thread safe: yes
		//
		ND_ virtual NativeWindow  GetNative ()										C_NE___ = 0;

		// Returns input actions class.
		//   Thread safe: yes
		//
		ND_ virtual IInputActions&  InputActions ()									__NE___	= 0;

		// Returns current window mode.
		//   Thread safe: no
		//
		ND_ virtual EWindowMode  GetCurrentMode ()									C_NE___ = 0;
		ND_ bool				 IsFullScreenMode ()								C_NE___	{ return EWindowMode_IsFullScreen( GetCurrentMode() ); }
		ND_ bool				 IsWindowedMode ()									C_NE___	{ return not IsFullScreenMode(); }

		// Allow to hide window from screen capture tools.
		//   Thread safe: no
		//
		ND_ virtual bool  SetDisplayMode (EWindowDisplayMode)						__NE___ { return false; }


	// surface api

		// Create or recreate rendering surface (swapchain).
		//   Thread safe: main thread only, must be synchronized with 'GetSurface()'
		//
		ND_ virtual bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc)	__NE___	= 0;

		// Returns render surface reference.
		// Surface must be successfully created using 'CreateRenderSurface()'.
		//   Thread safe: must be synchronized with 'CreateRenderSurface()'
		//
		ND_ virtual IOutputSurface&  GetSurface ()									__NE___	= 0;


	// mobile only

		// Set screen brightness.
		//   Thread safe: main thread only
		//
		ND_ virtual bool  SetBrightness (Percent level)								__NE___ = 0;


	// private api

		// Set window/monitor/system color space.
		// Should be compatible with swapchain color space.
		// Some platforms implicitly use swapchain color space and always returns 'true'.
		// Other platforms requires explicitly set same color space as in swapchain.
		// Android: scRGB color space will set maximum brightness.
		// This method is used by IOutputSurface implementation.
		//   Thread safe: main thread only
		//
		ND_ virtual bool  SetColorSpace (EColorSpace value)							C_NE___ = 0;


	// implementation

		// Convert to other window implementation.
		//   Thread safe:  yes
		//
		ND_ virtual IVRSession*			AsVRSession ()								__NE___ { return null; }
		ND_ virtual IDesktopWindow*		AsDesktopWindow ()							__NE___	{ return null; }
	};


} // AE::App
