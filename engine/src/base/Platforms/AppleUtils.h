// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_PLATFORM_MACOS) || defined(AE_PLATFORM_IOS)
# include "base/Platforms/UnixUtils.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{

	//
	// Apple Utils
	//

	struct AppleUtils final : UnixUtils
	{
		// Thread //
			static void		SetThreadName (NtStringView name)													__NE___;
		ND_ static String	GetThreadName ();

			static bool		SetThreadAffinity (const std::thread::native_handle_type &handle, uint coreIdx)		__NE___;
			static bool		SetThreadPriority (const std::thread::native_handle_type &handle, float priority)	__NE___;
			
		ND_	static uint		GetProcessorCoreIndex ()															__NE___;	// current logical CPU core
		
			static void		ThreadPause ()																		__NE___;
	};

} // AE::Base

#endif // AE_PLATFORM_MACOS or AE_PLATFORM_IOS
