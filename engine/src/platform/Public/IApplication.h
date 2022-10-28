// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
		IApplication
	Contains array of windows.
	May create IInputSurface for camera.

		IWindow
	Contains reference to IOutputSurface.
*/

#pragma once

#include "graphics/Public/SwapchainDesc.h"
#include "platform/Public/Monitor.h"
#include "platform/Public/IWindow.h"
#include "platform/Public/VRDevice.h"
#include "platform/Public/AppEnums.h"

namespace AE::App
{

	//
	// Application interface
	//

	class IApplication
	{
	// types
	public:
		class IAppListener
		{
		// interface
		public:
			virtual ~IAppListener () {}

			virtual void  BeforeWndUpdate (IApplication &) = 0;
			virtual void  AfterWndUpdate (IApplication &) = 0;

			virtual void  OnStart (IApplication &) = 0;
			virtual void  OnStop (IApplication &) = 0;
		};

		using Monitors_t		= FixedArray< Monitor, PlatformConfig::MaxMonitors >;
		using WndListenerPtr	= Unique< IWindow::IWndListener >;
		using VRDevListenerPtr	= Unique< IVRDevice::IVRDeviceEventListener >;
		using EnumWindowsFn_t	= Function< void (ArrayView< IWindow* >) >;
		using Duration_t		= IInputActions::Duration_t;


	// interface
	public:
		
		// Create new window.
		//   Android supports only one window.
		//   Windows, Linux, Mac supports multiple windows.
		//   Thread safe: main thread only
		//
		ND_ virtual WindowPtr  CreateWindow (WndListenerPtr, const WindowDesc &wndDesc, IInputActions* dst = null) = 0;

		
		// Create VR device.
		// Input actions can be used to redirect VR input actions to the same queue.
		//   Thread safe: yes
		//
		ND_ virtual VRDevicePtr  CreateVRDevice (VRDevListenerPtr, IInputActions* dst = null, IVRDevice::EDeviceType type = Default) = 0;


		// Read builtin file system.
		//   Thread safe: no
		//
		ND_ virtual RC<IFileStorage>  OpenBuiltinStorage () = 0;


		// Returns array of monitors.
		// Monitors must be updated if something has been changed (attached/detached monitor, monitor position changed in OS).
		//   Thread safe: main thread only
		//
		ND_ virtual Monitors_t  GetMonitors (bool update = false) = 0;


		// Returns required extension names for Vulkan.
		//   Thread safe: yes
		//
		ND_ virtual ArrayView<const char*>  GetVulkanInstanceExtensions () = 0;


		// Window may be closed from any thread,
		// this method internally lock window to prevent destruction.
		//   Thread safe: yes
		//
		//	virtual void  EnumWindows (const EnumWindowsFn_t &fn) = 0;


		// Close all windows and delete app.
		//   Thread safe: yes
		//
			virtual void  Terminate () = 0;

			
		// Returns API/Framework name.
		//   Thread safe: yes
		//
		ND_ virtual StringView  GetApiName () const = 0;


		// Returns time since startup.
		//   Thread safe: yes
		//
		ND_ virtual Duration_t  GetTimeSinceStart () const = 0;


		// TODO:
		//	- vibration (android)
		//	- set orientation (android)
	};


} // AE::App


extern std::unique_ptr<AE::App::IApplication::IAppListener>  AE_OnAppCreated ();
extern void													 AE_OnAppDestroyed ();
