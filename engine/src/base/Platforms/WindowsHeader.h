// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

// must be outside 'AE_PLATFORM_WINDOWS' scope!
#include "base/StdInclude.h"

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


# include "base/Defines/Undef.h"

#endif // AE_PLATFORM_WINDOWS
