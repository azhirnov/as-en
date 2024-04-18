// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

// must be outside 'AE_PLATFORM_WINDOWS' scope!
#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_WINDOWS

# define NOMINMAX
# define NOMCX
# define NOIME
# define NOSERVICE
# define WIN32_LEAN_AND_MEAN

# ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4668)
#	pragma warning (disable: 4005)
# endif

# include <Windows.h>
# include <WinSock2.h>
# include <ws2tcpip.h>
# include <processthreadsapi.h>
# include <timeapi.h>
# include <ntsecapi.h>
# include <ntstatus.h>
# include <Sddl.h>
# include <windowsx.h>
# include <hidusage.h>
# include <VersionHelpers.h>
	// Windows 8.1
# include <ShellScalingAPI.h>
	// Windows Vista
# include <LowLevelMonitorConfigurationAPI.h>
# include <PhysicalMonitorEnumerationAPI.h>
//# include <HighLevelMonitorConfigurationAPI.h>

# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif

# include "base/Defines/Undef.h"

#endif // AE_PLATFORM_WINDOWS
