// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include <type_traits>

#ifdef AE_COMPILER_MSVC
#	pragma warning (push, 1)
#	pragma warning (disable: 4244)
#endif
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wundefined-internal"
#endif

#ifndef AE_COMPILER_GCC  // failed to link

namespace
{
	#undef ASSERT
	#define AE_HAS_ATOMICS			1
	#define AE_shader_atomic_float	1
	#define AE_shader_atomic_float2	1

	#include <aestyle.glsl.h>
	#include "Math.glsl"

	//#include "AABB.glsl"

	//#include "Test_AEStyleGLSL.h"
}

#endif // not AE_COMPILER_GCC

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif
#if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
#endif
