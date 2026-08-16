// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"
# include "platform/Public/Application.h"
# include "platform/Android/WindowAndroid.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::Networking {
	struct IpAddress;
}
namespace AE::App
{

	//
	// Android Application
	//

	class ApplicationAndroid final : public ApplicationBase
	{
	// types
	private:
		using AWindow		= SharedPtr< WindowAndroid >;
		using WinID			= WindowAndroid::WinID;
		using AWindows_t	= FixedArray<Pair< WinID, AWindow >, PlatformConfig::MaxWindows >;

		struct StoragePath
		{
			AAssetManager *		jniAssetMngr	= null;
			Path				internalAppData;
			Path				internalCache;
			Path				externalAppData;
			Path				externalCache;
			Path				externalStorage;
		};
		using StoragePathSync_t = Threading::Synchronized< SharedMutex, StoragePath >;


	// variables
	private:
		AWindows_t					_andWindows;		// windows that created from java, may not match to 'ApplicationBase::_windows'

		Monitor						_displayInfo;
		WinID						_windowCounter	= 0;
		bool 						_started		= false;

		Locales_t					_locales;
		StoragePathSync_t			_paths;

		struct {
			JavaObj						application;
			JavaObj						assetManager;
		}							_java;
		struct {
			JavaMethod< jboolean () >				isNetworkConnected;
			JavaMethod< void (jstring, jboolean) >	showToast;
			//JavaMethod< void () >					createWindow;
			JavaMethod< jstring () >				getDefaultIpv4Gateway;
		}							_methods;

		DRC_ONLY(
			RWDataRaceCheck			_drCheck;	// protects: _java, _methods, _displayInfo
		)


	// methods
	private:
		explicit ApplicationAndroid (Unique<IAppListener>)								__NE___;

		void  _OnDestroy ()																__NE___;

		ND_ WinID  _AddAndroidWindow (SharedPtr<WindowAndroid> wnd)						__NE___;
			void   _AddWindow (SharedPtr<WindowBase>)									__NE_OV;

		static bool  _GetRouterIPAddress (void*, OUT Networking::IpAddress &)			__NE___;


	public:
		~ApplicationAndroid ()															__NE___;

		void  BeforeUpdate ()															__NE___;
		void  AfterUpdate ()															__NE___;

		void  SetRotation (int)															__NE___;
		void  ShowToast (NtStringView msg, bool longTime = false)						__NE___;

		ND_ static ApplicationAndroid*						_GetAppInstance ()			__NE___;
		ND_ static SharedPtr<WindowAndroid>					_GetAppWindow (WinID id)	__NE___;
		ND_ static Pair< SharedPtr<WindowAndroid>, WinID >	_NewWindow ()				__NE___;


	// IApplication //
		WindowPtr		CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)__NE_OV;

		void			Terminate ()													__NE_OV;
		StringView		GetApiName ()													C_NE_OV	{ return "Android"; }
		Locales_t		GetLocales ()													C_NE_OV	{ return _locales; }
		RC<IHwCamera>	GetHwCamera ()													__NE_OV	{ return null; }

		MonitorsView_t			GetMonitors (bool update = false)						__NE_OV;
		MonitorsView_t			GetCachedMonitors ()									C_NE_OV;
		RC<IVirtualFileStorage> OpenStorage (EAppStorage type)							__NE_OV;
		Path					GetStoragePath (EAppStorage type)						C_NE_OV;
		ArrayView<const char*>	GetVulkanInstanceExtensions ()							__NE_OV;


	// called from java
	private:
		static void JNICALL  native_OnCreate (JNIEnv*, jclass, jobject app,
											  jobject assetMngr,
											  jboolean isUnderDebugger)					__NE___;
		static void JNICALL  native_SetDirectories (JNIEnv*, jclass, jstring,
													jstring, jstring, jstring,
													jstring)							__NE___;
		static void JNICALL  native_SetDisplayInfo (JNIEnv*, jclass,
										jint minWidth, jint minHeight,
										jint maxWidth, jint maxHeight,
										float dpi, jint refreshRate, jint orientation,
										float avrLum, float maxLum, float minLum,
										jintArray cutoutRects, jint cutoutRectCount)	__NE___;
		static void JNICALL  native_SetSystemInfo (JNIEnv*, jclass, jstring, jstring)	__NE___;
		static void JNICALL  native_EnableCamera (JNIEnv*, jclass)						__NE___;

	public:
		static jint  OnJniLoad (JavaVM* vm)												__NE___;
		static void  OnJniUnload (JavaVM* vm)											__NE___;
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
