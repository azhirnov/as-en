// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

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
//-----------------------------------------------------------------------------


#ifdef AE_ENABLE_LOGS
/*
=================================================
	ToString (EOperationSystem)
=================================================
*/
	ND_ inline StringView  ToString (EOperationSystem value)
	{
		switch_enum( value )
		{
			case EOperationSystem::Windows :	return "Windows";
			case EOperationSystem::Android :	return "Android";
			case EOperationSystem::Linux :		return "Linux";
			case EOperationSystem::MacOS :		return "MacOS";
			case EOperationSystem::iOS :		return "iOS";
			case EOperationSystem::BSD :		return "BSD";
			case EOperationSystem::Emscripten :	return "Emscripten";
			case EOperationSystem::_Count :
			case EOperationSystem::Unknown :	break;
		}
		switch_end
		return Default;
	}

#endif // AE_ENABLE_LOGS

} // AE::Base
