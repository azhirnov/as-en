// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"
# include "platform/Public/IApplication.h"
# include "platform/Android/WindowAndroid.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

	//
	// Android Application
	//

	class ApplicationAndroid final : public ApplicationBase
	{
	// types
	private:
		using Window		= SharedPtr< WindowAndroid >;
		using WinID			= WindowAndroid::WinID;
		using AWindows_t	= FixedArray<Pair< WinID, Window >, PlatformConfig::MaxWindows >;


	// variables
	private:
		RecursiveMutex			_windowsGuard;
		AWindows_t				_windows;

		Monitor					_displayInfo;
		WinID					_windowCounter	= 0;
		bool 					_started		= false;
		
		struct {
			JavaObj					application;
			JavaObj					assetManager;
			AAssetManager *			jniAssetMngr	= null;
		}						_java;
		struct {
			JavaMethod< jboolean () >				isNetworkConnected;
			JavaMethod< void (jstring, jboolean) >	showToast;
			JavaMethod< void () >					createWindow;
		}						_methods;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;	// protects: _java, _methods, _displayInfo
		)


	// methods
	private:
		explicit ApplicationAndroid (Unique<IAppListener>)								__NE___;

		void _OnDestroy ()																__NE___;

		ND_ WinID  _AddWindow (SharedPtr<WindowAndroid> wnd)							__NE___;

	public:
		~ApplicationAndroid ()															__NE___;

		void  BeforeUpdate ()															__NE___;
		void  AfterUpdate ()															__NE___;

		ND_ static ApplicationAndroid*&						_GetAppInstance ()			__NE___;
		ND_ static SharedPtr<WindowAndroid>					_GetAppWindow (WinID id)	__NE___;
		ND_ static Pair< SharedPtr<WindowAndroid>, WinID >	_GetNewWindow ()			__NE___;


	// IApplication //
		WindowPtr		CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)__NE_OV;

		Monitors_t		GetMonitors (bool update = false)								__NE_OV;

		RC<IVirtualFileStorage> OpenBuiltinStorage ()									__NE_OV;

		ArrayView<const char*>	GetVulkanInstanceExtensions ()							__NE_OV;
		
		void			Terminate ()													__NE_OV;
		
		StringView		GetApiName ()													C_NE_OV	{ return "android"; }


	// called from java
	public:
		static void JNICALL  native_OnCreate (JNIEnv*, jclass, jobject app, jobject assetMngr)								__NE___;
		static void JNICALL  native_SetDirectories (JNIEnv*, jclass, jstring, jstring, jstring, jstring, jstring)			__NE___;
		static void JNICALL  native_SetDisplayInfo (JNIEnv*, jclass, jint width, jint height, jint ppi, jint orientation)	__NE___;
		static void JNICALL  native_SetSystemInfo (JNIEnv*, jclass)															__NE___;

		static jint  OnJniLoad (JavaVM* vm)																					__NE___;
		static void  OnJniUnload (JavaVM* vm)																				__NE___;
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
