// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once


#if defined(AE_DEBUG)
#  if defined(AE_DEVELOP) or defined(AE_PROFILE) or defined(AE_RELEASE)
#	error only one configuration must be enabled!
#  endif
#elif defined(AE_DEVELOP)
#  if defined(AE_DEBUG) or defined(AE_PROFILE) or defined(AE_RELEASE)
#	error only one configuration must be enabled!
#  endif
#elif defined(AE_PROFILE)
#  if defined(AE_DEBUG) or defined(AE_DEVELOP) or defined(AE_RELEASE)
#	error only one configuration must be enabled!
#  endif
#elif defined(AE_RELEASE)
#  if defined(AE_DEBUG) or defined(AE_DEVELOP) or defined(AE_PROFILE)
#	error only one configuration must be enabled!
#  endif
#else
#	error unknown configuration!
#endif

#if defined(AE_PLATFORM_LINUX)		or \
	defined(AE_PLATFORM_ANDROID)	or \
	defined(AE_PLATFORM_MACOS)		or \
	defined(AE_PLATFORM_IOS)		or \
	defined(AE_PLATFORM_WINDOWS)	or \
	defined(AE_PLATFORM_EMSCRIPTEN)
#else
#	error unknown platform!
#endif

#ifdef AE_PLATFORM_UNIX_BASED
#	if not (defined(AE_PLATFORM_LINUX) or defined(AE_PLATFORM_ANDROID) or defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS))
#		error platform is not Unix-based
#	endif
#endif

#ifdef AE_PLATFORM_APPLE
#	if not (defined(AE_PLATFORM_MACOS) or defined(AE_PLATFORM_IOS))
#		error platform is not Apple-based
#	endif
#	if not defined(AE_PLATFORM_TARGET_VERSION_MAJOR)
#		error AE_PLATFORM_TARGET_VERSION_MAJOR must be defined!
#	endif
#	if not defined(AE_PLATFORM_TARGET_VERSION_MINOR)
#		error AE_PLATFORM_TARGET_VERSION_MINOR must be defined!
#	endif
#endif

#ifdef AE_PLATFORM_MACOS
#	 if not ((AE_PLATFORM_TARGET_VERSION_MAJOR >= 10) and (AE_PLATFORM_TARGET_VERSION_MAJOR <= 13))
#		error AE_PLATFORM_TARGET_VERSION_MAJOR must be in range [10..13]
#	 endif
#endif
#ifdef AE_PLATFORM_IOS
#	 if not ((AE_PLATFORM_TARGET_VERSION_MAJOR >= 11) and (AE_PLATFORM_TARGET_VERSION_MAJOR <= 16))
#		error AE_PLATFORM_TARGET_VERSION_MAJOR must be in range [11..16]
#	 endif
#endif

#ifdef AE_PLATFORM_WINDOWS
#	if not defined(AE_PLATFORM_TARGET_VERSION_MAJOR)
#		error AE_PLATFORM_TARGET_VERSION_MAJOR must be defined!
#	else
#	 if not ((AE_PLATFORM_TARGET_VERSION_MAJOR >= 6) and (AE_PLATFORM_TARGET_VERSION_MAJOR <= 11))
#		error AE_PLATFORM_TARGET_VERSION_MAJOR must be in range [6..11]
#	 endif
#	endif
#endif

#if defined(AE_COMPILER_MSVC)	or \
	defined(AE_COMPILER_GCC)	or \
	defined(AE_COMPILER_CLANG)
#else
#	error unknown compiler!
#endif

#if defined(AE_CPU_ARCH_ARM64)	or \
	defined(AE_CPU_ARCH_ARM32)	or \
	defined(AE_CPU_ARCH_X64)	or \
	defined(AE_CPU_ARCH_X86)	or \
	defined(AE_CPU_ARCH_i686)
#else
#	error unknown CPU architecture!
#endif


// check features
#ifndef __cpp_threadsafe_static_init
#	error '__cpp_threadsafe_static_init' feature required
#endif


// SIMD
#ifdef AE_SIMD_NEON
# ifdef AE_SIMD_AVX
#	error AE_SIMD_AVX is not compatible with AE_SIMD_NEON
# endif
# ifdef AE_SIMD_SSE
#	error AE_SIMD_SSE is not compatible with AE_SIMD_NEON
# endif
# ifdef AE_SIMD_AES
#	error AE_SIMD_AES is not compatible with AE_SIMD_NEON
# endif
#endif

