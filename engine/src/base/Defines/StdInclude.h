// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

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
#include <memory>       // shared_ptr, weak_ptr, unique_ptr
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

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

#ifdef __cpp_lib_barrier
#  include <barrier>
#endif

#ifdef __has_include
# if __has_include(<bit>)
#   include <bit>
# endif
#endif

#ifdef AE_COMPILER_MSVC
# include <intrin.h>
# pragma intrinsic( _BitScanForward, _BitScanReverse )
# pragma intrinsic( __popcnt16, __popcnt )
# if AE_PLATFORM_BITS == 64
#   pragma intrinsic( _BitScanForward64, _BitScanReverse64 )
#   pragma intrinsic( __popcnt64 )
# endif
#endif

#ifdef AE_COMPILER_MSVC
# include <excpt.h>
#endif
