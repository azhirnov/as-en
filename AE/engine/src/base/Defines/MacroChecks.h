// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef __cplusplus
#   error Requires C++ compiler!
#endif
#if (__cplusplus != 199711L) and (__cplusplus < 201703L)
#   error Requires at least C++17 support
#endif

#if defined(AE_CFG_DEBUG)
#  if defined(AE_CFG_DEVELOP) or defined(AE_CFG_PROFILE) or defined(AE_CFG_RELEASE)
#   error only one configuration must be enabled!
#  endif
#elif defined(AE_CFG_DEVELOP)
#  if defined(AE_CFG_DEBUG) or defined(AE_CFG_PROFILE) or defined(AE_CFG_RELEASE)
#   error only one configuration must be enabled!
#  endif
#elif defined(AE_CFG_PROFILE)
#  if defined(AE_CFG_DEBUG) or defined(AE_CFG_DEVELOP) or defined(AE_CFG_RELEASE)
#   error only one configuration must be enabled!
#  endif
#elif defined(AE_CFG_RELEASE)
#  if defined(AE_CFG_DEBUG) or defined(AE_CFG_DEVELOP) or defined(AE_CFG_PROFILE)
#   error only one configuration must be enabled!
#  endif
#else
#   error unknown configuration!
#endif


#ifdef AE_DEBUG
# ifdef AE_RELEASE
#   error only one of AE_DEBUG and AE_RELEASE can be defined
# endif
#endif
#ifdef AE_RELEASE
# ifdef AE_DEBUG
#   error only one of AE_DEBUG and AE_RELEASE can be defined
# endif
#endif


#if defined(AE_PLATFORM_LINUX)      or \
    defined(AE_PLATFORM_ANDROID)    or \
    defined(AE_PLATFORM_MACOS)      or \
    defined(AE_PLATFORM_IOS)        or \
    defined(AE_PLATFORM_WINDOWS)    or \
    defined(AE_PLATFORM_EMSCRIPTEN)
#else
#   error unknown platform!
#endif

#ifdef AE_PLATFORM_UNIX_BASED
#   if not (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID) or defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS) or defined(AE_PLATFORM_EMSCRIPTEN))
#       error platform is not Unix-based
#   endif
#endif

#ifdef AE_PLATFORM_LINUX
#   if not (defined(__linux__) or defined(__gnu_linux__))
#       error platform is not Linux
#   endif
#endif

#ifdef AE_PLATFORM_APPLE
#   if not (defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS))
#       error platform is not Apple-based
#   endif
#   if not defined(AE_PLATFORM_TARGET_VERSION_MAJOR)
#       error AE_PLATFORM_TARGET_VERSION_MAJOR must be defined!
#   endif
#   if not defined(AE_PLATFORM_TARGET_VERSION_MINOR)
#       error AE_PLATFORM_TARGET_VERSION_MINOR must be defined!
#   endif
#   if not (defined(__APPLE__) or defined(MACOSX))
#       error platform is not Apple
#   endif
#endif

#ifdef AE_PLATFORM_MACOS
#    if not ((AE_PLATFORM_TARGET_VERSION_MAJOR >= 10) and (AE_PLATFORM_TARGET_VERSION_MAJOR <= 13))
#       error AE_PLATFORM_TARGET_VERSION_MAJOR must be in range [10..13]
#    endif
#endif

#ifdef AE_PLATFORM_IOS
#    if not ((AE_PLATFORM_TARGET_VERSION_MAJOR >= 11) and (AE_PLATFORM_TARGET_VERSION_MAJOR <= 16))
#       error AE_PLATFORM_TARGET_VERSION_MAJOR must be in range [11..16]
#    endif
#endif

#ifdef AE_PLATFORM_WINDOWS
#   if not defined(AE_PLATFORM_TARGET_VERSION_MAJOR)
#       error AE_PLATFORM_TARGET_VERSION_MAJOR must be defined!
#   else
#    if not ((AE_PLATFORM_TARGET_VERSION_MAJOR >= 6) and (AE_PLATFORM_TARGET_VERSION_MAJOR <= 11))
#       error AE_PLATFORM_TARGET_VERSION_MAJOR must be in range [6..11]
#    endif
#   endif
#   if not (defined(_WIN32) or defined(_WIN64) or defined(__CYGWIN__) or defined(__MINGW32__))
#       error platform is not WIndows
#   endif
#endif

#if defined(AE_COMPILER_MSVC)   or \
    defined(AE_COMPILER_GCC)    or \
    defined(AE_COMPILER_CLANG)
