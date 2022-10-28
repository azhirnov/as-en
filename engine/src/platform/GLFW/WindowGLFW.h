// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_GLFW

# include "platform/Private/WindowBase.h"
# include "platform/GLFW/InputActionsGLFW.h"

namespace AE::App
{
	class ApplicationGLFW;


	//
	// GLFW Window
	//

	class WindowGLFW final : public WindowBase
	{
		friend class ApplicationGLFW;

	// variables
	private:
		GLFWwindow *		_window			= null;
		float2				_contentScale;	// TODO: remove

		InputActionsGLFW	_input;
		bool				_lockAndHideCursor	= false;


	// methods
	public:
		~WindowGLFW ();

		ND_ GLFWwindow*  GetGLFWwindow ()	const	{ return _window; }


	// IWindow //
		void	Close () override;
		
		uint2	GetSurfaceSize ()	const override;
		Monitor	GetMonitor ()		const override;

		IInputActions&	InputActions ()		  override	{ return _input; }
		NativeWindow	GetNative ()	const override;

		void  SetSize (const uint2 &size) override;
		void  SetPosition (const int2 &pos) override;
		void  SetPosition (Monitor::ID monitor, const int2 &pos) override;
		void  SetTitle (NtStringView title) override;
		void  SetFocus () const override;


	private:
		WindowGLFW (ApplicationGLFW &app, Unique<IWndListener>, IInputActions*);

		ND_ bool  _Create (const WindowDesc &desc);
			void  _Destroy ();
		ND_ bool  _ProcessMessages ();
			void  _LockAndHideCursor (bool value);

		static void  _GLFW_RefreshCallback (GLFWwindow* wnd);
		static void  _GLFW_ResizeCallback (GLFWwindow* wnd, int w, int h);
		static void  _GLFW_KeyCallback (GLFWwindow* wnd, int key, int, int, int);
		static void  _GLFW_MouseButtonCallback (GLFWwindow* wnd, int button, int action, int mods);
		static void  _GLFW_CursorPosCallback (GLFWwindow* wnd, double xpos, double ypos);
		static void  _GLFW_MouseWheelCallback (GLFWwindow* wnd, double dx, double dy);
		static void  _GLFW_IconifyCallback (GLFWwindow* wnd, int iconified);
		static void  _GLFW_WindowContentScaleCallback (GLFWwindow* wnd, float, float);
		static void  _GLFW_WindowFocusCallback (GLFWwindow* wnd, int focused);
	};


} // AE::App

#endif // AE_ENABLE_GLFW
