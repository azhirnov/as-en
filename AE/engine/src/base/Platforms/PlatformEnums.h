// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Common.h"
#include "base/Utils/EnumSet.h"

namespace AE::Base
{

	enum class EOperationSystem : ubyte
	{
		Unknown		= 0,
		Windows,
		Android,
		Linux,
		MacOS,
		iOS,
		BSD,
		Emscripten,
		_Count
	};


	enum class EThreadPriority : ubyte
	{
		PerFrame,		// high priority for per frame tasks, should be used for 'EThread::PerFrame' and 'EThread::Renderer'.
		PerFrameLow,
		Default,		// thread created with this priority.
		Background,		// low priority to execute large tasks, can be used for 'EThread::Background'.
		BackgroundLow,
		Highest,		// only for special use case.
		_Count
	};


	enum class EThreadPowerThrottling : ubyte
	{
		Auto,
		Enable,
		Disable,
	};


	enum class ESystemSleepState : ubyte
	{
		AllowSleep,
		AllowLowPowerMode,				// allow to enter to low power mode, but never sleep
		DontSleep_AllowTurnDisplayOff,
		DisplayAlwaysOn,

		Default		= AllowSleep,
	};

} // AE::Base
