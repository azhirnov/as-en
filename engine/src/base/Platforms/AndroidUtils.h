// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_ANDROID
# include "base/Platforms/UnixUtils.h"
# include "base/Containers/NtStringView.h"

namespace AE::Base
{

	//
	// Android Utils
	//

	struct AndroidUtils final : UnixUtils
	{
		// Thread //
			static void		SetThreadName (NtStringView name);
		ND_ static String	GetThreadName ();

			static bool		SetThreadAffinity (const std::thread::native_handle_type &handle, uint coreIdx);
			static bool		SetThreadPriority (const std::thread::native_handle_type &handle, float priority);
			
		ND_	static uint		GetProcessorCoreIndex ();	// current logical CPU core
		
			static void		ThreadPause ();
	};

} // AE::Base

#endif // AE_PLATFORM_ANDROID
