// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/StdInclude.h"

#ifdef AE_PLATFORM_ANDROID
# include <android/asset_manager_jni.h>

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
		explicit ApplicationAndroid (Unique<IAppListener>);

		void _OnDestroy ();

		ND_ WinID  _AddWindow (SharedPtr<WindowAndroid> wnd);

	public:
		~ApplicationAndroid ();

		void  BeforeUpdate ();
		void  AfterUpdate ();

		ND_ static ApplicationAndroid*&						_GetAppInstance ();
		ND_ static SharedPtr<WindowAndroid>					_GetAppWindow (WinID id);
		ND_ static Pair< SharedPtr<WindowAndroid>, WinID >	_GetNewWindow ();


	// IApplication //
		WindowPtr		CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*) override;

		Monitors_t		GetMonitors (bool update = false) override;

		RC<IVirtualFileStorage> OpenBuiltinStorage () override;

		ArrayView<const char*>	GetVulkanInstanceExtensions () override;
		
		void			Terminate () override;
		
		StringView		GetApiName () const override	{ return "android"; }


	// called from java
	public:
		static void JNICALL  native_OnCreate (JNIEnv*, jclass, jobject app, jobject assetMngr);
		static void JNICALL  native_SetDirectories (JNIEnv*, jclass, jstring, jstring, jstring, jstring, jstring);
		static void JNICALL  native_SetDisplayInfo (JNIEnv*, jclass, jint width, jint height, jint ppi, jint orientation);
		static void JNICALL  native_SetSystemInfo (JNIEnv*, jclass);

		static jint  OnJniLoad (JavaVM* vm);
		static void  OnJniUnload (JavaVM* vm);
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
