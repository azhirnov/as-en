// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
 docs:
	https://software.intel.com/sites/landingpage/IntrinsicsGuide
	https://www.officedaytime.com/simd512e/
	https://www.felixcloutier.com/x86/

	ps    -- packed single precision
	pd    -- packed double precision
	epi32 -- packed 32-bit integers
	epu32 -- packed 32-bit unsigned integers
	epi64 -- packed 64-bit integers

	TODO: use AE_VCALL
*/

#pragma once

#if 0
#	include <mmintrin.h>		// MMX
#	include <xmmintrin.h>		// SSE
#	include <emmintrin.h>		// SSE2
#	include <pmmintrin.h>		// SSE3
#	include <tmmintrin.h>		// SSSE3
#	include <smmintrin.h>		// SSE4.1
#	include <nmmintrin.h>		// SSE4.2
#	include <ammintrin.h>		// SSE4A (SSE5)
#	include <wmmintrin.h>		// AES
#	include <immintrin.h>		// AVX, AVX2, FMA
#	include <zmmintrin.h>		// AVX512
#endif

#if AE_SIMD_SSE > 0

# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wignored-attributes"
# endif

# include "base/Common.h"
# include "base/CompileTime/Math.h"
# include "base/Utils/Helpers.h"

# include "base/SIMD/X64RuntimeConfig.h"

# include "base/SIMD/SSE128.h"
# include "base/SIMD/AVX256.h"
# include "base/SIMD/AVX512.h"
# include "base/SIMD/X64Crypto.h"

# include "base/SIMD/SSE128.inl.h"
# include "base/SIMD/AVX256.inl.h"
# include "base/SIMD/AVX512.inl.h"

# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
# endif

#endif // AE_SIMD_SSE
