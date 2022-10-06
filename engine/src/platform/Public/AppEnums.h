// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{
	using namespace AE::Base;

	enum class EGestureType : ubyte
	{
		// continious
		//   state: Begin -> Update -> End/Cancel
		Hold,
		Move,
		LongPress_Move,		// long press to start gesture, then move - only for mouse or touch
		Scale2D,			// 2 pointer gesture
		Rotate2D,			// 2 pointer gesture

		// single
		//   state: End
		Down,				// on key down
		Click,				// on key up - may be slower
		DoubleClick,
		LongPress,

		_Count,
		Unknown	= 0xF,

		// flags
		//_Millimeters	= 1 << 8,
		//_Pixels			= 1 << 9,
		//_SNorm			= 1 << 10,
	};


	enum class EGestureState : ubyte
	{
		Begin,
		Update,
		End,		// used for single event too
		Cancel,		// for Android
		Outside,	// for Android
		Unknown	= 0xF
	};


}	// AE::App
