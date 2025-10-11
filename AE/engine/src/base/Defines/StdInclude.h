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


#include <cstdint>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <type_traits>
#include <utility>
#include <typeindex>

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
#include <optional>
#include <string_view>
#include <chrono>
#include <algorithm>
#include <functional>
#include <random>
#include <exception>

#include <streambuf>
#include <sstream>

#include <atomic>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <charconv>
#include <filesystem>

// C++20
#include <bit>
#include <source_location>
#include <coroutine>
#include <concepts>

#ifdef __cpp_lib_barrier
#  include <barrier>
#endif
#ifdef __cpp_lib_semaphore
#	include <semaphore>
#endif
#ifdef __cpp_lib_stacktrace
#	include <stacktrace>
#endif


#ifdef AE_COMPILER_MSVC
# include <excpt.h>								// GetExceptionCode()
# define AE_SEH_STACK_OVERFLOW		0xC00000FDL	// EXCEPTION_STACK_OVERFLOW
# define AE_SEH_ACCESS_VIOLATION	0xC0000005L	// EXCEPTION_ACCESS_VIOLATION

# include <intrin.h>

// TODO: enable intrinsics in compiler
# pragma intrinsic( _BitScanForward, _BitScanReverse )
# pragma intrinsic( __popcnt16, __popcnt )
# if AE_PLATFORM_BITS == 64
#	pragma intrinsic( _BitScanForward64, _BitScanReverse64 )
#	pragma intrinsic( __popcnt64 )
# endif

#endif // AE_COMPILER_MSVC

