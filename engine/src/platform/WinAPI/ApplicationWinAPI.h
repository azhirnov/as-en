// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_WINDOWS) && !defined(AE_ENABLE_GLFW)

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

		StringView	GetClassName () const		{ return _className; }
		void*		GetHInstance ()	const		{ return _instance; }

		static int  Run (Unique<IAppListener>);


	// IApplication //
		WindowPtr	CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*) override;

		Monitors_t	GetMonitors (bool update = false) override;
		
		StringView	GetApiName () const override	{ return "winapi"; }
		
		RC<IVirtualFileStorage> OpenBuiltinStorage () override { return null; }

		ArrayView<const char*>  GetVulkanInstanceExtensions () override;


	private:
		explicit ApplicationWinAPI (Unique<IAppListener>);
		~ApplicationWinAPI ();

		ND_ bool  _RegisterClass ();
			void  _MainLoop ();

			void  _UpdateMinitors (OUT Monitors_t &) const;
	};


} // AE::App

#endif // AE_PLATFORM_WINDOWS and not AE_ENABLE_GLFW
