// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_GLFW

# ifdef AE_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32 1
#	include "base/Platforms/WindowsHeader.cpp.h"
# endif
# ifdef AE_PLATFORM_MACOS
#	undef null
# endif

// don't include OpenGL
# define __gl_h_

# include "GLFW/glfw3.h"

# ifdef AE_PLATFORM_MACOS
#	include <Cocoa/Cocoa.h>
# endif

# include "GLFW/glfw3native.h"
# include "base/Defines/Undef.h"

# ifdef AE_PLATFORM_MACOS
#	define null nullptr
# endif

#endif // AE_ENABLE_GLFW
