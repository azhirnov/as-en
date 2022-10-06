// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS

# define NOMINMAX
# define NOMCX
# define NOIME
# define NOSERVICE
# define WIN32_LEAN_AND_MEAN

# ifdef AE_COMPILER_MSVC
#	pragma warning (push)
#	pragma warning (disable: 4668)
#	include <Windows.h>
#	pragma warning (pop)
# endif


# undef DeleteFile
# undef CreateWindow
# undef CreateDirectory
# undef DrawText
# undef GetMonitorInfo
# undef BitScanForward
# undef BitScanReverse
# undef Yield
# undef CreateSemaphore
# undef LoadImage
# undef MemoryBarrier
# undef GetObject
# undef GetClassName

#endif // AE_PLATFORM_WINDOWS
