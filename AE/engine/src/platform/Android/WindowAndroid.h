// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"
# include "platform/Private/WindowBase.h"
# include "platform/Android/InputActionsAndroid.h"
# include "platform/Private/WindowSurface.h"

namespace AE::App
{

	//
	// Android Window
	//

	class WindowAndroid final : public WindowBaseWithSurface
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
			JavaObj					activity;
			ANativeWindow*			nativeWindow	= null;
		}						_java;
		struct {
			JavaMethod< void () >						close;
			JavaMethod< jboolean (jfloat) >				setWndBrightness;
			JavaMethod< jboolean (jboolean, jboolean) >	setHDR;
		}						_methods;


	// methods
	public:
		WindowAndroid ()								__NE___;


	// IWindow //
		void  Close ()									__NE_OV;
		bool  SetBrightness (Percent)					__NE_OV;
		bool  SetColorSpace (EColorSpace)				C_NE_OV;

		uint2			GetSurfaceSize ()				C_NE_OV;
		Monitor			GetMonitor ()					C_NE_OV;

		IInputActions&	InputActions ()					__NE_OV	{ return _input; }
		NativeWindow	GetNative ()					C_NE_OV;


	private:
		void  _Init (Unique<IWndListener>	listener,
					 const WindowDesc		&desc,
					 IInputActions*			dstActions)	__NE___;
		
		bool  ProcessMessages ()						__NE_OV	{ DBG_WARNING("don't use it");  return false; }

		ND_ ApplicationAndroid&  _App ()				__NE___;


	// called from java
	private:
		static WinID JNICALL  native_OnCreate (JNIEnv*, jclass, jobject wnd)										__NE___;
		static void  JNICALL  native_OnDestroy (JNIEnv*, jclass, WinID wndId)										__NE___;
		static void  JNICALL  native_OnStart (JNIEnv*, jclass, WinID wndId)											__NE___;
		static void  JNICALL  native_OnStop (JNIEnv*, jclass, WinID wndId)											__NE___;
		static void  JNICALL  native_OnEnterForeground (JNIEnv*, jclass, WinID wndId)								__NE___;
		static void  JNICALL  native_OnEnterBackground (JNIEnv*, jclass, WinID wndId)								__NE___;
		static void  JNICALL  native_SurfaceChanged (JNIEnv*, jclass, WinID wndId, jobject surface)					__NE___;
		static void  JNICALL  native_SurfaceDestroyed (JNIEnv*, jclass, WinID wndId)								__NE___;
		static void  JNICALL  native_Update (JNIEnv*, jclass, WinID wndId)											__NE___;
		static void  JNICALL  native_OnKey (JNIEnv*, jclass, WinID wndId, jint keycode, jint action, jint count)	__NE___;
		static void  JNICALL  native_OnTouch (JNIEnv*, jclass, WinID wndId, jint action, jint changedIndex,
											  jint touchCount, jfloatArray touchData)								__NE___;
		static void  JNICALL  native_OnOrientationChanged (JNIEnv*, jclass, WinID wndId, jint newOrientation)		__NE___;
		static void  JNICALL  native_UpdateSensor (JNIEnv*, jclass, WinID wndId, int sensor, jfloatArray values)	__NE___;
		static void  JNICALL  native_SendBatteryStat1 (JNIEnv*, jclass, float current, float capacity, float)		__NE___;
		static void  JNICALL  native_SendBatteryStat2 (JNIEnv*, jclass, float level, float temperature,
														float voltage, jboolean isCharging)							__NE___;
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
