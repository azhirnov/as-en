// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	--- IApplication ---
	Contains array of windows.
	May create:
	 * IInputSurface for camera.
	 * IVirtualFileStorage for builtin resources.


	--- IWindow ---
	Contains reference to IOutputSurface.
*/

#pragma once

#include "graphics/Public/SwapchainDesc.h"
#include "platform/Public/Monitor.h"
#include "platform/Public/IWindow.h"
#include "platform/Public/VRDevice.h"
#include "platform/Public/AppEnums.h"
#include "platform/Public/HwCamera.h"

namespace AE::App
{
	enum class EAppStorage
	{
		Builtin,		// read-only
		Cache,			// read / write / execute
		ExternalCache,	// read / write
		//AppData
	};


	//
	// Application interface
	//

	class NO_VTABLE IApplication
	{
	// types
	public:
		enum class EAppEvent : uint
		{
			MonitorChanged,
			CameraEnabled,
		};

		class NO_VTABLE IAppListener : public NothrowAllocatable
		{
		// interface
		public:
			virtual ~IAppListener ()							__NE___	{}

			//   Thread safe: main thread only
			virtual void  BeforeWndUpdate (IApplication &)		__NE___ = 0;
			virtual void  AfterWndUpdate (IApplication &)		__NE___	= 0;

			//   Thread safe: main thread only
			virtual void  OnStart (IApplication &)				__NE___	= 0;
			virtual void  OnStop (IApplication &)				__NE___	= 0;

			//   Thread safe: main thread only
			virtual void  OnEvent (IApplication &, EAppEvent)	__NE___ {}
		};

		using WndListenerPtr	= Unique< IWindow::IWndListener >;
		using VRDevListenerPtr	= Unique< IVRDevice::IVRDeviceEventListener >;
		using EnumWindowsFn_t	= Function< void (ArrayView< IWindow* >) >;
		using Duration_t		= IInputActions::Duration_t;
		using Locales_t			= FixedArray< LocaleName, 4 >;


	// interface
	public:

		// Create new window.
		//   Android supports only one window.
		//   Windows, Linux, Mac supports multiple windows.
		//   Thread safe: main thread only
		//
		ND_ virtual WindowPtr  CreateWindow (WndListenerPtr, const WindowDesc &wndDesc,
											 IInputActions* dstIA = null)					__NE___ = 0;


		// Create VR device.
		// Input actions can be used to redirect VR input actions to the same queue.
		//   Thread safe: yes
		//
		ND_ virtual VRDevicePtr  CreateVRDevice (VRDevListenerPtr,
												 IInputActions*			dstIA	= null,
												 IVRDevice::EDeviceType	type	= Default)	__NE___ = 0;


		// Open OS-specific file storage. Returns null if not supported.
		//   Thread safe: yes
		//
		ND_ virtual RC<IVirtualFileStorage>  OpenStorage (EAppStorage type)					__NE___	= 0;


		// Retuns OS-specific absolute path or empty is not supported.
		//   Thread safe: yes
		//
		ND_ virtual Path  GetStoragePath (EAppStorage type)									__NE___	= 0;


		// Returns array of monitors.
		// Monitors must be updated if something has been changed (attached/detached monitor, monitor position changed in OS).
		//   Thread safe: main thread only
		//
		ND_ virtual ArrayView<Monitor>  GetMonitors (bool update = false)					__NE___	= 0;


		// Returns required extension names for Vulkan.
		//   Thread safe: yes
		//
		ND_ virtual ArrayView<const char*>  GetVulkanInstanceExtensions ()					__NE___	= 0;


		// Window may be closed from any thread,
		// this method internally lock window to prevent destruction.
		//   Thread safe: yes
		//
		//	virtual void  EnumWindows (const EnumWindowsFn_t &fn)							__NE___	= 0;


		// Close all windows and delete app.
		//   Thread safe: yes
		//
			virtual void  Terminate ()														__NE___	= 0;


		// Returns API/Framework name.
		//   Thread safe: yes
		//
		ND_ virtual StringView  GetApiName ()												C_NE___ = 0;


		// Returns time since startup.
		//   Thread safe: yes
		//
		ND_ virtual Duration_t  GetTimeSinceStart ()										C_NE___ = 0;


		// Returns array of supported locales.
		//   Thread safe: yes
		//
		ND_ virtual Locales_t  GetLocales ()												C_NE___ = 0;


		// Returns pointer to hardware camera manager.
		// Can be null if not exists or have not permission.
		// May be non-null after event 'CameraEnabled'.
		//   Thread safe: yes
		//
		ND_ virtual RC<IHwCamera>  GetHwCamera ()											__NE___ { return Default; }


		// TODO:
		//	- vibration (android)
		//	- set orientation (android)
	};



	//
	// Thread-safe wrapper for IApplication
	//

	class IApplicationTS
	{
	// types
	public:
		using Duration_t		= IApplication::Duration_t;
		using Locales_t			= IApplication::Locales_t;

	// variables
	private:
		Ptr<IApplication>	_app;

	// methods
	public:
		IApplicationTS ()													__NE___ {}
		explicit IApplicationTS (IApplication &app)							__NE___ : _app{&app} {}

		ND_ RC<IVirtualFileStorage>		OpenStorage (EAppStorage type)		__NE___	{ return _app->OpenStorage( type ); }
		ND_ ArrayView<const char*>		GetVulkanInstanceExtensions ()		__NE___	{ return _app->GetVulkanInstanceExtensions(); }
			void						Terminate ()						__NE___	{ return _app->Terminate(); }
		ND_ StringView					GetApiName ()						C_NE___	{ return _app->GetApiName(); }
		ND_ Duration_t					GetTimeSinceStart ()				C_NE___	{ return _app->GetTimeSinceStart(); }
		ND_ Locales_t					GetLocales ()						C_NE___	{ return _app->GetLocales(); }
		ND_ RC<IHwCamera>				GetHwCamera ()						__NE___ { return _app->GetHwCamera(); }
	};

} // AE::App


extern std::unique_ptr<AE::App::IApplication::IAppListener>  AE_OnAppCreated ();
extern void													 AE_OnAppDestroyed ();


#ifdef AE_CPP_DETECT_MISMATCH

# ifdef AE_PLATFORM_ANDROID
#	pragma detect_mismatch( "AE_PLATFORM_ANDROID", "1" )
# else
#	pragma detect_mismatch( "AE_PLATFORM_ANDROID", "0" )
# endif

# ifdef AE_ENABLE_GLFW
#	pragma detect_mismatch( "AE_ENABLE_GLFW", "1" )
# else
#	pragma detect_mismatch( "AE_ENABLE_GLFW", "0" )
# endif

# ifdef AE_ENABLE_OPENVR
#	pragma detect_mismatch( "AE_ENABLE_OPENVR", "1" )
# else
#	pragma detect_mismatch( "AE_ENABLE_OPENVR", "0" )
# endif

# ifdef AE_WINAPI_WINDOW
#	pragma detect_mismatch( "AE_WINAPI_WINDOW", "1" )
# else
#	pragma detect_mismatch( "AE_WINAPI_WINDOW", "0" )
# endif

#endif // AE_CPP_DETECT_MISMATCH
