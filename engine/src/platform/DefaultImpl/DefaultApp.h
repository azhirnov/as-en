// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/IApplication.h"
#include "platform/DefaultImpl/AppConfig.h"
#include "platform/DefaultImpl/IBaseApp.h"

#if defined(AE_ENABLE_VULKAN)
#	include "graphics/Vulkan/VDevice.h"
#	include "VulkanSyncLog.h"

#elif defined(AE_ENABLE_METAL)
#	include "graphics/Metal/MDevice.h"

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	include "graphics/Remote/RDevice.h"

#else
#	error not implemented
#endif


namespace AE::App
{

	//
	// Application Event Listener
	//

	class DefaultAppListener : public IApplication::IAppListener
	{
	// variables
	protected:
	  #if defined(AE_ENABLE_VULKAN)
		Graphics::VDeviceInitializer	_vulkan;

	  #elif defined(AE_ENABLE_METAL)
		Graphics::MDeviceInitializer	_metal;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Graphics::RDeviceInitializer	_remote;
		
	  #else
	  #	error not implemented
	  #endif
			
		Array<WindowPtr>			_windows;
		VRDevicePtr					_vrDevice;
		RC<IBaseApp>				_impl;

		const AppConfig				_config;
		Threading::EThreadArray		_allowProcessInMain;


	// methods
	protected:
		explicit DefaultAppListener (const AppConfig &, RC<IBaseApp>)	__NE___;
	public:
		~DefaultAppListener ()							__NE_OV;

		ND_ AppConfig const&	Config ()				C_NE___	{ return _config; }
		ND_ IBaseApp &			GetBaseApp ()			__NE___	{ return *_impl; }


	// IAppListener //
		void  OnStart (IApplication &)					__NE_OV;
		void  OnStop  (IApplication &)					__NE_OV;
		
		void  BeforeWndUpdate (IApplication &)			__NE_OV;
		void  AfterWndUpdate (IApplication &)			__NE_OV;


	protected:
		ND_ bool  _OnStartImpl (IApplication &app)		__NE___;

	private:
	  #if defined(AE_ENABLE_VULKAN)
		bool  _CreateVulkan (IApplication &app);
		bool  _DestroyVulkan ();

	  #elif defined(AE_ENABLE_METAL)
		bool  _CreateMetal (IApplication &app);
		bool  _DestroyMetal ();

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		bool  _CreateRemoteGraphics (IApplication &app);
		bool  _DestroyRemoteGraphics ();
		
	  #else
	  #	error not implemented
	  #endif
	};


	
	//
	// Window Event Listener
	//

	class DefaultIWndListener : public IWindow::IWndListener
	{
	// variables
	private:
		RC<IBaseApp>			_impl;
		DefaultAppListener &	_app;


	// methods
	public:
		DefaultIWndListener (RC<IBaseApp> impl, DefaultAppListener &app) __NE___ :
			_impl{RVRef(impl)}, _app{app}
		{}


	// IWndListener //
		void  OnStateChanged (IWindow &, EState)	__NE_OV;
		void  OnUpdate (IWindow &)					__NE_OV {}
		
		void  OnResize (IWindow &, const uint2 &)	__NE_OV {}
		void  OnSurfaceCreated (IWindow &)			__NE_OV;
		void  OnSurfaceDestroyed (IWindow &)		__NE_OV;
	};



	//
	// VR Device Listener
	//

	class DefaultVRDeviceListener : public IVRDevice::IVRDeviceEventListener
	{
	// variables
	private:
		RC<IBaseApp>			_impl;
		DefaultAppListener &	_app;


	// methods
	public:
		DefaultVRDeviceListener (RC<IBaseApp> impl, DefaultAppListener &app) :
			_impl{RVRef(impl)}, _app{app}
		{}


	// IVRDeviceEventListener //
		void  OnUpdate (IVRDevice &)						__NE_OV	{}
		void  OnStateChanged (IVRDevice &, EState state)	__NE_OV;
	};


} // AE::App
