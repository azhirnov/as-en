// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Application.h"
#include "platform/BaseAppV1/AppConfig.h"
#include "platform/BaseAppV1/IBaseApp.h"

#if defined(AE_ENABLE_VULKAN)
#	include "graphics_rhi/Vulkan/VDevice.h"

#elif defined(AE_ENABLE_METAL)
#	include "graphics_rhi/Metal/MDevice.h"

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	include "graphics_rhi/Remote/RDevice.h"

#else
#	error not implemented
#endif


namespace AE::AppV1
{

	//
	// Application Event Listener
	//

	class AppCoreV1 : public IApplication::IAppListener
	{
	// types
	private:
		class WindowEventListener;


	// variables
	private:
	  #if defined(AE_ENABLE_VULKAN)
		Graphics::VDeviceInitializer	_device;

	  #elif defined(AE_ENABLE_METAL)
		Graphics::MDeviceInitializer	_device;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		Graphics::RDeviceInitializer	_device;

	  #else
	  #	error not implemented
	  #endif

		Array<WindowPtr>			_windows;
		RC<IBaseApp>				_impl;

		const AppConfig				_config;
		Threading::EThreadArray		_allowProcessInMain;


	// methods
	protected:
		explicit AppCoreV1 (const AppConfig &, RC<IBaseApp>)__NE___;
	public:
		~AppCoreV1 ()										__NE_OV;

		ND_ AppConfig const&	GetConfig ()				C_NE___	{ return _config; }
		ND_ IBaseApp &			GetBaseApp ()				__NE___	{ return *_impl; }

		ND_ auto const&			GetMainThreadMask ()		C_NE___	{ return _allowProcessInMain; }


	// IAppListener //
		void  OnStart (IApplication &)						__NE_OV;
		void  OnStop  (IApplication &)						__NE_OV;

		void  BeforeWndUpdate (IApplication &)				__NE_OV;
		void  AfterWndUpdate (IApplication &)				__NE_OV;


	protected:
		ND_ bool  _OnStartImpl (IApplication &)				__NE___;
			void  _CreateVRDevice (IApplication &)			__NE___;

	private:
		ND_ bool  _InitGraphics (IApplication &)			__NE___;
			void  _DestroyGraphics ()						__NE___;
	};



	//
	// Window Event Listener
	//

	class AppCoreV1::WindowEventListener final : public IWindow::IWndListener
	{
	// variables
	private:
		RC<IBaseApp>		_impl;
		AppCoreV1 &			_app;


	// methods
	public:
		WindowEventListener (RC<IBaseApp> impl, AppCoreV1 &app) __NE___ :
			_impl{RVRef(impl)}, _app{app}
		{}


	// IWndListener //
		void  OnStateChanged (IWindow &, EState)	__NE_OV;
		void  OnSurfaceCreated (IWindow &)			__NE_OV;
		void  OnSurfaceDestroyed (IWindow &)		__NE_OV;
	};


} // AE::AppV1