#else
#   error unknown compiler!
#endif

#ifdef AE_COMPILER_MSVC
# ifndef _MSC_VER
#   error not a MSVC compiler!
# endif
#endif

#ifdef AE_COMPILER_GCC
# if not (defined(__GNUC__) or defined(__MINGW32__))
#   error not a GNU C compiler!
# endif
#endif

#ifdef AE_COMPILER_CLANG
# if not (defined(__clang__) or defined(__llvm__ ))
#   error not a Clang compiler!
# endif
#endif

#if defined(AE_CPU_ARCH_ARM64)  or \
    defined(AE_CPU_ARCH_ARM32)  or \
    defined(AE_CPU_ARCH_X64)    or \
    defined(AE_CPU_ARCH_X86)    or \
    defined(AE_CPU_ARCH_i686)
#else
#   error unknown CPU architecture!
#endif


// check features
#ifndef AE_PLATFORM_EMSCRIPTEN  // TODO: flag for emscripten multithreading
# ifndef __cpp_threadsafe_static_init
#   error '__cpp_threadsafe_static_init' feature required
# endif
#endif


// SIMD
#if AE_SIMD_NEON
# if AE_SIMD_AVX
#   error AE_SIMD_AVX is not compatible with AE_SIMD_NEON
# endif
# if AE_SIMD_SSE
#   error AE_SIMD_SSE is not compatible with AE_SIMD_NEON
# endif
# if AE_SIMD_AES
#   error AE_SIMD_AES is not compatible with AE_SIMD_NEON
# endif
#endif

#if (AE_SIMD_AVX | AE_SIMD_SSE | AE_SIMD_AES)
# if (AE_SIMD_NEON | AE_SIMD_NEON_HALF)
#   error AE_SIMD_AVX and AE_SIMD_SSE and AE_SIMD_AES are not compatible with AE_SIMD_NEON
# endif
#endif

#ifndef AE_HAS_SIMD
#   error AE_HAS_SIMD must be defined
#endif


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_CFG_DEBUG
#   pragma detect_mismatch( "AE_CFG_DEBUG", "1" )
#  else
#   pragma detect_mismatch( "AE_CFG_DEBUG", "0" )
#  endif

#  ifdef AE_CFG_DEVELOP
#   pragma detect_mismatch( "AE_CFG_DEVELOP", "1" )
#  else
#   pragma detect_mismatch( "AE_CFG_DEVELOP", "0" )
#  endif

#  ifdef AE_CFG_PROFILE
#   pragma detect_mismatch( "AE_CFG_PROFILE", "1" )
#  else
#   pragma detect_mismatch( "AE_CFG_PROFILE", "0" )
#  endif

#  ifdef AE_CFG_RELEASE
#   pragma detect_mismatch( "AE_CFG_RELEASE", "1" )
#  else
#   pragma detect_mismatch( "AE_CFG_RELEASE", "0" )
#  endif

#  if defined(AE_FAST_HASH) and AE_FAST_HASH
#   pragma detect_mismatch( "AE_FAST_HASH", "1" )
#  else
#   pragma detect_mismatch( "AE_FAST_HASH", "0" )
#  endif

#  ifdef AE_CI_BUILD
#   pragma detect_mismatch( "AE_CI_BUILD", "1" )
#  else
#   pragma detect_mismatch( "AE_CI_BUILD", "0" )
#  endif

#  ifdef AE_ENABLE_MEMLEAK_CHECKS
#   pragma detect_mismatch( "AE_ENABLE_MEMLEAK_CHECKS", "1" )
#  else
#   pragma detect_mismatch( "AE_ENABLE_MEMLEAK_CHECKS", "0" )
#  endif


// platforms
#  ifdef AE_PLATFORM_WINDOWS
#   pragma detect_mismatch( "AE_PLATFORM_WINDOWS", "1" )
#  else
#   pragma detect_mismatch( "AE_PLATFORM_WINDOWS", "0" )
#  endif

#  ifdef AE_PLATFORM_LINUX
#   pragma detect_mismatch( "AE_PLATFORM_LINUX", "1" )
#  else
#   pragma detect_mismatch( "AE_PLATFORM_LINUX", "0" )
#  endif

#  ifdef AE_PLATFORM_ANDROID
#   pragma detect_mismatch( "AE_PLATFORM_ANDROID", "1" )
#  else
#   pragma detect_mismatch( "AE_PLATFORM_ANDROID", "0" )
#  endif

