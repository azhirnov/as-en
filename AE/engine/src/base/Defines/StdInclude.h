// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

// contains '__has_cpp_attribute', '__cpp_lib_*' and other
#ifdef __has_include
# if __has_include(<version>)
#	include <version>
# endif
#endif

#include "base/Defines/Config.h"
#include "base/Defines/Attribs.h"
#include "base/Defines/Errors.h"
#include "base/Defines/Helpers.h"
#include "base/Defines/MacroChecks.h"


#include <type_traits>
#include <cstdint>
#include <utility>
#include <tuple>
#include <variant>
#include <vector>
#include <string>
#include <array>
#include <memory>		// shared_ptr, weak_ptr, unique_ptr
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <bitset>
#include <cstring>
#include <cmath>
#include <optional>
#include <string_view>
#include <typeindex>
#include <type_traits>
#include <chrono>
#include <algorithm>
#include <functional>
#include <random>
#include <atomic>
#include <thread>
#include <exception>
#include <cstdlib>

#include <cstdio>
#include <streambuf>
#include <sstream>

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

#include <sstream>
#include <charconv>

#ifdef __cpp_lib_barrier
#  include <barrier>
#endif
#ifdef __cpp_lib_semaphore
#	include <semaphore>
#endif
#ifdef __cpp_lib_coroutine
#	include <coroutine>
#endif

#ifdef __has_include
# if __has_include(<bit>)
#	include <bit>
# endif
#endif

#ifdef AE_COMPILER_MSVC
# include <excpt.h>							// GetExceptionCode()
#define AE_SEH_STACK_OVERFLOW	0xC00000FDL	// EXCEPTION_STACK_OVERFLOW
#define AE_SEH_ACCESS_VIOLATION	0xC0000005L	// EXCEPTION_ACCESS_VIOLATION

# include <intrin.h>
# include <mmintrin.h>		// MMX
# include <xmmintrin.h>		// SSE
# include <emmintrin.h>		// SSE2
# include <pmmintrin.h>		// SSE3
# include <tmmintrin.h>		// SSSE3
# include <smmintrin.h>		// SSE4.1
# include <nmmintrin.h>		// SSE4.2
# include <ammintrin.h>		// SSE4A
# include <wmmintrin.h>		// AES
# include <immintrin.h>		// AVX, AVX2, FMA

# ifndef AE_COMPILER_CLANG_CL
#	include <zmmintrin.h>	// AVX512
# endif

# pragma intrinsic( _BitScanForward, _BitScanReverse )
# pragma intrinsic( __popcnt16, __popcnt )
# if AE_PLATFORM_BITS == 64
#	pragma intrinsic( _BitScanForward64, _BitScanReverse64 )
#	pragma intrinsic( __popcnt64 )
# endif
#endif

