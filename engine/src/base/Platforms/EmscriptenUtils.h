// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_EMSCRIPTEN
# include "base/Platforms/UnixUtils.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{
	
	//
	// Emscripten Utils
	//

	struct EmscriptenUtils final : UnixUtils
	{
		// Thread //
			static void		SetCurrentThreadName (NtStringView name)								__NE___;
		ND_ static String	GetCurrentThreadName ()													__Th___;
		
		ND_ static ThreadHandle  GetCurrentThreadHandle ()											__NE___;

			static bool		SetThreadAffinity (const ThreadHandle &handle, uint coreIdx)			__NE___;
			static bool		SetThreadPriority (const ThreadHandle &handle, float priority)			__NE___;
			
			static bool		SetCurrentThreadAffinity (uint coreIdx)									__NE___;
			static bool		SetCurrentThreadPriority (float priority)								__NE___;

		ND_	static uint		GetProcessorCoreIndex ()												__NE___;	// current logical CPU core
		
			static void		ThreadPause ()															__NE___;
	};


} // AE::Base

#endif // AE_PLATFORM_EMSCRIPTEN