#  ifdef AE_PLATFORM_MACOS
#   pragma detect_mismatch( "AE_PLATFORM_MACOS", "1" )
#  else
#   pragma detect_mismatch( "AE_PLATFORM_MACOS", "0" )
#  endif

#  ifdef AE_PLATFORM_IOS
#   pragma detect_mismatch( "AE_PLATFORM_IOS", "1" )
#  else
#   pragma detect_mismatch( "AE_PLATFORM_IOS", "0" )
#  endif

#  ifdef AE_PLATFORM_EMSCRIPTEN
#   pragma detect_mismatch( "AE_PLATFORM_EMSCRIPTEN", "1" )
#  else
#   pragma detect_mismatch( "AE_PLATFORM_EMSCRIPTEN", "0" )
#  endif

#  ifdef AE_DISABLE_THREADS
#   pragma detect_mismatch( "AE_DISABLE_THREADS", "1" )
#  else
#   pragma detect_mismatch( "AE_DISABLE_THREADS", "0" )
#  endif

#  ifdef AE_PLATFORM_TARGET_VERSION_MAJOR
#   if defined(AE_PLATFORM_WINDOWS)
#     pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MINOR", "0" )
#     if AE_PLATFORM_TARGET_VERSION_MAJOR == 11
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "11" )      // Win11
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 10
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "10" )      // Win10
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 8
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "8" )       // Win8
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 7
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "7" )       // Win7
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 6
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "6" )       // Vista
#     else
#       error unsupported AE_PLATFORM_TARGET_VERSION_MAJOR for AE_PLATFORM_WINDOWS
#     endif
#   elif defined(AE_PLATFORM_MACOS)
#     if AE_PLATFORM_TARGET_VERSION_MAJOR == 13
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "13" )      // macos 13.0
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 12
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "12" )      // macos 12.0
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 11
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "11" )      // macos 11.0
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 10 and AE_PLATFORM_TARGET_VERSION_MINOR == 15
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "10.15" )   // macos 10.15
#     else
#       error unsupported AE_PLATFORM_TARGET_VERSION_MAJOR for AE_PLATFORM_MACOS
#     endif
#   elif defined(AE_PLATFORM_IOS)
#     if AE_PLATFORM_TARGET_VERSION_MAJOR == 16
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "16" )      // ios 16
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 15
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "15" )      // ios 15
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 14
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "14" )      // ios 14
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 13
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "13" )      // ios 13
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 12
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "12" )      // ios 12
#     elif AE_PLATFORM_TARGET_VERSION_MAJOR == 11
#       pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "11" )      // ios 11
#     else
#       error unsupported AE_PLATFORM_TARGET_VERSION_MAJOR for AE_PLATFORM_MACOS
#     endif
#   endif
#  else
#   pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "0" )
#   pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MINOR", "0" )
#  endif


// compilers
#  ifdef AE_COMPILER_MSVC
#   pragma detect_mismatch( "AE_COMPILER_MSVC", "1" )
#  else
#   pragma detect_mismatch( "AE_COMPILER_MSVC", "0" )
#  endif

#  ifdef AE_COMPILER_CLANG
#   pragma detect_mismatch( "AE_COMPILER_CLANG", "1" )
#  else
#   pragma detect_mismatch( "AE_COMPILER_CLANG", "0" )
#  endif

#  ifdef AE_COMPILER_GCC
#   pragma detect_mismatch( "AE_COMPILER_GCC", "1" )
#  else
#   pragma detect_mismatch( "AE_COMPILER_GCC", "0" )
#  endif


// architecture
#  ifdef AE_CPU_ARCH_ARM64
#   pragma detect_mismatch( "AE_CPU_ARCH_ARM64", "1" )
#  else
#   pragma detect_mismatch( "AE_CPU_ARCH_ARM64", "0" )
#  endif

#  ifdef AE_CPU_ARCH_ARM32
#   pragma detect_mismatch( "AE_CPU_ARCH_ARM32", "1" )
#  else
#   pragma detect_mismatch( "AE_CPU_ARCH_ARM32", "0" )
#  endif

#  ifdef AE_CPU_ARCH_X86
#   pragma detect_mismatch( "AE_CPU_ARCH_X86", "1" )
#  else
#   pragma detect_mismatch( "AE_CPU_ARCH_X86", "0" )
#  endif

#  ifdef AE_CPU_ARCH_X64
#   pragma detect_mismatch( "AE_CPU_ARCH_X64", "1" )
#  else
#   pragma detect_mismatch( "AE_CPU_ARCH_X64", "0" )
#  endif