#if defined(AE_SIMD_AVX) or defined(AE_SIMD_SSE) or defined(AE_SIMD_AES)
# if defined(AE_SIMD_NEON) or defined(AE_SIMD_NEON_HALF)
#	error AE_SIMD_AVX and AE_SIMD_SSE and AE_SIMD_AES are not compatible with AE_SIMD_NEON
# endif
#endif


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_DEBUG
#	pragma detect_mismatch( "AE_DEBUG", "1" )
#  else
#	pragma detect_mismatch( "AE_DEBUG", "0" )
#  endif

#  ifdef AE_DEVELOP
#	pragma detect_mismatch( "AE_DEVELOP", "1" )
#  else
#	pragma detect_mismatch( "AE_DEVELOP", "0" )
#  endif

#  ifdef AE_PROFILE
#	pragma detect_mismatch( "AE_PROFILE", "1" )
#  else
#	pragma detect_mismatch( "AE_PROFILE", "0" )
#  endif

#  ifdef AE_RELEASE
#	pragma detect_mismatch( "AE_RELEASE", "1" )
#  else
#	pragma detect_mismatch( "AE_RELEASE", "0" )
#  endif

#  if defined(AE_FAST_HASH) and AE_FAST_HASH
#	pragma detect_mismatch( "AE_FAST_HASH", "1" )
#  else
#	pragma detect_mismatch( "AE_FAST_HASH", "0" )
#  endif

#  ifdef AE_CI_BUILD
#	pragma detect_mismatch( "AE_CI_BUILD", "1" )
#  else
#	pragma detect_mismatch( "AE_CI_BUILD", "0" )
#  endif

#  ifdef AE_ENABLE_MEMLEAK_CHECKS
#	pragma detect_mismatch( "AE_ENABLE_MEMLEAK_CHECKS", "1" )
#  else
#	pragma detect_mismatch( "AE_ENABLE_MEMLEAK_CHECKS", "0" )
#  endif


// platforms
#  ifdef AE_PLATFORM_WINDOWS
#	pragma detect_mismatch( "AE_PLATFORM_WINDOWS", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_WINDOWS", "0" )
#  endif

#  ifdef AE_PLATFORM_LINUX
#	pragma detect_mismatch( "AE_PLATFORM_LINUX", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_LINUX", "0" )
#  endif

#  ifdef AE_PLATFORM_ANDROID
#	pragma detect_mismatch( "AE_PLATFORM_ANDROID", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_ANDROID", "0" )
#  endif

#  ifdef AE_PLATFORM_MACOS
#	pragma detect_mismatch( "AE_PLATFORM_MACOS", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_MACOS", "0" )
#  endif

#  ifdef AE_PLATFORM_IOS
#	pragma detect_mismatch( "AE_PLATFORM_IOS", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_IOS", "0" )
#  endif

#  ifdef AE_PLATFORM_EMSCRIPTEN
#	pragma detect_mismatch( "AE_PLATFORM_EMSCRIPTEN", "1" )
#  else
#	pragma detect_mismatch( "AE_PLATFORM_EMSCRIPTEN", "0" )
#  endif

#  ifdef AE_PLATFORM_TARGET_VERSION_MAJOR
#	if defined(AE_PLATFORM_WINDOWS)
#	  pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MINOR", "0" )
#	  if AE_PLATFORM_TARGET_VERSION_MAJOR == 11
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "11" )		// Win11
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 10
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "10" )		// Win10
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 8
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "8" )		// Win8
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 7
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "7" )		// Win7
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 6
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "6" )		// Vista
#	  else
#	    error unsupported AE_PLATFORM_TARGET_VERSION_MAJOR for AE_PLATFORM_WINDOWS
#	  endif
#	elif defined(AE_PLATFORM_MACOS)
#	  if AE_PLATFORM_TARGET_VERSION_MAJOR == 13
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "13" )		// macos 13.0
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 12
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "12" )		// macos 12.0
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 11
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "11" )		// macos 11.0
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 10 and AE_PLATFORM_TARGET_VERSION_MINOR == 15
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "10.15" )	// macos 10.15
#	  else
#	    error unsupported AE_PLATFORM_TARGET_VERSION_MAJOR for AE_PLATFORM_MACOS
#	  endif
#	elif defined(AE_PLATFORM_IOS)
#	  if AE_PLATFORM_TARGET_VERSION_MAJOR == 16
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "16" )		// ios 16
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 15
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "15" )		// ios 15
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 14
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "14" )		// ios 14
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 13
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "13" )		// ios 13
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 12
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "12" )		// ios 12
#	  elif AE_PLATFORM_TARGET_VERSION_MAJOR == 11
#	    pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "11" )		// ios 11
#	  else
#	    error unsupported AE_PLATFORM_TARGET_VERSION_MAJOR for AE_PLATFORM_MACOS
#	  endif
#	endif
#  else
#	pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MAJOR", "0" )
#	pragma detect_mismatch( "AE_PLATFORM_TARGET_VERSION_MINOR", "0" )
#  endif


