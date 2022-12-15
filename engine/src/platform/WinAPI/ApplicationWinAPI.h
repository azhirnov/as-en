// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_WINDOWS) and not defined(AE_ENABLE_GLFW)

# include "platform/Private/ApplicationBase.h"
# include "platform/WinAPI/WindowWinAPI.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{
	
	//
	// WinAPI Application
	//

	class ApplicationWinAPI final : public ApplicationBase
	{
	// types
	private:
		using Window		= WeakPtr< WindowWinAPI >;
		using WindowArray_t	= FixedArray< Window, PlatformConfig::MaxWindows >;


	// variables
	private:
		RecursiveMutex		_windowsGuard;
		WindowArray_t		_windows;

		void *				_instance;		// HMODULE
		String				_className;

		Monitors_t			_cachedMonitors;


	// methods
	public:

		StringView	GetClassName ()														C_NE___	{ return _className; }
		void*		GetHInstance ()														C_NE___	{ return _instance; }

		static int  Run (Unique<IAppListener>)											__NE___;


	// IApplication //
		WindowPtr	CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)	__NE_OV;

		Monitors_t	GetMonitors (bool update = false)									__NE_OV;
		
		StringView	GetApiName ()														C_NE_OV	{ return "winapi"; }
		
		RC<IVirtualFileStorage> OpenBuiltinStorage ()									__NE_OV { return null; }

		ArrayView<const char*>  GetVulkanInstanceExtensions ()							__NE_OV;


	private:
		explicit ApplicationWinAPI (Unique<IAppListener>)								__NE___;
		~ApplicationWinAPI ()															__NE___;

		ND_ bool  _RegisterClass ()														__NE___;
			void  _MainLoop ()															__NE___;

			void  _UpdateMinitors (OUT Monitors_t &)									C_NE___;
	};


} // AE::App

#endif // AE_PLATFORM_WINDOWS and not AE_ENABLE_GLFW
