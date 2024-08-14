// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	[supported platforms and features](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/Platforms.md)
*/

#pragma once

#include "platform/Platform.pch.h"

namespace AE::App::_hidden_
{
	// tag: NamedID UID
	static constexpr uint	AppIDs_Start = 3 << 24;

} // AE::App::_hidden_


namespace AE::App
{
	using namespace AE::Base;

	using Threading::Atomic;
	using Threading::AtomicByte;
	using Threading::AsyncTask;
	using Threading::Promise;

	#if AE_ENABLE_DATA_RACE_CHECK
	using Threading::DataRaceCheck;
	using Threading::RWDataRaceCheck;
	using Threading::SingleThreadCheck;
	#endif

	using VFS::IVirtualFileStorage;


	class IApplication;
	class IWindow;
	class IVRDevice;
	class IOutputSurface;


	using WindowPtr		= SharedPtr< IWindow >;
	using VRDevicePtr	= SharedPtr< IVRDevice >;


	// Used to identify different gamepads and to send feedback actions (vibrations, ...).
	enum class ControllerID : ushort
	{
		Default		= 0,
		Invalid		= 0xFFFF,

		_First		= 1,
		_VRFirst	= 1 << 14,

	  // default IDs:
		Mouse		= _First,
		Keyboard,
		Touchscreen,
		Sensor,

		VR_HMD		= _VRFirst,
		VR_LeftHand,
		VR_RightHand,
	};


	using InputModeName		= NamedID< 32, App::_hidden_::AppIDs_Start + 1, AE_OPTIMIZE_IDS >;
	using InputActionName	= NamedID< 32, App::_hidden_::AppIDs_Start + 2, AE_OPTIMIZE_IDS >;
	using LocaleName		= NamedID< 8,  App::_hidden_::AppIDs_Start + 3, AE_OPTIMIZE_IDS >;	// format: en-US, ru-RU
	using ViewModeName		= NamedID< 32, App::_hidden_::AppIDs_Start + 4, AE_OPTIMIZE_IDS >;

	static constexpr ViewModeName	ViewModeName_Initial {"initial"};
	static constexpr ViewModeName	ViewModeName_Paused {"paused"};


	struct PlatformConfig
	{
		static constexpr uint	MaxMonitors		= 4;
		static constexpr uint	MaxWindows		= 4;
	};

} // AE::App