// compilers
#  ifdef AE_COMPILER_MSVC
#	pragma detect_mismatch( "AE_COMPILER_MSVC", "1" )
#  else
#	pragma detect_mismatch( "AE_COMPILER_MSVC", "0" )
#  endif

#  ifdef AE_COMPILER_CLANG
#	pragma detect_mismatch( "AE_COMPILER_CLANG", "1" )
#  else
#	pragma detect_mismatch( "AE_COMPILER_CLANG", "0" )
#  endif

#  ifdef AE_COMPILER_GCC
#	pragma detect_mismatch( "AE_COMPILER_GCC", "1" )
#  else
#	pragma detect_mismatch( "AE_COMPILER_GCC", "0" )
#  endif


// architecture
#  ifdef AE_CPU_ARCH_ARM64
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM64", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM64", "0" )
#  endif

#  ifdef AE_CPU_ARCH_ARM32
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM32", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_ARM32", "0" )
#  endif

#  ifdef AE_CPU_ARCH_X86
#	pragma detect_mismatch( "AE_CPU_ARCH_X86", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_X86", "0" )
#  endif

#  ifdef AE_CPU_ARCH_X64
#	pragma detect_mismatch( "AE_CPU_ARCH_X64", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_X64", "0" )
#  endif

#  ifdef AE_CPU_ARCH_i686
#	pragma detect_mismatch( "AE_CPU_ARCH_i686", "1" )
#  else
#	pragma detect_mismatch( "AE_CPU_ARCH_i686", "0" )
#  endif


// SIMD
#  ifdef AE_SIMD_NEON
#	pragma detect_mismatch( "AE_SIMD_NEON", "1" )
#  else
#	pragma detect_mismatch( "AE_SIMD_NEON", "0" )
#  endif

#  ifdef AE_SIMD_NEON_HALF
#	pragma detect_mismatch( "AE_SIMD_NEON_HALF", "1" )
#  else
#	pragma detect_mismatch( "AE_SIMD_NEON_HALF", "0" )
#  endif

#  ifdef AE_SIMD_AVX
#	if AE_SIMD_AVX == 0
#	  pragma detect_mismatch( "AE_SIMD_AVX", "0" )
#	elif AE_SIMD_AVX == 1
#	  pragma detect_mismatch( "AE_SIMD_AVX", "1" )
#	elif AE_SIMD_AVX == 2
#	  pragma detect_mismatch( "AE_SIMD_AVX", "2" )
#	else
#	  error unsupported value in 'AE_SIMD_AVX'
#	endif
#  else
#	pragma detect_mismatch( "AE_SIMD_AVX", "0" )
#  endif

#  ifdef AE_SIMD_SSE
#	if AE_SIMD_SSE == 0
#	  pragma detect_mismatch( "AE_SIMD_SSE", "0" )
#	elif AE_SIMD_SSE == 42
#	  pragma detect_mismatch( "AE_SIMD_SSE", "42" )
#	elif AE_SIMD_SSE == 41
#	  pragma detect_mismatch( "AE_SIMD_SSE", "41" )
#	elif AE_SIMD_SSE == 40
#	  pragma detect_mismatch( "AE_SIMD_SSE", "40" )
#	elif AE_SIMD_SSE == 31
#	  pragma detect_mismatch( "AE_SIMD_SSE", "31" )
#	elif AE_SIMD_SSE == 30
#	  pragma detect_mismatch( "AE_SIMD_SSE", "30" )
#	else
#	  error unsupported value in 'AE_SIMD_SSE'
#	endif
#  else
#	pragma detect_mismatch( "AE_SIMD_SSE", "0" )
#  endif

#  ifdef AE_SIMD_AES
#	pragma detect_mismatch( "AE_SIMD_AES", "1" )
#  else
#	pragma detect_mismatch( "AE_SIMD_AES", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH

