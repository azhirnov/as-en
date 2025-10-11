// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Game Application designed for games and VR.

	Don't add androidx dependencies in 'build.gradle' !
*/

#pragma once

#include "platform/Android/AndroidCommon.h"

#ifdef AE_PLATFORM_ANDROID
# include "platform/Public/Common.h"
# include "platform/Public/Application.h"
# include "platform/Android/GameWindowAndroid.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

	//
	// Game Android Application
	//

	class GameAppAndroid final : public ApplicationBase
	{
	// types
	private:
		struct StoragePath
		{
			Path	internalAppData;
			Path	internalCache;
			Path	externalAppData;
			Path	externalCache;
		};
		using StoragePathSync_t = Threading::Synchronized< Threading::RWSpinLock, StoragePath >;

		using EState = IWindow::IWndListener::EState;


	// variables
	private:
		SharedPtr<WindowBase>		_window;

		Monitor						_displayInfo;

		StoragePathSync_t			_paths;

		void *						_nativeApp	= null;		// android_app
		EState						_state		= EState::Unknown;

		DRC_ONLY(
			RWDataRaceCheck			_drCheck;				// protects: _displayInfo, _nativeApp
		)


	// methods
	public:
		~GameAppAndroid ()																		__NE___;

		void  BeforeUpdate ()																	__NE___;
		void  AfterUpdate ()																	__NE___;

		void  SetRotation (int)																	__NE___;
		void  ShowToast (NtStringView msg, bool longTime = false)								__NE___;

		ND_ void*						_GetAndroidApp ()										C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _nativeApp; }
		ND_ static GameAppAndroid*		_GetAppInstance ()										__NE___;


	// IApplication //
		WindowPtr		CreateWindow (WndListenerPtr, const WindowDesc &, IInputActions*)		__NE_OV;
		WindowPtr		CreateVRSession (WndListenerPtr, IInputActions*, IVRSession::EDeviceType)__NE_OV;

		void			Terminate ()															__NE_OV;
		StringView		GetApiName ()															C_NE_OV	{ return "Android"; }
		Locales_t		GetLocales ()															C_NE_OV	{ return {}; }

		MonitorsView_t			GetMonitors (bool update = false)								__NE_OV;
		MonitorsView_t			GetCachedMonitors ()											C_NE_OV;
		RC<IVirtualFileStorage> OpenStorage (EAppStorage type)									__NE_OV;
		Path					GetStoragePath (EAppStorage type)								__NE_OV;
		ArrayView<const char*>	GetVulkanInstanceExtensions ()									__NE_OV;

	private:
		explicit GameAppAndroid (Unique<IAppListener>)											__NE___;

		void  _OnDestroy ()																		__NE___;
		void  _MainLoop (void*)																	__NE___;

		static void  _HandleCmd (void* app, int32_t cmd)										__NE___;

	public:
		friend void  ::android_main (android_app* app);
	};


} // AE::App

#endif // AE_PLATFORM_ANDROID
