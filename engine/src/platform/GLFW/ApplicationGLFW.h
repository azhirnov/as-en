// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_GLFW
# include "platform/Public/IApplication.h"
# include "platform/GLFW/WindowGLFW.h"
# include "platform/Private/ApplicationBase.h"

typedef struct GLFWmonitor GLFWmonitor;

namespace AE::App
{

	//
	// GLFW Application
	//

	class ApplicationGLFW final : public ApplicationBase
	{
	// types
	private:
		using Window		= WeakPtr< WindowGLFW >;
		using WindowArray_t	= FixedArray< Window, PlatformConfig::MaxWindows >;


	// variables
	private:
		RecursiveMutex		_windowsGuard;
		WindowArray_t		_windows;
		
		Monitors_t			_cachedMonitors;


	// methods
	public:
		static int   Run (Unique<IAppListener>);
		static bool  GetMonitorInfo (GLFWmonitor*, OUT Monitor &);


	// IApplication //
		WindowPtr	CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*) override;

		Monitors_t	GetMonitors (bool update = false) override;
		
		StringView	GetApiName () const override	{ return "glfw"; }

		RC<IVirtualFileStorage> OpenBuiltinStorage () override { return null; }

		ArrayView<const char*>	GetVulkanInstanceExtensions () override;


	private:
		explicit ApplicationGLFW (Unique<IAppListener>);
		~ApplicationGLFW ();

		void  _MainLoop ();

		void  _UpdateMinitors (OUT Monitors_t &) const;
	};


} // AE::App

#endif // AE_ENABLE_GLFW
