// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_LINUX
# include "base/Platforms/UnixUtils.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{

	//
	// Linux Utils
	//

	struct LinuxUtils final : UnixUtils
	{
		// Thread //
			static void		SetThreadName (NtStringView name)													__NE___;
		ND_ static String	GetThreadName ();

			static bool		SetThreadAffinity (const ThreadHandle &handle, uint coreIdx)						__NE___;
			static bool		SetThreadPriority (const ThreadHandle &handle, float priority)						__NE___;
			
		ND_	static uint		GetProcessorCoreIndex ()															__NE___;	// current logical CPU core
		
			static void		ThreadPause ()																		__NE___;


		// OS //
		ND_ static String		GetOSName ()																	__NE___;
	};

} // AE::Base

#endif // AE_PLATFORM_LINUX
