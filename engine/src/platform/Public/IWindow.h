// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	State chain:
		Created -> Destroyed
		Created -> Started -> Stopped -> Destroyed
		Created -> Started -> InForeground -> InBackground -> Stopped -> Destroyed
		Created -> Started -> InForeground -> Focused -> InBackground -> Stopped -> Destroyed
*/

#pragma once

#include "graphics/Public/SwapchainDesc.h"
#include "graphics/Public/NativeWindow.h"

#include "platform/Public/Monitor.h"
#include "platform/Public/InputActions.h"

namespace AE::App
{
	using Graphics::NativeWindow;



	//
	// Window Description
	//

	struct WindowDesc
	{
		String			title;
		uint2			size		= {640, 480};
		Monitor::ID		monitorId	= Default;

		bool			resizable	: 1;
		bool			fullscreen	: 1;
		bool			borderless	: 1;

		WindowDesc () :
			resizable{false}, fullscreen{false}, borderless{false}
		{}
	};
	


	//
	// Window interface
	//

	class IWindow
	{
	// types
	public:
		class IWndListener
		{
		// types
		public:
			enum class EState
			{
				Unknown,
				Created,
				Started,
				InForeground,	// Unfocused ?
				Focused,
				InBackground,
				Stopped,
				Destroyed,
			};

		// interface
		public:
			virtual ~IWndListener () {}

			virtual void  OnUpdate (IWindow &wnd) = 0;
			virtual void  OnResize (IWindow &wnd, const uint2 &newSize) = 0;
			
			virtual void  OnSurfaceCreated (IWindow &wnd) = 0;
			virtual void  OnSurfaceDestroyed (IWindow &wnd) = 0;

			virtual void  OnStateChanged (IWindow &wnd, EState state) = 0;
		};

		using EState = IWndListener::EState;


	// interface
	public:

		// Close window.
		//   Thread safe: no
		//
		virtual void  Close () = 0;
		
		// Return surface size.
		//   Window size with border will be greater.
		//   Thread safe: main thread only
		//
		ND_ virtual uint2  GetSurfaceSize () const = 0;
		
		// Returns current state.
		//   Thread safe: no
		//
		ND_ virtual EState  GetState () const = 0;
		
		// Returns current monitor for window.
		//   Thread safe: main thread only
		//
		ND_ virtual Monitor  GetMonitor () const = 0;
		
		// Returns window native handles.
		//   Thread safe: yes
		//
		ND_ virtual NativeWindow  GetNative () const = 0;
		
		// Returns input actions class.
		//   Thread safe: yes
		//
		ND_ virtual IInputActions&  InputActions () = 0;


	// surface api
		
		// Create or recreate rendering surface (swapchain).
		//   Thread safe: main thread only, must be synchronized with 'GetSurface()'
		//
		ND_ virtual bool  CreateRenderSurface (const Graphics::SwapchainDesc &desc) = 0;
		
		// Returns render surface reference.
		// Surface must be successfully created using 'CreateRenderSurface()'.
		//   Thread safe: must be synchronized with 'CreateRenderSurface()'
		//
		ND_ virtual IOutputSurface&  GetSurface () = 0;


	// desctop only
		
		// Set focus to the window.
		//   Thread safe: main thread only
		//
		virtual void  SetFocus () const = 0;
		
		// Set surface size.
		//   Thread safe: main thread only
		//
		virtual void  SetSize (const uint2 &size) = 0;
		
		// Set window position.
		//   Thread safe: main thread only
		//
		virtual void  SetPosition (const int2 &pos) = 0;
		virtual void  SetPosition (Monitor::ID monitor, const int2 &pos) = 0;
		
		// Set window title.
		//   Thread safe: main thread only
		//
		virtual void  SetTitle (NtStringView title) = 0;

		// TODO:
		//	hide cursor
		//	set cursor pos
	};


} // AE::App
