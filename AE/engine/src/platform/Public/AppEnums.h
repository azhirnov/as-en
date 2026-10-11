// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{
	using namespace AE::Base;

	enum class EGestureType : ubyte
	{
		// single
		//   state: End
		Down,				// params:							|	on key down
		Click,				// params:							|	on key up - may be slower
		DoubleClick,		// params:							|	on first click - update, on second click - end

		// continuous
		//   state: Begin -> Update -> End/Cancel
		Hold,				// params:							|
		LongPress,			// params: f1(0..1)					|
		Move,				// params: f1/2/3/4					|
		LongPress_Move,		// params: f3(pos, 0..1)			|	long press to start gesture, then move - only for mouse or touch
		ScaleRotate2D,		// params: f2/4(delta, absolute)	|	2 pointer gesture
		//EdgeSwipe,		// params: f2(0..1, side)

		_Count,
		Unknown	= _Count
	};


	enum class EGestureState : ubyte
	{
		Begin,
		Update,
		End,		// used for single event too
		Cancel,		// for Android
		Outside,	// for Android
		_Count
	};

	NdCxIn bool  EGestureState_IsActive (EGestureState state) __NE___
	{
		return state  <= EGestureState::Update;
	}


	enum class EAppStorage
	{
		Builtin,		// read-only
		Cache,			// read / write / execute
		ExternalCache,	// read / write		// TODO: shared cache?
		UserData,		// read / write
		SharedData,		// read / write, shared between apps ('Documents' on Windows, '/sdcard' on Android)
		//AppData
		_Count
	};


	// Used to identify different gamepads and to send feedback actions (vibrations, ...).
	enum class ControllerID : ushort
	{
		Default			= 0,
		Invalid			= 0xFFFF,

		_First			= 1,
		_GamepadFirst	= 1 << 12,
		_VRFirst		= 1 << 14,

	  // default IDs:
		Mouse			= _First,
		Keyboard,
		Touchscreen,
		Sensor,

		Gamepad			= _GamepadFirst,

		VR_HMD			= _VRFirst,
		VR_LeftHand,
		VR_RightHand,
	};


	enum class EWindowDisplayMode : ubyte
	{
		AlwaysVisible,
		VisibleOnScreen_ClippedForCapture,		// capture will show black window
		VisibleOnScreen_HiddenForCapture,		// window completelly hidden in capture
	};


	static constexpr StringView			StoragePrefix_Builtin		{"builtin:/"};
	static constexpr StringView			StoragePrefix_Cache			{"cache:/"};
	static constexpr StringView			StoragePrefix_ExternalCache	{"ext-cache:/"};
	static constexpr StringView			StoragePrefix_SharedData	{"shared-data:/"};
	static constexpr StringView			StoragePrefix_UserData		{"user-data:/"};

	static constexpr VFS::StorageName	Storage_Builtin				{StoragePrefix_Builtin};
	static constexpr VFS::StorageName	Storage_Cache				{StoragePrefix_Cache};
	static constexpr VFS::StorageName	Storage_ExternalCache		{StoragePrefix_ExternalCache};
	static constexpr VFS::StorageName	Storage_SharedData			{StoragePrefix_SharedData};
	static constexpr VFS::StorageName	Storage_UserData			{StoragePrefix_UserData};

} // AE::App
