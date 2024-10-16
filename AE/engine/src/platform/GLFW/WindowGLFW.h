// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/GLFW/GLFWCommon.h"

#ifdef AE_ENABLE_GLFW
# include "platform/Private/WindowBase.h"
# include "platform/GLFW/InputActionsGLFW.h"

namespace AE::App
{

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

		RectI				_lastWindowSize;	// before switching to fullscreen


	// methods
	public:
		~WindowGLFW ()												__NE_OV;

		ND_ GLFWwindow*  GetGLFWwindow ()							C_NE___	{ return _window; }


	// IWindow //
		void	Close ()											__NE_OV;

		uint2	GetSurfaceSize ()									C_NE_OV;
		Monitor	GetMonitor ()										C_NE_OV;

		IInputActions&	InputActions ()								__NE_OV	{ return _input; }
		NativeWindow	GetNative ()								C_NE_OV;

		void  SetSize (const uint2 &size)							__NE_OV;
		void  SetPosition (const int2 &pos)							__NE_OV;
		void  SetPosition (Monitor::ID monitor, const int2 &pos)	__NE_OV;
		void  SetTitle (NtStringView title)							__NE_OV;
		void  SetFocus ()											C_NE_OV;
		bool  SetMode (EWindowMode mode, Monitor::ID monitor)		__NE_OV;


	private:
		WindowGLFW (ApplicationGLFW &, Unique<IWndListener>, IInputActions*) __NE___;

		ND_ bool  _Create (const WindowDesc &desc)					__NE___;
			void  _Destroy ()										__NE___;
		ND_ bool  _ProcessMessages ()								__NE___;
			void  _LockAndHideCursor (bool value)					__NE___;

		static void  _GLFW_RefreshCallback (GLFWwindow* wnd)										__NE___;
		static void  _GLFW_ResizeCallback (GLFWwindow* wnd, int w, int h)							__NE___;
		static void  _GLFW_KeyCallback (GLFWwindow* wnd, int key, int, int, int)					__NE___;
		static void  _GLFW_MouseButtonCallback (GLFWwindow* wnd, int button, int action, int mods)	__NE___;
		static void  _GLFW_CursorPosCallback (GLFWwindow* wnd, double xpos, double ypos)			__NE___;
		static void  _GLFW_MouseWheelCallback (GLFWwindow* wnd, double dx, double dy)				__NE___;
		static void  _GLFW_IconifyCallback (GLFWwindow* wnd, int iconified)							__NE___;
		static void  _GLFW_WindowContentScaleCallback (GLFWwindow* wnd, float, float)				__NE___;
		static void  _GLFW_WindowFocusCallback (GLFWwindow* wnd, int focused)						__NE___;
	};


} // AE::App

#endif // AE_ENABLE_GLFW
