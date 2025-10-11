// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/GLFW/GLFWCommon.h"

#ifdef AE_ENABLE_GLFW
# include "platform/GLFW/WindowGLFW.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

	//
	// GLFW Application
	//

	class ApplicationGLFW final : public ApplicationBase
	{
	// variables
	private:
		Monitors_t			_cachedMonitors;

		Locales_t			_locales;


	// methods
	public:
		static int   Run (Unique<IAppListener>)											__NE___;
		static bool  GetMonitorInfo (GLFWmonitor*, OUT Monitor &)						__NE___;


	// IApplication //
		WindowPtr	CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)	__NE_OV;
		StringView	GetApiName ()														C_NE_OV	{ return "GLFW"; }
		Locales_t	GetLocales ()														C_NE_OV	{ return _locales; }

		MonitorsView_t			GetMonitors (bool update = false)						__NE_OV;
		MonitorsView_t			GetCachedMonitors ()									C_NE_OV;
		RC<IVirtualFileStorage> OpenStorage (EAppStorage)								__NE_OV { return null; }
		Path					GetStoragePath (EAppStorage)							__NE_OV	{ return {}; }
		ArrayView<const char*>	GetVulkanInstanceExtensions ()							__NE_OV;
		RC<IScreenCapture>		StartScreenCapture (const IScreenCapture::Config &)		__NE_OV;
		Unique<ISendInput>		CreateInputSender ()									__NE_OV;


	private:
		explicit ApplicationGLFW (Unique<IAppListener>)									__NE___;
		~ApplicationGLFW ()																__NE___;

		void  _MainLoop ()																__NE___;

		void  _UpdateMonitors (OUT Monitors_t &)										C_NE___;
		void  _GetLocales (OUT Locales_t &)												C_NE___;
	};


} // AE::App

#endif // AE_ENABLE_GLFW
