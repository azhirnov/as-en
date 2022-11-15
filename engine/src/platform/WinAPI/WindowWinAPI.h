// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_WINDOWS) && !defined(AE_ENABLE_GLFW)

# include "platform/Private/WindowBase.h"
# include "platform/WinAPI/InputActionsWinAPI.h"
# include "platform/IO/WindowSurface.h"

namespace AE::App
{
	class ApplicationWinAPI;


	//
	// WinAPI Window
	//

	class WindowWinAPI final : public WindowBase
	{
		friend class ApplicationWinAPI;

	// types
	private:
		enum class EVisibility
		{
			VisibleFocused,
			VisibleUnfocused,
			Invisible,
		};


	// variables
	private:
		void*				_wnd				= null;		// HWND
		uint2				_surfaceSize;
		int2				_windowPos;
		bool				_requestQuit		= false;
		bool				_lockAndHideCursor	= false;
		void*				_cursorHandle		= null;		// HCURSOR

		InputActionsWinAPI	_input;


	// methods
	public:
		~WindowWinAPI ();

	// IWindow //
		void	Close () override;
		
		uint2		GetSurfaceSize ()	const override;
		Monitor		GetMonitor ()		const override;
		
		IInputActions&	InputActions ()		  override	{ return _input; }
		NativeWindow	GetNative ()	const override;

		void  SetSize (const uint2 &size) override;
		void  SetPosition (const int2 &pos) override;
		void  SetPosition (Monitor::ID monitor, const int2 &pos) override;
		void  SetTitle (NtStringView title) override;
		void  SetFocus () const override;


	private:
		explicit WindowWinAPI (ApplicationWinAPI &app, Unique<IWndListener>, IInputActions*);

		ND_ ApplicationWinAPI&  _GetApp () const;

		ND_ bool  _Create (const WindowDesc &desc);
			void  _Destroy ();
		ND_ bool  _Update ();
		ND_ ssize _ProcessMessage (uint uMsg, usize wParam, ssize lParam);
			void  _UpdateDescription ();
			void  _ShowWindow (EVisibility value) const;

			void  _LockAndHideCursor (bool value);
			void  _ClipCursor ();
	};


} // AE::App

#endif // AE_PLATFORM_WINDOWS and not AE_ENABLE_GLFW
