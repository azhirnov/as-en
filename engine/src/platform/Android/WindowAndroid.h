// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID

# include <android/native_window.h>

# include "platform/Public/Common.h"
# include "platform/Private/WindowBase.h"
# include "platform/Android/Java.h"
# include "platform/Android/InputActionsAndroid.h"
# include "platform/Private/WindowSurface.h"


namespace AE::App
{
	using namespace AE::Java;
	class ApplicationAndroid;


	//
	// Android Window
	//

	class WindowAndroid final : public WindowBase
	{
		friend class ApplicationAndroid;

	// types
	private:
		using WinID	= jint;


	// variables
	private:
		uint2					_surfSize;
		InputActionsAndroid		_input;

		struct {
			JavaObj					window;
			ANativeWindow*			nativeWindow	= null;
		}						_java;
		struct {
			JavaMethod< void () >	close;
		}						_methods;


	// methods
	public:
		WindowAndroid ();


	// IWindow //
		void	Close () override;

		uint2	GetSurfaceSize ()	const override;
		Monitor	GetMonitor ()		const override;

		IInputActions&	InputActions ()		  override	{ return _input; }
		NativeWindow	GetNative ()	const override;

		// desctop only
		void  SetSize (const uint2 &) override {}
		void  SetPosition (const int2 &) override {}
		void  SetPosition (Monitor::ID, const int2 &) override {}
		void  SetTitle (NtStringView) override {}
		void  SetFocus () const override {}


	private:
		void  _Init (Unique<IWndListener> listener, IInputActions* dstActions);

		ND_ ApplicationAndroid&  _App ();


	// called from java
	public:
		static WinID JNICALL  native_OnCreate (JNIEnv*, jclass, jobject wnd);
		static void  JNICALL  native_OnDestroy (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_OnStart (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_OnStop (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_OnEnterForeground (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_OnEnterBackground (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_SurfaceChanged (JNIEnv*, jclass, WinID wndId, jobject surface);
		static void  JNICALL  native_SurfaceDestroyed (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_Update (JNIEnv*, jclass, WinID wndId);
		static void  JNICALL  native_OnKey (JNIEnv*, jclass, WinID wndId, jint keycode, jint action);
		static void  JNICALL  native_OnTouch (JNIEnv*, jclass, WinID wndId, jint action, jint count, jfloatArray data);
		static void  JNICALL  native_OnOrientationChanged (JNIEnv*, jclass, WinID wndId, jint newOrientation);
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
