// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_MACOS) || defined(AE_PLATFORM_IOS)
# include "base/Platforms/UnixUtils.h"

namespace AE::Base
{

	//
	// Apple Utils
	//

	struct AppleUtils final : UnixUtils
	{
		// Thread //
			static bool		NanoSleep (nanoseconds relativeTime);

			static void		SetThreadName (NtStringView name);
		ND_ static String	GetThreadName ();

			static bool		SetThreadAffinity (const std::thread::native_handle_type &handle, uint coreIdx);
			static bool		SetThreadPriority (const std::thread::native_handle_type &handle, float priority);
			
		ND_	static uint		GetProcessorCoreIndex ();	// current logical CPU core
		
			static void		ThreadPause ();
	};

} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
