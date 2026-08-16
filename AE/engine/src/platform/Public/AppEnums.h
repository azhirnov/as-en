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

	static constexpr VFS::StorageName	Storage_Builtin			{"builtin:/"};
	static constexpr VFS::StorageName	Storage_Cache			{"cache:/"};
	static constexpr VFS::StorageName	Storage_ExternalCache	{"ext-cache:/"};
	static constexpr VFS::StorageName	Storage_SharedData		{"shared-data:/"};
	static constexpr VFS::StorageName	Storage_UserData		{"user-data:/"};

} // AE::App