#  ifdef AE_CPU_ARCH_i686
#   pragma detect_mismatch( "AE_CPU_ARCH_i686", "1" )
#  else
#   pragma detect_mismatch( "AE_CPU_ARCH_i686", "0" )
#  endif


// SIMD
#  if AE_SIMD_NEON
#   pragma detect_mismatch( "AE_SIMD_NEON", "1" )
#  else
#   pragma detect_mismatch( "AE_SIMD_NEON", "0" )
#  endif

#  if AE_SIMD_NEON_HALF
#   pragma detect_mismatch( "AE_SIMD_NEON_HALF", "1" )
#  else
#   pragma detect_mismatch( "AE_SIMD_NEON_HALF", "0" )
#  endif

#  ifdef AE_SIMD_AVX
#   if AE_SIMD_AVX == 0
#     pragma detect_mismatch( "AE_SIMD_AVX", "0" )
#   elif AE_SIMD_AVX == 1
#     pragma detect_mismatch( "AE_SIMD_AVX", "1" )
#   elif AE_SIMD_AVX == 2
#     pragma detect_mismatch( "AE_SIMD_AVX", "2" )
#   else
#     error unsupported value in 'AE_SIMD_AVX'
#   endif
#  else
#   pragma detect_mismatch( "AE_SIMD_AVX", "0" )
#  endif

#  ifdef AE_SIMD_SSE
#   if AE_SIMD_SSE == 0
#     pragma detect_mismatch( "AE_SIMD_SSE", "0" )
#   elif AE_SIMD_SSE == 42
#     pragma detect_mismatch( "AE_SIMD_SSE", "42" )
#   elif AE_SIMD_SSE == 41
#     pragma detect_mismatch( "AE_SIMD_SSE", "41" )
#   elif AE_SIMD_SSE == 40
#     pragma detect_mismatch( "AE_SIMD_SSE", "40" )
#   elif AE_SIMD_SSE == 31
#     pragma detect_mismatch( "AE_SIMD_SSE", "31" )
#   elif AE_SIMD_SSE == 30
#     pragma detect_mismatch( "AE_SIMD_SSE", "30" )
#   elif AE_SIMD_SSE == 20
#     pragma detect_mismatch( "AE_SIMD_SSE", "20" )
#   else
#     error unsupported value in 'AE_SIMD_SSE'
#   endif
#  else
#   pragma detect_mismatch( "AE_SIMD_SSE", "0" )
#  endif

#  if AE_SIMD_AES
#   pragma detect_mismatch( "AE_SIMD_AES", "1" )
#  else
#   pragma detect_mismatch( "AE_SIMD_AES", "0" )
#  endif

#  if AE_HAS_SIMD
#   pragma detect_mismatch( "AE_HAS_SIMD", "1" )
#  else
#   pragma detect_mismatch( "AE_HAS_SIMD", "0" )
#  endif

#  if AE_OPTIMIZE_IDS
#   pragma detect_mismatch( "AE_OPTIMIZE_IDS", "1" )
#  else
#   pragma detect_mismatch( "AE_OPTIMIZE_IDS", "0" )
#  endif

#  ifdef AE_ENABLE_ABSEIL
#   pragma detect_mismatch( "AE_ENABLE_ABSEIL", "1" )
#  else
#   pragma detect_mismatch( "AE_ENABLE_ABSEIL", "0" )
#  endif

#  ifdef AE_ENABLE_BROTLI
#   pragma detect_mismatch( "AE_ENABLE_BROTLI", "1" )
#  else
#   pragma detect_mismatch( "AE_ENABLE_BROTLI", "0" )
#  endif

#  ifdef AE_ENABLE_UTF8PROC
#   pragma detect_mismatch( "AE_ENABLE_UTF8PROC", "1" )
#  else
#   pragma detect_mismatch( "AE_ENABLE_UTF8PROC", "0" )
#  endif

#  ifdef FP_FAST_FMA
#   pragma detect_mismatch( "FP_FAST_FMA", "1" )
#  else
#   pragma detect_mismatch( "FP_FAST_FMA", "0" )
#  endif
#  ifdef FP_FAST_FMAF
#   pragma detect_mismatch( "FP_FAST_FMAF", "1" )
#  else
#   pragma detect_mismatch( "FP_FAST_FMAF", "0" )
#  endif
#  ifdef FP_FAST_FMAL
#   pragma detect_mismatch( "FP_FAST_FMAL", "1" )
#  else
#   pragma detect_mismatch( "FP_FAST_FMAL", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

