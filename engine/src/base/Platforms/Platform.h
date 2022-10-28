// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Platforms/ThreadUtils.h"

#include "base/Platforms/AndroidUtils.h"
#include "base/Platforms/AppleUtils.h"
#include "base/Platforms/LinuxUtils.h"
#include "base/Platforms/WindowsUtils.h"

#include "base/Platforms/WindowsLibrary.h"
#include "base/Platforms/UnixLibrary.h"

#include "base/Platforms/WindowsProcess.h"

#include "base/Platforms/WindowsPageAllocator.h"

namespace AE::Base
{
	#if defined(AE_PLATFORM_WINDOWS)
		using PlatformUtils = WindowsUtils;
		using Process		= WindowsProcess;
	
	#elif defined(AE_PLATFORM_ANDROID)
		using PlatformUtils = AndroidUtils;

	#elif defined(AE_PLATFORM_LINUX)
		using PlatformUtils = LinuxUtils;
		
	#elif defined(AE_PLATFORM_APPLE)
		using PlatformUtils = AppleUtils;

	#endif


	#if defined(AE_PLATFORM_WINDOWS)
		using Library = WindowsLibrary;
		
	#elif defined(AE_PLATFORM_UNIX_BASED)
		using Library = UnixLibrary;

	#endif

		
	#if defined(AE_PLATFORM_WINDOWS)
		using OSPageAllocator = WindowsPageAllocator;

	#endif
	
} // AE::Base
